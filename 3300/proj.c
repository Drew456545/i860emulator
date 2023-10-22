/* behavioral simulation of MC88100 subset for CPSC 3300 at Clemson
*
* reference manual is http://www.bitsavers.org/components/motorola/
* 88000/MC88100_RISC_Microprocessor_Users_Manual_2ed_1990.pdf
*
* processor state subset
* 32 x 32-bit general registers
* note that r0 is always 0
* two 32-bit instruction address registers
* fip - fetch address pointer
* xip - execute address pointer
*
* memory
* byte-addressable
* big-endian addressing
* aligned accesses
* - an instruction starts on an address that is a multiple of 4
* - a data word starts on an address that is a multiple of 4
*
* note - we limit this simulation to a 1 MiB memory
*
* note - instructions are one word (four bytes) in length and
* we also limit the instruction subset in this simulation to
* operations on on word-length data
*
* we implement all three addressing modes, see pages 3-7 to 3-10
*
* register indirect with zero-extended immediate index
* eff_addr = reg[ s1 ] + imm16
*
* register indirect with index
* eff_addr = reg[ s1 ] + reg[ s2 ]
*
* register indirect with scaled index (word scaling = 2)
* eff_addr = reg[ s1 ] + ( reg[ s2 ] << 2 )
*
* we implement 20 instructions derived from 12 base instructions
*
* halt is added for the simulation
* add is described on pages 3-29 to 3-30
* bcnd is described on pages 3-13 to 3-14 and 3-35 to 3-36
* br is described on pages 3-16 and 3-37
* ext is described on pages 3-44 to 3-45
* extu is described on pages 3-46 to 3-47
* mak is described on pages 3-70 to 3-71
* rot is described on page 3-76
* ld is described on pages 3-65 to 3-66
* lda is described on pages 3-67 to 3-68
* st is described on pages 3-79 to 3-80
* sub is described on pages 3-82 to 3-83
*
* decoding and instruction formats (op1 is first six bits)
*
* op1 = 0 => halt
*
* op1 = 0x05, 0x09, 0x0d, 0x1c, 0x1d => 0101 
* opcodes are ld, st, lda, add, sub, respectively
* format has two registers and a 16-bit immediate
* immediate value is zero-extended
* signed words in normal mode used for load/stores,
* so p = 01, ty = 01, and u = 0
* carry and borrow are not used, so i = 0 and o = 0
*
* op1 = 0x30 => br
* format has a single 26-bit displacement
* displacement is sign-extended
* displacement is in words and calculated from the
* address of the current instruction rather than
*
* op1 = 0x3a => bcnd
* format has mask, register, and 16-bit displacement
* displacement is sign-extended
* displacement is in words and calculated from the
* address of the current instruction rather than
* the updated fetch address
* assert checks that displacement is non-zero
* delayed branching is not used, so n = 0
*
* op1 = 0x3c => ext, extu, mak, rot
* format has two registers and a 5-bit immediate
* ext, extu, and mak are used as shifts so w5 = 0
* the updated fetch address
* assert checks that displacement is non-zero
* delayed branching is not used, so n = 0
*
* op1 = 0x3d => ld, st, lda, add, sub
* format has three registers
* for add and sub:
* carry and borrow are not used, so i = 0 and o = 0
* for ld, sta, and lda:
* signed words in normal mode used for load/stores,
* so p = 01, ty = 01, and u = 0
* if bit 9 = 1, the third register is scaled
*/
/* behavioral simulation of MC88100 subset for CPSC 3300 at Clemson
*
* reference manual is http://www.bitsavers.org/components/motorola/
* 88000/MC88100_RISC_Microprocessor_Users_Manual_2ed_1990.pdf
*
* processor state subset
* 32 x 32-bit general registers
* note that r0 is always 0
* two 32-bit instruction address registers
* fip - fetch address pointer
* xip - execute address pointer
*
* memory
* byte-addressable
* big-endian addressing
* aligned accesses
* - an instruction starts on an address that is a multiple of 4
* - a data word starts on an address that is a multiple of 4
*
* note - we limit this simulation to a 1 MiB memory
*
* note - instructions are one word (four bytes) in length and
* we also limit the instruction subset in this simulation to
* operations on on word-length data
*
* we implement all three addressing modes, see pages 3-7 to 3-10
*
* register indirect with zero-extended immediate index
* eff_addr = reg[ s1 ] + imm16
*
* register indirect with index
* eff_addr = reg[ s1 ] + reg[ s2 ]
*
* register indirect with scaled index (word scaling = 2)
* eff_addr = reg[ s1 ] + ( reg[ s2 ] << 2 )
*
* we implement 20 instructions derived from 12 base instructions
*
* halt is added for the simulation
* add is described on pages 3-29 to 3-30
* bcnd is described on pages 3-13 to 3-14 and 3-35 to 3-36
* br is described on pages 3-16 and 3-37
* ext is described on pages 3-44 to 3-45
* extu is described on pages 3-46 to 3-47
* mak is described on pages 3-70 to 3-71
* rot is described on page 3-76
* ld is described on pages 3-65 to 3-66
* lda is described on pages 3-67 to 3-68
* st is described on pages 3-79 to 3-80
* sub is described on pages 3-82 to 3-83
*
* decoding and instruction formats (op1 is first six bits)
*
* op1 = 0 => halt
*
* op1 = 0x05, 0x09, 0x0d, 0x1c, 0x1d => 0101 
* opcodes are ld, st, lda, add, sub, respectively
* format has two registers and a 16-bit immediate
* immediate value is zero-extended
* signed words in normal mode used for load/stores,
* so p = 01, ty = 01, and u = 0
* carry and borrow are not used, so i = 0 and o = 0
*
* op1 = 0x30 => br
* format has a single 26-bit displacement
* displacement is sign-extended
* displacement is in words and calculated from the
* address of the current instruction rather than
*
* op1 = 0x3a => bcnd
* format has mask, register, and 16-bit displacement
* displacement is sign-extended
* displacement is in words and calculated from the
* address of the current instruction rather than
* the updated fetch address
* assert checks that displacement is non-zero
* delayed branching is not used, so n = 0
*
* op1 = 0x3c => ext, extu, mak, rot
* format has two registers and a 5-bit immediate
* ext, extu, and mak are used as shifts so w5 = 0
* the updated fetch address
* assert checks that displacement is non-zero
* delayed branching is not used, so n = 0
*
* op1 = 0x3d => ld, st, lda, add, sub
* format has three registers
* for add and sub:
* carry and borrow are not used, so i = 0 and o = 0
* for ld, sta, and lda:
* signed words in normal mode used for load/stores,
* so p = 01, ty = 01, and u = 0
* if bit 9 = 1, the third register is scaled
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

/* since the simulation deals only with one-word instructions and */
/* one-word operands, we represent memory as an array of words */
#define MEM_SIZE_IN_WORDS 256*1024
int mem[MEM_SIZE_IN_WORDS];
/* processor state, simulation state, and instruction fields */
int reg[32] = {0}, /* general register set, r0 is always 0 */
xip = 0, /* execute instruction pointer */
fip = 0, /* fetch instruction pointer */
halt_flag = 0, /* set by halt instruction */
verbose = 0, /* governs amount of detail in output */
ir, /* 32-bit instruction register */
op1, /* 6-bit primary opcode in bits 31 to 27 */
op2, /* 6-bit secondary opcode in bits 15 to 10 */
d, /* 5-bit destination register identifier */
s1, /* 5-bit source 1 register identifier */
s2, /* 5-bit source 2 register identifier */
imm16, /* 16-bit immediate field */
scaled, /* scaled addressing mode bit 9 */
eff_addr; /* 32-bit effective address */
/* dynamic execution statistics */
int inst_fetches = 0,
memory_reads = 0,
memory_writes = 0,
branches = 0,
taken_branches = 0;
/* load memory from stdin */
#define INPUT_WORD_LIMIT 255
void get_mem(){
int w, count = 0;

if( verbose > 1 ) 
    printf( "reading words in hex from stdin:\n" );

while( scanf( "%x", &w ) != EOF ){
    if( verbose > 1 ) printf( " 0%08x\n", w );
    if( count > INPUT_WORD_LIMIT ){
        printf( "too many words loaded\n" );
        exit( 0 );
}

mem[ count ] = w;
count++;

}

if( verbose > 1 ) printf( "\n" );

}

void read_mem( int eff_addr, int reg_index ){
int word_addr = eff_addr >> 2;

if( verbose ) printf( " read access at address %x\n", eff_addr );

assert( ( word_addr >= 0 ) && ( word_addr < MEM_SIZE_IN_WORDS ) );

reg[ reg_index ] = mem[ word_addr ];

memory_reads++;
}

void write_mem( int eff_addr, int reg_index ){

int word_addr = eff_addr >> 2;

if( verbose ) printf( " write access at address %x\n", eff_addr );

assert( ( word_addr >= 0 ) && ( word_addr < MEM_SIZE_IN_WORDS ) );

mem[ word_addr ] = reg[ reg_index ];
memory_writes++;

}
/* extract fields - switch statements are in main loop */
void decode(){
op1 = ( ir >> 26 ) & 0x3f;
op2 = ( ir >> 10 ) & 0x3f;
d = ( ir >> 21 ) & 0x1f;
s1 = ( ir >> 16 ) & 0x1f;
s2 = ir & 0x1f;
imm16 = ir & 0xffff;
scaled = ( ir >> 9 ) & 1;
}

void halt(){
if( verbose ) 
    printf( "halt\n" );
halt_flag = 1;
}

void imm_ld(){ /* pages 3-65 to 3-66 */
if( verbose ) 
    printf( "ld r%x,r%x,%x\n", d, s1, imm16 );
eff_addr = reg[ s1 ] + imm16; /* zero extended */
read_mem( eff_addr, d );
}

void imm_st(){ /* pages 3-79 to 3-80 */
if( verbose ) 
    printf( "st r%x,r%x,%x\n", d, s1, imm16 );
eff_addr = reg[ s1 ] + imm16; /* zero extended */
write_mem( eff_addr, d );
}

void imm_lda(){ /* pages 3-67 to 3-68 */
if( verbose ) printf( "lda r%x,r%x,%x\n", d, s1, imm16 );
reg[ d ] = reg[ s1 ] + imm16; /* zero extended */
}

void imm_add(){ /* carry not used; pages 3-29 to 3-30 */
if( verbose ) printf( "add r%x,r%x,%x\n", d, s1, imm16 );
reg[ d ] = reg[ s1 ] + imm16; /* zero extended */
}

void imm_sub(){ /* borrow not used; pages 3-82 to 3-83 */
if( verbose ) printf( "sub r%x,r%x,%x\n", d, s1, imm16 );
reg[ d ] = reg[ s1 ] - imm16; /* zero extended */
}

void br(){ /* n = 0; * pages 3-16 and 3-37 */
int d26 = ir & 0x03ffffff; /* branch displacement field */
assert( d26 != 0 );

if( verbose ) printf( "br %x", d26 );
    d26 = d26 << 6; /* sign extend */
    d26 = d26 >> 6;
    fip = xip + ( d26 << 2 );

if( verbose ){
    if( ( d26 < 0 ) || ( d26 > 9 ) ){
        printf( " (= decimal %d)\n", d26 );
    }
else{
    printf( "\n" );
}
}

branches++;
taken_branches++;
}
void bcnd(){ /* n = 0; pages 3-13 to 3-14 and 3-35 to 3-36 */
    int max_neg = 0, /* bit 24 in instruction format */
    lt0msc = 0, /* bit 23 (note: minus special case of max neg) */
    eq0 = 0, /* bit 22 */
    gt0 = 0, /* bit 21 */
    taken = 0; /* logical flag to determine if branch is taken */
    assert( imm16 != 0 );
if( verbose ){
    printf( "bcnd " );
    switch( d ){
    case 0x0: printf( "never" ); break;
    case 0x1: printf( "gt0" ); break;
    case 0x2: printf( "eq0" ); break;
    case 0x3: printf( "ge0" ); break;
    case 0xc: printf( "lt0" ); break;
    case 0xd: printf( "ne0" ); break;
    case 0xe: printf( "le0" ); break;
    case 0xf: printf( "always" ); break;
    default: printf( "mask=%x", d );
}
printf( ",r%x,%x", s1, imm16 );
}
if( reg[ s1 ] == 0x80000000 ) max_neg = 1;

if( ( reg[ s1 ] < 0 ) && !max_neg ) lt0msc = 1;

if( reg[ s1 ] == 0 ) eq0 = 1;

if( reg[ s1 ] > 0 ) gt0 = 1;

if( max_neg && ( d & 0x8 ) ) taken = 1;

if( lt0msc && ( d & 0x4 ) ) taken = 1;

if( eq0 && ( d & 0x2 ) ) taken = 1;

if( gt0 && ( d & 0x1 ) ) taken = 1;
// printf(" number = %08x results = %x%x%x%x taken = %x\n",
// reg[s1],max_neg,lt0msc,eq0,gt0,taken);
branches++;
int d16 = imm16; /* 16-bit displacement */
d16 = d16 << 16; /* sign extend */
d16 = d16 >> 16;

if( verbose ){
if( ( d16 < 0 ) || ( d16 > 9 ) ){
    printf( " (= decimal %d)\n", d16 );
}
else{
    printf( "\n" );
}
}
if( taken ){
    fip = xip + ( d16 << 2 );
    taken_branches++;
}
}
void ext(){ /* immediate form, w5 = 0: pages 3-25 and 3-46 */
    if( verbose ) 
        printf( "ext r%x,r%x,%x\n", d, s1, s2 );
    reg[ d ] = reg[ s1 ] >> s2; /* s2 is o5 shift count field */
}
void extu(){ /* immediate form, w5 = 0: pages 3-25 and 3-47 */
    unsigned value; /* unsigned data type to get logical shift in C */
    if( verbose ) 
        printf( "extu r%x,r%x,%x\n", d, s1, s2 );

    value = reg[ s1 ];
    value = value >> s2; /* s2 is o5 shift count field */
    reg[ d ] = value;
}
void mak(){ /* immediate form, w5 = 0: pages 3-26 and 3-70 to 3-71 */
    if( verbose ) 
        printf( "mak r%x,r%x,%x\n", d, s1, s2 );

    reg[ d ] = reg[ s1 ] << s2; /* s2 is o5 shift count field */
}
/* for rotate
*
* (32-n) bits n bits
* +-------------+-------+
* | A | B |
* +-------------+-------+
*
* value 1 = AB shift left (32-n) bits
* +-------+-------------+
* | B | 0 |
* +-------+-------------+
*
* value 2 = AB logical shift right n bits
* +-------+-------------+
* | 0 | A |
* +-------+-------------+
*
* now or the two values together
* +-------+-------------+
* | B | A |
* +-------+-------------+
*/
void rot(){ /* to the right, immediate form; pages 3-26 and 3-76 */
    unsigned value1, value2; /* unsigned data type for logical shift */
    if( verbose ) printf( "rot r%x,r%x,%x\n", d, s1, s2 );
        value1 = value2 = reg[ s1 ];
        value1 = value1 << ( 32 - s2 ); /* s2 is o5 rotate count field */
        value2 = value2 >> s2;
        reg[ d ] = value1 | value2;
}   

void ld(){ /* pages 3-65 to 3-66     WTF IS THIS*/
if( scaled ){
if( verbose ) printf( "ld r%x,r%x[r%x]\n", d, s1, s2 );
eff_addr = reg[ s1 ] + ( reg[ s2 ] << 2 );
}else{
if( verbose ) printf( "ld r%x,r%x,r%x\n", d, s1, s2 );
eff_addr = reg[ s1 ] + reg[ s2 ];
}
read_mem( eff_addr, d );
}

void st(){ /* pages 3-79 to 3-80 */
if( scaled ){
if( verbose ) printf( "st r%x,r%x[r%x]\n", d, s1, s2 );
eff_addr = reg[ s1 ] + ( reg[ s2 ] << 2 );
}else{
if( verbose ) printf( "st r%x,r%x,r%x\n", d, s1, s2 );
eff_addr = reg[ s1 ] + reg[ s2 ];
}
write_mem( eff_addr, d );
}

void lda(){ /* pages 3-67 to 3-68 */
if( scaled ){
if( verbose ) printf( "lda r%x,r%x[r%x]\n", d, s1, s2 );
reg[ d ] = reg[ s1 ] + ( reg[ s2 ] << 2 );
}else{
if( verbose ) printf( "lda r%x,r%x,r%x\n", d, s1, s2 );
reg[ d ] = reg[ s1 ] + reg[ s2 ];
}
}

void add(){ /* carry not used; pages 3-29 to 3-30 */
if( verbose ) printf( "add r%x,r%x,r%x\n", d, s1, s2 );
reg[ d ] = reg[ s1 ] + reg[ s2 ];
}

void sub(){ /* borrow not used; pages 3-82 to 3-83 */
if( verbose ) printf( "sub r%x,r%x,r%x\n", d, s1, s2 );
reg[ d ] = reg[ s1 ] - reg[ s2 ];
}

void unknown_op(){
printf( "unknown instruction %08x\n", ir );
printf( " op1=%x", op1 );
printf( " op2=%x", op2 );
printf( " d=%x", d );
printf( " s1=%x", s1 );
printf( " s2=%x\n", s2 );
printf( "program terminates\n" );
exit( -1 );
}
//
int main( int argc, char **argv ){
if( argc > 1 ){
if( ( argv[1][0] == '-' ) && ( argv[1][1] == 't' ) ){
verbose = 1;
}else if( ( argv[1][0] == '-' ) && ( argv[1][1] == 'v' ) ){
verbose = 2;
}else{
printf( "usage:\n");
printf( " %s for just execution statistics\n", argv[0] );
printf( " %s -t for instruction trace\n", argv[0] );
printf( " %s -v for instructions, registers, and memory\n", argv[0] );
printf( "input is read as hex 32-bit values from stdin\n" );
exit( -1 );
}
}
get_mem();
if( verbose ) printf( "instruction trace:\n" );
while( !halt_flag ){
if( verbose ) printf( "at %02x, ", fip );
ir = mem[ fip >> 2 ]; /* adjust for word addressing of mem[] */
xip = fip;
fip = xip + 4;
inst_fetches++;
decode();
switch( op1 ){
case 0x00: halt(); break;
case 0x05: imm_ld(); break;
case 0x09: imm_st(); break;
case 0x0d: imm_lda(); break;
case 0x1c: imm_add(); break;
case 0x1d: imm_sub(); break;
case 0x30: br(); break;
case 0x3a: bcnd(); break;
case 0x3c:
switch( op2 ){
case 0x24: ext(); break;
case 0x26: extu(); break;
case 0x28: mak(); break;
case 0x2a: rot(); break;
default: unknown_op();
}
break;
case 0x3d:
switch( op2 ){
case 0x05: ld(); break;
case 0x09: st(); break;
case 0x0d: lda(); break;
case 0x1c: add(); break;
case 0x1d: sub(); break;
default: unknown_op();
}
break;
default: unknown_op();
}
reg[ 0 ] = 0; /* make sure that r0 stays 0 */
if( ( verbose > 1 ) || ( halt_flag && ( verbose == 1 )) ){
for( int i = 0; i < 8 ; i++ ){
printf( " r%x: %08x", i , reg[ i ] );
printf( " r%x: %08x", i + 8 , reg[ i + 8 ] );
printf( " r%x: %08x", i + 16, reg[ i + 16 ] );
printf( " r%x: %08x\n", i + 24, reg[ i + 24 ] );
}
}
}
if( verbose ) printf( "\n" );
printf( "execution statistics (in decimal):\n" );
printf( " instruction fetches = %d\n", inst_fetches );
printf( " data words read = %d\n", memory_reads );
printf( " data words written = %d\n", memory_writes );
printf( " branches executed = %d\n", branches );
if( taken_branches == 0 ){
printf( " branches taken = 0\n" );
}else{
printf( " branches taken = %d (%.1f%%)\n",
taken_branches, 100.0*((float)taken_branches)/((float)branches) );
}
return 0;
}
