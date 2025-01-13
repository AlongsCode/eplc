#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <string>
#include <string_view>
void set_console_code() {

    std::wcout.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);
    std::wcout.tie(nullptr);
    // 设置宽字符输出流的本地化，支持中文显示
    std::wcout.imbue(std::locale("chs"));
    // 设置控制台字体
    #ifdef _WIN32
        CONSOLE_FONT_INFOEX fontInfo{ 0 };
        fontInfo.cbSize = sizeof(fontInfo);
        fontInfo.dwFontSize.Y = 16;
        fontInfo.FontWeight = FW_NORMAL;
        wcscpy(fontInfo.FaceName, L"Constantia"); // 使用新宋体字体
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fontInfo);
    #endif
}


std::string ansi2u8(const std::string_view& ansiString) {
#ifdef _WIN32
    // 转换ansi到宽字符（Unicode）
    int nRetLen = MultiByteToWideChar(CP_ACP, 0, ansiString.data(), -1, nullptr, 0);
    std::wstring unicodeStr(nRetLen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString.data(), -1, &unicodeStr[0], nRetLen);
    // 转换ansi到utf8
    nRetLen = WideCharToMultiByte(CP_UTF8, 0, unicodeStr.data(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8Str(nRetLen, '\0');
    WideCharToMultiByte(CP_UTF8, 0, unicodeStr.data(), -1, &utf8Str[0], nRetLen, nullptr, nullptr);
    return utf8Str;
#endif 
}

std::wstring ansi2wstr(const std::string_view& utf8String) {
#ifdef _WIN32
    // 转换UTF-8到宽字符（Unicode）
    int nRetLen = MultiByteToWideChar(CP_ACP, 0, utf8String.data(), -1, nullptr, 0);
    std::wstring unicodeStr(nRetLen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, utf8String.data(), -1, &unicodeStr[0], nRetLen);
    return unicodeStr;

#endif
}
std::wstring u82wstr(const std::string_view& utf8String) {
#ifdef _WIN32
    // 转换UTF-8到宽字符（Unicode）
    int nRetLen = MultiByteToWideChar(CP_UTF8, 0, utf8String.data(), -1, nullptr, 0);
    std::wstring unicodeStr(nRetLen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8String.data(), -1, &unicodeStr[0], nRetLen);
    return unicodeStr;

#endif
}
namespace elibstl_debug {
	void OutputString(LPCSTR str) {
		OutputDebugStringW(ansi2wstr(str).c_str());
	}
}