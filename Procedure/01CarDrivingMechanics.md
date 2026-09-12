# Step 01: Car Driving Mechanics (C++ & Raylib)

The first step in creating a self-driving car is to create a 2d car in a canvas and be able to drive it using keyboard. Adding realistic physics is the key to a realistic simulation and adding controls is key to making it drivable.

---

## 📁 Project Structure

```text
Self_Driving_Car/
├── include/
│   ├── Controls.hpp            # Input handling and control scheme abstraction
│   └── Car.hpp                 # Car kinematics, physics simulation, and rendering
├── main.cpp                    # Game loop, canvas grid, HUD telemetry, and execution
├── Procedure/
│   └── carDrivingMechanics.md  # Step 01 documentation
└── build.sh                    # Build and run script for MSYS2 / UCRT64
```

---

## 1. `include/Controls.hpp`

### Purpose
Abstracts input control from the car itself. This separation allows the same `Car` class to be driven by a human via keyboard or later by artificial intelligence (Neural Networks) and dummy traffic scripts.

### Key Elements:
- **`ControlType` Enum**: Defines the controlling entity (`KEYBOARD`, `AI`, or `DUMMY`), allowing seamless switching of input sources without modifying car physics.
- **Directional State Flags**: Encapsulates four boolean flags (`forward`, `reverse`, `left`, `right`) representing active control inputs.
- **`Update()` Method**: When set to `ControlType::KEYBOARD`, polls real-time hardware input via Raylib's `IsKeyDown()`, supporting both **Arrow Keys** (`KEY_UP`, `KEY_DOWN`, `KEY_LEFT`, `KEY_RIGHT`) and **WASD** (`KEY_W`, `KEY_S`, `KEY_A`, `KEY_D`).
- **`Reset()` Method**: Clears all active directional states during simulation restarts.

---

## 2. `include/Car.hpp`

### Purpose
Implements the 2D vehicle kinematics and physics model, including forward/reverse acceleration limits, linear friction, dynamic angular steering, trigonometric displacement, and rotated visual rendering.

### Physics & Mathematics Explained:
1. **Kinematic Attributes & Constraints**:
   - **Position & Dimensions**: Represented by coordinates $(x, y)$ and physical dimensions $(width, height)$.
   - **Velocity & Acceleration**: Speed increments by $0.2$ per frame up to a maximum forward speed of $3.0$, and decrements in reverse to a clamped limit of $-1.5$ (half of `maxSpeed`).
2. **Linear Friction & Threshold Clamping**:
   - Applies a constant resistive force of $0.05$ per frame opposite to the direction of motion.
   - When the absolute speed falls below the friction constant ($|speed| < friction$), speed is locked to $0.0$ to eliminate sub-pixel jitter.
3. **Realistic Steering Dynamics (Reverse Flip)**:
   - Steering is only applied when the vehicle is in motion ($speed \ne 0$).
   - A directional multiplier $flip = (speed > 0) \,?\, 1.0 \,:\, -1.0$ is computed:
     - Turning left increases the heading angle $\theta$ counter-clockwise.
     - In reverse, $flip = -1.0$ inverts the turning angle so the rear swings naturally as in real automotive steering.
4. **Trigonometric Coordinate Integration**:
   - In 2D screen space, $(0, 0)$ is top-left and moving forward at $\theta = 0$ corresponds to moving up (negative $Y$).
   - On every frame, displacements are computed via trigonometry:
     $$\Delta x = -\sin(\theta) \times speed$$
     $$\Delta y = -\cos(\theta) \times speed$$
5. **Centered Pivot Pro-Rendering (`Draw()`)**:
   - Uses Raylib's `DrawRectanglePro()` with the origin set to $\{ width/2, height/2 \}$ to rotate the rectangle about its geometric center.
   - Converts counter-clockwise radian angles into Raylib's clockwise rotation degrees:
     $$\text{rotationDegrees} = -\theta \times \text{RAD2DEG}$$
6. **State Reset (`Reset()`)**:
   - Resets spatial coordinates to $(startX, startY)$, zeroes velocity and heading angle, and clears damaged flags.

---

## 3. `main.cpp`

### Purpose
Initializes the Raylib graphics window, establishes the 60 FPS update loop, draws a visual background grid to provide motion perspective, and renders real-time telemetry HUD.

### Features:
- **Motion Reference Grid**: Renders subtle grid lines across the canvas at 40px intervals to give immediate visual feedback of speed and trajectory.
- **HUD Telemetry Overlay**: Displays active frame rate (`DrawFPS`), forward/reverse speed vs max speed, heading angle (in both radians and degrees), and $(X, Y)$ world coordinates.
- **Simulation Reset**: Monitors the <kbd>R</kbd> key to instantly reposition the vehicle to the canvas center and zero all velocities.

---

## 4. `build.sh`

### Purpose
Automates compilation via MSYS2 / UCRT64 using `g++`, linking Raylib alongside core Windows system graphics and multimedia dependencies (`-lopengl32 -lgdi32 -lwinmm -lm`), and including the local `./include` directory.

### Build & Run Commands (UCRT64):
```bash
./build.sh main.cpp
```
