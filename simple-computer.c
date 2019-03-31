// This code is created and owned by professor Shao Zili
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_MEM_SIZE  64  //The max memory size - 64 words (256 bytes)

typedef struct memory{
	uint32_t addr[MAX_MEM_SIZE];
}MEMORY;

typedef struct cpu{
	//Control registers
	uint32_t PC;	//Program counter
	uint32_t IR; 	//Instruction regiser
	uint32_t PSR;	//Processor Status Register

	//General purpose register
	int R[4];	// 4 Registers 
}CPU;

typedef struct computer{
	CPU cpu ;
	MEMORY memory;
}COMPUTER;


enum {halt, NOP, addi, move_reg, movei, lw, sw, blez};

int computer_load_init(COMPUTER *, char *);
int cpu_cycle(COMPUTER *);
int fetch(COMPUTER * );

int print_cpu(COMPUTER * );
int print_memory(COMPUTER * );
int print_instruction(int, uint32_t );
int decode(uint32_t , uint8_t * , uint8_t * , uint8_t * , int8_t * );
int execute(COMPUTER *, uint8_t *, uint8_t *, uint8_t *, int8_t *);



int main(int argc, char ** args)
{

	printf("----------------------------------------------------------------\n");
	printf("|Simple von Neumann Computer for CSCI 3150 (Zili Shao@CSE,CUHK)|\n");
	printf("----------------------------------------------------------------\n");
	
	if( argc != 3 ){
		printf("\nUsage: ./cpu program start_addr\n");
		printf("\t program: program file name; start_addr: the start address for initial PC\n \n");
		exit(-1);
	}

	COMPUTER comp;

	//Initialize: Load the program into the memory, and initialize all regisrters;
	if ( computer_load_init(&comp, args[1]) < 0 ){    
		printf("Error: computer_poweron_init()\n");
		exit(-1);
	}

	//Set PC and start the cpu execution cycle
	comp.cpu.PC = atoi(args[2]);
	if( comp.cpu.PC >= MAX_MEM_SIZE || comp.cpu.PC < 0){
		printf("Error: start_addr should be in 0-63.\n");
		exit(-1);
	}   


	// Execute CPU cyles: fetch, decode, execution, and increment PC; Repeat
	while(1){
		printf("\n\nBefore\n");
		print_cpu(&comp);

		if( cpu_cycle(&comp) < 0 )
			break;

		printf("After\n");
		print_cpu(&comp);
	} 

  	return 0;
}

int cpu_cycle(COMPUTER * cp)
{
	uint8_t opcode, sreg, treg;
	int8_t  immediate;

	if( fetch(cp) < 0)
		return -1;

	if ( decode(cp->cpu.IR, &opcode, &sreg, &treg, &immediate) < 0)
		return -1;

	if( execute(cp, &opcode, &sreg, &treg, &immediate) < 0 )
		return -1;

	return 0;
}

int fetch(COMPUTER * cp)
{
	uint32_t pc = cp->cpu.PC;

	if (pc < 0 || pc > 63 ){
		printf("PC is not in 0-63.\n");
		return -1;
	}else{
		cp->cpu.IR = cp->memory.addr[pc];
		return 0;
	}
}

int decode(uint32_t instr, uint8_t * p_opcode, uint8_t * p_sreg, uint8_t * p_treg, int8_t * p_imm)
{
	*p_opcode = (instr & 0xFF000000) >> 24;
	if (*p_opcode > blez) {
		printf("Invalid instruction\n");
		return -1;
	}

	*p_sreg = (instr & 0x00FF0000) >> 16;
	*p_treg = (instr & 0x0000FF00) >> 8;
	*p_imm = instr & 0x000000FF;

	return 0;
}

int execute(COMPUTER *cp, uint8_t * p_opcode, uint8_t * p_sreg, uint8_t * p_treg, int8_t * p_imm)
{	
	switch( * p_opcode){
                case halt:
                        // halt (0x00000000) Effect: Stop CPU and exit
                        return -1;
                case NOP:
                        // NOP  (0x01000000) Effect: PC <- PC+1
                        cp->cpu.PC +=1;
                        break;
                case addi:
                        //addi  (0x02xxyyzz) Effect: R[yy] <- R[xx] + zz; PC <- PC+1
                        cp->cpu.R[*p_treg] = cp->cpu.R[*p_sreg] + *p_imm;
                        cp->cpu.PC +=1;
                        break;
                case move_reg:
                        //move_reg (0x03xxyy00) Effect: R[yy] <- R[xx]; PC <- PC+1

                        /*your code*/

                        break;
                case movei:
                        //movei (0x0400yyzz) Effect: R[yy] <- zz; PC <- PC+1

                        /*your code*/

                        break;
                case lw:
                        //lw-load word: (0x05xxyyzz) Effect: R[yy] <- M{ R[xx] + zz}; PC <- PC+1

                        /*your code*/

                        break;
                case sw:
                        //sw-store word: (0x06xxyyzz) Effect: M{R[xx]+zz} <- R[yy]; PC <- PC+1 

                        /*your code*/

                        break;
                case blez:
                        //blez - Branch on less than or equal: (0x07xx00zz)
                        //Effect: if R[xx] == 0 PC <- PC + 1 + zz
                        //        else PC <- PC +1

                        /*your code*/

                        break;
                default:
                        printf("Illegal instruction\n");
                        return -1;

	}

	return 0;
}

int computer_load_init(COMPUTER * cp, char * file)
{
	//load the image file
	int fd;
	int ret;

	// open the file 
	if ( ( fd = open( file, O_RDONLY ) ) < 0 ){
		printf("Error: open().\n");
		exit(-1);
	}

	// read from the program file (the program file <= 256 bytes) into the memory
	if( ( ret = read( fd, &cp->memory, MAX_MEM_SIZE*4)) < 0 ){
		printf("Error: read().\n");
		exit(-1);
	}else if ( ret > (MAX_MEM_SIZE*4) ) {
		printf("Error: read() - Program is too big. \n");
		exit(-1);
	}  

	//Initialize all registers
	cp->cpu.PC=0;	//Program counter
	cp->cpu.IR=0; 	//Instruction regiser
	cp->cpu.PSR=0;	//Processor Status Register

	//General purpose register
	cp->cpu.R[0]=0;	// General register No. 0 
	cp->cpu.R[1]=0;	// General register No. 1 
	cp->cpu.R[2]=0;	// General register No. 2 
	cp->cpu.R[3]=0;	// General register No. 3

	return 0;
}

int print_cpu(COMPUTER * cp)
{
	printf("CPU Registers: PC-%d, IR-0x%x, PSR-0x%x, R[0]-%d, R[1]-%d, R[2]-%d, R[3]-%d\n",
	         	cp->cpu.PC, cp->cpu.IR, cp->cpu.PSR, cp->cpu.R[0], cp->cpu.R[1], cp->cpu.R[2], cp->cpu.R[3]);

	return 0;

}

int print_memory(COMPUTER * cp)
{
	//print the memory contents	
	int i;
       	for (i =0; i<64; i++){
               print_instruction(i, cp->memory.addr[i]);
	}

	return 0;
}

int print_instruction(int i, uint32_t inst)
{
	int8_t * p = (int8_t *) &inst;

	int8_t low_addr_value = *p;
	int8_t sec_addr_value = *(p+1);
	int8_t third_addr_value = *(p+2);
	int8_t high_addr_value = *(p+3);
	/* Intel32 is little endian - the least significant byte first (lowest address) and the most significant byte last (highest address) */ 
	printf("[%d]: Instruction-0x%x;LowAddr-%d,Second-%d,Third-%d,HighAddr-%d\n",
                    i, inst,low_addr_value, sec_addr_value, third_addr_value, high_addr_value); 
	return 0;
}
