#ifndef EPL_SYS_INFO_HPP
#define EPL_SYS_INFO_HPP
#include"utils.hpp"
/*语言类型*/
enum class EplLanguage : std::uint32_t {
	GBK = 1,   // 国标编码
	ASCII,     // ASCII 编码
	BIG5,      // 大五码
	SJIS       // Shift JIS 编码
};

/*.E源码类型*/
enum class FileType : std::uint32_t
{
	E = 0x01,/*易源码*/
	EC = 0x03/*易模块*/
};
/*编译类型*/
enum class CompileType : std::uint32_t
{
	WindowsForm = 0x00,
	WindowsConsole = 0x01,
	WindowsDLL = 0x02,
	WindowsEC = 0x03E8,
	LinuxConsole = 0x2710,
	LinuxEC = 0x2AF8
};
struct EplSystemInfo
{
	std::uint16_t m_EplMajor{};/*易语言主版本*/
	std::uint16_t m_EplMinor{};/*易语言次版本*/
	int Unknow_1{};
	EplLanguage m_Language{};/*语言*/
	std::uint16_t m_CodeMajor{};/*源码主版本*/
	std::uint16_t m_CodeMinor{};/*源码次版本*/
	FileType m_FileType{};/*文件类型*/
	int Unknow_4{};
	CompileType m_CompileType{};/*编译类型*/
	int Unknow_5[8]{};
};

#endif 
