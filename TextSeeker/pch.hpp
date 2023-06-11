#pragma once

#ifndef PCH_H
#define PCH_H

#include <string>
#include <Windows.h>
#include <iostream>
#include <string_view>

#include "framework.h"
#include "TextSeeker.h"
#include "commctrl.h"
#include "uxtheme.h"
#include "Windowsx.h"
#include <memory>
#include <filesystem>
#include <list>
#include <ShlObj_core.h>
#include <format>
#include <thread>

namespace _fs = std::filesystem;

#pragma comment(lib, "UxTheme.lib")

#include "u_filesystem.hpp"
#include "TextSeeker.hpp"
#include "ui_inputfields.hpp"
#include "ui_fileexplorer.hpp"

inline HWND g_hMainWnd;
inline bool b_LHeld = false;
#endif