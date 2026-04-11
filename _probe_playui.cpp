#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
int main(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { std::cerr << SDL_GetError() << std::endl; return 1; }
    int flags = IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags) { std::cerr << IMG_GetError() << std::endl; return 1; }
    SDL_Surface* s = IMG_Load("Resources/UI/MainMenu/playUI.png");
    if (!s) { std::cerr << IMG_GetError() << std::endl; return 1; }
    std::cout << "w=" << s->w << " h=" << s->h << " pitch=" << s->pitch << " bpp=" << (int)s->format->BytesPerPixel << " format=" << SDL_GetPixelFormatName(s->format->format) << std::endl;
    SDL_FreeSurface(s);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
