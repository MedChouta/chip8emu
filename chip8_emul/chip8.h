#ifndef CHIP8
#define CHIP8

#include <stdint.h>

#define MEMSIZE 4096 //Memory size
#define STACKSIZE 16 //Stack size
#define REG 16 //Number of registers


#define A 10
#define B 11
#define C 12
#define D 13
#define E 14
#define F 15


struct Memory {
    uint8_t progSpace[MEMSIZE]; 
    uint8_t V[REG]; //Registers
    uint8_t SP; //Stack Pointer
    uint8_t DT; //Delay timer register
    uint8_t ST; //Sound timer register

    uint16_t I; //Register that stores memory addresses
    uint16_t PC; //Program counter or instruction pointer
    uint16_t stack[STACKSIZE];
};

uint16_t pop(uint16_t stack[]);

#endif