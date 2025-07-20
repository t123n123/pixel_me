#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raygui.h"
#include "../styles/jungle/style_jungle.h"
#include "../styles/genesis/style_genesis.h"

#define DEFAULT_SIZE 32
#define START_POS 400, 400
#define BOX_WIDTH 250
#define BOX_HEIGHT 150

int FPS    = 60;
int WIDTH  = 800;
int HEIGHT = 600;

Vector2 center_box(int width, int height) {
    return (Vector2) {
       .x = WIDTH / 2 - width / 2,
       .y = HEIGHT / 2 - height / 2 
    };
}

struct {
    char * path;
    int width;
    int height;
    int size_set;
} command_options;
    
Image image_file;

static int parse_opt(int key, char *arg, struct argp_state *state) {
    switch(key) {
        case 'd': { 
            char *sep = strchr(arg, ':');
            *sep = '\0';
            command_options.width = atoi(arg);
            command_options.height = atoi(sep + 1);
            command_options.size_set = true;
        }
        break;
        case ARGP_KEY_ARG: {
            if (command_options.path != NULL) {
                argp_failure(state, 1, 0, "too many files");
            } else {
                command_options.path = arg;
            }
        }
        break;
        case ARGP_KEY_END: {
            if (command_options.path == NULL) {
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
    bool info_show = false;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE );
    InitWindow(WIDTH, HEIGHT, "Pixel ME");
    SetWindowMinSize(800, 600);
    SetWindowPosition(START_POS);
    SetTargetFPS(FPS);
    GuiLoadStyleGenesis();
    SetExitKey(KEY_Q);

    char info_buffer[200];

    if (FileExists(command_options.path)) {
        image_file = LoadImage(command_options.path);
        command_options.width = image_file.width;
        command_options.height= image_file.height;
    } else {
        if (!command_options.size_set) {
            printf("File size not set, defaulting to %dx%d", DEFAULT_SIZE, DEFAULT_SIZE);
            command_options.width = DEFAULT_SIZE;
            command_options.height = DEFAULT_SIZE;
        }
        image_file = GenImageColor(command_options.width, command_options.height, WHITE);
    }

    Texture2D file_texture = LoadTextureFromImage(image_file);

    if (strlen(command_options.path) > 25) {
        sprintf(info_buffer, "File: ...%s\nSize: %d x %d", 
            command_options.path + strlen(command_options.path) - 25,
            command_options.width,
            command_options.height
        );
    } else {
        sprintf(info_buffer, "File: %s\nSize: %d x %d", 
            command_options.path,
            command_options.width,
            command_options.height
        );
    }

    while(!exit && !WindowShouldClose()) {

        if (IsWindowResized()) {
            WIDTH = GetScreenWidth();
            HEIGHT = GetScreenHeight();
        }

        if (IsKeyPressed(KEY_S) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            ExportImage(image_file, command_options.path);
        }

        if (IsKeyPressed(KEY_I) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            info_show ^= 1;
        }

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        DrawTextureEx(file_texture, (Vector2) { 0 }, 0, fmin(WIDTH / file_texture.width, HEIGHT / file_texture.height), WHITE);

        if (info_show) {
            switch(GuiMessageBox(
                    (Rectangle) { 
                        center_box(BOX_WIDTH, BOX_HEIGHT).x,
                        center_box(BOX_WIDTH, BOX_HEIGHT).y,
                        BOX_WIDTH,
                        BOX_HEIGHT
                    },
                    "Hello",
                    info_buffer,
                    "Exit;Back"
            )) {
                case 1:
                    exit = true;
                    break;
                case 2: case 0:
                    info_show = false;
                    break;
            } 
        }

        EndDrawing();
    }

    CloseWindow();
}