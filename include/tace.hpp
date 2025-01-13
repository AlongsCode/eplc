#ifndef EPLTACE_HPP
#define EPLTACE_HPP
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
std::wstring ansi2wstr(const std::string_view& utf8String);
std::string ansi2u8(const std::string_view& ansiString);
std::wstring u82wstr(const std::string_view& String);
#ifdef _DEBUG



namespace elibstl_debug {

    namespace detail {
        // pt_impl 函数用于将不同类型的值转换为字符串并附加到给定字符串中
        template <class Ty>
        static auto pt_impl(std::string& str, Ty v) -> void {
            std::stringstream ss;
            ss << v;
            str.append(ss.str() + " | ");
        }
    }

    // pt 函数用于将不同类型的值转换为字符串并附加到给定字符串中
    template <class Ty>
    static void pt(std::string& str, Ty v) {
        detail::pt_impl(str, v);
    }

    // 以下是 pt 函数的其他重载版本，用于支持不同类型的值


    static void pt(std::string& str, const std::string& s)
    {
        std::stringstream ss;
        if (s.empty())
        {
            ss << "[空文本] | ";
        }
        else
        {
            ss << "[文本:" << s.length() << "]" << "\"" << s << "\" | ";;
        }
        str.append(ss.str());
    }
    static void pt(std::string& str, const std::vector<unsigned char>& s)
    {
        if (s.empty())
        {
            str.append(" [字节集:(空)] |");
            return;
        }
        for (size_t i = 0; i < s.size(); i++)
        {
            if (i == 0) {
                str.append("  [字节集 :" + std::to_string((int)s.size()) + " ]{");
            }
            str.append(std::to_string((int)s.data()[i]));
            str.append(",");
        }
        str.pop_back();
        str.append("} | ");

    };
    template <class Ty>
    static void pt(std::string& str, const std::vector<Ty>& v) {
        if (v.empty()) {
            str.append(" 数组:(空) |");
            return;
        }

        for (const auto& item : v) {
            str.append(detail::pt_impl(str, item));
            str.append(",");
        }

        str.pop_back(); // 移除最后一个逗号
        str.append("} | ");
    }

    // OutputString 函数用于输出字符串
    void OutputString(const char* str);

    // debug_put_elibstl 函数用于将调试信息输出到字符串并调用 OutputString 函数
    template <class... T>
    static void debug_put_elibstl(const std::string& file, const int line, T... args) {
        std::string str = "* ";
        str.append("[行:").append(std::to_string(line)).append("] ").append(file).push_back('\n');
        std::initializer_list<int>{(pt(str, std::forward<T>(args)), 0)...};
        str.pop_back(); // 移除最后一个管道符
        str.pop_back(); // 移除最后一个空格
        str.push_back('\n');
        OutputString(str.c_str());
    }
}
// 宏定义eplerrorstr 格式化错误码
#define eplerrorstr(str)  std::string("\n\n警告!!!\n\n出现一个严重的异常错误:\n来自文件:").append(std::filesystem::path(__FILE__).filename().string()).append("\n出错位置:").append("[函数 | 行:").append(std::to_string(__LINE__)).append("]").append(__FUNCTION__).append("\n错误信息:").append("\"").append(str).append("\"")
// 宏定义 debug_put 用于调用 debug_put_elibstl 函数，传递文件名、行号以及其他参数
#define debug_put(...) elibstl_debug::debug_put_elibstl(__FILE__, __LINE__, __VA_ARGS__)
// 宏定义 debug_throw 用于抛出运行时错误
#define debug_throw(str) throw std::runtime_error(eplerrorstr(str))
// 宏定义 debug_break 用于中断程序执行并输出调试信息
#define debug_break(str) {debug_put(str);__debugbreak(); }
// 宏定义debug_catch 捕获异常并输出
#define debug_catch() catch (std::runtime_error& e) { debug_break(e.what()); }
#else
#define eplerrorstr(str)  std::string("\n\n警告!!!\n\n出现一个严重的异常错误:\n来自文件:").append(std::filesystem::path(__FILE__).filename().string()).append("\n出错位置:").append("[函数 | 行:").append(std::to_string(__LINE__)).append("]").append(__FUNCTION__).append("\n错误信息:").append("\"").append(str).append("\"").append("\n\n程序退出!\n")
#define debug_put(...)
#define debug_break(str)
#define debug_throw(str) throw std::runtime_error(eplerrorstr(str))
#define debug_catch() catch (std::runtime_error& e) { eprogramutils::appio::ioput(u82wstr(e.what())); exit(0); }
#endif

#endif // !EPLTACE_HPP
