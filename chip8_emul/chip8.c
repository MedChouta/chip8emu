#include <stdio.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include "chip8.h"


int main(int argc, char** argv){
    int i, j;
    char screen[64*32] = {" "};
/*
    SDL_Surface *window;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_SetVideoMode(
        1280,
        640,
        32,
        SDL_HWSURFACE
    );

    SDL_WM_SetCaption("CHIP-8", NULL);

    SDL_FillRect(window, NULL, SDL_MapRGB(window->format, 0,0,0));

    if (window == NULL){
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Rect r[64*32] = {0};
    SDL_Surface *surfRect[64*32] = {0};
    int j = 0;

    for(i = 0; i < 64*32; i++){
        if (i % 64 == 0){
            j++;
            printf("hh: %d\n", i);
        }
        r[i].w = 0;
        r[i].h = 0;
        r[i].x = 1280/64 * (i % 64);
        r[i].y = j * 640/32;

        surfRect[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, 1280/64, 640/32, 32, 0, 0, 0, 0);        
        //sleep(5);

    }
    */

    printf("J: %d", j);

    struct Memory memory; //RAM

    memory.PC = 0x200;
    memory.I = 0;
    memory.SP = 0;

    memory.DT = 0;
    memory.ST = 0;


    unsigned char Keyboard[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };


    unsigned char font[80] = {
        0xF0,0x90,0x90,0x90,0xF0, //0
        0x20,0x60,0x20,0x20,0x70, //1
        0xF0,0x10,0xF0,0x80,0xF0, //2
        0xF0,0x10,0xF0,0x10,0xF0, //3
        0x90,0x90,0xF0,0x10,0x10, //4
        0xF0,0x80,0xF0,0x10,0xF0, //5
        0xF0,0x80,0xF0,0x90,0xF0, //6
        0xF0,0x10,0x20,0x40,0x40, //7
        0xF0,0x90,0xF0,0x90,0xF0, //8
        0xF0,0x90,0xF0,0x10,0xF0, //9
        0xF0,0x90,0xF0,0x90,0x90, //A
        0xE0,0x90,0xE0,0x90,0xE0, //B
        0xF0,0x80,0x80,0x80,0xF0, //C
        0xE0,0x90,0x90,0x90,0xE0, //D
        0xF0,0x80,0xF0,0x80,0xF0, //E
        0xF0,0x80,0xF0,0x80,0x80  //F
    };


    for(i = 0; i < 80; i++){
        memory.progSpace[i] = font[i];
    }


    for(i = 0; i < 16; i++){
        memory.stack[i] = 0;
        memory.V[i] = 0;
    }

    FILE *rom;
    rom = fopen("ROM.ch8", "rb");

    //printf("\nSIZE: %ld\n", sizeof(dump));


    uint8_t *dump = (uint8_t*)malloc(MEMSIZE * sizeof(uint8_t));

    size_t size = fread(dump, 1, MEMSIZE, rom);


    for(i = 0; i < size; i++){
        memory.progSpace[i + memory.PC] = dump[i];
    }

    //STANDARD CHIP-8 Instructions

    uint8_t posX, posY = 0;

    int k;

    while(1){

        uint16_t instruction = memory.progSpace[memory.PC] << 8 | memory.progSpace[memory.PC + 1];
        uint8_t instructionSplit[4] = {instruction >> 12, (instruction & 0xfff) >> 8, (instruction & 0xff) >> 4, (instruction & 0xf)};

        uint8_t x = instructionSplit[1];
        uint8_t y = instructionSplit[2];
        uint8_t N = instructionSplit[3];


        time_t t;
        srand((unsigned) time(&t));
        uint8_t randomNumber = rand() % 256;

       printf(" PC: %d | OPCODE: %X \n", memory.PC,instruction);

        switch (instruction)
        {
        case 0x00E0:
            for(i = 0; i < 64*32; i++)
                screen[i] = '0';
            memory.PC += 2;
            break;

        default:
            
            switch (instructionSplit[0]) // first part of the instruction
            {
            case 0x1: //JP addr
                memory.PC = instruction & 0xfff;
                break;
            case 0x6: //LD Vx, byte
                memory.V[x] = instruction & 0xff;
                memory.PC += 2;
                break;

            case 0x7:
                memory.V[x] += instruction & 0xff;
                memory.PC += 2;
                break;
            case 0xA: //LD I, addr
                memory.I = instruction & 0xfff;
                memory.PC += 2;
                break;
            case 0xD: //DRW Vx, Vy, nibblec
                x %= 64;
                y %= 32;
                
                memory.V[F] = 0;
//                printf("REG [I]: %d\n", memory.I - 512);
                for(i = 0; i < N; i++){
                    y+=i;
                    if(y < 64){
                    for(k = 0; k < 8; k++){
                        x+=k;
                        if(x < 64){
                            if (((memory.progSpace[memory.I + i]) << k) & 0b10000000){
                                if (screen[x * y] == '0'){
                                    screen[x * y] = ' ';
                                    memory.V[F] = 1;
                                }
                                else{
                                    screen[x * y] = '0';
                                }
                            }
                        }
                }
                }
            }
            memory.PC += 2;
            break;

            default:
                memory.PC += 2;
      //  sleep(1);

            }   

        for(i = 0; i < 64*32; i++){
            printf("%c", screen[i]);

            if((i + 1) % 64 == 0){
                printf("\n");
            }
        }
    }
    }
    free(dump);
    return EXIT_SUCCESS;
}