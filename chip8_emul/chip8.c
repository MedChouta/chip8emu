#include <stdio.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "chip8.h"


int main(int argc, char** argv){
    int i;

    SDL_Surface *window;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_SetVideoMode(
        1280,
        640,
        32,
        SDL_clHWSURFACE
    );

    SDL_WM_SetCaption("CHIP-8", NULL);

    SDL_FillRect(window, NULL, SDL_MapRGB(window->format, 0,0,0));

    SDL_Flip(window);

    sleep(5);

    if (window == NULL){
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Rect r[64*32] = {0};
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

    }

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
    rom = fopen("Rom.ch8", "rb");
    
    uint8_t *dump = malloc(MEMSIZE * sizeof(uint8_t));

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

        time_t t;
        srand((unsigned) time(&t));
        uint8_t randomNumber = rand() % 256;

       printf(" PC: %d | OPCODE: %X \n", memory.PC,instruction);

        switch (instruction)
        {
        case 0x00E0:
            printf("CLS");
            break;
        case 0x00EE:
            memory.PC = memory.stack[memory.SP - 1];
            memory.SP--;
            printf("STACK POINTER: %d\n", memory.SP);
            break;
        default:
            
            switch (instructionSplit[0]) // first part of the instruction
            {
            case 0x1: //JP addr
                memory.PC = instruction & 0xfff;
                break;

            case 0x2:
                memory.SP++;
                memory.stack[memory.SP - 1] = memory.PC + 2;
                printf("STACK: %d\n", memory.stack[memory.SP - 1]);
                memory.PC = instruction & 0xfff;
                printf("STACK POINTER: %d\n", memory.SP);
                break;

            case 0x3: //SE Vx, byte
                if (memory.V[x] == instruction & 0xff)
                    memory.PC += 4;
                else
                    memory.PC += 2;
                break;
            
            case 0x4:
                if (memory.V[x] != instruction & 0xff)
                    memory.PC += 4;
                else
                    memory.PC += 2;
                break;

            case 0x5: // SE Vx, Vy
                if (memory.V[x] == memory.V[y])
                    memory.PC += 4;
                else
                    memory.PC += 2;
                break;

            case 0x6: //LD Vx, byte
                memory.V[x] = instruction & 0xff;
                memory.PC += 2;
                break;

            case 0x7:
                memory.V[x] += instruction & 0xff;
                memory.PC += 2;
                break;

            case 0x8:
                switch (instructionSplit[3])
                {
                case 0x0: //LD Vx, Vy
                    memory.V[x] = memory.V[y];
                    memory.PC += 2;
                    break;

                case 0x1: //OR Vx, Vy
                    memory.V[x] = memory.V[x] | memory.V[y];
                    memory.PC += 2;
                    break;

                case 0x2: //AND Vx, Vy
                    memory.V[x] = memory.V[x] & memory.V[y];
                    memory.PC += 2;
                    break;
                
                case 0x3: //XOR Vx, Vy
                    memory.V[x] = memory.V[x] ^ memory.V[y];
                    memory.PC += 2;
                    break;
                
                case 0x4: //ADD Vx, Vy
                    if (memory.V[x] + memory.V[y] > 255)
                        memory.V[F] = 1;
                    else
                        memory.V[F] = 0;
                    
                    memory.V[x] += memory.V[y];
                    memory.PC += 2;

                    break;

                case 0x5: //Sub Vx, Vy
                    if (memory.V[x] > memory.V[y])
                        memory.V[F] = 1;
                    else
                        memory.V[F] = 0;
                    
                    memory.V[x] -= memory.V[y];
                    memory.PC += 2;
                    break;
                case 0x6: //SHR Vx {, Vy}
                    memory.V[F] = memory.V[x] & 1;
                    memory.V[x] >>= 1;
                    memory.PC += 2;
                    break;
                case 0x7: //SUBN Vx, Vy
                    if (memory.V[y] > memory.V[x])
                        memory.V[F] = 1;
                    else
                        memory.V[F] = 0;
                    
                    memory.V[x] = memory.V[y] - memory.V[x];
                    memory.PC += 2;
                    break;
                case 0xE: //SHL Vx {, Vy}
                    memory.V[F] = memory.V[x] & (1 << 15);
                    memory.V[x] <<= 1;
                    memory.PC += 2;
                    break;
                }
                break;
            case 0x9: //SNE Vx, Vy
                if (memory.V[x] != memory.V[x])
                    memory.PC += 4;
                else
                    memory.PC += 2;
                break;
            case 0xA: //LD I, addr
                memory.I = instruction & 0xfff;
                memory.PC += 2;
                break;
            case 0xB: //JP V0, addr
                memory.PC += (instruction & 0xfff) + memory.V[0];
                memory.PC += 2;
                break;
            case 0xC: //RND Vx, byte
                memory.V[x] = randomNumber & (instruction & 0xfff);
                memory.PC += 2;
                break;
            case 0xD: //DRW Vx, Vy, nibblec
                posX = x % 64;
                posY = y % 32;
                memory.V[F] = 0;
                printf("REG [I]: %d\n", memory.I - 512);
                for(i = 0; i < instructionSplit[3]; i++){
                    for(k = 0; k < 8; k++){
                        if ((memory.progSpace[memory.I + i - 512] >> k) & 1){
                            for (j = 0; j < 64*32; j++){
                                if (r[j].x == 1280/64 * posX && r[j].y == 640/32 * posY){
                                    if(r[j].w != 0 && r[j].h != 0){
                                        r[j].w = 0;
                                        r[j].h = 0;
                                        memory.V[F] = 1;
                                    }
                                    else{
                                        r[j].w = 1280/64;
                                        r[j].h = 640/32;
                                    }
                                }
                            }
                        }

                        posX++;

                        if(posX > 63)
                            break;
                    }
                    
                    posY++;
                    if(posY > 31)
                        break;
                }
                memory.PC += 2;
                break;
            case 0xE:
                memory.PC += 2;
                break;
            case 0xF:
                switch(instructionSplit[3]){
                    case 0x7: //LD Vx, DT
                        memory.V[x] = memory.DT;
                        memory.PC += 2;
                        break;
                    case 0xA: //LD Vx, K
                        memory.PC += 2;
                        break;
                    case 0x8: //LD ST, Vx
                        memory.ST = memory.V[x];
                        memory.PC += 2;
                        break;
                    case 0xE: //ADD I, Vx
                        memory.I += memory.V[x];
                        memory.PC += 2;
                        break;
                    case 0x9:
                        memory.I = memory.V[x] * 0x5;
                        memory.PC += 2;
                        break;
                    case 0x3:
                        memory.progSpace[memory.I] = memory.V[x] / 100;
                        memory.progSpace[memory.I + 1] = (memory.V[x] / 10) % 10;
                        memory.progSpace[memory.I + 2] = (memory.V[x] % 100) % 10;	
                        memory.PC += 2;
                        break;
                    case 0x5:
                        switch (y)
                        {
                        case 0x1: //LD DT, Vx
                            memory.DT = memory.V[x];
                            memory.PC += 2;
                            break;
                        case 0x5:
                            for(i = 0; i < x; i++){
                                memory.progSpace[memory.I + i] = memory.V[i];
                            }

                            memory.PC += 2;
                            break;
                        case 0x6:
                            for(i = 0; i < x; i++){
                                memory.progSpace[memory.I + i] = memory.V[i];
                            }

                            memory.PC += 2;
                        
                            break;
                        }
                }

        }
        }
    
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(dump);
    return EXIT_SUCCESS;
}