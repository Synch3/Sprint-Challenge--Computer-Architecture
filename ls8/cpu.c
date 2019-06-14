#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>


/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->PC = 0;
  memset(cpu->registers, 0, sizeof(cpu->registers));
  memset(cpu->ram, 0, sizeof(cpu->ram));
  memset(cpu->FL, 0, sizeof(cpu->FL));
  
}
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char MAR)
{
  return cpu->ram[MAR];
}
void cpu_ram_write(struct cpu *cpu, int MAR, unsigned char value)
{
  return cpu->ram[MAR] = value;
}
void cpu_load(struct cpu *cpu,int argc, char *argv[])
{
  FILE *fp;
  int index = 0;
  int address = 0;
  char line[256];
  if(!argv[1]){
    puts("ERROR: MISSING FILE TO RUN");
    exit(1);
  }
  fp = fopen(argv[1],"r");
  printf("Reading file %s...\n\n", argv[1]);
  if(fp!=NULL){
    while(fgets(line,sizeof(line),fp)!= NULL){
      char *endptr;
      unsigned char byte = strtoul(line, &endptr,2);
      if(endptr == line){
        continue;
      }
      cpu->ram[address++] = byte;
    }
  }
  fclose(fp);
  // TODO: Replace this with something less hard-coded
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      cpu->registers[regA] *= cpu->registers[regB];
      break;
    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1;
  unsigned char command, operandA,operandB;
  int stack[256];
  int stack_pointer = 0;
  char return_addr;
  unsigned char address;
  while (running) {
    command = cpu_ram_read(cpu, cpu->PC);
    operandA = cpu_ram_read(cpu, cpu->PC + 1);
    operandB = cpu_ram_read(cpu, cpu->PC + 2);
    switch(command)
    {
      case HLT:
        running = 0;
        break;
      case LDI:
        cpu->registers[operandA] = operandB;
        cpu->PC += 3;
        break;
      case PRN:
        printf("%d\n", cpu->registers[operandA]);
        cpu->PC += 2;
        break;
      case MUL:
        alu(cpu, ALU_MUL,operandA, operandB);
        cpu->PC += 3;
        break;
      case PUSH:
        stack_pointer -= 1;
        stack[stack_pointer] = cpu->registers[operandA];
        cpu->PC += 2;
        break;
      case POP:
        cpu->registers[operandA] = stack[stack_pointer];
        stack_pointer += 1;
        cpu->PC += 2; 
        break;
      case CALL:
          cpu->registers[7]--;
          cpu_ram_write(cpu, cpu->registers[7],cpu->PC + 2);
          cpu->PC = cpu->registers[operandA];
          break;
      case RET:
          address = cpu_ram_read(cpu, cpu->registers[7]);
          cpu->registers[7]++;
          cpu->PC = address;
          break;
      case ADD:
        cpu->registers[operandA] += operandB;
        cpu->PC += 3;
        break;
      case JMP:
        cpu->PC = cpu->registers[operandA]; 
        break;
      case CMP:
        if(cpu->registers[operandA] == cpu->registers[operandB]){
          cpu->FL[7] = 1;
        }else if(cpu->registers[operandA] > cpu->registers[operandB]){
          cpu->FL[6] = 1;
        }else{
          cpu->FL[5] = 1;
        }
        cpu->PC += 3;
        break;
      case JEQ:
        if(cpu->FL[7] ==1){
          cpu->PC = cpu->registers[operandA]; 
        }
        else{
          cpu->PC+=2;
        }
        break;
      case JNE:
        if(cpu->FL[7] ==0){
          cpu->PC=cpu->registers[operandA];
        }else{
          cpu->PC += 2; 
        } 
        break;
    }
    
  }
}
