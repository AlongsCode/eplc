#ifndef EPL_COMPLIER_H
#define EPL_COMPLIER_H
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "tace.hpp"
#include "utils.hpp"
#include "EplDefine.h"

/*辅助函数,主要是用于编码转换*/




/*以下为代码解析器依赖*/

/*易语言段信息分析器*/
#include "EplSegMent.hpp"
/*易语言系统信息段处理*/
#include "EplSysInfo.hpp"
/*易语言用户信息定义处理*/
#include "EplUserInfo.hpp"
/*模块信息处理*/
#include "EplCDependencies.hpp"
/*程序段*/
#include "EplCodeSection.hpp"
/*程序资源段*/
#include "EplResourceSection.hpp"
/*程序文件数据分区*/
#include "EplFolderSection.hpp"
/*类公开性*/
#include "EplClassPublicity.hpp"
/*基础对象映射部分*/
#include "EplObjMap.hpp"
/*AST抽象语法树代码部分*/
#include "EplCodeAst.hpp"
/*代码解析器*/
#include "EplCodeAnalyzer.hpp"


/*代码翻译器*/
#include"EplCodeTranslator.hpp"
#endif
