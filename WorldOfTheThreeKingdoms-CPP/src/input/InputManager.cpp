#include "InputManager.h"
#include "ui/FocusManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace GameManager {

// Static member initialization
Vector2 InputManager::PositionPre = Vector2(0.0f, 0.0f);
Vector2 InputManager::Position = Vector2(0.0f, 0.0f);
float InputManager::OriginPoX = 0.0f;
float InputManager::OriginPoY = 0.0f;
int InputManager::PoX = 0;
int InputManager::PoY = 0;

Vector2 InputManager::Scale1 = Vector2(1.0f, 1.0f);
Vector2 InputManager::Scale2 = Vector2(1.0f, 1.0f);
Vector2 InputManager::ScaleOne = Vector2(1.0f, 1.0f);
Vector2 InputManager::ScaleDraw = Vector2(1.0f, 1.0f);
Vector2 InputManager::RealScale = Vector2(1.0f, 1.0f);

bool InputManager::IsPressed = false;
bool InputManager::IsDown = false;
bool InputManager::IsReleasePre = false;
bool InputManager::IsDownPre = false;
bool InputManager::IsReleased = false;
bool InputManager::IsMoved = false;
bool InputManager::IsPosChanged = false;
bool InputManager::IsBackPressed = false;

int InputManager::PoXStart = 0;
int InputManager::PoYStart = 0;
int InputManager::PoXMove = 0;
int InputManager::PoYMove = 0;
Vector2 InputManager::PosMoveStart = Vector2(0.0f, 0.0f);
Vector2 InputManager::PosMoveEnd = Vector2(0.0f, 0.0f);

float InputManager::firstPinchDistance = 0.0f;
float InputManager::nowPinchDistance = 0.0f;
float InputManager::PinchMove = 0.0f;

Vector2 InputManager::CollectionPosBase = Vector2(0.0f, 0.0f);
float InputManager::CollectionScaleBase = 1.0f;
bool InputManager::CollectionPos = false;
std::vector<Vector2> InputManager::CollectionPositions;

float InputManager::PressTimeSpan = 0.1f;
float InputManager::PressTimeElapsed = 0.0f;
float InputManager::SleepTime = 0.0f;

int InputManager::SWidth = 1024;
int InputManager::SHeight = 768;

int InputManager::ClickTime = 1;

// Private static members
std::chrono::time_point<std::chrono::high_resolution_clock> InputManager::lastBackPressedTime;
bool InputManager::isBackPressedRaw = false;
int InputManager::previousMouseX = 0;
int InputManager::previousMouseY = 0;
bool InputManager::previousMouseDown = false;

void InputManager::Update(float deltaTime, const SDL_Event* event)
{
    // Update timing
    PressTimeElapsed += deltaTime;

    // Save previous state
    IsDownPre = IsDown;
    IsReleasePre = IsReleased;

    // Reset per-frame flags
    IsDown = IsPressed = IsReleased = IsMoved = IsPosChanged = false;

    // Update sleep timer
    if (SleepTime > 0.0f)
    {
        SleepTime -= deltaTime;
        if (SleepTime < 0.0f)
            SleepTime = 0.0f;
    }

    // Get current mouse state
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    bool mouseDown = (mouseState & SDL_BUTTON_LMASK) != 0;

    // Store raw screen coordinates
    OriginPoX = static_cast<float>(mouseX);
    OriginPoY = static_cast<float>(mouseY);

    // Apply scaling
    if (Scale1.x != 1.0f || Scale1.y != 1.0f)
    {
        PoX = static_cast<int>(OriginPoX / Scale1.x);
        PoY = static_cast<int>(OriginPoY / Scale1.y);
    }
    else
    {
        PoX = static_cast<int>(OriginPoX);
        PoY = static_cast<int>(OriginPoY);
    }

    // Track position changes
    PositionPre = Position;
    Position.x = static_cast<float>(PoX);
    Position.y = static_cast<float>(PoY);

    // Detect mouse state changes
    IsDown = mouseDown;

    if (!previousMouseDown && mouseDown)
    {
        // Mouse button pressed
        IsPressed = true;
        PoXStart = PoX;
        PoYStart = PoY;
    }

    if (previousMouseDown && !mouseDown)
    {
        // Mouse button released
        IsReleased = true;
    }

    if (previousMouseDown && mouseDown)
    {
        // Mouse moving while held
        IsMoved = true;
        PoXMove = mouseX - previousMouseX;
        PoYMove = mouseY - previousMouseY;
        PosMoveStart = Vector2(static_cast<float>(previousMouseX), static_cast<float>(previousMouseY));
        PosMoveEnd = Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY));
    }

    // Check for position change
    if (Position != PositionPre)
    {
        IsPosChanged = true;
    }

    // Update collection positions if recording
    if (CollectionPos)
    {
        Vector2 newPos = (Position / CollectionScaleBase) + CollectionPosBase;
        if (CollectionPositions.empty())
        {
            CollectionPositions.push_back(newPos);
        }
        else
        {
            Vector2 last = CollectionPositions.back();
            float dx = std::abs(newPos.x - last.x);
            float dy = std::abs(newPos.y - last.y);
            if (dx >= 2.0f || dy >= 2.0f)
            {
                CollectionPositions.push_back(newPos);
            }
        }
    }

    // Update previous state for next frame
    previousMouseX = mouseX;
    previousMouseY = mouseY;
    previousMouseDown = mouseDown;

    // Get keyboard state from SDL
    int numKeys = 0;
    const Uint8* keyState = SDL_GetKeyboardState(&numKeys);
    if (keyState && keyState[SDL_SCANCODE_ESCAPE])
    {
        // Check for ESC key with debounce
        auto now = std::chrono::high_resolution_clock::now();
        bool canPress = true;

        if (lastBackPressedTime.time_since_epoch().count() != 0)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBackPressedTime);
            if (elapsed.count() < 500)
                canPress = false;
        }

        if (canPress && !isBackPressedRaw)
        {
            IsBackPressed = true;
            isBackPressedRaw = true;
            lastBackPressedTime = now;
        }
    }

    // Handle per-event key presses for navigation (Tab)
    if (event && event->type == SDL_KEYDOWN)
    {
        SDL_KeyboardEvent k = event->key;
        if (k.keysym.sym == SDLK_TAB)
        {
            // If shift is held, move focus backward
            bool shift = (k.keysym.mod & KMOD_SHIFT) != 0;
            try {
                if (shift) {
                    WO3K::FocusManager::MoveFocusPrev();
                } else {
                    WO3K::FocusManager::MoveFocusNext();
                }
            } catch(...) {}
        }
    }
    else
    {
        isBackPressedRaw = false;
    }
}

void InputManager::HandleMouseMotion(const SDL_MouseMotionEvent& motion)
{
    // Handled in Update() via SDL_GetMouseState
}

void InputManager::HandleMouseButton(const SDL_MouseButtonEvent& button)
{
    // Handled in Update() via SDL_GetMouseState
}

void InputManager::HandleKeyboard(const SDL_KeyboardEvent& key)
{
    // Handled in Update() via SDL_GetKeyboardState
}

bool InputManager::HasKeys()
{
    int numKeys = 0;
    const Uint8* keyState = SDL_GetKeyboardState(&numKeys);
    if (keyState && numKeys > 0)
    {
        // Check if any key is pressed
        for (int i = 0; i < numKeys; ++i)
        {
            if (keyState[i])
                return true;
        }
    }
    return false;
}

std::string InputManager::ConvertKeyToChar(SDL_Keycode key, bool shift)
{
    return std::string(1, KeycodeToChar(key, shift));
}

char InputManager::KeycodeToChar(SDL_Keycode keycode, bool shift)
{
    switch (keycode)
    {
        case SDLK_SPACE:
            return ' ';
        case SDLK_0:
            return shift ? ')' : '0';
        case SDLK_1:
            return shift ? '!' : '1';
        case SDLK_2:
            return shift ? '@' : '2';
        case SDLK_3:
            return shift ? '#' : '3';
        case SDLK_4:
            return shift ? '$' : '4';
        case SDLK_5:
            return shift ? '%' : '5';
        case SDLK_6:
            return shift ? '^' : '6';
        case SDLK_7:
            return shift ? '&' : '7';
        case SDLK_8:
            return shift ? '*' : '8';
        case SDLK_9:
            return shift ? '(' : '9';
        case SDLK_KP_0:
            return '0';
        case SDLK_KP_1:
            return '1';
        case SDLK_KP_2:
            return '2';
        case SDLK_KP_3:
            return '3';
        case SDLK_KP_4:
            return '4';
        case SDLK_KP_5:
            return '5';
        case SDLK_KP_6:
            return '6';
        case SDLK_KP_7:
            return '7';
        case SDLK_KP_8:
            return '8';
        case SDLK_KP_9:
            return '9';
        case SDLK_a:
            return shift ? 'A' : 'a';
        case SDLK_b:
            return shift ? 'B' : 'b';
        case SDLK_c:
            return shift ? 'C' : 'c';
        case SDLK_d:
            return shift ? 'D' : 'd';
        case SDLK_e:
            return shift ? 'E' : 'e';
        case SDLK_f:
            return shift ? 'F' : 'f';
        case SDLK_g:
            return shift ? 'G' : 'g';
        case SDLK_h:
            return shift ? 'H' : 'h';
        case SDLK_i:
            return shift ? 'I' : 'i';
        case SDLK_j:
            return shift ? 'J' : 'j';
        case SDLK_k:
            return shift ? 'K' : 'k';
        case SDLK_l:
            return shift ? 'L' : 'l';
        case SDLK_m:
            return shift ? 'M' : 'm';
        case SDLK_n:
            return shift ? 'N' : 'n';
        case SDLK_o:
            return shift ? 'O' : 'o';
        case SDLK_p:
            return shift ? 'P' : 'p';
        case SDLK_q:
            return shift ? 'Q' : 'q';
        case SDLK_r:
            return shift ? 'R' : 'r';
        case SDLK_s:
            return shift ? 'S' : 's';
        case SDLK_t:
            return shift ? 'T' : 't';
        case SDLK_u:
            return shift ? 'U' : 'u';
        case SDLK_v:
            return shift ? 'V' : 'v';
        case SDLK_w:
            return shift ? 'W' : 'w';
        case SDLK_x:
            return shift ? 'X' : 'x';
        case SDLK_y:
            return shift ? 'Y' : 'y';
        case SDLK_z:
            return shift ? 'Z' : 'z';
        case SDLK_LEFTBRACKET:
            return shift ? '{' : '[';
        case SDLK_RIGHTBRACKET:
            return shift ? '}' : ']';
        case SDLK_COMMA:
            return shift ? '<' : ',';
        case SDLK_PERIOD:
            return shift ? '>' : '.';
        case SDLK_MINUS:
            return shift ? '_' : '-';
        case SDLK_EQUALS:
            return shift ? '+' : '=';
        case SDLK_SLASH:
            return shift ? '?' : '/';
        case SDLK_SEMICOLON:
            return shift ? ':' : ';';
        case SDLK_QUOTE:
            return shift ? '"' : '\'';
        case SDLK_BACKSLASH:
            return shift ? '|' : '\\';
        case SDLK_BACKQUOTE:
            return shift ? '~' : '`';
        default:
            return '\0';
    }
}

void InputManager::Clear()
{
    IsPressed = false;
    IsDown = false;
    IsReleased = false;
    IsMoved = false;
    IsPosChanged = false;
    IsBackPressed = false;
    PoXMove = 0;
    PoYMove = 0;
}

} // namespace GameManager
