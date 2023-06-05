#pragma once

#ifndef PCH_H
#define PCH_H

#include <string>
#include <Windows.h>
#include <iostream>

#include "framework.h"
#include "TextSeeker.h"
#include "commctrl.h"
#include "uxtheme.h"
#include "Windowsx.h"

#pragma comment(lib, "UxTheme.lib")

#include "ui_inputfields.hpp"
#include "ui_fileexplorer.hpp"


inline HWND g_hMainWnd;
inline bool b_LHeld = false;
#endif