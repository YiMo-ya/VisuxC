#include "SideBar.h"
#include "src/Shared.h"
#include "src/main/Lang.h"
#include "src/user/User.h"

#include "src/main/Debug.h"

#include "src/bottombar/BottomBar.h"

namespace SideBar
{
	NOXS; NOSTD;

	namespace SideBarLevel1
	{
		static RenderTexture rt;
		static bool NeedRedraw = false;

		class SideBarIcon
		{
		public:

			IMAGE img;
			wstring name;

			EV4 NameRectSize;

			void SetName(wstring& ChineseIndex)
			{
				name = Translate::Translate(ChineseIndex, UserData::Lang);
			}

			void SetIcon(filesystem::path& path)
			{
				if (!XImage::NewImage(img, path))
				{
					XImage::NewImage(img, ExePath + L"\\Image\\ErrorIcon.dll");
				}
			}
		};
		vector<SideBarIcon> SideBarLevel1Icons;

		void Draw(RenWin& window)
		{
			//初始化
			static bool Init = false;
			if (!Init)
			{
				NeedRedraw = true;

				ifstream file(ExePath + L"\\Config\\SideBar\\Config.ini");
				if (!file) return;

				SideBarIcon cur;
				std::string line;

				while (std::getline(file, line)) {
					if (line.empty()) continue;

					size_t eq = line.find('=');
					if (eq != std::string::npos) {
						std::string key = line.substr(0, eq);
						std::string val = line.substr(eq + 1);

						if (key == "name") {
							std::wstring wname(val.begin(), val.end());
							cur.SetName(wname);
						}
						else if (key == "imgname" || key == "imagename") {
							std::wstring wimg(val.begin(), val.end());
							filesystem::path p = ExePath + L"Image\\SideBar\\" + wimg + L".dll";
							cur.SetIcon(p);
						}
					}
					else if (line == "}") {
						SideBarLevel1Icons.push_back(cur);
						cur = SideBarIcon();
					}
				}

				Init = true;
			}

			//大小
			static int Size = SideBarLevel1Icons.size();

			static int w = ScreenSize.x / 12;
			static int StartY = ScreenSize.y * 0.03;
			int h = WindowSize.y - StartY - BottomBar::h;
			if (NeedRedraw || XMsg::WindowMsg::IsWindowResize())
			{
				NeedRedraw = false;
				if (rt.resize({ unsigned int(w),unsigned int(h) }))
				{
					rt.clear(Color::Transparent);

					//底色
					static Color FillColor = UserData::BackColor == Color(30, 30, 30) ? Color(50, 50, 50, 200) : Color(220, 220, 220, 200);
					XGraph::SetFillColor(FillColor);
					XGraph::RectangleShape::FillRect_WithoutBorder(0, 0, w, h, rt);

					//调试
					Debug::DrawDebugRect(rt, 0, 0, w, h);

					rt.display();
				}
			}

			Sprite s(rt.getTexture());
			s.setPosition({ 0.0,float(StartY) });
			window.draw(s);

			
		}
	}

	namespace SideBarLevel2
	{

	}
}