#include "opcode.h"
#include <stdio.h>

unsigned char buf[]=

    "\x68\x62\x2e\x2e\x2e" /* pushl   $0x2e2e2e62            */
    "\x89\xe7"             /* movl    %esp,%edi              */
    "\x33\xc0"             /* xorl    %eax,%eax              */
    "\x88\x47\x03"         /* movb    %al,0x3(%edi)          */
    "\x57"                 /* pushl   %edi                   */
    "\xb0\x88"             /* movb    $0x88,%al              */
    "\x50"                 /* pushl   %eax                   */
    "\xcd\x80"             /* int     $0x80                  */
    "\x57"                 /* pushl   %edi                   */
    "\xb0\x3d"             /* movb    $0x3d,%al              */
    "\x50"                 /* pushl   %eax                   */
    "\xcd\x80"             /* int     $0x80                  */
    "\x47"                 /* incl    %edi                   */
    "\x33\xc9"             /* xorl    %ecx,%ecx              */
    "\xb1\xff"             /* movb    $0xff,%cl              */
    "\x57"                 /* pushl   %edi                   */
    "\x50"                 /* pushl   %eax                   */
    "\xb0\x0c"             /* movb    $0x0c,%al              */
    "\xcd\x80"             /* int     $0x80                  */
    "\xe2\xfa"             /* loop    <shellcode +31>        */
    "\x47"                 /* incl    %edi                   */
    "\x57"                 /* pushl   %edi                   */
    "\xb0\x3d"             /* movb    $0x3d,%al              */
    "\x50"                 /* pushl   %eax                   */
    "\xcd\x80";            /* int     $0x80                  */


int main(int argc, char *argv[])
{
    Diza diza = {0};
    Instruction inst = {0};
    unsigned int size;
    unsigned int i;

    i=0;
    diza.buf=buf;
    diza.buflen=sizeof(buf);

    while(diza.index < diza.buflen)
    {
        inst.defaddr=4;
        inst.defdata=4;
        inst.flag=0;
        inst.opcodecnt=0;
        diza.flag=0;
        

        size=disassemble(&diza,&inst);

        size+=i;

        for(;i<size;i++)
        {
            printf(" %02X", diza.buf[i]);
        }
        printf("\n");
    }
    return 0;
}
