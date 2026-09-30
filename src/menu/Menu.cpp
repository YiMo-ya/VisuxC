#include "Menu.h"
#include "src/Shared.h"
#include "src/info/Message.h"
#include "src/main/Lang.h"
#include "src/user/User.h"
#include "src/main/Debug.h"

//左上角菜单

namespace Menu
{
	NOXS; NOSTD;

	static RenderTexture rt;
	//脏标记
	static bool NeedReRraw = false;

	//子项0为该项名称例如文件(F)，从1开始为子项内容
	static vector<vector<wstring>> SideBarName;

	//按钮文字距离
	static int FontSpace;

	//初始化
	void Init()
	{
		//文字距离初始化
		FontSpace = ScreenSize.x * 0.04;

		//首次启动刷新
		NeedReRraw = true;

		//配置解释器
		wstring SideBarConfigPath = ExePath + L"\\Config\\SideBar\\Config.ini";

		std::ifstream Config(SideBarConfigPath);
		if (!Config.is_open())
		{
			Message::ShowMessage(
				Translate::Translate(L"加载Visux配置失败，重新安装或许能够解决问题。", UserData::Lang),
				Translate::Translate(L"加载配置失败", UserData::Lang),
				ICOTYPE_ERROR,
				{ L"确定" },
				3,
				L"Visux"
			);
			exit(0x02);
		}

		std::string line;
		while (std::getline(Config, line))
		{
			// 跳过空行和 { }
			if (line.empty() || line == "{" || line == "}")
				continue;

			// 去掉行首行尾空格
			line.erase(0, line.find_first_not_of(" \t"));
			line.erase(line.find_last_not_of(" \t") + 1);

			if (line.rfind("name=", 0) == 0)
			{
				std::string nameValue = line.substr(5);
				if (nameValue.empty())
					continue;

				// 去掉 nameValue 首尾空格（防止配置里写了 "name= 文件(F) "）
				nameValue.erase(0, nameValue.find_first_not_of(" \t"));
				nameValue.erase(nameValue.find_last_not_of(" \t") + 1);

				SideBarName.emplace_back();
				SideBarName.back().push_back(
					Translate::Translate(
						XString::Convert::utf8_to_wstring(nameValue),
						UserData::Lang
					)
				);
			}
			else
			{
				if (!SideBarName.empty() && !line.empty())
				{
					SideBarName.back().push_back(
						Translate::Translate(
							XString::Convert::utf8_to_wstring(line),
							UserData::Lang
						)
					);
				}
			}
		}
	}

	void Draw(RenWin& window)
	{
		//初始化
		static bool IsInit = false;
		if (!IsInit)
		{
			Init();

			IsInit = true;
		}

		//渲染
		static int w = ScreenSize.x * 0.3, h = ScreenSize.y * 0.03;
		if (NeedReRraw || XMsg::WindowMsg::IsWindowResize())
		{
			if (rt.resize({ unsigned int(w),unsigned int(h) }))
			{
				rt.clear(Color::Transparent);

				//调试
				Debug::DrawDebugRect(rt, 0, 0, w, h);

				static int FontSize = FONTSIZE;

				XText::SetFontConfig(FONTCOLOR, FontSize);
				XText::SetFontAdjust(ADJUST_CENTER, ADJUST_CENTER);

				static int Size = SideBarName.size();
				for (int i = 0; i < Size; i++)
				{
					wstring name = SideBarName[i][0];

					XText::Xyprintf(FontSpace / 2 + FontSpace * i, h / 2, name, rt);

					Debug::DrawDebugRect(rt, FontSpace * i, 0, FontSpace, h);
				}

				rt.display();
			}
		}

		

		Sprite s(rt.getTexture());
		s.setPosition({ 0.0,0.0 });
		window.draw(s);
	}
}