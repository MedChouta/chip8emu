#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    SDL_Surface *ecran = NULL, *pixel[640][256] = {NULL};
    SDL_Rect position;
    int i = 0;
    int j = 0;

    SDL_Init(SDL_INIT_VIDEO);
    ecran = SDL_SetVideoMode(640, 256, 32, SDL_HWSURFACE);
   
    for (i = 0 ; i < 256 ; i++){
        for(j = 0; j < 640; j++){
            pixel[j][i] = SDL_CreateRGBSurface(SDL_HWSURFACE, 0, 0, 32, 0, 0, 0, 0);
        }
    }

    SDL_WM_SetCaption("Mon dégradé en SDL !", NULL);
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

    for (i = 0 ; i < 256 ; i++)
    {
        for (j = 0; j < 640; j++){
            position.x = j; // Les lignes sont à gauche (abscisse de 0)
            position.y = i; // La position verticale dépend du numéro de la ligne
            SDL_FillRect(pixel[j][i], NULL, SDL_MapRGB(ecran->format, 255,255, 255));
            SDL_BlitSurface(pixel[j][i], NULL, ecran, &position);
        }
    }

    SDL_Flip(ecran);


    sleep(5);

    for (i = 0 ; i < 256 ; i++){ // N'oubliez pas de libérer les 256 surfaces
        for(j = 0; j < 640; j++){
        SDL_FreeSurface(pixel[j][i]);
    }
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}