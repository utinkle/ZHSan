#pragma once

#include <SDL.h>
#include <chrono>
#include <optional>
#include <vector>
#include "platform/Types.h"

namespace GameManager {

/// <summary>
/// InputManager handles all keyboard, mouse, and touch input.
/// Mirrors C# GameManager.InputManager (495 lines).
/// Maps SDL2 events to XNA/MonoGame input model.
/// </summary>
class InputManager {
public:
    // Mouse/Touch position tracking
    static Vector2 PositionPre;      // Previous frame position
    static Vector2 Position;         // Current frame position
    static float OriginPoX;          // Raw screen X before scaling
    static float OriginPoY;          // Raw screen Y before scaling
    static int PoX;                  // Scaled X position
    static int PoY;                  // Scaled Y position

    // Scaling factors
    static Vector2 Scale1;           // Menu scaling
    static Vector2 Scale2;           // Game scaling
    static Vector2 ScaleOne;         // Default (1,1)
    static Vector2 ScaleDraw;        // Draw scaling
    static Vector2 RealScale;        // Actual screen scaling

    // Button states
    static bool IsPressed;           // Button pressed this frame
    static bool IsDown;              // Button currently down
    static bool IsReleasePre;        // Button released last frame
    static bool IsDownPre;           // Button was down last frame
    static bool IsReleased;          // Button released this frame
    static bool IsMoved;             // Position moved this frame
    static bool IsPosChanged;        // Position changed this frame

    // Back button (ESC key)
    static bool IsBackPressed;

    // Drag detection
    static int PoXStart;             // Drag start X
    static int PoYStart;             // Drag start Y
    static int PoXMove;              // Mouse move delta X
    static int PoYMove;              // Mouse move delta Y
    static Vector2 PosMoveStart;
    static Vector2 PosMoveEnd;

    // Gesture/pinch tracking (for touch)
    static float firstPinchDistance;
    static float nowPinchDistance;
    static float PinchMove;

    // Collection/recording positions
    static Vector2 CollectionPosBase;
    static float CollectionScaleBase;
    static bool CollectionPos;
    static std::vector<Vector2> CollectionPositions;

    // Timing
    static float PressTimeSpan;      // Time threshold for press detection
    static float PressTimeElapsed;
    static float SleepTime;

    // Screen dimensions
    static int SWidth;               // Screen width
    static int SHeight;              // Screen height

    // Click counter
    static int ClickTime;

    /// <summary>
    /// Update input state based on SDL events.
    /// Call this once per frame with delta time.
    /// </summary>
    static void Update(float deltaTime, const SDL_Event* event = nullptr);

    /// <summary>
    /// Handle mouse motion events
    /// </summary>
    static void HandleMouseMotion(const SDL_MouseMotionEvent& motion);

    /// <summary>
    /// Handle mouse button events
    /// </summary>
    static void HandleMouseButton(const SDL_MouseButtonEvent& button);

    /// <summary>
    /// Handle keyboard events
    /// </summary>
    static void HandleKeyboard(const SDL_KeyboardEvent& key);

    /// <summary>
    /// Check if any keys are currently pressed
    /// </summary>
    static bool HasKeys();

    /// <summary>
    /// Get character representation of a key
    /// </summary>
    static std::string ConvertKeyToChar(SDL_Keycode key, bool shift);

    /// <summary>
    /// Clear keyboard/input state
    /// </summary>
    static void Clear();

    /// <summary>
    /// Convert SDL keycode to character
    /// </summary>
    static char KeycodeToChar(SDL_Keycode keycode, bool shift);

private:
    static std::chrono::time_point<std::chrono::high_resolution_clock> lastBackPressedTime;
    static bool isBackPressedRaw;
    static int previousMouseX;
    static int previousMouseY;
    static bool previousMouseDown;
};

} // namespace GameManager
