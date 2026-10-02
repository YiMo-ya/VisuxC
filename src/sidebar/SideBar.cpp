#include "SideBar.h"
#include "src/Shared.h"
#include "src/main/Lang.h"
#include "src/user/User.h"
#include "src/info/Message.h"

#include "src/main/Debug.h"

#include "src/bottombar/BottomBar.h"
#include "src/menu/Menu.h"

namespace SideBar
{
	NOXS; NOSTD;

	class SideBarIcon
	{
	public:

		IMAGE img;
		wstring name;

		int Width;

		EV IconXOffset;

		float GetTextRenderWidth(const std::wstring& text, unsigned int fontSize, const sf::Font& font)
		{
			sf::Text label(font);
			label.setString(text);
			label.setCharacterSize(fontSize);
			// 不需要 setPosition，bounds 是 local 坐标，跟位置无关

			sf::FloatRect bounds = label.getLocalBounds();
			return bounds.size.x;
		}

		void SetName(wstring& ChineseIndex)
		{
			name = Translate::Translate(ChineseIndex, UserData::Lang);

			Font font;
			wstring FontPath = ExePath + L"\\Font\\zh.dll";
			font.openFromFile(FontPath);
			Width = GetTextRenderWidth(name, FONTSIZE, font) + ScreenSize.x * 0.02;
		}

		void SetIcon(filesystem::path& path)
		{
			if (!XImage::NewImage(img, path))
			{
				XImage::NewImage(img, ExePath + L"\\Image\\ErrorIcon.dll");
			}

			img.color = UserData::BackColor == Color(30, 30, 30) ? Color::White : Color::Black;
		}
	};
	vector<SideBarIcon> SideBarLevel1Icons;

	namespace SideBarLevel1
	{
		static RenderTexture rt;
		static bool NeedRedraw = false;

		//提示
		EV4 NameRect;
		EV NameSize;
		EV NameAlpha;
		int InTime;

		

		//侧边栏选择引索
		int SideBarIndex = -1;
		int SideBarInIndex = -1;
		//侧边栏底线位置
		EV SideBarLinePosY;
		//侧边栏底线大小
		EV SideBarLineSize;
		static int SIDEBAR_LINE_SIZE;

		int w;

		//绘制侧边栏
		void Draw(RenWin& window)
		{
			//初始化
			static bool Init = false;
			if (!Init)
			{
				NeedRedraw = true;

				ifstream file(ExePath + L"\\Config\\SideBar\\Config.ini");
				if (!file)
				{
					Message::ShowMessage(
						Translate::Translate(L"加载Visux配置失败，重新安装或许能够解决问题。", UserData::Lang),
						Translate::Translate(L"加载配置失败", UserData::Lang),
						ICOTYPE_ERROR,
						{ L"确定" },
						3,
						L"Visux"
					);

					exit(0x03);
				}

				SideBarIcon cur;
				std::string line;

				while (std::getline(file, line)) {
					if (line.empty()) continue;

					size_t eq = line.find('=');
					if (eq != std::string::npos) {
						std::string key = line.substr(0, eq);
						std::string val = line.substr(eq + 1);

						if (key == "name") {
							wstring wname = XString::Convert::utf8_to_wstring(val);
							cur.SetName(wname);
						}
						else if (key == "imgname") {
							std::wstring wimg(val.begin(), val.end());
							filesystem::path p = ExePath + L"\\Image\\SideBar\\" + wimg + L".dll";
							cur.SetIcon(p);
						}
					}
					else if (line == "}") {
						SideBarLevel1Icons.push_back(cur);
						cur = SideBarIcon();
					}
				}

				SIDEBAR_LINE_SIZE = ScreenSize.x / 100;

				w = ScreenSize.x / 30;

				Init = true;
			}

			SideBarLinePosY.UpdateAnimation(XEase::EaseBasic::easeOut, 4);
			SideBarLineSize.UpdateAnimation(XEase::EaseBasic::easeOut, 4);

			//图标数据大小
			static int Size = SideBarLevel1Icons.size();
			

			//图标缩放与间隔
			static float Scale = w * 0.6 / 512.0;
			static int Space = w;

			static int StartY = Menu::h;
			int h = WindowSize.y - StartY - BottomBar::h;
			if (NeedRedraw || XMsg::WindowMsg::IsWindowResize()
				|| SideBarLineSize.IsAnimation() || SideBarLinePosY.IsAnimation())
			{
				NeedRedraw = false;
				if (rt.resize({ unsigned int(w),unsigned int(h) }))
				{
					rt.clear(Color::Transparent);

					//图标
					for (int i = 0; i < Size; i++)
					{
						auto& Icon = SideBarLevel1Icons[i];

						if (i == SideBarIndex) Icon.img.color.a = 255;
						else if(i == SideBarInIndex) Icon.img.color.a = 225;
						else Icon.img.color.a = 200;

						//当前页背景
						if (i == SideBarIndex)
						{
							static Color BackFillColor = UserData::BackColor == Color(30, 30, 30) ? Color(80, 80, 80) : Color(200, 200, 200);
							XGraph::SetFillColor(BackFillColor);
							XGraph::RectangleShape::FillRoundRect_WithoutBorder(w * 0.1, Space * i + Space / 2 - w * 0.4, w * 0.8, w * 0.8, ROUNDSIZE, rt);
						}

						//预选页背景
						if (i == SideBarInIndex)
						{
							static Color BackFillColor = UserData::BackColor == Color(30, 30, 30) ? Color(60, 60, 60) : Color(230, 230, 230);
							XGraph::SetFillColor(BackFillColor);
							XGraph::RectangleShape::FillRoundRect_WithoutBorder(w * 0.1, Space * i + Space / 2 - w * 0.4, w * 0.8, w * 0.8, ROUNDSIZE, rt);
						}

						//更新动画
						Icon.IconXOffset.UpdateAnimation(XEase::EaseBasic::easeOut, 4);

						XImage::PutScaleImage(Icon.img, w / 2 + Icon.IconXOffset.value, Space * i + Space / 2, Scale, Scale, rt, 0.5, 0.5);
					}

					//线条
					if (SideBarLineSize.value > 0)
					{
						XGraph::SetColor(UserData::MainColor);
						static int lw = ScreenSize.x / 300;
						XGraph::LineShape::SetLineWidth(lw);
						XGraph::LineShape::Line(
							w * 0.2, SideBarLinePosY.value - SideBarLineSize.value,
							w * 0.2, SideBarLinePosY.value + SideBarLineSize.value, rt);
					}

					//调试
					Debug::DrawDebugRect(rt, 0, 0, w, h);

					rt.display();
				}
			}

			Sprite s(rt.getTexture());
			s.setPosition({ 0.0,float(StartY) });
			window.draw(s);

			//按键
			if(XMsg::MouseMsg::IsMouseIn(0,StartY,w, Space * Size))
			{
				for (int i = 0; i < Size; i++)
				{
					if (XMsg::MouseMsg::IsMouseIn(0, StartY + Space * i, w, Space))
					{
						SideBarInIndex = i;
						NeedRedraw = true;
						if (XMsg::MouseMsg::IsMouseDown(VK::MouseLeft))
						{
							SideBarLevel1Icons[SideBarIndex].IconXOffset.SetAnimation(0, 10);
							SideBarIndex = i;
							SideBarLevel1Icons[i].IconXOffset.SetAnimation(w * 0.1, 10);
							NeedRedraw = true;

							if (SideBarLineSize.value > 0)
							{
								SideBarLinePosY.SetAnimation(Space * i + Space / 2, 10);
							}
							else
							{
								SideBarLinePosY.SetAnimationStartValue(Space * i + Space / 2);
								SideBarLineSize.SetAnimation(SIDEBAR_LINE_SIZE, 10);
							}

							//展开二级列表
							if (SideBarLevel2::SideBarLevel2Width.value <= 0) SideBarLevel2::SideBarLevel2Width.SetAnimation(w * 4, 10);

							//更新二级列表
							SideBarLevel2::NeedRedraw = true;
						}
					}
				}

				//简介计时器
				static int InIndex = -1;
				if (InTime < 15)
				{
					InTime += 1;
					InIndex = -1;
				}
				else
				{
					if (InIndex != SideBarInIndex)
					{
						InIndex = SideBarInIndex;

						if(NameRect.w.value <= 0) NameRect.x.SetAnimationStartValue(w * 0.5);
						NameRect.x.SetAnimation(w * 0.6, 10);

						if (NameRect.h.value <= 0) NameRect.y.SetAnimationStartValue(StartY + Space * SideBarInIndex + Space / 4);
						NameRect.y.SetAnimation(StartY + Space * SideBarInIndex + Space / 2, 10);

						NameRect.w.SetAnimationStartValue(SideBarLevel1Icons[SideBarInIndex].Width * 0.9);
						NameRect.w.SetAnimation(SideBarLevel1Icons[SideBarInIndex].Width, 10);
						NameRect.h.SetAnimationStartValue(w * 0.3);
						NameRect.h.SetAnimation(w * 0.4, 10);

						NameSize.SetAnimationStartValue(FONTSIZE * 0.6);
						NameSize.SetAnimation(FONTSIZE, 10);

						NameAlpha.SetAnimationStartValue(100);
						NameAlpha.SetAnimation(255, 10);
					}
				}
			}
			else
			{
				if(NameRect.w.end != 0) NameRect.w.SetAnimation(0, 10);
				if (NameRect.h.end != 0) NameRect.h.SetAnimation(0, 10);
				if (NameSize.end != 0) NameSize.SetAnimation(0, 10);
				if (NameAlpha.end != 0) NameAlpha.SetAnimation(0, 10);

				NeedRedraw = true;
				SideBarInIndex = -1;

				InTime = 0;
			}

			for (int i = 0; i < Size; i++) Debug::DrawDebugRect(window,0, StartY + Space * i, w, Space);

		}

		//绘制侧边栏提示
		void DrawTips(RenWin& window)
		{
			//简介
			NameRect.x.UpdateAnimation(XEase::EaseBasic::easeOut, 4);
			NameRect.y.UpdateAnimation(XEase::EaseBasic::easeOut, 4);
			NameRect.w.UpdateAnimation(XEase::EaseBasic::easeOut, 4);
			NameRect.h.UpdateAnimation(XEase::EaseBasic::easeOut, 4);
			NameSize.UpdateAnimation(XEase::EaseBasic::easeOut, 4);
			NameAlpha.UpdateAnimation(XEase::EaseBasic::easeOut, 4);

			//显示简介
			if (NameRect.w.value > 0 && NameRect.h.value > 0 && NameAlpha.value > 0)
			{
				static IMAGE Shadow;

				//阴影初始化
				static bool ShadowInit = false;
				if (!ShadowInit)
				{
					XImage::NewImage(Shadow, ExePath + L"\\Image\\SideBar\\Shadow.dll");
					ShadowInit = true;
				}

				//绘制阴影
				Shadow.color.a = NameAlpha.value / 2;
				static float ShadowScale = 0.2;
				float RectScaleX = NameRect.w.value * (1 + ShadowScale) / 500.0, RectScaleY = NameRect.h.value * (1 + ShadowScale) / 150.0;
				XImage::PutScaleImage(Shadow,
					NameRect.x.value - NameRect.w.value * ShadowScale * 0.5,
					NameRect.y.value - NameRect.h.value * ShadowScale * 1.5,
					RectScaleX, RectScaleY, window);

				//绘制背景
				static Color BackFillColor = UserData::BackColor == Color(30, 30, 30) ? Color(50, 50, 50) : Color(250, 250, 250);
				BackFillColor.a = NameAlpha.value * 0.9;
				XGraph::SetFillColor(BackFillColor);
				XGraph::RectangleShape::FillRoundRect_WithoutBorder(
					NameRect.x.value, NameRect.y.value,
					NameRect.w.value, NameRect.h.value, ROUNDSIZE, window);

				//绘制文字
				if (SideBarInIndex > -1)
				{
					static Color FontColor = FONTCOLOR;
					FontColor.a = NameAlpha.value;
					XText::SetFontConfig(FONTCOLOR, NameSize.value);
					XText::SetFontAdjust(ADJUST_CENTER, ADJUST_CENTER);

					XText::Xyprintf(
						NameRect.x.value + NameRect.w.value / 2,
						NameRect.y.value + NameRect.h.value / 2,
						SideBarLevel1Icons[SideBarInIndex].name, window);
				}
			}
		}
	}

	namespace SideBarLevel2
	{
		RenderTexture rt;
		bool NeedRedraw = false;

		EV SideBarLevel2Width;

		void Draw(RenWin& window)
		{
			//更新动画
			SideBarLevel2Width.UpdateAnimation(XEase::EaseBasic::easeOut, 4);

			//开始Y坐标
			static int StartY = Menu::h;
			//高度
			int h = WindowSize.y - StartY - BottomBar::h;

			static int Space = ScreenSize.x / 300;

			//纹理尺寸大于0时绘制
			if(SideBarLevel2Width.value > 0)
			{
				if (SideBarLevel2Width.IsAnimation() || NeedRedraw || XMsg::WindowMsg::IsWindowResize())
				{
					if (rt.resize({ unsigned int(SideBarLevel2Width.value),unsigned int(h) }))
					{
						rt.clear(Color::Transparent);

						//绘制底色
						static int lw = ScreenSize.x / 500;
						XGraph::LineShape::SetLineWidth(lw);
						XGraph::SetColor(Color(120, 120, 120));
						XGraph::RectangleShape::RoundRect(0, 0, SideBarLevel2Width.value, h, ROUNDSIZE, rt);

						//绘制名字
						XText::SetFontConfig(FONTCOLOR, FONTSIZE);
						XText::SetFontAdjust(ADJUST_LEFT, ADJUST_TOP);

						static int FontSpace = ScreenSize.x / 200;
						XText::Xyprintf(FontSpace, FontSpace, SideBarLevel1Icons[SideBarLevel1::SideBarIndex].name, rt);

						rt.display();
					}
				}

				
			}

			//调整大小
			static bool IsResize = false;
			static int ResizeTipLineAlpha = 0;
			if (XMsg::MouseMsg::IsMousePress(VK::MouseLeft) && SideBarLevel2Width.value > 0)
			{
				if (XMsg::MouseMsg::IsMouseIn(SideBarLevel1::w + SideBarLevel2::SideBarLevel2Width.value, StartY, Space, h))
				{
					IsResize = true;
				}

				if (IsResize)
				{
					int x = XMsg::MouseMsg::GetMousePosWindow().x;

					SideBarLevel2Width.SetAnimationStartValue(x - SideBarLevel1::w);

					ResizeTipLineAlpha = 255;

					//小于一个阈值，直接收回
					if (SideBarLevel2Width.value < Space)
					{
						SideBarLevel2Width.SetAnimationStartValue(0);
						IsResize = false;

						SideBarLevel1::SideBarLineSize.SetAnimation(0, 10);
						SideBarLevel1Icons[SideBarLevel1::SideBarIndex].IconXOffset.SetAnimation(0, 10);
						SideBarLevel1::SideBarIndex = -1;
						SideBarLevel1::NeedRedraw = true;
					}

					NeedRedraw = true;
				}
			}
			else
			{
				if (IsResize) IsResize = false;

				if (ResizeTipLineAlpha - 30 > 0) ResizeTipLineAlpha -= 30;
				else ResizeTipLineAlpha = 0;
			}
			
			//纹理尺寸大于0时绘制
			if (SideBarLevel2Width.value > 0)
			{
				Sprite s(rt.getTexture());
				s.setPosition({ (float)SideBarLevel1::w,(float)StartY });
				window.draw(s);
			}

			//调整大小提示线
			if (ResizeTipLineAlpha > 0)
			{
				static int lw = ScreenSize.x / 300;
				XGraph::LineShape::SetLineWidth(lw);
				XGraph::LineShape::SetLineCap(LINECAP_FLAT);
				Color LineColor = UserData::MainColor;
				LineColor.a = ResizeTipLineAlpha;
				XGraph::SetColor(LineColor);
				XGraph::LineShape::Line(
					SideBarLevel1::w + SideBarLevel2::SideBarLevel2Width.value + Space / 2, StartY,
					SideBarLevel1::w + SideBarLevel2::SideBarLevel2Width.value + Space / 2, StartY + h,
					window);
				XGraph::LineShape::SetLineCap();
			}
		}
	}
}