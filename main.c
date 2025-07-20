#include <stdio.h>
#include <argp.h>
#include <math.h>
#include "raylib.h"
#include "raygui.h"
#include "../styles/jungle/style_jungle.h"
#include "../styles/genesis/style_genesis.h"

int WIDTH =  800;
int HEIGHT =  600;
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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_BORDERLESS_WINDOWED_MODE);
    InitWindow(WIDTH, HEIGHT, "Pixel ME");
    SetWindowMinSize(800, 600);
    SetWindowPosition(START_POS);
    SetTargetFPS(FPS);
    GuiLoadStyleGenesis();

    Image image_file = LoadImage(fileinfo.filename);
    Texture2D file_texture = LoadTextureFromImage(image_file);
    UnloadImage(image_file);


    while(!exit && !WindowShouldClose() ) {

        if (IsWindowResized()) {
            WIDTH = GetScreenWidth();
            HEIGHT = GetScreenHeight();
        }

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        DrawTextureEx(file_texture, (Vector2) { 0 }, 0, fmin(WIDTH / file_texture.width, HEIGHT / file_texture.height), WHITE);

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