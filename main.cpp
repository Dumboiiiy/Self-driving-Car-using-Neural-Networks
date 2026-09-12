#include <iostream>
#include "raylib.h"
#include "include/Car.hpp"
#include "include/Road.hpp"

int main() {
    // Large, spacious window with title bar and standard minimize/close buttons
    const int screenWidth = 1280;
    const int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "Self-Driving Car Simulation - Step 02: Defining Road");
    SetTargetFPS(60);

    // Place the road on the left side to reserve the right side for the Neural Network visualizer
    const float roadCenterX = 280.0f;
    const float roadWidth = 240.0f;
    Road road(roadCenterX, roadWidth, 3);

    // Initialize car in the middle lane (lane index 1) at y = 100
    Car car(road.GetLaneCenter(1), 100.0f, 30.0f, 50.0f);

    // Setup 2D camera focused on the road column, keeping car at 70% of screen height
    Camera2D camera = { 0 };
    camera.target = Vector2{ road.x, car.y };
    camera.offset = Vector2{ road.x, screenHeight * 0.7f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        // Update car physics and steering
        car.Update();

        // Follow car vertically along the road
        camera.target.y = car.y;

        // Reset car position if 'R' is pressed
        if (IsKeyPressed(KEY_R)) {
            car.Reset(road.GetLaneCenter(1), 100.0f);
        }

        // Render Frame
        BeginDrawing();
        ClearBackground(Color{ 30, 30, 30, 255 }); // Environment terrain background

        // World-space rendering (Road and Car viewport)
        BeginMode2D(camera);
        {
            road.Draw(car.y, static_cast<float>(screenHeight));
            car.Draw();
        }
        EndMode2D();

        // -------------------------------------------------------------
        // Right Side: Neural Network & Telemetry Dashboard Area
        // -------------------------------------------------------------
        const int panelX = 560;
        const int panelWidth = screenWidth - panelX - 20;

        // Dashboard background container
        DrawRectangle(panelX, 20, panelWidth, screenHeight - 80, Fade(DARKGRAY, 0.4f));
        DrawRectangleLines(panelX, 20, panelWidth, screenHeight - 80, Fade(GRAY, 0.6f));

        // Telemetry section
        DrawText("TELEMETRY & SENSORS", panelX + 20, 40, 20, YELLOW);
        DrawFPS(panelX + 20, 75);
        DrawText(TextFormat("Speed:    %.2f / %.2f", car.speed, car.maxSpeed), panelX + 20, 105, 18, RAYWHITE);
        DrawText(TextFormat("Angle:    %.2f rad (%.1f deg)", car.angle, car.angle * RAD2DEG), panelX + 20, 135, 18, RAYWHITE);
        DrawText(TextFormat("Position: (X: %.1f, Y: %.1f)", car.x, car.y), panelX + 20, 165, 18, RAYWHITE);
        DrawText(TextFormat("Lanes:    %d (Width: %.0fpx)", road.laneCount, road.width), panelX + 20, 195, 18, RAYWHITE);

        // Neural Network placeholder section
        DrawRectangle(panelX + 20, 240, panelWidth - 40, screenHeight - 340, Fade(BLACK, 0.5f));
        DrawRectangleLines(panelX + 20, 240, panelWidth - 40, screenHeight - 340, Fade(DARKGRAY, 0.8f));
        DrawText("NEURAL NETWORK VISUALIZER", panelX + 40, 260, 18, SKYBLUE);
        DrawText("(Reserved for Brain / Neural Network Nodes)", panelX + 40, 290, 14, GRAY);

        // Bottom control helper
        DrawRectangle(10, screenHeight - 45, screenWidth - 20, 32, Fade(DARKGRAY, 0.85f));
        DrawText("Controls: [Arrow Keys / WASD] Drive | [R] Reset Position | [Esc] Exit", 20, screenHeight - 38, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}