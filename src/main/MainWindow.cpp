#include "MainWindow.h"
#include "src/Shared.h"
#include "src/user/User.h"

#include "src/menu/Menu.h"
#include "src/sidebar/SideBar.h"
#include "src/bottombar/BottomBar.h"

namespace MainWindow
{
	NOXS; NOSTD;

	void App(RenWin& window)
	{
		while (XMsg::IsOpen(window))
		{
			XWindow::DelayFps(window);

			WindowSize = XWindow::GetWindowSize(window);

			//未启用DWM绘制边框
			if (!EnableDWM)
			{
				Shared::DrawBorder(window);
			}
			else
			{
				if (XWindow::IsFocus(window))
				{
					XWindow::DWM::SetWindowBorderColor(window, UserData::MainColor);
				}
				else
				{
					XWindow::DWM::SetWindowBorderColor(window, Color(150,150,150));
				}
			}

			static Color BackColor = UserData::BackColor == Color(30, 30, 30) ? Color(0, 0, 0, 100) : Color(255, 255, 255, 100);
			XGraph::SetFillColor(Color(BackColor));
			XGraph::RectangleShape::FillRect_WithoutBorder(0, 0, WindowSize.x, WindowSize.y, window);

			//菜单消息
			Menu::Msg(window);

			//绘制底部消息栏
			BottomBar::Draw(window);

			//绘制右侧侧边栏
			SideBar::SideBarLevel1::Draw(window);

			//绘制菜单
			Menu::Draw(window);
		}
	}
}