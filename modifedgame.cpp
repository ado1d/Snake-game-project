#include "inc/SDL.h"
#include "inc/SDL_ttf.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

#undef main

const int SEGMENT_SIZE = 20;

struct SnakeSegment {
    int x, y;
};

struct Food {
    int x, y;
};

struct Barrier {
    int x, y, width, height;
};

struct BonusFood {
    int x, y;
    bool active;
    Uint32 timer;  // timer 
};

enum class Direction {
    UP, DOWN, LEFT, RIGHT
};

bool checkCollision(const SnakeSegment &point, const vector<SnakeSegment> &snake) {
    for (const auto &segment : snake) {
        if (point.x == segment.x && point.y == segment.y) {
            return true;
        }
    }
    return false;
}

bool checkCollision(const SnakeSegment &point, const vector<Barrier> &barriers) {
    for (const auto &barrier : barriers) {
        if (point.x < barrier.x + barrier.width &&
            point.x + SEGMENT_SIZE > barrier.x &&
            point.y < barrier.y + barrier.height &&
            point.y + SEGMENT_SIZE > barrier.y) {
            return true;
        }
    }
    return false;
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const string &text, int x, int y) {
    SDL_Color textColor = {255, 255, 255, 255};
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
    srand(time(0));

    const int SCREEN_WIDTH = 640 * 2;
    const int SCREEN_HEIGHT = 480 * 2;
    const int INITIAL_SNAKE_LENGTH = 5;
    const int INITIAL_X = 100;
    const int INITIAL_Y = 100;
    const int SNAKE_SPEED = 20;
    const int BONUS_LIFE_TIME = 4000;  //for 4 sec bonus food

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "Error: SDL failed to initialize\nSDL Error: " << SDL_GetError() << '\n';
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Error: Failed to create window\nSDL Error: " << SDL_GetError() << '\n';
        SDL_Quit();
        return EXIT_FAILURE;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Error: Failed to create renderer\nSDL Error: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (TTF_Init() < 0) {
        cerr << "Error: TTF failed to initialize\nTTF Error: " << TTF_GetError() << '\n';
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
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

    vector<SnakeSegment> snake;
    Food food = {rand() % (SCREEN_WIDTH / SEGMENT_SIZE) * SEGMENT_SIZE,
                 rand() % (SCREEN_HEIGHT / SEGMENT_SIZE) * SEGMENT_SIZE};
    vector<Barrier> barriers = {
        {SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, SEGMENT_SIZE, SEGMENT_SIZE * 5},   
        {400, 300, SEGMENT_SIZE * 3, SEGMENT_SIZE}    
    };
    BonusFood bonusFood = {0, 0, false, 0};  // intializing timer for bonus food

    Direction currentDirection = Direction::RIGHT;
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
        SnakeSegment segment;
        segment.x = INITIAL_X - (i * SEGMENT_SIZE);
        segment.y = INITIAL_Y;
        snake.push_back(segment);
    }

    SDL_Event event;
    bool quit = false;
    int points = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (currentDirection != Direction::DOWN)
                            currentDirection = Direction::UP;
                        break;
                    case SDLK_DOWN:
                        if (currentDirection != Direction::UP)
                            currentDirection = Direction::DOWN;
                        break;
                    case SDLK_LEFT:
                        if (currentDirection != Direction::RIGHT)
                            currentDirection = Direction::LEFT;
                        break;
                    case SDLK_RIGHT:
                        if (currentDirection != Direction::LEFT)
                            currentDirection = Direction::RIGHT;
                        break;
                    default:
                        break;
                }
            }
        }

        if (!quit) {
            SnakeSegment newHead = {snake[0].x, snake[0].y};
            switch (currentDirection) {
                case Direction::UP:
                    newHead.y -= SNAKE_SPEED;
                    break;
                case Direction::DOWN:
                    newHead.y += SNAKE_SPEED;
                    break;
                case Direction::LEFT:
                    newHead.x -= SNAKE_SPEED;
                    break;
                case Direction::RIGHT:
                    newHead.x += SNAKE_SPEED;
                    break;
                default:
                    break;
            }

            if (newHead.x < 0)
                newHead.x = SCREEN_WIDTH - SEGMENT_SIZE;
            else if (newHead.x >= SCREEN_WIDTH)
                newHead.x = 0;

            if (newHead.y < 0)
                newHead.y = SCREEN_HEIGHT - SEGMENT_SIZE;
            else if (newHead.y >= SCREEN_HEIGHT)
                newHead.y = 0;

            if (checkCollision(newHead, snake) || checkCollision(newHead, barriers)) {
                quit = true;  // game ends
            }

            if (newHead.x == food.x && newHead.y == food.y) {
                points++;  // Increasing points afrer eating food
                food = {rand() % (SCREEN_WIDTH / SEGMENT_SIZE) * SEGMENT_SIZE,
                        rand() % (SCREEN_HEIGHT / SEGMENT_SIZE) * SEGMENT_SIZE};
                SnakeSegment tail = snake.back();
                for (int i = 0; i < 5; ++i) {
                    snake.push_back(tail);
                }

                if (points % 7 == 0) {
                    bonusFood = {rand() % (SCREEN_WIDTH / SEGMENT_SIZE) * SEGMENT_SIZE,
                                 rand() % (SCREEN_HEIGHT / SEGMENT_SIZE) * SEGMENT_SIZE,
                                 true,
                                 SDL_GetTicks()};  // set the timer for bonus food after reaching all 7 points 

                    Barrier barrier = {rand() % (100 / SEGMENT_SIZE) * SEGMENT_SIZE,
                                   rand() % (100 / SEGMENT_SIZE) * SEGMENT_SIZE,
                                   SEGMENT_SIZE * (rand() % 5 + 1),
                                   SEGMENT_SIZE * (rand() % 5 + 1)};
                    barriers.push_back(barrier);
                }

                // Barrier barrier = {rand() % (100 / SEGMENT_SIZE) * SEGMENT_SIZE,
                //                    rand() % (100 / SEGMENT_SIZE) * SEGMENT_SIZE,
                //                    SEGMENT_SIZE * (rand() % 5 + 1),
                //                    SEGMENT_SIZE * (rand() % 5 + 1)};
                // barriers.push_back(barrier);
            }

            // for checking collision with bonus food and check timer
            if (bonusFood.active && newHead.x == bonusFood.x && newHead.y == bonusFood.y) {
                points += 10;  // increase 10 points
                bonusFood = {0, 0, false, 0};  // resetting bonus food
            } else if (bonusFood.active && (SDL_GetTicks() - bonusFood.timer) > BONUS_LIFE_TIME) {
                bonusFood = {0, 0, false, 0};  // deactivate bonus food after a certain time
            }

            for (int i = snake.size() - 1; i > 0; --i) {
                snake[i] = snake[i - 1];
            }

            snake[0] = newHead;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect foodRect = {food.x, food.y, SEGMENT_SIZE, SEGMENT_SIZE};
            SDL_RenderFillRect(renderer, &foodRect);

            // show bonus food if active
            if (bonusFood.active) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // blue color for bonus food
                SDL_Rect bonusFoodRect = {bonusFood.x, bonusFood.y, SEGMENT_SIZE, SEGMENT_SIZE};
                SDL_RenderFillRect(renderer, &bonusFoodRect);
            }

            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            for (const auto &barrier : barriers) {
                SDL_Rect barrierRect = {barrier.x, barrier.y, barrier.width, barrier.height};
                SDL_RenderFillRect(renderer, &barrierRect);
            }

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            for (const auto &segment : snake) {
                SDL_Rect rect = {segment.x, segment.y, SEGMENT_SIZE, SEGMENT_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
            
            renderText(renderer, font, "Points: " + to_string(points), 10, 10);

            SDL_RenderPresent(renderer);

            SDL_Delay(100);
        }
    }
    auto newFont = TTF_OpenFont("E:\\Bungee_Spice\\BungeeSpice-Regular.ttf", 50);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderText(renderer, newFont, "Game Over - Points: " + to_string(points), SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2);
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
