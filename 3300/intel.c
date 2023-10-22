#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define INPUT_WORD_LIMIT 255

#define NUM_REGS 32
#define MEMORY_SIZE 262144 // 1MiB / 32 (word size) = num of words = mem_size

int count;
int reg[NUM_REGS] = {0};   // 32 general-purpose registers
int mem[MEMORY_SIZE];   // 1 MiB memory space
int ip = 0;
int cc = 0;
int xip = 0;
int fip = 0;
int halt_flag = 0;
int verbose = 0;
int trace = 0;
int inst_fetches = 0;
int memory_reads = 0;
int memory_writes = 0;
int branches = 0;
int taken_branches = 0;
int ir;
int op;
int src1, src2, dest;
int immed_16_bit, offset_16_bit, immed_5_bit, offset_26_bit;

// TODO: Implement other instruction functions

#define INPUT_WORD_LIMIT 255
void get_mem(){
  int w, count = 0;

  if( verbose > 1 ) printf( "reading words in hex from stdin:\n" );
  while( scanf( "%x", &w ) != EOF ){
    if( verbose > 1 ) printf( "  0%08x\n", w );
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
  if( verbose ) printf( "  read access at address %x\n", eff_addr );  reg[ reg_index ] = mem[ word_addr ];
  memory_reads++;
}

void write_mem( int eff_addr, int reg_index ){
  int word_addr = eff_addr >> 2;
  if( verbose ) printf( "  write access at address %x\n", eff_addr );
  mem[ word_addr ] = reg[ reg_index ];
  memory_writes++;
}

void decode() {
int op;
    if(cc == 1) op = ir >> 26;


}

int main( int argc, char **argv ){

  if( argc > 1 ){
    if( ( argv[1][0] == '-' ) && ( argv[1][1] == 't' ) ){
      verbose = 1;
    }else if( ( argv[1][0] == '-' ) && ( argv[1][1] == 'v' ) ){
      verbose = 2;
    }else{
      printf( "usage:\n");
      printf( "  %s for just execution statistics\n", argv[0] );
      printf( "  %s -t for instruction trace\n", argv[0] );
      printf( "  %s -v for instructions, registers, and memory\n", argv[0] );
      printf( "input is read as hex 32-bit values from stdin\n" );
      exit( -1 );
    }
  }

  get_mem();

  if( verbose ) printf( "instruction trace:\n" );
  while( !halt_flag ){

    if( verbose ) printf( "at %02x, ", fip );
    ir = mem[ fip >> 2 ];  /* adjust for word addressing of mem[] */
    xip = fip;
    fip = xip + 4;
    inst_fetches++;

    decode();

    

    reg[ 0 ] = 0;  /* make sure that r0 stays 0 */

    if( ( verbose > 1 ) || ( halt_flag && ( verbose == 1 )) ){
      for( int i = 0; i < 8 ; i++ ){
        printf( "  r%x: %08x", i , reg[ i ] );
        printf( "  r%x: %08x", i + 8 , reg[ i + 8 ] );
        printf( "  r%x: %08x", i + 16, reg[ i + 16 ] );
        printf( "  r%x: %08x\n", i + 24, reg[ i + 24 ] );
      }
    }
  }

  if( verbose ) printf( "\n" );
  printf( "execution statistics (in decimal):\n" );
  printf( "  instruction fetches = %d\n", inst_fetches );
  printf( "  data words read     = %d\n", memory_reads );
  printf( "  data words written  = %d\n", memory_writes );
  printf( "  branches executed   = %d\n", branches );
  if( taken_branches == 0 ){
    printf( "  branches taken      = 0\n" );
  }else{
    printf( "  branches taken      = %d (%.1f%%)\n",
      taken_branches, 100.0*((float)taken_branches)/((float)branches) );
  }
  return 0;
}
