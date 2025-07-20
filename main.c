#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include "../styles/jungle/style_jungle.h"



int main() {
    InitWindow(800, 600, "Pixel ME");
    SetTargetFPS(60);

    GuiLoadStyleJungle();

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        GuiMessageBox(
            (Rectangle){ 100, 100, 600, 400 },
            "Hello",
            "Welcome",
            "Exit;Start"
        );
        EndDrawing();
    }
    CloseWindow();
}