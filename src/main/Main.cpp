#include "Xs/Xs.h"
#include "src/Shared.h"
#include "src/user/User.h"
#include "src/info/Message.h"

#include "Debug.h"
#include "MainWindow.h"

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
	Debug::Debug = true;

	//启用高DPI
	XWindow::SetDPIAware();

	//初始化屏幕大小
	ScreenSize = XSystem::Info::GetScreenSize();

	FONTSIZE = ScreenSize.x / 100;

	//初始化用户数据
	UserData::Read();

	//自适应字体大小
	if (UserData::Lang == EN) FONTSIZE *= 0.9;

	//检查DWM是否可用
	EnableDWM = CanCallDwmSetWindowAttribute();

	//初始化文字颜色
	FONTCOLOR = UserData::BackColor == Color(30, 30, 30) ? Color::White : Color::Black;

	//圆角大小
	ROUNDSIZE = ScreenSize.x / 200;

	//字体位置
	wstring FontPath = ExePath + L"\\Font\\zh.dll";
	if (!XFile::Exists(FontPath))
	{
		Message::ShowMessage(L"找不到字体文件", L"初始化错误", ICOTYPE_ERROR);
	}
	Font font;
	font.openFromFile(FontPath);
	XText::SetFont(font);
}

#pragma endregion

int main()
{
	InitApp();

	RenWin window;

	XWindow::CreateGraphWindow(window, -1,-1,ScreenSize.x / 1.3, ScreenSize.y / 1.3, L"VisuxC");
	XWindow::SetWindowMinSize(window, ScreenSize.x / 1.5, ScreenSize.y / 1.5);

	if (EnableDWM)
	{
		if(UserData::CanEnableDWM)
		{
			XWindow::DWM::SetWindowBackType(window, BACKTYPE_HEAVYMICA);
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

	MainWindow::App(window);
}