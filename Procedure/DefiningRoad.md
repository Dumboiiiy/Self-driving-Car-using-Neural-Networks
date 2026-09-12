# Step 02: Defining the Road (C++ & Raylib)

The second step in building the self-driving car simulation is constructing the road environment. A well-defined road provides spatial constraints, designated driving lanes, visible boundary borders, and the foundational geometric infrastructure needed for future raycasting distance sensors and collision detection.

---

## 📁 Files Created & Modified in this Step

```text
Self_Driving_Car/
├── include/
│   ├── Utils.hpp               # [NEW] Math utility functions (Linear Interpolation)
│   ├── Road.hpp                # [NEW] Road class with lane geometry, infinite borders & rendering
│   ├── Controls.hpp            # Existing input handling
│   └── Car.hpp                 # Existing car physics and kinematics
├── main.cpp                    # [MODIFIED] Camera follow system, road integration, and lane spawning
├── Procedure/
│   ├── carDrivingMechanics.md  # Step 01 documentation
│   └── DefiningRoad.md         # [NEW] Step 02 documentation
└── build.sh                    # Build and run script
```

---

## 1. Mathematical Utilities (`include/Utils.hpp`)

### Purpose & Logic
Defines reusable mathematical helper functions required for geometric calculations across the simulation.

### Crucial Functions & Simulation Impact:
- **`Lerp(float A, float B, float t)`**:
  - Computes linear interpolation between start point $A$ and end point $B$ given fraction $t \in [0.0, 1.0]$ using the formula $A + (B - A) \cdot t$.
  - **Simulation Impact**: Essential for computing evenly spaced lane divider boundaries across the road width without hardcoding pixel values.

---

## 2. Road Environment & Geometry (`include/Road.hpp`)

### Purpose & Architecture
The `Road` class encapsulates the physical dimensions, lane counts, geometric border vectors, and visual rendering of the road. 

### Geometric Structure & Crucial Concepts:
- **`Segment` Structure**:
  - Encapsulates a 2D line segment using two `Vector2` endpoints (`start` and `end`).
  - Represents the structural borders of the road in world coordinates.
- **Infinite Vertical Boundaries (`top` & `bottom`)**:
  - The vertical limits of the road are set to extreme values ($-1,000,000$ to $+1,000,000$).
  - **Simulation Impact**: Because the car travels along the road over long continuous runs, the borders are treated as continuous infinite lines. These exact segment endpoints will later serve as target collision lines for the car's raycasting sensor system.
- **Border Vector Array (`borders`)**:
  - Stores the left boundary segment ($\text{topLeft} \to \text{bottomLeft}$) and right boundary segment ($\text{topRight} \to \text{bottomRight}$).
  - Storing borders as a collection of segments ensures modularity: curved roads, obstacles, or complex track segments can be added to this array in future steps without modifying the sensor or collision code.

### Member Functions:
- **`GetLaneCenter(int laneIndex)`**:
  - Computes the exact horizontal ($X$) center coordinate of any lane index (0-indexed from left to right).
  - Divides total road width by the lane count, adds half a lane width to the left road edge, and offsets by the requested lane index (clamped to the maximum available lane).
  - **Simulation Impact**: Allows spawning cars, traffic vehicles, or reset points perfectly aligned in the center of any chosen lane rather than guessing pixel coordinates.
- **`Draw(float viewCenterY, float viewHeight)`**:
  - **Optimized Viewport Culling**: Because the road mathematically spans millions of units, rendering the entire length each frame would degrade performance. The function calculates `visibleTop` and `visibleBottom` based on the car's current $Y$ position and only renders the active camera view.
  - **Stationary World-Grid Dash Synchronization**: Dashed lane dividers cycle with 20px dashes and 20px gaps. The starting dash position is locked to a multiple of the dash cycle ($40\text{px}$) in world space using `std::floor(visibleTop / cycle) * cycle`. This ensures lane markings remain completely stationary in the world as the car and camera move past them.
  - **Asphalt Surface & Solid Borders**: Renders the dark asphalt base rectangle between the left and right edges, along with solid boundary lines on outer borders.

---

## 3. Camera Tracking & Simulation Flow (`main.cpp`)

### Purpose & Structural Updates
Integrates the `Road` and `Car` objects and introduces a dynamic 2D viewport system that follows the vehicle.

### Crucial Additions & Simulation Impact:
- **Lane-Aligned Vehicle Initialization**:
  - The car is spawned at `(road.GetLaneCenter(1), 100.0f)` with dimensions $30 \times 50$, placing it directly in the middle lane of a 3-lane road.
- **2D Camera Tracking (`Camera2D`)**:
  - Uses Raylib's `Camera2D` system with an offset set to $(screenWidth / 2, screenHeight \times 0.7)$.
  - On every frame, `camera.target.y` is updated to match `car.y`.
  - **Simulation Impact**: Keeps the car positioned at a fixed $70\%$ height on screen while the world, road markings, and borders scroll smoothly beneath it, providing an intuitive perspective for self-driving navigation.
- **Spacious Layout & Neural Network Visualizer Canvas**:
  - Initializes a large $1280 \times 850$ window with standard title bar controls (minimize, maximize, close).
  - The road is rendered on the left portion of the screen, with the camera focused directly along the road axis.
  - The entire right section of the window ($x \ge 560\text{px}$) is framed and reserved for real-time telemetry, sensor readings, and the future Neural Network visualizer.
- **Reset Logic**:
  - Pressing <kbd>R</kbd> re-centers the car to `road.GetLaneCenter(1)` and zeroes out accumulated velocity and steering angles.

---

## 4. Code Architecture & Conventions

- **Header-Only Modularity**: Clean separation into `include/` with `#pragma once` guards prevents circular dependencies and simplifies incremental compilation.
- **Extensible Geometry**: Border segments are decoupled from rendering logic, preparing the data structures for direct input into Ray-Casting sensors in the next step.
- **Separation of Concerns**: `Road` manages environmental bounds, `Controls` manages input states, and `Car` manages motion dynamics.
