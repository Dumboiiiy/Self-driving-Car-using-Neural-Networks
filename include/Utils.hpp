#pragma once

// Linear interpolation between value A and B using interpolant t (0.0 to 1.0)
inline float Lerp(float A, float B, float t) {
    return A + (B - A) * t;
}
