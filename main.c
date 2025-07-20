#include <stdio.h>
#include <argp.h>
#include "raylib.h"
#include "raygui.h"
#include "../styles/jungle/style_jungle.h"

#define WIDTH 800
#define HEIGHT 600
#define START_POS 400, 400
#define FPS 60

struct {
    char * filename;
    int filewidth;
    int fileheight;
} fileinfo;

static int parse_opt(int key, char *arg, struct argp_state *state) {
    switch(key) {
        case 'd': printf("%s\n", arg); break;
        case ARGP_KEY_ARG: {
            if (fileinfo.filename != NULL) {
                argp_failure(state, 1, 0, "too many files");
            } else {
                fileinfo.filename = arg;
            }
        }
        break;
        case ARGP_KEY_END: {
            if (fileinfo.filename == NULL) {
                argp_failure(state, 1, 0, "no file specified");
            }
        }
    }
    return 0;
}

struct argp_option options[] = {
    { "dim", 'd', "width:height", 0, "Specify canvas size"},
    { "size", 's', "width:height", OPTION_ALIAS},
    { 0 }
};

int main(int argc, char** argv) {

    struct argp argp = { options, parse_opt, "filename" };

    if(argp_parse(&argp, argc, argv, 0, 0, 0)) {
        return -1;
    }

    bool exit = false;
    bool start = true;
    InitWindow(WIDTH, HEIGHT, "Pixel ME");
    SetWindowPosition(START_POS);
    SetTargetFPS(FPS);

    GuiLoadStyleJungle();

    while(!exit && !WindowShouldClose() ) {

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        if (start) {
            switch(GuiMessageBox(
                    (Rectangle) { 100, 100, 600, 400 },
                    "Hello",
                    fileinfo.filename,
                    "Exit;Start"
            )) {
                case 1:
                    exit = true;
                    break;
                case 2: case 0:
                    start = false;
                    break;
            } 
        }

        EndDrawing();
    }
    CloseWindow();
}