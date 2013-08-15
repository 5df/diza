#include "opcode.h"

#include <stdio.h>

int disassemble(Diza *diza, Instruction *inst)
{
    //assert(diza->index < diza->len);
    int ret;
    unsigned int i;
    unsigned char opcode; 
    int index0;

    index0=diza->index;

    // Lets GOOoooo
    ret = prefixcb(diza,inst);
    // We are done with the opcodes here
    // XXX check disas flag

    addrcb(diza,inst);
    datacb(diza,inst);
    
    inst->len = diza->index - index0;

    printf(" = %d\n",inst->len);

    return inst->len;
}

int assemble(Diza *diza, Instruction *inst)
{
    return -1;
}

// Private

int datacb(Diza *diza, Instruction *inst)
{
    unsigned i,d;

    d=0;
    if(inst->flag & FLAG_DATA1) d+=0x1;
    if(inst->flag & FLAG_DATA2) d+=0x2;
    if(inst->flag & FLAG_DATA4) d+=0x4;

    if(inst->flag & FLAG_DATA66)
        d += inst->defdata;
        
    inst->datasize = d;

    for(i=0;i<d;i++)
    {
        inst->data_b[i] = diza->buf[diza->index++];
    }

    return diza->flag;
}


int addrcb(Diza *diza, Instruction *inst)
{
    unsigned long i,a;
    
    a=0;
    if(inst->flag & FLAG_ADDR1) a+=0x1;
    if(inst->flag & FLAG_ADDR2) a+=0x2;
    if(inst->flag & FLAG_ADDR4) a+=0x4;
    if(inst->flag & FLAG_ADDR67)
        a += inst->defaddr;

    inst->addrsize = a;
    
    for(i=0;i<a;i++)
    {
        inst->addr_b[i] = diza->buf[diza->index++];
    }
    return diza->flag;
}





int sibcb(Diza *diza, Instruction *inst)
{
    unsigned int ret;

    printf(" sib |");

    ret = diza->flag;
    // Store the flag
    inst->flag |= FLAG_SIB;
    inst->sib = diza->buf[diza->index];
    // Next
    ++diza->index;

    // XXX nothing after sib ?
    return ret | DIZA_DONE;
}

int modrmcb(Diza *diza, Instruction *inst)
{
    unsigned char op;
    unsigned int flag;
    int ret; 
    unsigned char sibbyte;


    ret = diza->flag;
    op = diza->buf[diza->index];
    flag = table[op];
    
    if(inst->flag & FLAG_MODRM)
    {
        printf(" modrm: %02X |",op);
     
        inst->modrm = op;
        // Store the fag
        inst->flag |= FLAG_MODRM;
        // Next
        ++diza->index;

        if((op & 0x38) == 0x20)
        {
            if(inst->opcode[0] == 0xFF)
                flag |= FLAG_STOP;
        }

        unsigned char mod = op  & 0xC0;
        unsigned char rm = op & 0x07;

        if(mod != 0xC0)
        {
            // 32 bit
            if(inst->defaddr == 4)
            {
                if(rm == 4)
                {
                    ret |= sibcb(diza,inst);
                    rm = op & 0x07;
                }
                if(mod == 0x40)
                {
                    inst->flag |= FLAG_ADDR1;
                }
                else
                if(mod == 0x80)
                {
                    inst->flag |= FLAG_ADDR4;
                }
                else
                {
                    if(rm == 5)
                        inst->flag |= FLAG_ADDR4;
                }

            }
            // 16 bit
            else
            {
                if(mod == 0x40)
                {
                    inst->flag |= FLAG_ADDR1;
                }
                else
                if(mod == 0x00)
                {
                    inst->flag |= FLAG_ADDR2;
                }
                else
                {
                    if(rm == 6)
                    {
                        inst->flag |= FLAG_ADDR2;
                    }
                }
            }
        }// MODRM
    }
    return ret | DIZA_DONE; 
}

int opcodecb(Diza *diza, Instruction *inst)
{
    unsigned char op;
    unsigned int flag;
    int ret;
    unsigned int cnt;


    ret = diza->flag;
    op = diza->buf[diza->index];
    
    printf(" opcode: %02X |",op);
    
    if(inst->opcodecnt)
        flag = table[256 + op];
    else
        flag = table[op];

    // We store the opcode count
    cnt = inst->opcodecnt;
    ++inst->opcodecnt;
    // We store the opcode flag
    inst->flag |= flag;
    // Next
    ++diza->index;

    // Extended opcodes
    if(op == 0x0F)
    {
        // Is the first prefix
        if(cnt==0)
        {
            inst->opcode[cnt] = op;
            ret |= opcodecb(diza,inst);
        }
    }
    else
    {
        ret |= modrmcb(diza,inst);
    }
    return ret;
}

int prefixcb(Diza *diza, Instruction *inst)
{
    unsigned char op;
    unsigned int flag;
    int ret;

    ret = diza->flag;
    op=diza->buf[diza->index];
    flag=table[op];

    // XXX check out for twice same prefix
    if (false) return ret | DIZA_STOP;    // twice LOCK,SEG,REP,66,67

    if(flag & (FLAG_67+FLAG_66+FLAG_SEG+FLAG_REP))
    {
        printf(" prefix: %02X |",op);
        if (flag & FLAG_67) // Address-size override prefix
        {
          inst->defaddr ^= 2^4;
        }
        else
        if (flag & FLAG_66) // Operand-size override prefix
        {
          inst->defdata ^= 2^4;
        }
        else
        if (flag & FLAG_SEG) // Segment override prefix
        {
          inst->seg = op;
        }
        else
        if (flag & FLAG_REP) // Repeat prefix (REPNE/REPNZ, REP or REPE/REPZ)
        {
          inst->rep = op;
        }
        //
        // LOCK
        //
        
        // We store the flag
        inst->flag |= flag;
        // Next
        ++diza->index;

        ret |= prefixcb(diza,inst);
    }
    else
    {
        // This opcode is not a prefix, maybe an opcode ?
        ret |= opcodecb(diza,inst);
    }
    return ret;
}


