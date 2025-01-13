#include"eplc.h"
void set_console_code();

//定义的命令行参数

enum class error_type :std::uint16_t
{
	/*成功*/
	success = 0,
	/*未输入有效文件路径*/
	no_input_path = 1,
	/*解析失败*/
	parse_failed = 2
};


/*
-o <output>: 指定输出文件的名称。
-c: 转译源文件但不链接，生成目标.cpp文件。
-Wall: 开启所有警告信息。
-std=<standard>: 指定使用的 C 或 C++ 标准（例如 -std=c++11）。
-I <include_path>: 添加包含文件搜索路径。
-L <library_path>: 添加库文件搜索路径。
-l <library>: 链接使用的库。
-g: 生成调试信息。
-O<level>: 指定优化级别，例如 -O2。*/

constexpr
const
wchar_t*
error_code[] = {
	L"成功",
	L"请输入文件路径",
	L"解析失败"
};

auto main(int argc, const char* argv[], const char* envp[]) -> int {
	using enum  error_type;
	set_console_code();/*utf-8下兼容ansi的中文显示*/
	auto put_error{ [](error_type code) {eprogramutils::appio::ioput(L"\n来自易语言编译器的警告。\n你想要执行无效的指令。\n错误码:",static_cast<std::uint32_t>(code),  "\n错误文本:",error_code[static_cast<std::uint32_t>(code)]); } };
	auto put_success{ []() {eprogramutils::appio::ioput(L"\n来自易语言编译器的提示。\n你的指令已经成功执行。"); } };
	if (argc < 2) {
		put_error(no_input_path);
		return 0;
	}


	

	std::string path{ argv[1] };
	eprogramutils::appio::ioput(ansi2wstr(path.c_str()));
	//成员较多,防止栈溢出
	auto analyzer{ std::make_unique<EplCodeAnalyzer>() };
	if (!analyzer->ParseEcode(path.c_str()))
	{
		put_error(parse_failed);
		return 0;
	}


	EplTranslator et;
	et.Translate(path, analyzer->getAllStruct(), analyzer->getTree());

	//while (true)
	//{
	//	std::wstring help;
	//	std::wcin >> help;
	//	//debug_put(help);
	//}

	put_success();
}