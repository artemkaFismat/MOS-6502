/* artemka_fismat 30-May-25. (c) 6205*/

#include <stdio.h>
#include <stdint.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define MAX_MEM_SIZE 1024 * 64

#define LDA_IM  0xA9
#define LDA_ZP 0xA5

typedef struct {;
	u8 data[MAX_MEM_SIZE];
} MEM;

typedef struct {
	u16 pc, sp, ps;
	u8 A, X, Y;

	u8 C : 1; 
	u8 Z : 1;        
	u8 I : 1;	
	u8 D : 1;	 
	u8 V : 1;
	u8 N : 1;
} CPU;


void memInit(MEM* mem) {
	for (size_t i = 0; i < sizeof(mem -> data); ++i) mem -> data[i] = 0;
}

void cpuReset(CPU* cpu, MEM* mem) {
	cpu -> pc = 0xFFFC;
	cpu -> sp = 0x0100;

	cpu -> A = 0;
	cpu -> X = 0;
	cpu -> Y = 0;

	cpu -> C = 0;
	cpu -> Z = 0;
	cpu -> I = 0;
	cpu -> D = 0;
	cpu -> V = 0;
	cpu -> N = 0;

	memInit(mem);
}

void LDA_SET_STATUS(CPU* cpu) {cpu -> Z = (cpu -> A == 0); cpu -> N = (cpu -> A & 0b100000000) > 0;}

u8 operator(MEM* mem, u32 addr) {
	return mem -> data[addr];
}

u8 fetchU8(u32* memCycl, MEM* mem, CPU* cpu) {
	u8 data = mem -> data[cpu -> pc];
	++cpu -> pc;
	--*memCycl;
	return data;
}

u8 readU8(u8* zpa, MEM* mem, CPU* cpu) {
	u8 data = mem -> data[cpu -> pc];
	--*zpa;
	return data;
}

void executeMemory(u32* memCycl, MEM* mem, CPU* cpu) {
	while (*memCycl > 0) {
		u8 ins = fetchU8(memCycl, mem, cpu);
		switch (ins) {
			case LDA_IM:
				u8 val = fetchU8(memCycl, mem, cpu);
				cpu -> A = val;
				LDA_SET_STATUS(cpu);
				break;
			case LDA_ZP:
				u8 zpa = fetchU8(memCycl, mem, cpu);
				u8* uzpa;
				uzpa = &zpa;
				cpu -> A = readU8(uzpa, mem, cpu);
				LDA_SET_STATUS(cpu);
				break;
			default:
				printf("%s\n", "instruction error");
				break;
		}
	}
}


int main(void) {
	MEM mem;
	CPU cpu;
	cpuReset(&cpu, &mem);
	// mem_init(&mem);
	mem.data[0xFFFC] = LDA_IM;
	mem.data[0xFFFD] = LDA_ZP;
	u32 cl = 2;
	executeMemory(&cl, &mem, &cpu);

	// for (int i = 0; i < sizeof(mem.data); ++i) printf("%d", mem.data[i]);
	return 0;
}
