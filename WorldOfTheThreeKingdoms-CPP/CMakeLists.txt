
cmake_minimum_required(VERSION 3.8)
project(WorldOfTheThreeKingdomsCPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_THIRD_PARTY_SDL "Use SDL2 shipped in project third_party folder" ON)

if(USE_THIRD_PARTY_SDL)
	# Search under this project's third_party directory for common SDL2 layouts
	set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

	# Common path shipped: third_party/bin/win32/SDL2-<version>/{include,lib,x64,x86}
	file(GLOB SDL2_ROOTS RELATIVE ${THIRD_PARTY_DIR} "bin/*/SDL2-*")
	if(SDL2_ROOTS)
		list(GET SDL2_ROOTS 0 SDL2_ROOT_REL)
		set(SDL2_ROOT "${THIRD_PARTY_DIR}/${SDL2_ROOT_REL}")
	else()
		# fallback to common known path
		set(SDL2_ROOT "${THIRD_PARTY_DIR}/bin/win32/SDL2-2.32.4")
	endif()

	set(SDL2_INCLUDE_DIR "${SDL2_ROOT}/include")
	set(SDL2_LIB_DIR "${SDL2_ROOT}/lib")

	# prefer x64 libs if present
	if(EXISTS "${SDL2_LIB_DIR}/x64")
		set(SDL2_LIB_DIR "${SDL2_LIB_DIR}/x64")
	elseif(EXISTS "${SDL2_LIB_DIR}/x86")
		set(SDL2_LIB_DIR "${SDL2_LIB_DIR}/x86")
	endif()

	find_path(SDL2_FOUND_INCLUDE SDL.h PATHS ${SDL2_INCLUDE_DIR} NO_DEFAULT_PATH)
	if(NOT SDL2_FOUND_INCLUDE)
		message(FATAL_ERROR "SDL2 headers not found under ${SDL2_INCLUDE_DIR}. Ensure third_party contains SDL2 dev files.")
	endif()

	find_file(SDL2_LIB_FILE NAMES SDL2.lib SDL2.dll PATHS ${SDL2_LIB_DIR} NO_DEFAULT_PATH)
	if(NOT SDL2_LIB_FILE)
		message(FATAL_ERROR "SDL2 library not found under ${SDL2_LIB_DIR}. Ensure third_party contains SDL2 .lib or .dll")
	endif()

	# SDL on Windows typically requires SDL2main.lib which provides WinMain glue.
	find_file(SDL2MAIN_LIB_FILE NAMES SDL2main.lib PATHS ${SDL2_LIB_DIR} NO_DEFAULT_PATH)
	if(NOT SDL2MAIN_LIB_FILE)
		message(WARNING "SDL2main.lib not found under ${SDL2_LIB_DIR}. If missing, link errors for main may occur. If you plan to use SDL on Windows, add SDL2main.lib to the lib folder.")
	endif()

	# Try to detect SDL_image and SDL_mixer in the same third_party root. These enable PNG/JPG loading and basic audio.
	find_path(SDL2_IMAGE_INCLUDE SDL_image.h PATHS ${SDL2_ROOT}/include ${SDL2_ROOT}/include/SDL2 NO_DEFAULT_PATH)
	find_file(SDL2_IMAGE_LIB_FILE NAMES SDL2_image.lib SDL2-image.lib SDL_image.lib PATHS ${SDL2_LIB_DIR} NO_DEFAULT_PATH)
	if(SDL2_IMAGE_INCLUDE AND SDL2_IMAGE_LIB_FILE)
		add_library(SDL2::SDL2_image UNKNOWN IMPORTED)
		set_target_properties(SDL2::SDL2_image PROPERTIES IMPORTED_LOCATION "${SDL2_IMAGE_LIB_FILE}" INTERFACE_INCLUDE_DIRECTORIES "${SDL2_ROOT}/include")
	else()
		set(SDL2_IMAGE_LIB_FILE "")
	endif()

	find_path(SDL2_MIXER_INCLUDE SDL_mixer.h PATHS ${SDL2_ROOT}/include ${SDL2_ROOT}/include/SDL2 NO_DEFAULT_PATH)
	find_file(SDL2_MIXER_LIB_FILE NAMES SDL2_mixer.lib SDL_mixer.lib PATHS ${SDL2_LIB_DIR} NO_DEFAULT_PATH)

	if(SDL2_MIXER_INCLUDE AND SDL2_MIXER_LIB_FILE)
		add_library(SDL2::SDL2_mixer UNKNOWN IMPORTED)
		set_target_properties(SDL2::SDL2_mixer PROPERTIES IMPORTED_LOCATION "${SDL2_MIXER_LIB_FILE}" INTERFACE_INCLUDE_DIRECTORIES "${SDL2_ROOT}/include")
	else()
		set(SDL2_MIXER_LIB_FILE "")
	endif()

	# Create imported library target for core SDL2
	add_library(SDL2::SDL2 UNKNOWN IMPORTED)
	set_target_properties(SDL2::SDL2 PROPERTIES
		IMPORTED_LOCATION "${SDL2_LIB_FILE}"
		INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}")

	add_executable(WorldOfTheThreeKingdomsCPP 
		src/main.cpp 
		src/platform/PlatformSDL.cpp 
		src/render/SpriteBatch.cpp 
		src/render/Camera.cpp 
		src/platform/Texture.cpp
		src/game/GameGlobal/Setting.cpp
		src/game/Session.cpp
		src/game/MainGame.cpp
		src/game/CacheManager.cpp
		src/screen/Screen.cpp
		src/screen/ScreenManager.cpp
 		src/screen/MainMenuScreen.cpp
		src/screen/LoadingScreen.cpp
		src/screen/GameScreen.cpp
		src/ui/Panel.cpp
		src/ui/Button.cpp
		src/ui/ListView.cpp
		src/ui/ScrollBar.cpp
		src/ui/FlowPanel.cpp
		src/ui/ShadowCache.cpp
		src/ui/BoxPanel.cpp
		src/audio/SoundManager.cpp
		src/ui/FocusManager.cpp
		src/screen/MainGameScreen.cpp
		src/ui/Control.h
		src/ui/FontManager.cpp
		src/input/InputManager.cpp
		third_party/source/pugixml-1.15/pugixml.cpp
	)
	target_include_directories(WorldOfTheThreeKingdomsCPP PRIVATE src ${SDL2_INCLUDE_DIR} third_party/source)



	# Link SDL2main if present, otherwise just SDL2
	if(SDL2MAIN_LIB_FILE)
		add_library(SDL2::SDL2main UNKNOWN IMPORTED)
		set_target_properties(SDL2::SDL2main PROPERTIES IMPORTED_LOCATION "${SDL2MAIN_LIB_FILE}")
		target_link_libraries(WorldOfTheThreeKingdomsCPP PRIVATE SDL2::SDL2main SDL2::SDL2)
	else()
		target_link_libraries(WorldOfTheThreeKingdomsCPP PRIVATE SDL2::SDL2)
	endif()

	# Link optional SDL_image and SDL_mixer if found
	if(SDL2_IMAGE_LIB_FILE)
		target_link_libraries(WorldOfTheThreeKingdomsCPP PRIVATE SDL2::SDL2_image)
		target_compile_definitions(WorldOfTheThreeKingdomsCPP PRIVATE SDL_IMAGE_FOUND=1)
	endif()
	if(SDL2_MIXER_LIB_FILE)
		target_link_libraries(WorldOfTheThreeKingdomsCPP PRIVATE SDL2::SDL2_mixer)
		target_compile_definitions(WorldOfTheThreeKingdomsCPP PRIVATE SDL_MIXER_FOUND=1)
	endif()

	# Test executable for Setting config loading
	add_executable(TestSetting 
		src/test_setting.cpp 
		src/game/GameGlobal/Setting.cpp
		third_party/source/pugixml-1.15/pugixml.cpp
	)
	target_include_directories(TestSetting PRIVATE src third_party/source)

# Try to detect SDL2_ttf
find_path(SDL2_TTF_INCLUDE SDL_ttf.h PATHS ${SDL2_ROOT}/include ${SDL2_ROOT}/include/SDL2 NO_DEFAULT_PATH)
find_file(SDL2_TTF_LIB_FILE NAMES SDL2_ttf.lib SDL_ttf-2.0.lib SDL2_ttf-2.0.lib PATHS ${SDL2_LIB_DIR} NO_DEFAULT_PATH)
if(SDL2_TTF_INCLUDE AND SDL2_TTF_LIB_FILE)
	add_library(SDL2::SDL2_ttf UNKNOWN IMPORTED)
	set_target_properties(SDL2::SDL2_ttf PROPERTIES IMPORTED_LOCATION "${SDL2_TTF_LIB_FILE}" INTERFACE_INCLUDE_DIRECTORIES "${SDL2_ROOT}/include")
	set(SDL2_TTF_FOUND 1)
	message(STATUS "Found SDL2_ttf: ${SDL2_TTF_LIB_FILE}")
else()
	set(SDL2_TTF_FOUND 0)
endif()

if(SDL2_TTF_FOUND)
	target_link_libraries(WorldOfTheThreeKingdomsCPP PRIVATE SDL2::SDL2_ttf)
	target_compile_definitions(WorldOfTheThreeKingdomsCPP PRIVATE SDL_TTF_FOUND=1)
	target_link_libraries(TestSetting PRIVATE SDL2::SDL2_ttf)
	target_compile_definitions(TestSetting PRIVATE SDL_TTF_FOUND=1)
endif()

else()
	message(FATAL_ERROR "This project is configured to use the third_party SDL2 only. Set USE_THIRD_PARTY_SDL=ON and provide SDL2 under the project's third_party folder.")
endif()

if(MSVC)
	target_compile_options(WorldOfTheThreeKingdomsCPP PRIVATE /W3)
else()
	target_compile_options(WorldOfTheThreeKingdomsCPP PRIVATE -Wall -Wextra -Wpedantic)
endif()


