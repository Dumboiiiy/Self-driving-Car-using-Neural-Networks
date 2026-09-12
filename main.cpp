#include <bits/stdc++.h>
#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "My First Raylib App");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Hello, Raylib!", 300, 200, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}