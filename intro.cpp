#include <SDL2/SDL.h>       // this is for the 2d level stuff
#include <SDL2/SDL_ttf.h>   // another thing we need for the fonts
#include <SDL2/SDL_image.h> // this is for the images in this code, specifically the stars
#include <iostream>         // this is just for the c++ stuff like printing to the console
#include <cstdlib>          // for the random generator functions
#include <ctime>            // for seeding the random generator

const int SCREEN_WIDTH = 500;       // this is the width of the screen, can be optimized later
const int SCREEN_HEIGHT = 720;      // height of the screen
const int TOTAL_MENU_ITEMS = 3;     // only 3 items on the main menu
const int TOTAL_SETTINGS_ITEMS = 2; // only 2 items in the settings menu
const float STAR_SPEED = 1.0f;      // default speed for moving the stars

// function to render text on the screen
void renderCenteredText(SDL_Renderer *renderer, const char *text, SDL_Color color, TTF_Font *font, int windowWidth, int y)
{
    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect destRect;
    destRect.w = surface->w;
    destRect.h = surface->h;
    destRect.x = (windowWidth - destRect.w) / 2;
    destRect.y = y;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    SDL_DestroyTexture(texture);
}

// function to load textures from an image file
SDL_Texture *loadTexture(const char *path, SDL_Renderer *renderer)
{
    SDL_Surface *loadedSurface = IMG_Load(path);
    if (loadedSurface == nullptr)
    {
        std::cerr << "Failed to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (newTexture == nullptr)
    {
        std::cerr << "Failed to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
    }

    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

// structure to represent each star
struct Star
{
    float x, y;             // current position of the star
    float targetX, targetY; // target position where the star should move
};

// function to move each star smoothly towards its target position
void moveStar(Star &star)
{
    float dx = star.targetX - star.x;
    float dy = star.targetY - star.y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance < STAR_SPEED)
    {
        star.x = star.targetX;
        star.y = star.targetY;
        star.targetX = rand() % SCREEN_WIDTH;
        star.targetY = rand() % SCREEN_HEIGHT;
    }
    else
    {
        star.x += (dx / distance) * STAR_SPEED;
        star.y += (dy / distance) * STAR_SPEED;
    }
}

// function to render the CCP background (red with moving stars)
void renderCCPBackground(SDL_Renderer *renderer, SDL_Texture *starTexture, Star stars[], int numStars)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < numStars; i++)
    {
        moveStar(stars[i]);
        SDL_Rect starRect = {(int)stars[i].x, (int)stars[i].y, 30, 30};
        SDL_RenderCopy(renderer, starTexture, nullptr, &starRect);
    }
}

// function to add random edgy Chinese text to the screen
void renderRandomChineseText(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    const char *randomTexts[] = {
        "革命万岁",         // "Long live the revolution"
        "无产阶级统治",     // "Proletariat rule"
        "星星之火可以燎原", // "A single spark can start a prairie fire"
        "红色如血",         // "Red like blood"
        "共产主义永存"      // "Communism forever"
    };

    srand((unsigned)time(0));
    int randomIndex = rand() % 5;

    renderCenteredText(renderer, randomTexts[randomIndex], color, font, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
}

// Title screen function
void showTitleScreen(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, SDL_Texture *titleTexture)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (titleTexture)
    {
        SDL_Rect titleRect = {(SCREEN_WIDTH - 400) / 2, 100, 400, 100}; // Center title image
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    }
    else
    {
        renderCenteredText(renderer, "Chud Bullet Hell", color, font, SCREEN_WIDTH, 200);
    }

    renderCenteredText(renderer, "Press Enter to Start", color, font, SCREEN_WIDTH, 300);
    SDL_RenderPresent(renderer);

    bool quit = false;
    SDL_Event e;

    // Wait for Enter key to start
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                exit(0);
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_RETURN)
                {
                    quit = true;
                }
            }
        }
    }
}

// Main menu function
void showMainMenu(SDL_Renderer *renderer, TTF_Font *font, SDL_Color yellow, SDL_Texture *starTexture, Star stars[], int numStars)
{
    bool quit = false;
    SDL_Event e;
    int selectedItem = 0;

    const char *menuItems[TOTAL_MENU_ITEMS] = {
        "Play",
        "Settings",
        "Exit"};

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_DOWN:
                    selectedItem = (selectedItem + 1) % TOTAL_MENU_ITEMS;
                    break;
                case SDLK_UP:
                    selectedItem = (selectedItem - 1 + TOTAL_MENU_ITEMS) % TOTAL_MENU_ITEMS;
                    break;
                case SDLK_RETURN:
                    if (selectedItem == 0) // Play
                    {
                        std::cout << "Start Game!" << std::endl;
                    }
                    else if (selectedItem == 1) // Settings
                    {
                        // Show settings menu (not implemented here)
                    }
                    else if (selectedItem == 2) // Exit
                    {
                        quit = true;
                    }
                    break;
                }
            }
        }

        renderCCPBackground(renderer, starTexture, stars, numStars);
        renderRandomChineseText(renderer, font, yellow);

        for (int i = 0; i < TOTAL_MENU_ITEMS; i++)
        {
            SDL_Color color = (i == selectedItem) ? SDL_Color{255, 255, 255, 255} : yellow;
            std::string menuText = std::string(i == selectedItem ? "--> " : "") + std::string(menuItems[i]);
            renderCenteredText(renderer, menuText.c_str(), color, font, SCREEN_WIDTH, 200 + i * 50);
        }

        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == nullptr)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowResizable(window, SDL_FALSE); // Prevent window resizing

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("imgs/NotoSansTC-Regular.ttf", 24);
    if (font == nullptr)
    {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture *starTexture = loadTexture("imgs/Yellow-Star.png", renderer);
    if (starTexture == nullptr)
    {
        std::cerr << "Failed to load star texture!" << std::endl;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture *titleTexture = loadTexture("imgs/Title.png", renderer); // optional title image
    Star stars[8];                                                       // create 8 stars
    for (int i = 0; i < 8; i++)
    {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].targetX = rand() % SCREEN_WIDTH;
        stars[i].targetY = rand() % SCREEN_HEIGHT;
    }

    SDL_Color yellow = {255, 255, 0, 255}; // Yellow color for text

    showTitleScreen(renderer, font, yellow, titleTexture); // Show title screen

    showMainMenu(renderer, font, yellow, starTexture, stars, 8); // Show main menu

    SDL_DestroyTexture(starTexture);
    SDL_DestroyTexture(titleTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
