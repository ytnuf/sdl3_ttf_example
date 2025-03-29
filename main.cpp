
#include <SDL3/SDL_init.h>

#include <iostream>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Quit();
    std::cout<<"hello SDL"<<std::endl;
    return 0;
}
