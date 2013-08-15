#ifndef DIZA_OPCODE_HEADER
#define DIZA_OPCODE_HEADER


// The flag returned by a Diza opcode's callback
enum dizaflag
{
    DIZA_STOP=0x1,    // Error in the Diza'embling
    DIZA_DONE=0x1<<1
};

// The flag associated with an opcode
enum opcodeflag
{
    FLAG_ADDR1=0x1,      // 
    FLAG_ADDR2=0x1<<1,
    FLAG_ADDR4=0x1<<2,
    FLAG_ADDR67=0x1<<3,
    FLAG_DATA1=0x1<<4,
    FLAG_DATA2=0x1<<5,
    FLAG_DATA4=0x1<<6,
    FLAG_DATA66=0x1<<7,
    FLAG_64bit=0x1<<8,         // 64bit opcode
    FLAG_SEG=0x1<<9,
    FLAG_REP=0x1<<10,
    FLAG_66=0x1<<11,
    FLAG_67=0x1<<12,
    FLAG_ERROR=0x1<<13,
    FLAG_BAS=0x1<<14,
    FLAG_BAD=0x1<<15,
    FLAG_OPCODE2=0x1<<16,
    FLAG_REL=0x1<<17,
    FLAG_STOP=0x1<<18,
    FLAG_MODRM=0x1<<19,
    FLAG_LOCK=0x1<<20,
    FLAG_SIB=0x1<<21
};

struct Instruction
{
    unsigned char defaddr; // addr user defined size (4 for 32bit, 8 for 64bit)
    unsigned char defdata; // data user defined size in bytes
    unsigned long len;     // total length
    unsigned int flag;     // flags
    unsigned long addrsize;
    unsigned long datasize;
    unsigned char rep; // repeat prefix
    unsigned char seg; // segment prefix
    unsigned char opcode[4]; // opcodes
    unsigned char opcodecnt; // opcodes count
    unsigned char modrm;   // modrm byte
    unsigned char sib;     // sib byte
    union
    {
        unsigned char addr_b[8];
        unsigned short addr_w[4];
        unsigned long addr_d[2];
        char addr_c[8];
        short addr_s[4];
        long addr_l[2];
    };
    union
    {
        unsigned char data_b[8];
        unsigned short data_w[4];
        unsigned long data_d[2];
        char data_c[8];
        short data_s[4];
        long data_l[2];
    };
};


///////// DIZA.H //////////////////////////////////:

// The flag set for the disassembly engine

struct Diza
{
    int flag;
    unsigned char *buf;
    unsigned long buflen;
    unsigned long index;
};

int disassemble(Diza *, Instruction *);
int assemble(Diza *, Instruction *);

// The different callback
int prefixcb(Diza *, Instruction *);
// The opcode callback
int opcodecb(Diza *, Instruction *);
// The modrm callback
int modrmcb(Diza *diza, Instruction *inst);
// The sib callback
int sibcb(Diza *, Instruction *inst);
// The addr callback
int addrcb(Diza *diza, Instruction *inst);
// The data callback
int datacb(Diza *diza, Instruction *inst);


//////////// TABLE.H //////////////////////////////:

extern unsigned int table[512];


#endif //OPCODE_HEADER
