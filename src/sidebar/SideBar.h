#pragma once
#include "Xs/Xs.h"

namespace SideBar
{
	namespace SideBarLevel1
	{
		extern int w;

		void Draw(RenWin& window);

		void DrawTips(RenWin& window);
	}

	namespace SideBarLevel2
	{
		extern bool NeedRedraw;
		extern xs::EV SideBarLevel2Width;
		void Draw(RenWin& window);
	}
}