#ifndef EPL_TRANSLATOR_HPP
#define EPL_TRANSLATOR_HPP
#include"tace.hpp"
#include"utils.hpp"
#include"EplCodeAst.hpp"
/*代码翻译器:用于将代码解析为AST*/
class EplCodeTranslator
{
	using MemStream=eprogramutils::MemArchiveStream;
public:
	EplCodeTranslator() = default;
	EplCodeTranslator(MemStream& method) {
	
	
	}
};

#endif
