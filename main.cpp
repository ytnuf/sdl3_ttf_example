
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    TTF_Quit();
    SDL_Quit();
    std::cout<<"hello SDL ttf"<<std::endl;
    return 0;
}
