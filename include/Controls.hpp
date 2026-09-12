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
