#include "spimcore.h"

// Helper method that emulates a Ripple-Carry Adder
// Ripple-Carry adders aren't exactly fast or efficient
// I think C can handle a O(n) operation where n = 32
// May implement Partial Carry Lookahead Adder if needed 
unsigned ALUadder(unsigned A, unsigned B)
{
    unsigned result = 0;
    char bitA, bitB, sum = 0, carry = 0;
    int i;

    for (i = 0; i < 32; i++)
    {
        // Get bits one by one starting with LSB
        bitA = (A >> i) & 1;
        bitB = (B >> i) & 1;

        // Get the sum bit
        sum = bitA ^ bitB ^ carry;
        
        // Add bit to the result
        result += sum << i;

        // Update carry
        carry = bitA & bitB | carry & (bitA ^ bitB);
    }

    return result;
}

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
    switch(ALUControl)
    {
        case 0: // 000 add | ALUresult = A + B
            
            // Just add the two numbers
            *ALUresult = ALUadder(A, B);

            break;
        case 1: // 001 subtract | ALUresult = A - B
            
            // Add A and two's complement of B
            *ALUresult = ALUadder(A, ~B + 1);
            break;

        case 2: // 010 set less than | ALUresult = (A < B)

            // Subtract B from A
            *ALUresult = ALUadder(A, ~B + 1);

            // Get the sign of the result
            *ALUresult = (*ALUresult >> 31) & 1;

            // Set Zero appropriately
            // This is done by NORing all bits of the result so that if any bit of the result is set to 1 Zero is set to 0
            // But we already have the MSB stored as the LSB in ALUresult so we can use that instead and apply a logical NOT
            *Zero = !(*ALUresult);

            break;
        case 3: // 011 unsigned set less than | ALUresult = (A < B)
            
            // We're already working with unsigned numbers
            *ALUresult = (A < B) ? 1 : 0;
            // Set Zero appropriately
            *Zero = !(*ALUresult);

            break;
        case 4: // 100 AND | ALUresult = A AND B

            *ALUresult = A & B;

            break;
        case 5: // 101 OR | ALUresult = A OR B

            *ALUresult = A | B;

            break;
        case 6: // 110 shift B left 16 bits | ALUresult = A<<16

            *ALUresult = *ALUresult << 16;

            break;
        case 7: // 111 NOT A | ALUresult = NOT A

            *ALUresult = ~(*ALUresult);

            break;
    }
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if(PC % 4 != 0){
        return 1;
    }
    *instruction = Mem[PC>>2];
    return 0;
    

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = (instruction & 0xfc000000) >> 26;
    *r1 = (instruction & 0x03c00000) >> 22;
    *r2 = (instruction & 0x003c0000) >> 18;
    *r3 = (instruction & 0x0003c000) >> 14;
    *funct = (instruction & 0x0000003f) >> 0;
    *offset = (instruction & 0x0000ffff) >> 0;
    *jsec = (instruction & 0x03ffffff) >> 0;
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    switch (op)
    {
    case 0://R-type
        controls->ALUOp = 7;
        controls->ALUSrc = 0;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 1;
        controls->RegWrite = 1;
        return 0;
    case 8://add immediate
        controls->ALUOp = 0;
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
        return 0;
    case 35: //load word
        controls->ALUOp = 0;
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
        return 0;
    default:
        return 1;
    }
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{

}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}