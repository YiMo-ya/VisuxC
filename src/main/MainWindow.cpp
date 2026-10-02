#include "MainWindow.h"
#include "src/Shared.h"
#include "src/user/User.h"

#include "src/menu/Menu.h"
#include "src/sidebar/SideBar.h"
#include "src/bottombar/BottomBar.h"

namespace MainWindow
{
	NOXS; NOSTD;

	void DrawWorkRect(RenWin& window)
	{
		if (EnableDWM)
		{
			static Color BackColor = UserData::BackColor == Color(30, 30, 30) ? Color(40, 40, 40, 200) : Color(255, 255, 255, 200);
			XGraph::SetFillColor(Color(BackColor));
			XGraph::RectangleShape::FillRect_WithoutBorder(0, 0, WindowSize.x, WindowSize.y, window);
		}

		static Color BackColor = UserData::BackColor == Color(30, 30, 30) ? Color(20, 20, 20, 200) : Color(200, 200, 200, 200);
		XGraph::SetFillColor(Color(BackColor));
		static int StartY = Menu::h;
		static int Space = ScreenSize.x / 300;

		static int lw = ScreenSize.x / 1920;
		XGraph::LineShape::SetLineWidth(lw);
		static Color LineColor = UserData::BackColor == Color(30, 30, 30) ? Color(100, 100, 100) : Color(100, 100, 255);
		XGraph::SetColor(Color(120,120,120));

		int StartX = SideBar::SideBarLevel1::w;
		if (SideBar::SideBarLevel2::SideBarLevel2Width.value > 0) StartX += SideBar::SideBarLevel2::SideBarLevel2Width.value + Space;

		XGraph::RectangleShape::FillRoundRect(
			StartX,
			StartY,
			WindowSize.x - StartX - Space * 3,
			WindowSize.y - StartY - BottomBar::h, ROUNDSIZE, window);
	}

	void App(RenWin& window)
	{

		Menu::h = ScreenSize.y * 0.04;

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

			//绘制工作区背景
			DrawWorkRect(window);

			//菜单消息
			Menu::Msg(window);

			//绘制底部消息栏
			BottomBar::Draw(window);

			//绘制右侧侧边栏
			SideBar::SideBarLevel1::Draw(window);

			//绘制右侧侧边栏（2级）
			SideBar::SideBarLevel2::Draw(window);

			//绘制右侧侧边栏（1级）提示
			SideBar::SideBarLevel1::DrawTips(window);

			//绘制菜单
			Menu::Draw(window);
		}
	}
}