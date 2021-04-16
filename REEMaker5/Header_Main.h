#pragma once
/*
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
* ░▒▓      _    _                _             _____      _            _             _		▓▒░
* ░▒▓     | |  | |              | |           |  __ \    (_)          (_)           | |		▓▒░
* ░▒▓     | |__| | ___  __ _  __| | ___ _ __  | |__) | __ _ _ __   ___ _ _ __   __ _| |		▓▒░
* ░▒▓     |  __  |/ _ \/ _` |/ _` |/ _ \ '__| |  ___/ '__| | '_ \ / __| | '_ \ / _` | |		▓▒░
* ░▒▓     | |  | |  __/ (_| | (_| |  __/ |    | |   | |  | | | | | (__| | |_) | (_| | |		▓▒░
* ░▒▓     |_|  |_|\___|\__,_|\__,_|\___|_|    |_|   |_|  |_|_| |_|\___|_| .__/ \__,_|_|		▓▒░
* ░▒▓                                                                   | |					▓▒░
* ░▒▓                                                                   |_|					▓▒░
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
*  REEMaker 5 __ Grégory WENTZEL (c) 2021
*/
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés
#include <windows.h>
#include "resource.h"
#include "Shellapi.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <codecvt>
#include <locale>
#include <numeric>
#include <random>
#include <future>
#include <chrono>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <exception>
#include <thread>
#include <atlbase.h>
#include <string>
#include <sysinfoapi.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_syswm.h>
#define STB_IMAGE_IMPLEMENTATION
#include <imgui/stb_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl2.h>
#include <imgui/button/imguial_button.h>
#include <imgui/msgbox/imguial_msgbox.h>
#include <imgui/msgbox/imguial_msgbox.cpp>
#include <imgui/imgui_markdown.h>
#include <obfuscate.h>
#include <plf_nanotimer.h>
#include <fmt/core.h>
#include <fmt/chrono.h>
//#define PODOFO_HAVE_JPEG_LIB
#include <podofo/podofo.h>
#include <ctpl.h>

#define IMVEC4_COL16(R,G,B,A)    ImVec4((float)R/255.0f, (float)G/255.0f, (float)B/255.0f, (float)A/255.0f)
#define ICO_TEXT_CSTR(Icon,TexteUTF8) (const char*)std::string(Icon + std::string(TexteUTF8)).c_str()