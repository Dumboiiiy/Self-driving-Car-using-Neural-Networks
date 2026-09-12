#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include "raylib.h"
#include "Utils.hpp"

// Represents a 2D line segment between two points
struct Segment {
    Vector2 start;
    Vector2 end;
};

class Road {
public:
    float x;
    float width;
    int laneCount;

    float left;
    float right;
    float top;
    float bottom;

    std::vector<Segment> borders;

    Road(float x, float width, int laneCount = 3)
        : x(x), width(width), laneCount(laneCount) {
        left = x - width / 2.0f;
        right = x + width / 2.0f;

        const float infinity = 1000000.0f;
        top = -infinity;
        bottom = infinity;

        Vector2 topLeft = { left, top };
        Vector2 bottomLeft = { left, bottom };
        Vector2 topRight = { right, top };
        Vector2 bottomRight = { right, bottom };

        borders = {
            { topLeft, bottomLeft },
            { topRight, bottomRight }
        };
    }

    // Calculates the horizontal center coordinate for a specific lane index (0-indexed)
    float GetLaneCenter(int laneIndex) const {
        float laneWidth = width / laneCount;
        int clampedLane = std::min(laneIndex, laneCount - 1);
        return left + laneWidth / 2.0f + clampedLane * laneWidth;
    }

    // Renders the road surface, dashed lane dividers, and solid boundary borders
    void Draw(float viewCenterY = 0.0f, float viewHeight = 800.0f) const {
        float visibleTop = viewCenterY - viewHeight;
        float visibleBottom = viewCenterY + viewHeight;

        // 1. Draw asphalt road background
        DrawRectangleRec(
            Rectangle{ left, visibleTop, width, visibleBottom - visibleTop },
            Color{ 50, 50, 50, 255 }
        );

        // 2. Draw dashed lane dividers
        const float dashLength = 20.0f;
        const float gapLength = 20.0f;
        const float cycle = dashLength + gapLength;
        const float lineWidth = 5.0f;

        // Synchronize starting Y with world grid so dashes stay stationary during camera movement
        float startY = std::floor(visibleTop / cycle) * cycle;

        for (int i = 1; i <= laneCount - 1; i++) {
            float laneX = Lerp(left, right, static_cast<float>(i) / laneCount);

            for (float y = startY; y < visibleBottom; y += cycle) {
                DrawLineEx(
                    Vector2{ laneX, y },
                    Vector2{ laneX, y + dashLength },
                    lineWidth,
                    WHITE
                );
            }
        }

        // 3. Draw solid outer borders
        for (const auto& border : borders) {
            DrawLineEx(
                Vector2{ border.start.x, visibleTop },
                Vector2{ border.end.x, visibleBottom },
                lineWidth,
                WHITE
            );
        }
    }
};
