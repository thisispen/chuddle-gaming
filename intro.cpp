#include <SDL2/SDL.h>       // this is for the 2d level stuff
#include <SDL2/SDL_ttf.h>   // another thing we need for the fonts
#include <SDL2/SDL_image.h> // this is for the images in this code, specifically the stars
#include <iostream>         // this is just for the c++ stuff like printing to the console
#include <cstdlib>          // for the random generator functions
#include <ctime>            // for seeding the random generator

const int SCREEN_WIDTH = 640;       // this is the width of the screen, can be optimized later
const int SCREEN_HEIGHT = 480;      // height of the screen
const int TOTAL_MENU_ITEMS = 3;     // only 3 items on the main menu
const int TOTAL_SETTINGS_ITEMS = 2; // only 2 items in the settings menu
const float STAR_SPEED = 1.0f;      // default speed for moving the stars

// function to render text on the screen
void renderCenteredText(SDL_Renderer *renderer, const char *text, SDL_Color color, TTF_Font *font, int windowWidth, int y)
{
    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
    // we make a surface out of the font, text, and color
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    // then we create a texture from that surface

    SDL_Rect destRect;
    // this is the rectangle we'll use to display the text
    destRect.w = surface->w;                     // get the width from the surface
    destRect.h = surface->h;                     // get the height from the surface
    destRect.x = (windowWidth - destRect.w) / 2; // center the text horizontally
    destRect.y = y;                              // y position is passed as a parameter

    SDL_FreeSurface(surface);                              // free the surface to avoid memory leaks
    SDL_RenderCopy(renderer, texture, nullptr, &destRect); // copy the texture to the renderer
    SDL_DestroyTexture(texture);                           // destroy the texture to avoid memory leaks
}

// function to load textures from an image file
SDL_Texture *loadTexture(const char *path, SDL_Renderer *renderer)
{
    SDL_Surface *loadedSurface = IMG_Load(path); // load the image from file
    if (loadedSurface == nullptr)                // check if the image loaded correctly
    {
        std::cerr << "Failed to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface); // create a texture from the loaded surface
    if (newTexture == nullptr)                                                       // check if the texture was created
    {
        std::cerr << "Failed to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
    }

    SDL_FreeSurface(loadedSurface); // free the surface to avoid memory leaks

    return newTexture; // return the texture
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
    float dx = star.targetX - star.x;         // calculate the difference in x position
    float dy = star.targetY - star.y;         // calculate the difference in y position
    float distance = sqrt(dx * dx + dy * dy); // get the distance between the current position and the target

    // if the distance is small enough, the star has reached its target
    if (distance < STAR_SPEED)
    {
        star.x = star.targetX;
        star.y = star.targetY;
        // generate a new random target position
        star.targetX = rand() % SCREEN_WIDTH;
        star.targetY = rand() % SCREEN_HEIGHT;
    }
    else
    {
        // move the star towards the target
        star.x += (dx / distance) * STAR_SPEED;
        star.y += (dy / distance) * STAR_SPEED;
    }
}

// function to render the CCP background (red with moving stars)
void renderCCPBackground(SDL_Renderer *renderer, SDL_Texture *starTexture, Star stars[], int numStars)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // bright red background
    SDL_RenderClear(renderer);                        // clear the screen with the red color

    for (int i = 0; i < numStars; i++) // for each star
    {
        moveStar(stars[i]);                                             // move the star
        SDL_Rect starRect = {(int)stars[i].x, (int)stars[i].y, 30, 30}; // 30x30 star size
        SDL_RenderCopy(renderer, starTexture, nullptr, &starRect);      // render the star on the screen
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

    srand((unsigned)time(0));     // seed the random generator
    int randomIndex = rand() % 5; // pick a random phrase

    renderCenteredText(renderer, randomTexts[randomIndex], color, font, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
}

// Settings menu function
void showSettingsMenu(SDL_Renderer *renderer, TTF_Font *font, SDL_Color yellow, SDL_Texture *starTexture, Star stars[], int numStars)
{
    bool quit = false;
    SDL_Event e;
    int selectedItem = 0;

    const char *settingsItems[TOTAL_SETTINGS_ITEMS] = {
        "Control Configuration",
        "Back"};

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0) // handle events
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
                    selectedItem = (selectedItem + 1) % TOTAL_SETTINGS_ITEMS; // navigate menu items
                    break;
                case SDLK_UP:
                    selectedItem = (selectedItem - 1 + TOTAL_SETTINGS_ITEMS) % TOTAL_SETTINGS_ITEMS;
                    break;
                case SDLK_RETURN:
                    if (selectedItem == 0) // Control Configuration
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        SDL_RenderClear(renderer);
                        renderCenteredText(renderer, "Controls: Arrow Keys/WASD, J for A, K for B", yellow, font, SCREEN_WIDTH, 200);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(3000); // pause for 3 seconds
                    }
                    else if (selectedItem == 1) // Back
                    {
                        quit = true;
                    }
                    break;
                }
            }
        }

        renderCCPBackground(renderer, starTexture, stars, numStars); // render the background
        renderRandomChineseText(renderer, font, yellow);             // render random Chinese text

        // render menu items
        for (int i = 0; i < TOTAL_SETTINGS_ITEMS; i++)
        {
            SDL_Color color = (i == selectedItem) ? SDL_Color{255, 255, 255, 255} : yellow; // highlight the selected item
            std::string settingsText = std::string(i == selectedItem ? "--> " : "") + std::string(settingsItems[i]);
            renderCenteredText(renderer, settingsText.c_str(), color, font, SCREEN_WIDTH, 200 + i * 50);
        }

        SDL_RenderPresent(renderer); // present the rendered content
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
                        // game logic here
                    }
                    else if (selectedItem == 1) // Settings
                    {
                        showSettingsMenu(renderer, font, yellow, starTexture, stars, numStars);
                    }
                    else if (selectedItem == 2) // Exit
                    {
                        quit = true;
                    }
                    break;
                }
            }
        }

        renderCCPBackground(renderer, starTexture, stars, numStars); // render the background
        renderRandomChineseText(renderer, font, yellow);             // render random Chinese text

        // render menu items
        for (int i = 0; i < TOTAL_MENU_ITEMS; i++)
        {
            SDL_Color color = (i == selectedItem) ? SDL_Color{255, 255, 255, 255} : yellow; // highlight the selected item
            std::string menuText = std::string(i == selectedItem ? "--> " : "") + std::string(menuItems[i]);
            renderCenteredText(renderer, menuText.c_str(), color, font, SCREEN_WIDTH, 200 + i * 50);
        }

        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow("Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font
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

    // Load the star texture
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

    // Create stars array
    Star stars[8]; // 8 stars
    for (int i = 0; i < 8; i++)
    {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].targetX = rand() % SCREEN_WIDTH;
        stars[i].targetY = rand() % SCREEN_HEIGHT;
    }

    SDL_Color yellow = {255, 255, 0, 255}; // Yellow color for text

    // Show the main menu
    showMainMenu(renderer, font, yellow, starTexture, stars, 8);

    // Clean up
    SDL_DestroyTexture(starTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
