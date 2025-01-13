#include "krnln.hpp"
#include <windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

namespace krnln {
    void ErrorMsgBox(const std::wstring& filePath, const std::wstring& msg) {
#ifdef _WIN32
#
        std::wstring fileName{ PathFindFileNameW(filePath.c_str()) };/*路径不会为空*/
        std::wstring error_msg{ L"出现运行时异常,已被核心支持库拦截(编译后不会出现该提示，但同样会处理异常操作,防止重大错误引起崩溃!)\n出错文件：" + fileName + L"\n错误信息:" + msg };
        int result = ::MessageBoxW(GetDesktopWindow(), error_msg.c_str(), L"出现重大错误", MB_OK | MB_ICONERROR);
#endif
        constexpr auto eplruntimeerror{ 0x23416 };
        throw eplruntimeerror;
    }
}

