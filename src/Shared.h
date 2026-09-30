#pragma once

#include "Xs/Xs.h"

extern Vector2i WindowSize, ScreenSize;

extern int FONTSIZE;

extern std::wstring ExePath;

extern bool EnableDWM;

namespace Shared
{
	void DrawBorder(RenWin& window);
}