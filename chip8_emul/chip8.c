#include <stdio.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include "chip8.h"


int main(int argc, char** argv){
    uint8_t i, j;
    char screen[64][32];

    for(i = 0; i < 32; i++){
        for(j = 0; j < 64; j++)
            screen[j][i] = '0';
    }

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

    SDL_Surface *surfRect[64][32] = {NULL};

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
    rom = fopen("test_opcode.ch8", "rb");
    if(rom != NULL)
        printf("NON");
    //printf("\nSIZE: %ld\n", sizeof(dump));
    printf("DSLKGHSLKDHGKJSDFGHKJSDHGJK");

    uint8_t *dump = (uint8_t*)malloc(MEMSIZE * sizeof(uint8_t));
    size_t size = fread(dump, 1, MEMSIZE, rom);
    //printf("HELLO WORLD\n");

    printf("size: %ld", size);

    for(i = 0; i < size; i++){
      //  printf("HELLO WORLD %d\n", i);
        memory.progSpace[i + memory.PC] = dump[i];
    }

    //STANDARD CHIP-8 Instructions

    uint8_t posX, posY = 0;

    uint8_t k;
    int x, y;

    SDL_Rect pixel;


    while(1){
        
       // SDL_FillRect(window, NULL, SDL_MapRGB(window->format, 0,0,0));

        uint16_t instruction = memory.progSpace[memory.PC] << 8 | memory.progSpace[memory.PC + 1];
        uint8_t instructionSplit[4] = {instruction >> 12, (instruction & 0xfff) >> 8, (instruction & 0xff) >> 4, (instruction & 0xf)};

        uint8_t x = instructionSplit[1];
        uint8_t y = instructionSplit[2];
        uint8_t N = instructionSplit[3];


        time_t t;
        srand((unsigned) time(&t));
        uint8_t randomNumber = rand() % 256;

       printf("PC: %d | OPCODE: %X \n", memory.PC,instruction);

        switch (instruction)
        {
        case 0x00E0:
            for(i = 0; i < 32; i++){
                for(j = 0; j < 64; j++){
                    surfRect[j][i] = NULL;
                }

            }
            printf("\n");
            memory.PC += 2;
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
                printf("\tREG [V%X]: %X\n\n", x, memory.V[x]);
                memory.PC += 2;
                break;

            case 0x7:
                printf("\tREG [V%X] BEFORE: %X\n", x, memory.V[x]);
                memory.V[x] += instruction & 0xff;
                printf("\tREG [V%X] AFTER: %X\n\n", x, memory.V[x]);
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
                    memory.V[F] = memory.V[x] & (1 << 7);
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

                printf("\tREG [I]: %X\n\n", memory.I);

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
                printf("\tREG [V%X]: %d\n\n", x, memory.V[x]);
                printf("\tREG [V%X]: %d\n\n", y, memory.V[y]);
                posX = memory.V[x] % 64;
                posY = memory.V[y] % 32;
                
                printf("\t\t (x, y) = (%d, %d)\n", x, y);


                memory.V[F] = 0;
                printf("\tREG [I]: %X\n", memory.progSpace[memory.I]);
                for(i = 0; i < N; i++){
                    if(posY < 32){
                    printf("\t\tSprite #%d\n", i);
                    for(k = 0; k < 8; k++){
                        if(posX < 64){
                            if (((memory.progSpace[memory.I + i]) << k) & 0b10000000){
                                   // printf("\t\t\tPixel #%d => ", k);
                                    //printf("(x, y) = (%d, %d)\n", posX, posY);
                                    //printf("(w, h) = (%d, %d)\n", r[posX][posY].w , r[posX][posY].h);
                                if (surfRect[posX][posY] == NULL){
                                    
                                    surfRect[posX][posY] = SDL_CreateRGBSurface(SDL_HWSURFACE, 20, 20, 32, 0, 0, 0, 0);    
                                    pixel.x = 1280/64 * posX;
                                    pixel.y = 640/32 * posY;    

                                    SDL_FillRect(surfRect[posX][posY], NULL, SDL_MapRGB(window->format, 255,255,255));
                                    SDL_BlitSurface(surfRect[posX][posY], NULL, window, &pixel);  

                                    printf("(w, h) = (%d, %d)\n", surfRect[posX][posY]->w , surfRect[posX][posY]->h);
                                    memory.V[F] = 1;
                                }
                                else{
                                    surfRect[posX][posY] = NULL;
                                }
                            }
                            posX++;
                        }
                }
                }
                posY++;
                posX = memory.V[x] % 64;
                //printf("\t\t (x, y) = (%d, %d)\n", x, y);
            }
            printf("\n");
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
                    break;
                }
            break;

            default:
                memory.PC += 2;

            }   
        //sleep(1);
        }
        /*
    for(y = 0; y < 32; y++){
        for(x = 0; x < 64; x++){
            printf("(%d, %d)\n", pixel.x, pixel.y);
            SDL_BlitSurface(surfRect[x][y], NULL, window, &pixel);         
        }
    }
*/
        printf("\n");
        SDL_Flip(window);
    }
    

    free(dump);
   SDL_Quit();
    return EXIT_SUCCESS;
}