#include <iostream>
#include "raylib.h"
#include "include/Car.hpp"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Self-Driving Car Simulation");
    SetTargetFPS(60);

    // Initialize car at the center of the screen
    Car car(screenWidth / 2.0f, screenHeight / 2.0f, 30.0f, 50.0f);

    while (!WindowShouldClose()) {
        // Update car physics and controls
        car.Update();

        // Reset car position if 'R' is pressed
        if (IsKeyPressed(KEY_R)) {
            car.Reset(screenWidth / 2.0f, screenHeight / 2.0f);
        }

        // Draw
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        // Draw a subtle grid background for motion reference
        const int gridSize = 40;
        for (int x = 0; x < screenWidth; x += gridSize) {
            DrawLine(x, 0, x, screenHeight, Fade(GRAY, 0.3f));
        }
        for (int y = 0; y < screenHeight; y += gridSize) {
            DrawLine(0, y, screenWidth, y, Fade(GRAY, 0.3f));
        }

        // Render the car
        car.Draw();

        // Render HUD / Telemetry info
        DrawRectangle(10, 10, 240, 110, Fade(DARKGRAY, 0.8f));
        DrawRectangleLines(10, 10, 240, 110, BLACK);
        DrawFPS(20, 20);
        DrawText(TextFormat("Speed: %.2f / %.2f", car.speed, car.maxSpeed), 20, 45, 16, RAYWHITE);
        DrawText(TextFormat("Angle: %.2f rad (%.1f deg)", car.angle, car.angle * RAD2DEG), 20, 65, 16, RAYWHITE);
        DrawText(TextFormat("Pos: (%.1f, %.1f)", car.x, car.y), 20, 85, 16, RAYWHITE);

        // Render controls guide at the bottom
        DrawRectangle(10, screenHeight - 40, screenWidth - 20, 30, Fade(DARKGRAY, 0.8f));
        DrawText("Controls: [Arrow Keys] or [W/A/S/D] to Drive | [R] to Reset Position", 20, screenHeight - 33, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}