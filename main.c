#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "raylib.h"
#include "raygui.h"
#include "../styles/jungle/style_jungle.h"
#include "../styles/genesis/style_genesis.h"
#include "args.c"

#define DEFAULT_SIZE 32
#define START_POS 400, 400
#define BOX_WIDTH 250
#define BOX_HEIGHT 250
#define HIST_SIZE 10000
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

int FPS    = 120;
int WIDTH  = 800;
int HEIGHT = 600;

typedef enum EditorMode {
    EDITOR_MAIN,
    EDITOR_INFO,
    EDITOR_COLOR
} EditorMode;

void toggle(EditorMode * mode, EditorMode t) {
    *mode &= t;
    *mode ^= t;
}

typedef struct Change {
    Color old_color;
    int x;
    int y;
} Change;

struct {
    EditorMode editor_mode;
    Color current_color;
    bool should_exit;
    Image image;
    bool image_changed;
    Change history[HIST_SIZE + 1];
} state = {
    .editor_mode = EDITOR_MAIN,
    .current_color = BLACK,
    .should_exit = false,
    .image_changed = false,
};


char info_buffer[200];

Vector2 center_box(int width, int height) {
    return (Vector2) {
       .x = WIDTH / 2 - width / 2,
       .y = HEIGHT / 2 - height / 2 
    };
}

void show_info(Rectangle box_bounds, char * info_message) {
    switch(GuiMessageBox(
            box_bounds,
            "Info",
            info_message,
            "Exit;Back"
    )) {
        case 1:
            state.should_exit = true;
            break;
        case 2: case 0:
            toggle(&(state.editor_mode), EDITOR_INFO);
            break;
    } 
}

bool IsControlDown() {
    return (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL));
}
    
void update_info() {
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
}

int main(int argc, char** argv) {

    if (process_args(argc, argv)) {
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


    if (FileExists(command_options.path)) {
        state.image = LoadImage(command_options.path);
        command_options.width = state.image.width;
        command_options.height= state.image.height;
    } else {
        if (!command_options.size_set) {
            printf("File size not set, defaulting to %dx%d", DEFAULT_SIZE, DEFAULT_SIZE);
            command_options.width = DEFAULT_SIZE;
            command_options.height = DEFAULT_SIZE;
        }
        state.image = GenImageColor(command_options.width, command_options.height, WHITE);
    }

    Texture2D texture = LoadTextureFromImage(state.image);

    while(!state.should_exit && !WindowShouldClose()) {


        if (IsWindowResized()) {
            WIDTH = GetScreenWidth();
            HEIGHT = GetScreenHeight();
        }

        if (IsControlDown()) {
            if (IsKeyPressed(KEY_S)) {
                ExportImage(state.image, command_options.path);
            }
            if (IsKeyPressed(KEY_I)) {
                toggle(&state.editor_mode, EDITOR_INFO);
            }
            if (IsKeyPressed(KEY_C)) {
                toggle(&state.editor_mode, EDITOR_COLOR);
            }
        }

        int image_scale = min(WIDTH / state.image.width, HEIGHT / state.image.height);

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        if (state.image_changed) {
            texture = LoadTextureFromImage(state.image);
            state.image_changed = false;
        }

        DrawTextureEx(texture, (Vector2) { 0 }, 0, image_scale, WHITE);
        Rectangle box_bounds = (Rectangle) { 
            center_box(BOX_WIDTH, BOX_HEIGHT).x,
            center_box(BOX_WIDTH, BOX_HEIGHT).y,
            BOX_WIDTH,
            BOX_HEIGHT
        };
        switch(state.editor_mode) {
            case EDITOR_INFO: {
                DrawRectangle(0, 0, WIDTH, HEIGHT, (Color) {.a = 0x44, .r = 0xDD, .g = 0xDD, .b = 0xDD});
                update_info();
                show_info(box_bounds, info_buffer);
                break;
            }
            case EDITOR_COLOR: {
                DrawRectangle(0, 0, WIDTH, HEIGHT, (Color) {.a = 0x44, .r = 0xDD, .g = 0xDD, .b = 0xDD});
                GuiColorPicker(
                    box_bounds,
                    "Pick color",
                    &(state.current_color)
                );
                break;
            } 
            case EDITOR_MAIN: {
                Vector2 mouse_pos = GetMousePosition();
                int cell_x = mouse_pos.x / image_scale;
                int cell_y = mouse_pos.y / image_scale;

                Rectangle rect_pos = (Rectangle) {
                    image_scale * cell_x,
                    image_scale * cell_y,
                    image_scale,
                    image_scale,
                };

                if (rect_pos.x < state.image.width * image_scale 
                    && rect_pos.y < state.image.height * image_scale) 
                    DrawRectangleLinesEx(rect_pos, max(image_scale / 20, 1), state.current_color);

                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    if (!ColorIsEqual(GetImageColor(state.image, cell_x, cell_y), state.current_color)) {
                        memcpy(state.history + 1, state.history, sizeof(Change) * (HIST_SIZE - 1));
                        state.history[0] = (Change) {.old_color = GetImageColor(state.image, cell_x, cell_y), .x = cell_x, .y = cell_y };
                        ImageDrawPixel(&state.image, cell_x, cell_y, state.current_color);
                        state.image_changed = true;
                    }
                } 
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    state.current_color = GetImageColor(state.image, cell_x, cell_y);
                }
                if (IsKeyPressed(KEY_Z) && IsControlDown()) {
                    ImageDrawPixel(&state.image, state.history[0].x, state.history[0].y, state.history[0].old_color);
                    memcpy(state.history, state.history + 1, sizeof(Change) * (HIST_SIZE));
                    state.image_changed = true;
                }
                break;
            }
        }
        EndDrawing();
    }

    CloseWindow();
}
