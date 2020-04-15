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
        case 6: // 110 shift B left 16 bits | ALUresult = B<<16

            *ALUresult = B << 16;

            break;
        case 7: // 111 NOT A | ALUresult = NOT A

            *ALUresult = ~A;

            break;
        default:
            break;
    }
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
    if(PC % 4 != 0 || PC > 65536){
        return 1;
    }
    *instruction = Mem[PC>>2];
    return 0;
    

}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
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
int instruction_decode(unsigned op, struct_controls *controls)
{
    switch (op)
    {
    case 0://R-type
        controls->ALUOp = 7; //r-type
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
        controls->ALUOp = 0; //add
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
        controls->ALUOp = 0;//have to add for address 
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
        return 0;
    case 43: //store word
        controls->ALUOp = 0;//have to add for address
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 1;
        controls->RegDst = 0;
        controls->RegWrite = 0;
        return 0;
    case 15: //Load Upper Immediate
        controls->ALUOp = 6;//shift left
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
        return 0;
    case 4: //branch on equal
        controls->ALUOp = 1;//subtract and check for 0
        controls->ALUSrc = 0;
        controls->Branch = 1;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 0;
        return 0;
    case 10: //Set on less than immediate
        controls->ALUOp = 2;//set less than
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
        return 0;
    case 11: //Set on less than immediate unsigned
        controls->ALUOp = 3;//set less than unsigned
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
        return 0;
    case 2: //jump
        controls->ALUOp = 0;//0 for dont care because alu doesn't do anything for jump
        controls->ALUSrc = 0;
        controls->Branch = 0;
        controls->Jump = 1;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 0;
        return 0;
    default:
        return 1;
    }
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
    if(offset>>16){//if there is a 1 in the MSB
        *extended_value = offset | 0XFFFF0000; //or to set the leading zeros to one but to maintain last 16 bits
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{

    if(ALUOp >= 0 && ALUOp < 7)
    {
        ALU(data1, (ALUSrc) ? extended_value : data2, ALUOp, ALUresult, Zero);
    }        
    else if(ALUOp == 7)
    {
        switch (funct)
        {
            case 32: // add
                ALU(data1, data2, 0, ALUresult, Zero);
                break;
            case 34: // subtract
                ALU(data1, data2, 1, ALUresult, Zero);
                break;
            case 42: // set less than
                ALU(data1, data2, 2, ALUresult, Zero);
                break;
            case 43: // set less than unsigned
                ALU(data1, data2, 3, ALUresult, Zero);
                break;
            case 36: // AND
                ALU(data1, data2, 4, ALUresult, Zero);
                break;
            case 37: // OR
                ALU(data1, data2, 5, ALUresult, Zero);
                break;
            default:
                return 1; // Illegal instruction
        }
    }
    else
    {
        // Any other ALUOp value should cause a halt condition
        return 1;    
    }
    
    return 0;

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
    if(MemRead){
        if(ALUresult > 65536 || (ALUresult % 4) != 0)
            return 1;
        
        *memdata = Mem[ALUresult >> 2];
        
        return 0;
    }else if(MemWrite){
        if(ALUresult > 65536 || (ALUresult % 4) != 0)
            return 1;
        
        Mem[ALUresult >> 2] = data2;
        
        return 0;
    }else {
        return 0;
    }
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
    
    if(RegWrite){
       unsigned destinationReg = (RegDst) ? r2:r3; //use correct register based on RegDst;

       if(MemtoReg){
           Reg[destinationReg] = memdata;
           return;
       } else{
           Reg[destinationReg] = ALUresult;
           return;
       }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
    *PC += 4;
    if(Branch){
        *PC += extended_value << 2;
        return;
    } else if (Jump){
        *PC = *PC & 0XF0000000;
        *PC += jsec;
        return;
    }
}