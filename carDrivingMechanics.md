# Car Driving Mechanics (C++ & Raylib)

This document details the architecture, math, physics, and implementation of the 2D car driving simulation converted from JavaScript into C++ using [Raylib](https://www.raylib.com/).

---

## 📁 Project Structure

```text
Self_Driving_Car/
├── include/
│   ├── Controls.hpp            # Input handling and control scheme abstraction
│   └── Car.hpp                 # Car kinematics, physics simulation, and rendering
├── main.cpp                    # Game loop, canvas grid, HUD telemetry, and execution
├── build.sh                    # Build and run script for MSYS2 / UCRT64 / MinGW
└── carDrivingMechanics.md      # Documentation of implemented driving mechanics
```

---

## 1. `include/Controls.hpp`

### Purpose
Abstracts input control from the car itself. This separation allows the same `Car` class to be driven by a human via keyboard or later by artificial intelligence (Neural Networks) and dummy traffic scripts.

### Code Breakdown
```cpp
#pragma once
#include "raylib.h"

// Control types allow switching between keyboard control, AI, or dummy traffic in future steps
enum class ControlType {
    KEYBOARD,
    AI,
    DUMMY
};

class Controls {
public:
    bool forward;
    bool left;
    bool right;
    bool reverse;
    ControlType type;

    Controls(ControlType controlType = ControlType::KEYBOARD)
        : forward(false), left(false), right(false), reverse(false), type(controlType) {}

    void Update() {
        switch (type) {
            case ControlType::KEYBOARD:
                forward = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
                reverse = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
                left    = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
                right   = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
                break;
            case ControlType::AI:
            case ControlType::DUMMY:
                // Reserved for AI Neural Network outputs or dummy pathing
                break;
        }
    }

    void Reset() {
        forward = false;
        reverse = false;
        left    = false;
        right   = false;
    }
};
```

### Key Elements:
- **`ControlType` Enum**: Defines the driving agent (`KEYBOARD`, `AI`, or `DUMMY`).
- **Dual Key Mapping**: Supports both **Arrow Keys** and **W/A/S/D** via Raylib's `IsKeyDown()`.
- **`Reset()`**: Quickly clears all active input states when resetting the simulation.

---

## 2. `include/Car.hpp`

### Purpose
Implements the 2D vehicle physics model, including acceleration, top speeds, linear friction, dynamic reverse-steering mechanics, and trigonometric displacement.

### Code Breakdown
```cpp
#pragma once
#include <cmath>
#include "raylib.h"
#include "Controls.hpp"

class Car {
public:
    float x;
    float y;
    float width;
    float height;

    float speed;
    float acceleration;
    float maxSpeed;
    float friction;
    float angle; // in radians

    Controls controls;
    Color color;
    bool damaged;

    Car(float x, float y, float width = 30.0f, float height = 50.0f,
        ControlType controlType = ControlType::KEYBOARD, float maxSpeed = 3.0f,
        Color color = BLACK)
        : x(x), y(y), width(width), height(height),
          speed(0.0f), acceleration(0.2f), maxSpeed(maxSpeed),
          friction(0.05f), angle(0.0f),
          controls(controlType), color(color), damaged(false) {}

    void Update() {
        if (!damaged) {
            Move();
        }
    }

    void Draw() const {
        // Draw car body centered at (x, y) with rotation (in degrees)
        Rectangle rec = { x, y, width, height };
        Vector2 origin = { width / 2.0f, height / 2.0f };
        float rotationDegrees = -angle * RAD2DEG;

        DrawRectanglePro(rec, origin, rotationDegrees, color);
    }

    // Reset car position, velocity and orientation
    void Reset(float startX, float startY) {
        x = startX;
        y = startY;
        speed = 0.0f;
        angle = 0.0f;
        damaged = false;
        controls.Reset();
    }

private:
    void Move() {
        controls.Update();

        if (controls.forward) {
            speed += acceleration;
        }
        if (controls.reverse) {
            speed -= acceleration;
        }

        // Clamp speed
        if (speed > maxSpeed) {
            speed = maxSpeed;
        }
        if (speed < -maxSpeed / 2.0f) {
            speed = -maxSpeed / 2.0f;
        }

        // Apply friction
        if (speed > 0.0f) {
            speed -= friction;
        }
        if (speed < 0.0f) {
            speed += friction;
        }
        if (std::abs(speed) < friction) {
            speed = 0.0f;
        }

        // Steering logic (angular change dependent on speed direction)
        if (speed != 0.0f) {
            float flip = (speed > 0.0f) ? 1.0f : -1.0f;
            if (controls.left) {
                angle += 0.03f * flip;
            }
            if (controls.right) {
                angle -= 0.03f * flip;
            }
        }

        // Update position based on heading and speed
        x -= std::sin(angle) * speed;
        y -= std::cos(angle) * speed;
    }
};
```

### Physics & Mathematics Explained:
1. **Acceleration & Speed Clamping**:
   - Forward acceleration adds `0.2` to `speed` per frame up to `maxSpeed` (`3.0`).
   - Reverse acceleration subtracts `0.2` down to a reverse limit of `-maxSpeed / 2.0` (`-1.5`).
2. **Friction & Zero Cutoff**:
   - Constant linear drag (`friction = 0.05`) decelerates moving cars towards `0.0`.
   - `std::abs(speed) < friction` prevents speed oscillation around zero.
3. **Realistic Steering (Reverse Flip)**:
   - When driving forward (`speed > 0`), steering left increases `angle` counter-clockwise.
   - When reversing (`speed < 0`), `flip = -1.0` inverts the steering direction so the rear follows the wheels naturally.
4. **Trigonometric Coordinate Integration**:
   - Screen coordinate system has `(0, 0)` at top-left with $+Y$ pointing downward.
   - At `angle = 0`, moving forward moves up ($-Y$ direction):
     $$\Delta y = -\cos(\text{angle}) \times \text{speed}$$
     $$\Delta x = -\sin(\text{angle}) \times \text{speed}$$
5. **Raylib Pro Rendering**:
   - `DrawRectanglePro` uses a pivot `origin = { width/2, height/2 }` so the car rotates about its center.
   - Raylib expects degrees clockwise, while radians counter-clockwise is represented by `-angle * RAD2DEG`.

---

## 3. `main.cpp`

### Purpose
Initializes the Raylib graphics window, sets up the 60 FPS update loop, draws a visual background grid to provide motion perspective, and renders real-time telemetry HUD.

### Code Breakdown
```cpp
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
```

### Features:
- **Motion Grid**: Visual guide lines spaced at 40px intervals to clearly show car speed and heading.
- **HUD Panel**: Displays active FPS, current speed vs max speed, angle (both radians and degrees), and $(X, Y)$ world coordinates.
- **Interactive Reset**: Pressing <kbd>R</kbd> re-centers the car and zeroes all velocity.

---

## 4. `build.sh`

### Purpose
Automates compiling and running via MSYS2 UCRT64 / MinGW.

```bash
SRC=${1:-main.cpp}

g++ $SRC \
-I. \
-I./include \
-I/c/Users/Nachiket/Downloads/Dev/raylib-5.5/raylib-5.5/src \
-L/c/Users/Nachiket/Downloads/Dev/raylib-5.5/raylib-5.5/src \
-lraylib -lopengl32 -lgdi32 -lwinmm -lm \
-o app.exe

./app.exe
```

### Build & Run Commands (UCRT64):
```bash
./build.sh main.cpp
```
