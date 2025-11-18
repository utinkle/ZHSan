# C# to C++ Migration Plan for WorldOfTheThreeKingdoms

## Overview
This document outlines the prioritized migration strategy for porting the C# MonoGame project to C++ using SDL2.

## Architecture & Dependencies

### C# Dependencies → C++ Alternatives

| C# Component | Size | Dependencies | C++ Alternative | Status | Notes |
|---|---|---|---|---|---|
| **MainGame.cs** | Core | XNA/MonoGame Game | Custom GameLoop | TODO | Root game class, initializes Session/UI/Content |
| **Session** | Large | GameManager | Session Manager (Singleton) | TODO | Game state holder: Scenario, GlobalVariables, Parameters |
| **CacheManager** | Medium | Texture2D, SpriteBatch | AssetManager, TextureCache | TODO | Manages texture/font/sound caching |
| **InputManager** | Large | XNA Input APIs | SDL_Event + InputState | TODO | Keyboard/Mouse/Touch input tracking |
| **TextureRecs** | Medium | Dictionary | AssetRegistry | TODO | Maps texture names to metadata (width, height, cache policy) |
| **SpriteFontPlus** | Large | stb_truetype, FontStashSharp | SDL_ttf or port TtfFontBaker | IN-PROGRESS | Font loading/baking (currently in C# subfolder) |
| **GameGlobal** | Medium | Various | GlobalState | TODO | GlobalVariables, Parameters, Setting classes |
| **GameScreens** | Large | Screen abstraction | ScreenManager | TODO | Menu, Loading, MainGameScreen (partial classes) |
| **GameObjects** | Large | Entity/Component system | Entity Manager | TODO | Person, Troop, Architecture, etc. (complex object model) |
| **GameManager** | Large | Multiple subsystems | Manager classes | TODO | References, Influences, Conditions, Effects |
| **Tools** | Medium | Utilities | Utility functions | TODO | WebTools, JSON/Excel parsing helpers |
| **Platforms** | Small | Platform abstraction | Platform layer (PlatformSDL) | DONE | Already abstracted for XNA/Windows/Android/iOS |

## Migration Priority Phases

### Phase 1: Core Foundation (Foundation Layer) ✅ DONE
- ✅ SDL2 initialization and windowing (PlatformSDL)
- ✅ Basic rendering (SpriteBatch wrapper)
- ✅ Asset loading (Texture loader using SDL_image)
- ✅ CMake build system

### Phase 2: Game Loop & State Management (THIS ITERATION)
**Objective**: Create a functioning game loop that can initialize and update game state.

1. **MainGame.cs → src/game/MainGame.h/.cpp** (Priority: CRITICAL)
   - Port Game loop initialization
   - Initialize Session singleton
   - Set up Content/Asset managers
   - Handle Update/Draw cycle
   - **Dependencies**: Platform, Session, CacheManager
   - **Complexity**: High (680 lines, many initialization steps)
   - **Estimated effort**: 3-4 hours

2. **Session.cs → src/game/Session.h/.cpp** (Priority: CRITICAL)
   - Static singleton session holder
   - GameScenario, Parameters, GlobalVariables holders
   - Resolution parsing, platform constants
   - **Dependencies**: GameGlobal (Parameters, GlobalVariables)
   - **Complexity**: Medium (697 lines, mostly static data holders)
   - **Estimated effort**: 2-3 hours

3. **CacheManager.cs → src/manager/CacheManager.h/.cpp** (Priority: HIGH)
   - Texture cache management
   - Font pair holder (FontPair struct)
   - PlatformColor constants (small color palette)
   - **Dependencies**: Texture2D replacement, SDL_Texture
   - **Complexity**: Medium-High (753 lines, thread-safe caching logic)
   - **Estimated effort**: 3-4 hours

4. **InputManager.cs → src/input/InputManager.h/.cpp** (Priority: HIGH)
   - Keyboard state tracking
   - Mouse/touch position and input
   - Button press detection and delays
   - **Dependencies**: SDL_Event, SDL_GetKeyboardState, SDL_GetMouseState
   - **Complexity**: High (495 lines, complex state machine)
   - **Estimated effort**: 3-4 hours

5. **GameGlobal/* → src/game/GameGlobal/** (Priority: MEDIUM)
   - GlobalVariables class (game state variables)
   - Parameters class (game configuration)
   - Setting class (user settings/config)
   - **Dependencies**: JSON/file I/O
   - **Complexity**: Medium (need to parse XML/JSON config files)
   - **Estimated effort**: 2-3 hours

### Phase 3: UI & Screens (Menu Layer)
1. **Screen abstraction → src/screen/Screen.h/.cpp**
   - Base Screen class (virtual Update/Draw)
   - ScreenManager for screen stack
   - **Dependencies**: SpriteBatch, InputManager

2. **MainMenuScreen → src/screen/MainMenuScreen.h/.cpp**
   - Main menu UI
   - Start/Load game options
   - Settings menu

3. **LoadingScreen → src/screen/LoadingScreen.h/.cpp**
   - Asset loading progress display

### Phase 4: Game Objects (Entity System)
1. **GameObjects/* → src/game/objects/**
   - Person, Troop, Architecture, Item classes
   - Complex object model with relationships
   - Requires proper serialization/deserialization

2. **SpriteFontPlus → src/render/SpriteFont.h/.cpp**
   - Font loading and rendering
   - Glyph rendering using SDL_Texture or SDL_ttf

### Phase 5: Game Logic & Systems
1. **MainGameScreen → src/screen/MainGameScreen.h/.cpp**
2. **GameManager subsystems → src/manager/**
3. **Tools utilities → src/tools/**

---

## File Structure Layout

```
WorldOfTheThreeKingdoms-CPP/
├── src/
│   ├── main.cpp                          (entry point - DONE)
│   ├── platform/
│   │   ├── PlatformSDL.h/cpp            (SDL wrapper - DONE)
│   │   ├── Types.h                       (Vector2, Point, Color - DONE)
│   │   ├── Texture.h/cpp                 (Texture loader - DONE)
│   │   └── Platform.h/cpp                (platform abstraction)
│   ├── render/
│   │   ├── SpriteBatch.h/cpp             (sprite rendering - DONE)
│   │   ├── SpriteFont.h/cpp              (text rendering - TODO)
│   │   └── Renderer.h/cpp                (rendering context)
│   ├── input/
│   │   └── InputManager.h/cpp            (keyboard/mouse/touch - TODO)
│   ├── game/
│   │   ├── MainGame.h/cpp                (game loop - TODO)
│   │   ├── Session.h/cpp                 (game state singleton - TODO)
│   │   ├── GameGlobal/
│   │   │   ├── GlobalVariables.h/cpp
│   │   │   ├── Parameters.h/cpp
│   │   │   └── Setting.h/cpp
│   │   └── objects/
│   │       ├── GameEntity.h/cpp
│   │       ├── Person.h/cpp
│   │       ├── Troop.h/cpp
│   │       └── Architecture.h/cpp
│   ├── manager/
│   │   ├── CacheManager.h/cpp            (asset caching - TODO)
│   │   ├── ContentManager.h/cpp          (content loading)
│   │   ├── AudioManager.h/cpp            (sound/music)
│   │   └── ScreenManager.h/cpp
│   ├── screen/
│   │   ├── Screen.h/cpp
│   │   ├── ScreenManager.h/cpp
│   │   ├── MainMenuScreen.h/cpp
│   │   ├── LoadingScreen.h/cpp
│   │   └── MainGameScreen.h/cpp
│   ├── tools/
│   │   ├── Util.h/cpp
│   │   ├── JsonHelper.h/cpp
│   │   └── FileHelper.h/cpp
│   └── third_party/                      (includes, link time dependencies)
├── CMakeLists.txt                        (build configuration - DONE)
└── build/                                (cmake build output)
```

---

## Key Migration Decisions

### 1. Singleton Pattern
**Decision**: Use static/singleton instances for Session, CacheManager, InputManager, Setting
- **Rationale**: C# original uses static classes; C++ equivalent is static members + factory
- **Implementation**: `static Session& GetInstance()` or `extern Session g_session;`

### 2. Content Management
**Decision**: Folder-based asset discovery (no MonoGame Content Pipeline initially)
- **Rationale**: SDL has no built-in content pipeline; read files directly from `Content/` folder
- **Implementation**: `ContentManager` class wraps file I/O + texture/font loading

### 3. Configuration Files
**Decision**: Convert XML/JSON game settings to C++ structures
- **Rationale**: Games often have config files; port them as-is or convert to JSON/TOML
- **Implementation**: Use a JSON library (e.g., nlohmann/json) for Settings

### 4. Threading
**Decision**: Use `std::mutex` + `std::lock_guard` for thread safety (matches C# `lock()`)
- **Rationale**: Original C# code uses `lock(CacheLock)` for thread-safe cache updates
- **Implementation**: Introduce CacheManager thread safety via mutexes

### 5. Entity Serialization
**Decision**: Port C# JSON/XML serialization to C++ JSON library
- **Rationale**: GameObjects use reflection-based serialization in C#
- **Implementation**: Manual JSON marshaling or use reflection-like library (cereal, nlohmann)

---

## Porting Checklist for Phase 2

### MainGame.cs
- [ ] Analyze constructor initialization sequence
- [ ] Identify all static references (Setting, Session, CacheManager, InputManager)
- [ ] Port Window/graphics setup logic
- [ ] Port Update/Draw loop to match C++ expectations
- [ ] Create SpriteBatch instance
- [ ] Handle exception/error logging

### Session.cs
- [ ] Create static singleton instance
- [ ] Port Parameters and GlobalVariables holder
- [ ] Port resolution parsing logic
- [ ] Port Scenario holder and methods

### CacheManager.cs
- [ ] Create texture cache (std::unordered_map<string, Texture>)
- [ ] Implement cache eviction policies (Live, Scene, Page, Temp)
- [ ] Port FontPair struct and default font settings
- [ ] Add thread-safe locking

### InputManager.cs
- [ ] Map SDL_Event types to C# XNA Input APIs
- [ ] Create InputState struct (keyboard, mouse, touch)
- [ ] Implement button press state machine
- [ ] Port mouse position tracking and scaling

### GameGlobal/
- [ ] Create GlobalVariables class (game state container)
- [ ] Create Parameters class (config container)
- [ ] Create Setting class (user settings)
- [ ] Set up config file parsing (JSON or manual parsing)

---

## Testing Strategy

### Unit Tests (Per-Phase)
1. **Phase 2**: Test Session initialization, CacheManager caching, InputManager state transitions
2. Create simple test scenes to verify rendering pipeline

### Integration Tests
1. Launch MainGame → verify window appears
2. Load test texture → verify it renders
3. Press ESC → verify input detection and exit

### Performance Baseline
- Track frame time on initialization phase
- Benchmark texture loading and caching
- Monitor memory usage vs C# original

---

## Recommended Implementation Order

1. **Start with Phase 2 (this iteration)**:
   - Begin with `Session.h/.cpp` (simplest, foundational)
   - Then `GameGlobal/` classes (Parameters, GlobalVariables, Setting)
   - Then `CacheManager.h/.cpp` (uses Session)
   - Then `InputManager.h/.cpp` (uses Platform)
   - Finally `MainGame.h/.cpp` (orchestrates everything)

2. **Parallelizable**:
   - InputManager can be worked on independently
   - CacheManager can be tested with mock assets
   - Both rely only on Platform layer

3. **Blockers to watch**:
   - Asset file format discovery (does C# original use XML/JSON/binary?)
   - Font rendering (need to port SpriteFontPlus baking to C++)
   - Complex object serialization (defer to Phase 4)

---

## Summary

**Phase 2 Focus**: Create a minimal but functional game state management and input handling layer that can support the game loop. This prepares the foundation for UI (Phase 3) and game logic (Phase 4).

**Estimated total effort for Phase 2**: 13-18 hours (across MainGame, Session, CacheManager, InputManager, GameGlobal)

**Next milestone**: MainGame window + menu screen rendering + input response

