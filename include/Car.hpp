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
