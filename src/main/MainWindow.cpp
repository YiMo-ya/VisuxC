#include "MainWindow.h"
#include "src/Shared.h"

namespace MainWindow
{
	NOXS; NOSTD;

	void App(RenWin& window)
	{
		while (XMsg::IsOpen(window))
		{
			XWindow::DelayFps(window);

			//未启用DWM绘制边框
			if (!EnableDWM) Shared::DrawBorder(window);
		}
	}
}