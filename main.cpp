#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>

#define POLLING_DELAY 50
#define EXIT_BUTTON 9

int main(void) {

    bool quit = false;
    bool deviceFound = false;
    SDL_Event e;
    int num_joysticks = 0;
    SDL_Joystick* activeJoystick = nullptr;
    SDL_JoystickID activeJoystickID;
    auto joysticks = std::vector<SDL_Joystick*>();

    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        printf("SDL initialization failed.  Error: %s\n", SDL_GetError());
    }

    num_joysticks = SDL_NumJoysticks();
    for (int i = 0; i < num_joysticks; i++) {
        auto joystick = SDL_JoystickOpen(i);
        if (joystick == nullptr) {
            printf("Warning: unable to open game controller %d!  SDL Error: %s\n", i, SDL_GetError());
        }
        else {
            joysticks.push_back(joystick);
        }
    }

    // Detect first gamepad on which a key is pressed
    while (!deviceFound && !quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_JOYDEVICEADDED) {
                auto joystick = SDL_JoystickOpen(e.jdevice.which);
                if (joystick == nullptr) {
                    printf("Warning: unable to open game controller %d!  SDL Error: %s\n", e.jdevice.which, SDL_GetError());
                }
                else {
                    joysticks.push_back(joystick);
                }
            }
            else if (e.type == SDL_JOYDEVICEREMOVED) {
                auto joystick = SDL_JoystickFromInstanceID(e.jdevice.which);
                SDL_JoystickClose(joystick);
                std::cout << "Joystick removed: " << SDL_JoystickName(joystick);
            }
            else if (e.type == SDL_JOYBUTTONDOWN) {
                activeJoystick = SDL_JoystickFromInstanceID(e.jbutton.which);
                deviceFound = true;
                auto name = SDL_JoystickName(activeJoystick);
                std::cout << "Joystick selected: " << name << "<" << e.jbutton.which << ">" << std::endl;
            }
        }
        SDL_Delay(POLLING_DELAY);
    }

    // Stop listening to all other joysticks;
    activeJoystickID = SDL_JoystickInstanceID(activeJoystick);
    auto it = joysticks.begin();
    while (it != joysticks.end()) {
        if (SDL_JoystickInstanceID(*it) != activeJoystickID) {
            it = joysticks.erase(it);
        }
        else {
            it++;
        }
    }

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_JOYBUTTONUP) {
                if (e.jbutton.button == EXIT_BUTTON) {
                    printf("QUIT!\n");
                    quit = true;
                }
            }
        }
        SDL_Delay(POLLING_DELAY);
    }

    SDL_JoystickClose(activeJoystick);
    SDL_Quit();
    return 0;
}
