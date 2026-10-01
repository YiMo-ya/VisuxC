#include "BottomBar.h"

#include "src/Shared.h"
#include "src/user/User.h"

namespace BottomBar
{
	NOXS; NOSTD;

	static wstring Type = L"normal";

	static wstring Text = L"准备就绪";

	int h;

	void Draw(RenWin& window)
	{
		static bool Init = false;
		if (!Init)
		{
			h = ScreenSize.y * 0.03;

			Init = true;
		}

		//配色
		if (Type == L"normal" || Type == L"process" || Type == L"loading")
		{
			static Color FillColor = UserData::BackColor == Color(30, 30, 30) ? Color(50, 50, 50, 200) : Color(220, 220, 220, 200);
			XGraph::SetFillColor(FillColor);
		}
		else if (Type == L"error")
		{
			XGraph::SetFillColor(Color(255,100,100));
		}
		else if (Type == L"success")
		{
			XGraph::SetFillColor(Color(100, 255, 100));
		}
		else if (Type == L"stop")
		{
			XGraph::SetFillColor(Color(155, 155, 50));
		}
		else
		{
			static Color FillColor = UserData::BackColor == Color(30, 30, 30) ? Color(50, 50, 50, 200) : Color(220, 220, 220, 200);
			XGraph::SetFillColor(FillColor);
		}

		int StartY = WindowSize.y - h;

		//绘制底色
		XGraph::RectangleShape::FillRect_WithoutBorder(0, StartY, WindowSize.x, h, window);

		//绘制文字
		if (!Text.empty())
		{
			XText::SetFontConfig(FONTCOLOR, FONTSIZE);
			XText::SetFontAdjust(ADJUST_LEFT, ADJUST_CENTER);

			static int StartX = ScreenSize.x * 0.01;
			XText::Xyprintf(StartX, StartY + h / 2, Text, window);
		}
	}

	void SetText(const wstring& text)
	{

	}

	void SetShowType(const wstring& type)
	{
		Type = type;
	}
}