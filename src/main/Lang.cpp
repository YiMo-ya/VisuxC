#include "Lang.h"

//翻译语言

//注意，使用unordered_map

#include <string>
#include <unordered_map>

using WStringList = std::vector<std::wstring>;

std::unordered_map<std::wstring, WStringList> g_I18NTable =
{
    // 错误相关
    { L"在初始化用户数据时发生致命错误",
        {
            L"在初始化用户数据时发生致命错误",   // ZH
            L"A fatal error occurred while initializing user data", // EN
            L"ユーザーデータの初期化中に致命的なエラーが発生しました", // JA
            L"사용자 데이터 초기화 중 치명적인 오류가 발생했습니다", // KO
            L"Une erreur fatale s'est produite lors de l'initialisation des données utilisateur", // FR
            L"Bei der Initialisierung der Benutzerdaten ist ein schwerwiegender Fehler aufgetreten", // DE
            L"Произошла фатальная ошибка при инициализации данных пользователя" // RU
        }
    },

    // UI 通用
    { L"确定",
        { L"确定", L"OK", L"OK", L"확인", L"Confirmer", L"OK", L"ОК" }
    },

    { L"取消",
        { L"取消", L"Cancel", L"キャンセル", L"취소", L"Annuler", L"Abbrechen", L"Отмена" }
    },

    { L"提示",
        { L"提示", L"Tip", L"ヒント", L"팁", L"Conseil", L"Tipp", L"Совет" }
    },
};

namespace Translate
{
    const std::wstring& Translate(
        const std::wstring & chineseText,
        Language lang)
    {
        static const std::wstring empty;

        auto it = g_I18NTable.find(chineseText);
        if (it == g_I18NTable.end())
            return chineseText;

        int index = static_cast<int>(lang);
        if (index < 0 || index >= static_cast<int>(it->second.size()))
            return chineseText;

        const std::wstring& result = it->second[index];
        if (result.empty())
            return chineseText;

        return result;
    }
}