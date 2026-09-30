#include "Xs/Xs.h"
#include "Shared.h"
#include "User.h"

NOXS; NOSTD;

//初始化
#pragma region MyRegion

//检查DWM是否可用
bool CanCallDwmSetWindowAttribute()
{
	HMODULE hDwm = LoadLibraryW(L"dwmapi.dll");
	if (!hDwm)
		return false;

	FARPROC proc = GetProcAddress(hDwm, "DwmSetWindowAttribute");
	FreeLibrary(hDwm);
	return (proc != nullptr);
}

void InitApp()
{
	//启用高DPI
	XWindow::SetDPIAware();

	//初始化屏幕大小
	ScreenSize = XSystem::Info::GetScreenSize();

	//初始化用户数据
	UserData::Read();

	EnableDWM = CanCallDwmSetWindowAttribute();
}

#pragma endregion

//主逻辑
#pragma region MyRegion



void App(RenWin& window)
{
	while (XMsg::IsOpen(window))
	{
		XWindow::DelayFps(window);

		//未启用DWM绘制边框
		if (!EnableDWM) Shared::DrawBorder(window);
	}
}

#pragma endregion



int main()
{
	InitApp();

	RenWin window;

	XWindow::CreateGraphWindow(window, -1,-1,ScreenSize.x / 1.5, ScreenSize.y / 1.5, L"VisuxC");

	if (EnableDWM)
	{
		if(UserData::CanEnableDWM)
		{
			XWindow::DWM::SetWindowBackType(window, BACKTYPE_BLUR);
			XWindow::DWM::ExtendIntoClientArea(window, -1, -1, -1, -1);
			XWindow::DWM::SetWindowDarkMode(window, UserData::BackColor == Color(30, 30, 30) ? true : false);
		}
		else
		{
			XWindow::SetBackGroundColor(UserData::BackColor);
			XWindow::DWM::SetWindowTitleBarColor(window, UserData::BackColor);
		}
		XWindow::DWM::SetWindowBorderColor(window, UserData::MainColor);
		
	}
	else
	{
		XWindow::SetBackGroundColor(UserData::BackColor);
	}

	//初始化图标
	XWindow::SetIcon(window, L"Icon.dll");

	XWindow::SetWindowAlpha(window, 252);

	App(window);
}