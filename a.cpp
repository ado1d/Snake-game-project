#include<bits/stdc++.h>
#include "inc/SDL.h"
#include"inc/SDL_rect.h"
#include "inc/SDL_ttf.h"
using namespace std;

#undef main

const int segmentSize = 10;
const int screen_width = 640 * 2;
const int screen_height = 480 * 2;
const int ini_snake_lenghth = 5;
const int starting_x = 150;
const int staring_y = 480;

const int bonusFoodTimer = 4000;  //for 4 sec bonus food

struct snakeSegment {
    int x, y;
};
typedef vector<snakeSegment>        vSnake;

struct food {
    int x, y;
};
struct barrier {
    int x, y, width, height;
};
typedef vector<barrier>             vBarrier;

struct bonusFood {
    int x, y;
    bool ok;
    Uint32 timer;
};
enum class direction {
    up, down, left, right
};

bool collitionSnake(snakeSegment &point, vSnake &snake) {
    for (auto segment : snake) {
        if (point.x == segment.x && point.y == segment.y) return true;
    }
    return false;
}

bool collition_barrier(snakeSegment &head, vBarrier &barriers) {
    SDL_Rect headrect = {head.x, head.y, segmentSize, segmentSize};
    for (auto barrier : barriers) {
        SDL_Rect terget = {barrier.x, barrier.y, barrier.width, barrier.height}; 
        if(SDL_HasIntersection(&headrect, &terget)) return true;
    } 
    return false;
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const string &text, int x, int y) {
    SDL_Color textColor = {255, 0, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (!surface) {
        cerr << "Error: Unable to create text surface\nTTF Error: " << TTF_GetError() << '\n';
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        cerr << "Error: Unable to create text texture\nSDL Error: " << SDL_GetError() << '\n';
        SDL_FreeSurface(surface);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_FreeSurface(surface);
    SDL_Rect destRect = {x, y, 0, 0};
    SDL_QueryTexture(texture, nullptr, nullptr, &destRect.w, &destRect.h);
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    SDL_DestroyTexture(texture);
}

int main() {

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "Error: SDL failed to initialize\nSDL Error: " << SDL_GetError() << '\n';
        return EXIT_FAILURE;
    }
    
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (TTF_Init() < 0) {
        cerr << "Error: TTF failed to initialize\nTTF Error: " << TTF_GetError() << '\n';
        SDL_Quit();
        return EXIT_FAILURE;
    }


    TTF_Font *font = TTF_OpenFont("E:\\Bungee_Spice\\BungeeSpice-Regular.ttf", 25);


    if (!font) {
        cerr << "Error: Font not found\nTTF Error: " << TTF_GetError() << '\n';
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    vSnake snake;
    //food
    int random_x = abs(rand()) % (screen_width - 100), random_y = abs(rand()) % (screen_height - 100);
    SDL_Rect khabar = {random_x, random_y, segmentSize + 10, segmentSize + 10};  //for food

    vBarrier barriers;
    barriers.push_back({0 , screen_height - 300, 5, 300});
    barriers.push_back({0 , screen_height - 5, 300, 5});
    barriers.push_back({screen_width - 5, 0, 5, 300});
    barriers.push_back({screen_width - 300, 0, 300, 5});
    // barriers.push_back({100, 100, 200, 2});
    // barriers.push_back({100, 100, })

    direction curr_direction = direction:: right;
    for (int i = 0; i < ini_snake_lenghth; i++) {
        snakeSegment segment;
        segment.x = starting_x - (i * segmentSize), segment.y = staring_y;
        snake.push_back(segment);
    }

    bonusFood bonus = {0, 0, 0, 0};

    SDL_Event event;
    bool quit = false;
    int points = 0;
    bool paused = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (curr_direction != direction::down)
                            curr_direction = direction::up;
                        break;
                    case SDLK_DOWN:
                        if (curr_direction != direction::up)
                            curr_direction = direction::down;
                        break;
                    case SDLK_LEFT:
                        if (curr_direction != direction::right)
                            curr_direction = direction::left;
                        break;
                    case SDLK_RIGHT:
                        if (curr_direction != direction::left)
                            curr_direction = direction::right;
                        break;
                    case SDLK_SPACE:
                        paused = !paused;
                        break;
                    default:
                        break;
                }
            }
        }
        if (!quit && !paused) {
            snakeSegment newMatha = {snake[0].x, snake[0].y};
            switch (curr_direction) {
                case direction::up:
                    newMatha.y -= segmentSize;
                    break;
                case direction::down:
                    newMatha.y += segmentSize;
                    break;
                case direction::right:
                    newMatha.x += segmentSize;
                    break;
                case direction::left:
                    newMatha.x -= segmentSize;
                    break;
            }
            if (newMatha.x < 0)
                newMatha.x = screen_width - 0 - segmentSize;
            else if (newMatha.x >= screen_width - 0)
                newMatha.x = 0;

            if (newMatha.y < 0)
                newMatha.y = screen_height - 0 - segmentSize;
            else if (newMatha.y >= screen_height - 0)
                newMatha.y = 0;


            
            if(collitionSnake(newMatha, snake) || collition_barrier(newMatha, barriers)) {
                quit = true;  //terminates the game
            }
            SDL_Rect head_rect = {snake[0].x, snake[0].y, segmentSize, segmentSize}, khabar_rect = khabar;
            //food collision 
            if (SDL_HasIntersection(&head_rect, &khabar_rect)) {
                points++;
                int new_random_x = abs(rand()) % screen_width, new_random_y = abs(rand()) % screen_height;
                khabar = {new_random_x, new_random_y, segmentSize + 10, segmentSize + 10}; 
                snakeSegment tail = snake.back();
                for (int i = 0; i < 3; ++i) {
                    snake.push_back(tail);
                }
                if (points % 2 == 0) {
                    bonus = {rand() % (screen_width / segmentSize) * segmentSize, rand() % (screen_height / segmentSize) * segmentSize, true, SDL_GetTicks()};
                          
                }

            }
            SDL_Rect bonusFoodRect = {bonus.x, bonus.y, segmentSize + 15, segmentSize + 15 };
            if(bonus.ok && SDL_HasIntersection(&head_rect, &bonusFoodRect)) {
                points += 10;
                bonus = {0, 0, 0, 0};
            }else if (bonus.ok && (SDL_GetTicks() - bonus.timer) > bonusFoodTimer)   bonus = {0, 0, 0, 0};

            //move snake
            for (int i = snake.size() - 1; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
            snake[0] = newMatha;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            //draw food
            SDL_SetRenderDrawColor(renderer, rand() % 255,rand() % 150 , rand() % 255, 255);
            SDL_RenderFillRect(renderer, &khabar);
            if (bonus.ok) {
                SDL_SetRenderDrawColor(renderer, 0, 123, 255, 255);  // blue color for bonus food
                //SDL_Rect bonusFoodRect = {bonus.x, bonus.y, segmentSize + 15, segmentSize + 15 };
                SDL_RenderFillRect(renderer, &bonusFoodRect);
            }
            //draw barrier
            SDL_SetRenderDrawColor(renderer, 250, 180, 100, 255);
            for (const auto &barrier : barriers) {
                SDL_Rect barrierRect = {barrier.x, barrier.y, barrier.width, barrier.height};
                SDL_RenderFillRect(renderer, &barrierRect);
            }

            //draw snae
            SDL_SetRenderDrawColor(renderer, 0, 150, 100, 255);
            for (const auto &segment : snake) {
                SDL_Rect rect = {segment.x, segment.y, segmentSize, segmentSize};
                SDL_RenderFillRect(renderer, &rect);
            }
            renderText(renderer, font, "Points: " + to_string(points), 10, 10);
            SDL_RenderPresent(renderer);
            // int i = 50;
            // i = max(0, i - 5);
            SDL_Delay(40);
            
        }else if (paused) {
            // TTF_Font *newFont = TTF_OpenFont("E:\\Bungee_Spice\\BungeeSpice-Regular.ttf", 40);
            renderText(renderer, font, "Game Paused", screen_width / 2, 50);
            SDL_RenderPresent(renderer);
            
        }
    }
    auto newFont = TTF_OpenFont("E:\\Bungee_Spice\\BungeeSpice-Regular.ttf", 50);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderText(renderer, newFont, "Game Over - Points: " + to_string(points), screen_width / 4, screen_height / 2);
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);
    TTF_CloseFont(font);
    
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;

}