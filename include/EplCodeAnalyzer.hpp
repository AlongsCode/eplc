#ifndef EPL_CODEANALYZER_HPP
#define EPL_CODEANALYZER_HPP
#include <fstream>
#include <functional>
#include <filesystem>
#include <format>
#include"utils.hpp"

namespace {


	inline bool WriteDataToFile(const std::string& filename, const std::vector<unsigned char>& data)
	{
		if (filename.empty() || data.empty())
			return false;

		try
		{
			std::ofstream outFile(filename, std::ios::binary);
			if (!outFile.is_open())
				return false;

			outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
			return true;
		}
		catch (const std::exception&) {
			return false;
		}

	}

	inline bool WriteDataToFile(const std::string_view& filename, const std::string_view& data)
	{
		if (filename.empty() || data.empty())
			return false;
		try
		{
			std::ofstream outFile(filename.data(), std::ios::binary);
			if (!outFile.is_open())
				return false;

			outFile.write(data.data(), data.size());
			return true;
		}
		catch (const std::exception&) {
			return false;
		}
	}

}






/*易语言代码解析器*/
class EplCodeAnalyzer
{
	using Stream = eprogramutils::MemArchiveStream;
	Stream m_Stream;/*数据流*/
	std::string m_path;/*文件路径*/
	using LastTree = std::shared_ptr<eplcltype::FolderTree>;/*最终树*/
public:
	EplCodeAnalyzer() = default;
	bool ParseEcode(const std::string_view& path) {
		if (!m_Stream.open(path))
			return false;
		if (m_Stream.size() <= 8)/*甚至不足文件头*/
			return false;
		if (!CheckIsEfile(m_Stream.read<U64EPLMAGIC>()))//验校文件头{'C', 'N', 'W', 'T','E', 'P', 'R', 'G'}
			return false;

		m_path = path;
		/*如果流未读完则始终进行读取*/
		while (!m_Stream.is_end())
		{
			/*读取段信息*/
			auto Sm{ m_Stream.read<EplSegMent>() };
			if (!Sm.CheckThis())
				break;
			/*如果段名则代表读到尾段或数据错误*/
			if (Sm.GetName() == "程序资源段")
			{
				debug_put(Sm.GetKey());
				debug_put(Sm.GetDataSize());
			}


			if (Sm.GetKey() == 0)
				break;
			/*切片*/
			auto temp = m_Stream.memstream(Sm.GetDataSize());
			/*处理段数据*/
			ParsingSection(Sm, temp);
		}
		/*构建易语言代码数*/
		m_ObjformIdmap = m_CodeInfo.buildHashMap();
		//m_ConstformIdmap = m_ResInfo.buildConstHashMap();
		for (auto& [c, s] : m_ObjformIdmap)
		{
			debug_put(s->getName());
		}

		buildEplIrMap();

		return true;
	};
	auto getTree() {
		return m_FolderTree;
	}
	auto getAllStruct() {
		return m_EplStructMap;
	}
private:
	/*解析段*/
	void ParsingSection(const EplSegMent& Sm, Stream& stream) {
		if (stream.size() == 0)
		{
			return;
		}
		eprogramutils::appio::ioput(L"\n正在分析段:", ansi2wstr(Sm.GetName()), L"。数据流长度:", stream.size(), L"字节\n");
		if (Sm.GetKey() == 0x02007319)//系统信息段
			stream >> m_SystemInfo;
		else if (Sm.GetKey() == 0x01007319)//用户信息段
			stream >> m_UserInfo;
		else if (Sm.GetKey() == 0x04007319)//程序资源段
			stream >> m_ResInfo;
		else if (Sm.GetKey() == 0x03007319)//程序段
			stream >> m_CodeInfo;
		else if (Sm.GetKey() == 234910489)
			stream >> m_FolderInfo;
		else if (Sm.GetKey() == 184578841)
			stream >> m_ClassPublicity;
		else if (Sm.GetKey() == 201356057)
			stream >> m_CDependencies;
		/*else if (BlockName == "辅助信息段3")
			stream >> m_ProjectConfig;*/

	}
private:
	std::unordered_map<std::uint32_t, std::shared_ptr<eprogramutils::EplIDBase>> m_ObjformIdmap;
	std::unordered_map<std::uint32_t, std::shared_ptr<eprogramutils::EplIDBase>> m_ConstformIdmap;
	EplSystemInfo m_SystemInfo;/*系统信息*/
	EplUserInfo m_UserInfo;/*用户信息/基础配置信息*/
	EplCodeSection m_CodeInfo;/*程序段*/
	eplconstold::EplResourceSection m_ResInfo;/*资源段*/
	EplCDependencies m_CDependencies;/*易模块记录段*/
	EplFolderSection m_FolderInfo;/*编辑过滤器信息段/程序文件夹*/
	EplClassPublicity m_ClassPublicity;/*辅助信息段2/类公开性*/
	//EplEcPasswordConfig m_ProjectConfig;/*加密配置信息*/
private:
	using eobj_id = std::uint32_t;
	using eobj_hasid = std::shared_ptr<eprogramutils::EplIDBase>;
	using eobj = std::shared_ptr<eplcltype::BaseCodeObj>;
	/*完全初级映射表,主要用于提取名称*/
	std::unordered_map<eobj_id, eobj_hasid> m_Idmap;
	/*文件表,用于构建目录，传递给抽象语法树*/
	std::shared_ptr<eplcltype::FolderTree> m_FolderTree;
	/*完全顶级映射总表*/
	std::unordered_map<eobj_id, eobj> m_AstIdmap;
	/*映射常量表*/
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::UserDefinedConst>> m_EplConstMap;
	/*映射资源表*/
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::UserDefinedRes>> m_EplResMap;
	/*易语言类表*/
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::FinalClass>> m_EplClassMap;
	/*易语言结构体表*/
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::Struct>> m_EplStructMap;
	/*易语言全部数据类型表,包含类/基础数据类型/结构体*/
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::BaseDataType>> m_EplDataTypeMap;
	/*该表用于存储易语言共享函数*/
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::FianlShareFunction>> m_EplShareFunMap;
	//该表用于记录代码中调用支持库的数据类型
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::LibDefinedDataType>> m_EplLibDefinedDataTypeMap;
	//该表用于储存全局变量
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::GlobalVar>> m_EplGlobalVarMap;
	//程序集映射表
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::FinalAssembly>> m_EplAssemblyMap;
	//程序集文件和类文件对象映射表,用于文件树创建文件夹
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::BaseAssembly>> m_EplAssemblyAndClassMap;
	//总方法映射表
	std::unordered_map<eobj_id, std::shared_ptr<eplcltype::FinalAssemblyFunction>> m_EplFunction;

private:
	/*获取并填充支持库数据类型引用*/
	std::shared_ptr <eplcltype::LibDefinedDataType> getLibDefinedDataType(const eobj_id id) {
		if (m_EplLibDefinedDataTypeMap.contains(id))
			return m_EplLibDefinedDataTypeMap[id];
		else
		{
			m_EplLibDefinedDataTypeMap.emplace(id, std::make_shared<eplcltype::LibDefinedDataType>(id));
			return m_EplLibDefinedDataTypeMap[id];
		}
	}
	/*安全的获取数据类型*/
	std::shared_ptr<eplcltype::BaseDataType> getDataType(const eobj_id id, const std::string& up, const std::string name) {
		if (m_EplDataTypeMap.contains(id))
			return m_EplDataTypeMap[id];
		else if (elibcall::isFneDataType(id))
			return getLibDefinedDataType(id);

		std::stringstream ss;
		ss << "获取数据类型失败,未知数据类型ID:" << id << " | 调用位置:" << up << " | ";
		if (!name.empty())
		{
			ss << "类型名称:" << ansi2u8(name.c_str()).c_str() << "";
		}
		debug_throw(ss.str());
	}

	/*构建易语言到C++的中间语法树*/
	void buildEplIrMap() {
		debug_put("start", __FUNCTION__);
		auto putnewline = [](const std::wstring& str = {}) {eprogramutils::appio::ioput(str); eprogramutils::appio::ioput(L"\n--------------------------------------------------\n"); };
		putnewline(L"\n开始构建代码↓↓↓");
		try {
			buildIrResAndConstMap();/*构建文件树id名称映射表*/
			putnewline(L"资源段数据读取完毕,常量:" + std::to_wstring(m_EplConstMap.size()) + L"个,资源:" + std::to_wstring(m_EplResMap.size()) + L"个。");

			buildIrDataTypeMap();/*构建数据类型*/
			putnewline(L"数据类型构建完毕,类对象:" + std::to_wstring(m_EplClassMap.size()) + L"个,结构体:" + std::to_wstring(m_EplStructMap.size()) + L"个。");
			buildIrShareFunctionMap();/*构建共享函数映射表*/
			putnewline(L"共享函数映射表构建完毕,总计:" + std::to_wstring(m_EplShareFunMap.size()) + L"个。");
			buildIrGlobalVarMap();/*构建全局变量映射表*/
			putnewline(L"全局变量映射表构建完毕,总计:" + std::to_wstring(m_EplGlobalVarMap.size()) + L"个。");
			buildIrAssemblyMap();/*构建程序集映射表*/
			putnewline(L"程序集映射表构建完毕,程序集文件:" + std::to_wstring(m_EplAssemblyMap.size()) + L"个,类文件:" + std::to_wstring(m_EplClassMap.size()) + L"个");
			buildIrMethodMap();/*构建方法映射表*/
			putnewline(L"方法映射表构建完毕,全局方法:" + std::to_wstring(m_EplFunction.size()) + L"个");
			buildFoldTree();/*构建文件树*/

			putnewline(L"文件树构建完毕:\n");
			if (false)
			{
				m_FolderTree->printTree();
			}
			//为防止控制台输出过慢,手动编辑缓冲区

			//设置完毕,输出tree

			putnewline(L"ast完全构建完毕,开始交由转译器");


		}debug_catch();
	}
	void buildIrMethodMap() {
		debug_put("start", __FUNCTION__);
		/*构建方法映射初级表*/
		std::unordered_map<eobj_id, std::shared_ptr<EplMethod>>funcmap;
		/*丢失代码*/
		std::unordered_map<eobj_id, std::shared_ptr<EplMethod>> lostfuncmap;
		/*先构建*/
		for (const auto& c : m_CodeInfo.method_info)
		{
			if (!m_EplAssemblyAndClassMap.contains(c.m_Class_id))
			{
				//丢失的段
				lostfuncmap.emplace(c.m_id, funcmap[c.m_id]);
				continue;
			}

			m_EplFunction.emplace(c.m_id, std::make_shared<eplcltype::FinalAssemblyFunction>(c.m_name, c.m_id, m_EplAssemblyAndClassMap[c.m_Class_id], c.m_isPublic, c.m_remark));
		}
		std::unique_ptr<eplast::Parser> parser = std::make_unique<eplast::Parser>(m_EplGlobalVarMap, m_EplFunction, m_EplShareFunMap, m_EplStructMap);
		for (const auto& c : m_CodeInfo.method_info)
		{

			if (!m_EplFunction.contains(c.m_id))
			{
				continue;
			}
			auto& p{ m_EplFunction[c.m_id] };

			/*截取参数,返回值*/
			std::vector<std::shared_ptr<eplcltype::Param>> params;
			for (const auto& a : c.m_Parameters)
			{
				if (a.m_name.empty())
					continue;
				if (a.m_datatype == 0)//强制转换为int
					params.push_back(std::make_shared<eplcltype::Param>(a.m_name, a.m_id, m_EplDataTypeMap[static_cast<unsigned int>(wteplapi::DATA_TYPE::SDT_INT)], bool{ (a.m_flags & 2) != 0 }, bool{ (a.m_flags & 4) != 0 }, bool{ (a.m_flags & 8) != 0 }, a.m_comment));
				else
					params.push_back(std::make_shared<eplcltype::Param>(a.m_name, a.m_id, getDataType(a.m_datatype, __FUNCTION__, a.m_name), bool{ (a.m_flags & 2) != 0 }, bool{ (a.m_flags & 4) != 0 }, bool{ (a.m_flags & 8) != 0 }, a.m_comment));

			}
			p->setParam(params);
			p->setReturnType(getDataType(c.m_ReturnDataType, __FUNCTION__, c.m_name));
			/*也同样加到集内部*/
			m_EplAssemblyAndClassMap[c.m_Class_id]->addFunc(p);
			//设置变量
			for (const auto& v : c.m_Variables)
			{

				if (v.m_name.empty())
					continue;
				if (v.m_datatype == 0)//强制转换为int
					p->addVar(std::make_shared<eplcltype::Var>(v.m_name, v.m_id, m_EplDataTypeMap[static_cast<unsigned int>(wteplapi::DATA_TYPE::SDT_INT)], v.m_ubound, bool{ (v.m_flags & 1) != 0 }, v.m_comment));
				else
					p->addVar(std::make_shared<eplcltype::Var>(v.m_name, v.m_id, getDataType(v.m_datatype, __FUNCTION__, v.m_name), v.m_ubound, bool{ (v.m_flags & 1) != 0 }, v.m_comment));
			}
			debug_put(c.m_name,c.getID());

			auto ast = parser->parse(c.m_codedata.ExpressionData, p);
			std::string code;
			for (const auto& s : ast)
			{
				code += s->generateCode() + "\n";
			}
			p->setCode(code);
		}

	}
	void buildIrAssemblyMap() {
		debug_put("start", __FUNCTION__);
		/*构建程序集映射表*/
		auto assemblymap = m_CodeInfo.makeHashAssembly();
		/*构建程序集*/
		for (const auto& [id, c] : assemblymap)
		{
			m_EplAssemblyMap.emplace(id, std::make_shared<eplcltype::FinalAssembly>(c->m_name, id, c->m_remark));
		}
		/*填充*/
		for (const auto& [id, c] : m_EplAssemblyMap)
		{
			if (!assemblymap.contains(id))
			{
				debug_throw("原始程序集映射映射表中不存在对应ID");
			}
			std::vector<std::shared_ptr<eplcltype::Var>> vars;
			for (const auto& m : assemblymap[id]->m_Variables)
			{
				std::shared_ptr<eplcltype::Var> temp;
				if (m.m_name.empty())
					continue;
				
				if (m.m_datatype == 0)//强制转换为int
					temp = std::make_shared<eplcltype::Var>(m.m_name, m.getID(), m_EplDataTypeMap[static_cast<unsigned int>(wteplapi::DATA_TYPE::SDT_INT)], m.m_ubound, true, m.m_comment);
				else if (m_EplDataTypeMap.contains(m.m_datatype))
				{
					debug_put(m.m_name);
					temp = std::make_shared<eplcltype::Var>(m.m_name, m.getID(), getDataType(m.m_datatype, __FUNCTION__, m.m_name), m.m_ubound, true, m.m_comment);
				}
				else if (elibcall::isFneDataType(m.m_datatype))//判断是否为库函数
				{
					temp = std::make_shared<eplcltype::Var>(m.m_name, m.getID(), getLibDefinedDataType(m.m_datatype), m.m_ubound, true, m.m_comment);
				}
				else
				{
					debug_throw("构建数据类型表:结构体时出现未知数据类型!");
				}
				vars.push_back(temp);
			}
			c->setVars(vars);
		}


		/*构建程序集文件和类文件对象映射表,用于文件树创建文件夹*/
		for (const auto& [id, c] : m_EplAssemblyMap) {
			m_EplAssemblyAndClassMap.emplace(c->getID(), c);
		}
		for (const auto& [id, c] : m_EplClassMap) {

			std::shared_ptr<eplcltype::BaseAssembly> temp{ c };
			m_EplAssemblyAndClassMap.emplace(id, temp);
		}
	}

	void buildIrGlobalVarMap() {
		debug_put("start", __FUNCTION__);
		for (const auto& c : m_CodeInfo.global_variables)
		{
			m_EplGlobalVarMap.emplace(c.m_id, std::make_shared<eplcltype::GlobalVar>(c.m_name, c.m_id, getDataType(c.m_datatype, __FUNCTION__, c.m_name), c.m_ubound, c.m_comment));
			debug_put(c.m_name);
		}
	}
	void buildIrShareFunctionMap() {
		debug_put("start", __FUNCTION__);
		/*构建共享函数映射表*/
		for (const auto& c : m_CodeInfo.epl_share_lib)
		{
			m_EplShareFunMap.emplace(c.m_id, std::make_shared<eplcltype::FianlShareFunction>(c.m_name, c.m_id, c.LibraryName, c.EntryPoint, c.remark));
			auto& p{ m_EplShareFunMap[c.m_id] };
			std::vector< std::shared_ptr<eplcltype::Param>> params;
			for (const auto& a : c.Parameters)
			{
				params.push_back(std::make_shared<eplcltype::Param>(a.m_name, a.m_id, getDataType(a.m_datatype, __FUNCTION__, a.m_name), bool{ (a.m_flags & 2) != 0 }, false, bool{ (a.m_flags & 8) != 0 }, a.m_comment));
			}
			p->setParam(params);
			p->setReturnType(getDataType(c.ReturnDataType, __FUNCTION__, c.m_name));
		}
		for (const auto& [id, c] : m_EplShareFunMap)
		{
			debug_put(c->getName());
		}
	}
	void builDataSysType() {
		debug_put("start", __FUNCTION__);
		static std::unordered_map<wteplapi::DATA_TYPE, std::string> SystemDataTypeName{
			{ wteplapi::DATA_TYPE::SDT_BIN, "EplMemBin" },
			{ wteplapi::DATA_TYPE::SDT_BOOL, "EplBool" },//这里BOOL需要重新定义
			{ wteplapi::DATA_TYPE::SDT_BYTE, "char" },
			{ wteplapi::DATA_TYPE::SDT_DATE_TIME, "OleData" },
			{ wteplapi::DATA_TYPE::SDT_DOUBLE, "double" },
			{ wteplapi::DATA_TYPE::SDT_FLOAT, "float" },
			{ wteplapi::DATA_TYPE::SDT_INT, "int" },
			{ wteplapi::DATA_TYPE::SDT_INT64, "long long" },
			{ wteplapi::DATA_TYPE::SDT_SUB_PTR, "void*" },//子程序指针
			{ wteplapi::DATA_TYPE::SDT_SHORT, "short" },
			{ wteplapi::DATA_TYPE::SDT_TEXT, "epl_runtime_systype::eplstring" },//这里String需要重新定义,std::string存在16字节的栈区,照易语言的定义传递库和指针时容易出问题
			{ wteplapi::DATA_TYPE::_SDT_ALL, "EplAny" },//这里Any需要重新定义,std::any并不适用，支持原库需要传递指针
			{ wteplapi::DATA_TYPE::SDT_STATMENT, "条件语句型" },//这里Lambda需要重新定义,支持原库需要传递指针
			{ wteplapi::DATA_TYPE::_SDT_NULL, "void" }//空类型
		};
		/*构建系统类型*/
		for (const auto& [type_id, name] : SystemDataTypeName)
		{
			auto id{ static_cast<std::uint32_t>(type_id) };
			m_EplDataTypeMap.emplace(id, std::make_shared<eplcltype::FinalSysDataType>(name, id));
		}

	}
	/*构建数据类型*/
	void buildIrDataTypeMap() {
		debug_put("start", __FUNCTION__);
		/*提示:
		该函数执行时还未完全构建数据类型映射表,所以不能直接使用GetDataType函数,否则可能会出现未知数据类型
		*/




		builDataSysType();/*构建系统类型*/
		std::unordered_map<
			std::uint32_t,
			std::shared_ptr<EplStruct>
		> structmap;

		for (auto& c : m_CodeInfo.epl_struct)
		{
			structmap.emplace(c.m_id, std::make_shared<EplStruct>(c));
			debug_put(c.m_name);
		}
		/*基础类映射表*/
		auto classmap = m_CodeInfo.makeHashClass();

		/*构建顺序,填充顶层,递归填充成员*/


		/*填充结构体*/
		for (const auto& [id, c] : structmap)
		{
			m_EplDataTypeMap.emplace(id, std::make_shared<eplcltype::Struct>(c->m_name, id, c->remark));
		}
		/*填充类*/
		for (const auto& [id, c] : classmap)
		{
			m_EplDataTypeMap.emplace(id, std::make_shared<eplcltype::FinalClass>(c->m_name, id, c->m_remark));
		}


		/*开始填充成员*/
		for (const auto& [id, c] : m_EplDataTypeMap)
		{
			if (c->getObjType() == eplcltype::BaseCodeObj::type::Struct)
			{
				auto p{ std::dynamic_pointer_cast<eplcltype::Struct>(c) };
				if (!structmap.contains(p->getID()))
				{
					debug_throw("原始结构体映射表中不存在对应ID");
				}
				std::vector<std::shared_ptr<eplcltype::Member>> members;
				for (const auto& m : structmap[p->getID()]->m_Members)
				{
	
					debug_put(structmap[p->getID()]->getName());
					auto bref{ (m.m_flags & 0x2) != 0 };
					std::shared_ptr<eplcltype::Member> temp;
					if (m.m_name.empty())
						continue;

					if (m.m_datatype == 0)//强制转换为int
						members.push_back(std::make_shared<eplcltype::Member>(m.m_name, m.getID(), m_EplDataTypeMap[static_cast<unsigned int>(wteplapi::DATA_TYPE::SDT_INT)], bref, m.m_ubound, m.m_comment));
					
					if (m_EplDataTypeMap.contains(m.m_datatype))
					{
						members.push_back(std::make_shared<eplcltype::Member>(m.m_name, m.getID(), m_EplDataTypeMap[m.m_datatype], bref, m.m_ubound, m.m_comment));
					}
					else if (elibcall::isFneDataType(m.m_datatype))//判断是否为库函数
					{
						members.push_back(std::make_shared<eplcltype::Member>(m.m_name, m.getID(), getLibDefinedDataType(m.m_datatype), bref, m.m_ubound, m.m_comment));
					}
					else
					{
						debug_throw("构建数据类型表:结构体时出现未知数据类型!");
					}
					p->setMember(members);
				}
			}
			else if (c->getObjType() == eplcltype::BaseCodeObj::type::FinalClass)
			{
				auto p{ std::dynamic_pointer_cast<eplcltype::FinalClass>(c) };
				if (!classmap.contains(p->eplcltype::BaseAssembly::getID()))
				{
					debug_throw("原始结构体映射表中不存在对应ID");
				}
				std::vector<std::shared_ptr<eplcltype::Var>> classmembers_var;

				for (const auto& m : classmap[p->eplcltype::BaseAssembly::getID()]->m_Variables)
				{
					if (m.m_name.empty())
						continue;

					if (m.m_datatype == 0)//强制转换为int
						classmembers_var.push_back(std::make_shared<eplcltype::Var>(m.m_name, m.getID(), m_EplDataTypeMap[static_cast<unsigned int>(wteplapi::DATA_TYPE::SDT_INT)], m.m_ubound, true, m.m_comment));

					if (m_EplDataTypeMap.contains(m.m_datatype))
					{
						classmembers_var.push_back(std::make_shared<eplcltype::Var>(m.m_name, m.getID(), m_EplDataTypeMap[m.m_datatype], m.m_ubound, false, m.m_comment));

					}
					else if (elibcall::isFneDataType(m.m_datatype))//判断是否为库函数
					{
						classmembers_var.push_back(std::make_shared<eplcltype::Var>(m.m_name, m.getID(), getLibDefinedDataType(m.m_datatype), m.m_ubound, false, m.m_comment));
					}
					else
					{
						debug_throw("构建数据类型表:类对象时出现未知数据类型!");
					}
					p->setVars(classmembers_var);
				}
			}
		}


		/********************************已经完全构建*************************/
		/*填充父类*/
		for (const auto& [id, c] : m_EplDataTypeMap)
		{
			if (c->getObjType() == eplcltype::BaseCodeObj::type::FinalClass)
			{
				auto p{ std::dynamic_pointer_cast<eplcltype::FinalClass>(c) };
				auto baseid{ p->eplcltype::BaseAssembly::getID() };
				if (!classmap.contains(baseid))
				{
					debug_throw("原始结构体映射表中不存在对应ID");
				}
				debug_put("class base:", classmap[baseid]->m_base_class_id);
				if (classmap[baseid]->m_base_class_id == -1)//无父类
					continue;
				p->setParent(std::dynamic_pointer_cast<eplcltype::FinalClass>(getDataType(classmap[baseid]->m_base_class_id, __FUNCTION__, classmap[baseid]->m_name)));
			}

		}

		buildSpecificDataType();


	}
	/*构建具体数据类型*/
	void buildSpecificDataType() {
		debug_put("start", __FUNCTION__);
		for (const auto& [id, p] : m_EplDataTypeMap) {
			if (p->getObjType() == eplcltype::BaseCodeObj::type::Struct)
			{
				m_EplStructMap.emplace(p->getID(), std::dynamic_pointer_cast<eplcltype::Struct>(p));

			}
			else if (p->getObjType() == eplcltype::BaseCodeObj::type::FinalClass)
			{
				m_EplClassMap.emplace(p->getID(), std::dynamic_pointer_cast<eplcltype::FinalClass>(p));
			}
		}
	}
	/*构建文件树id名称映射表*/
	void buildIrResAndConstMap() {
		debug_put("start", __FUNCTION__);
		/*从资源段拿去资源*/
		for (const auto& c : m_ResInfo.g_epp_const_data) {
			if (c.id_type == eplconstold::epp_id_type::_const)
			{
				m_EplConstMap.emplace(c.id, std::make_shared<eplcltype::UserDefinedConst>(c.name, c.id, c.getVal(), c.is_public, static_cast<eplcltype::UserDefinedConst::const_type> (c.type), c.remark));
			}
			else
			{
				debug_throw("读取常量时出现常量外的数据,例如资源、窗口等");
			}
			debug_put(c.id, c.name);
		}
		for (const auto& c : m_ResInfo.g_epp_res_data)
		{
			if (c.id_type == eplconstold::epp_id_type::_res)
			{
				m_EplResMap.emplace(c.id, std::make_shared<eplcltype::UserDefinedRes>(c.name, c.id, c.data, c.is_public, c.remark));
			}
			else
			{
				debug_throw("读取资源时出现资源外的数据,例如常量、窗口等");
			}

		}

		for (const auto& [i, c] : m_EplConstMap)
		{
			m_AstIdmap.emplace(i, c);
			debug_put(i, std::dynamic_pointer_cast<eplcltype::UserDefinedConst>(c)->getConstType(), c->getName(), c->getVal());
		}
		debug_put(m_AstIdmap.size());
		for (const auto& [i, c] : m_EplResMap)
		{
			m_AstIdmap.emplace(i, c);
			debug_put(i, c->getData(), c->getName());
		}
		debug_put(m_AstIdmap.size());

	}



	/*建立文件树*/
	void buildFoldTree() {
		debug_put("start", __FUNCTION__);
		/*构建文件夹,看的我脑袋疼*/
		std::unordered_map<
			std::uint32_t,
			std::shared_ptr<EplCodeFolderInfo>
		> foldermap;
		std::unordered_map<eobj_id, std::shared_ptr<eplcltype::BaseAssembly>> temp_copy = m_EplAssemblyAndClassMap;
		for (auto& folder : m_FolderInfo.getFolders()) {
			foldermap.emplace(folder.Key, std::make_shared<EplCodeFolderInfo>(folder));
		}

		std::function<void(std::shared_ptr<eplcltype::FolderTree>&, std::uint32_t)> buildFoldTreeChirld;
		buildFoldTreeChirld = [&](std::shared_ptr<eplcltype::FolderTree>& t, std::uint32_t key) -> void {
			for (const auto& [k, c] : foldermap) {
				if (c->ParentKey == key) {
					auto p{ std::make_shared<eplcltype::FolderTree>(c->Name) };
					buildFoldTreeChirld(p, c->Key);
					for (const auto& id : c->Children)
					{
						if (id == 0)continue;

						if (temp_copy.contains(id))
						{
							p->addAssembly(temp_copy[id]);
							temp_copy.erase(id);
						}
					}
					t->addChirld(p);
				}
			}
			};

		m_FolderTree = std::make_shared<eplcltype::FolderTree>("base_src");
		buildFoldTreeChirld(m_FolderTree, 0);
		/*剩余的都在根目录*/
		for (const auto& [id, a] : temp_copy)
		{
			if (a)
			{
				m_FolderTree->addAssembly(a);
			}

		}

	}

};


/*易语言转译器(伪编译器):用于将易语言文件输入后调用易语言代码解析器构建抽象语法树,再通过翻译器将抽象语法树统合翻译为C++文件后,构建CMake调用clang gcc 或者MSVC编译*/
class EplTranslator
{
	std::shared_ptr<eplcltype::FolderTree> m_tree;
	std::filesystem::path m_root;
	std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::Struct>> m_allsturct;
public:
	EplTranslator() = default;
	auto putnewline(const std::wstring& str = {}) { eprogramutils::appio::ioput(str); eprogramutils::appio::ioput(L"\n--------------------------------------------------\n"); };
	auto initRootPath(const std::string_view& path) {
		m_root = std::filesystem::path(path).parent_path() / (std::filesystem::path(path).stem().string() + "_eplcp");
		putnewline(L"\n创建转译文件," + m_root.wstring());
		/*创建文件夹*/
		if (!std::filesystem::exists(m_root)) {
			//清空目录
			std::filesystem::remove_all(m_root);
		}
		std::filesystem::create_directories(m_root);
	}
	bool Translate(
		const std::string_view& path, 
		std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::Struct>> allsturct,
		std::shared_ptr<eplcltype::FolderTree> tree) {
		try {
		debug_put("start", __FUNCTION__);
		putnewline(L"\n已经接收到树,开始转译↓↓↓");
		m_tree = tree;
		m_allsturct = allsturct;
		initRootPath(path);
		/*翻译根目录*/
		TranslateFolder();

		/*构建CMake调用clang gcc 或者MSVC编译*/
		/*根据文件构建cmake*/
		std::stringstream cmaketext;
		cmaketext << "cmake_minimum_required(VERSION 3.0)\n";
		cmaketext << "project(" << std::filesystem::path(path).stem().string() << ")\n";
		cmaketext << "add_executable(" << std::filesystem::path(path).stem().string() << " " << std::filesystem::path(path).stem().string() << ".cpp)\n";


		return true;
		}debug_catch();
		return false;
	}
	std::vector<std::vector<std::vector<int>>> m_test;
	
	auto MakeArray(const std::vector<std::uint32_t>& dimensions, const std::string& type)->std::string {
		std::stringstream oss;
		oss << "epl_core_runtime::eplarray<" << type << ">";
		for (const auto& d : dimensions)
		{
			oss << "[" << d << "]";
		}
		return oss.str();
	}
	auto TranslateVar(const std::shared_ptr< eplcltype::Var>& v) -> std::string {
		std::stringstream out;
		if (v->isStatic())
		{
			out << "static ";
		}
		if (v->isArray())
		{


			auto MakeVector = [](const std::vector<std::uint32_t>& dimensions, const std::string& type)->std::string {
				std::stringstream oss;
				oss << "epl_core_runtime::eplarray<" << type << ">";
				for (const auto& d : dimensions)
				{
					oss << "[" << d << "]";
				}
				return oss.str();
				};

			out << MakeVector(v->getArrayDim(), v->getDataType()->getName()) << " " << v->getName();

		}
		else
		{
			out << v->getDataType()->getName() << " " << v->getName();
		}
		out << "{};\n";
		return out.str();
	}

	//声明对象
	void decObj(const std::string_view &obj, const std::string_view &name) {
		m_allclass << obj<<" " << name << ";\n";
	}

	/*翻译集文件*/
	auto TranslateFunctionFile(const std::shared_ptr<eplcltype::BaseAssembly>c, const  std::filesystem::path path) -> std::string {
		//已经翻译过的类不再翻译
		static std::unordered_map<std::uint32_t, const std::shared_ptr<eplcltype::Struct>> s_translated;
		
		/*用于写入声明文件的声明*/
		std::stringstream header;
		
		//创建文件
		std::filesystem::path file = path / (c->getName() + ".cpp");
		std::ofstream out(file);
		file = path / (c->getName() + ".h");

		std::ofstream declare(file);
		declare << "/*" << c->getName() << "*/\n";
		if (!out.is_open())
		{
			debug_throw("创建文件失败");
		}
		;
		putnewline(L"\n创建代码文件:" + (path / (c->getName())).wstring());
		out << "/*" << c->getName() << "*/\n";

		out << "#include\"" << m_headfile.string() << "\"\n"; // 头文件类型声明写入绝对路径
		
		header << "#include\"" << file.string() << "\"\n";
		/*判断类型*/
		if (c->getObjType() == eplcltype::BaseCodeObj::type::FinalAssembly)
		{
			declare << "namespace " << c->getName() << "{\n";
			/*程序集则声明命名空间*/
			out << "namespace " << c->getName() << "{\n";

			//匿名命名空间包裹程序集变量
			out << "namespace {\n";
			for (const auto& v : c->getVars())
			{
				debug_put(v->getDataType()->getName());
				//debug_put(v->getObjType());
				if (v!=nullptr)
					out << v->getDataType()->getName() << " " << v->getName() << "{};\n";
				
			}
			out << "};\n";
			for (const auto& f : c->getFunc())
			{
				std::stringstream funcname;//用于声明函数

				if (!f->getComment().empty())     funcname << "//" << f->getComment() << "\n";
				funcname << f->getReturnType()->getName();
				funcname << " " << f->getName() << "(";
				int s;
	/*			for (const auto& p : f->getParams())
				{
					funcname << p->getDataType()->getName() << " " << p->getName()<<",";
				}*/
				for (size_t i = 0; i < f->getParams().size(); i++)
				{
					auto p = f->getParams()[i];
					funcname << p->getDataType()->getName() << " " << p->getName() ;
					if (i!= f->getParams().size()-1)
					{
						funcname << ",";
					}

				}

				declare << funcname.str() << ");\n";
				out << funcname.str();
				out << ")";
				out << "{\n";

				for (const auto v : f->getVars())
				{
					out << TranslateVar(v);
				}


				out << f->getCode();
				out << "};\n";
			}
			declare << "};\n";
			out << "};\n";
		}
		else if (c->getObjType() == eplcltype::BaseCodeObj::type::FinalClass)
		{
			m_allclass << "class " << c->getName() << ";\n";
			declare << "class " << c->getName() << "{\n";

			/*类则声明类*/
			for (const auto& v : c->getVars())
			{

				declare << v->getDataType()->getName() << " " << v->getName() << "{};\n";
			}
			declare << "public: \n";
			for (const auto& f : c->getFunc())
			{

				if (!f->getComment().empty()) {
					out << "//" << f->getComment() << "\n";

				}
				const char initfunname[] = { 95,-77,-11,-54,-68,-69,-81,'\0' };//初始化的ansi
				const char destoryfunname[] = { 95,-49,-6,-69,-39,'\0' };//销毁
				if (f->getName() == initfunname)
				{
					declare << " " << c->getName() << "(";
					out << c->getName() << "::" << c->getName() << "(";
				}
				else if (f->getName() == destoryfunname)
				{
					declare << " ~" << c->getName() << "(";
					out << c->getName() << "::~" << c->getName() << "(";
				}
				else
				{
					declare << f->getReturnType()->getName() << " " << f->getName() << "(";
					out << f->getReturnType()->getName() << " " << c->getName() << "::" << f->getName() << "(";
				}



				for (size_t i = 1; const auto & p : f->getParams())
				{
					declare << p->getDataType()->getName() << " " << p->getName() << (i== f->getParams().size()?"" : ",");
					out << p->getDataType()->getName() << " " << p->getName() << (i == f->getParams().size() ? "" : ",");
					i++;
				}
				
				declare << ");\n";
				out << ")";
				out << "{\n";
				for (const auto v : f->getVars())
				{
					out << TranslateVar(v);
				}
				out << f->getCode();
				out << "};\n";
			}
			declare << "};\n";
		}
		else
		{
			debug_throw("未知类型");
		}
		out.close();
		return header.str();
	}




	enum TranslateStructType {
		SYS,//系统数据类型
		SYSPTR,//系统数据内存类型
		USER,//用户自定义类型
	};
	//返回 导出和回归的函数字符传
	std::pair<std::string, std::string>
		TranslateStructTypeFunction(
			bool isPtr,//是否是指针
			bool isArry,//是否是数组
			const std::string& typeName,//实际类型名称
			const std::string& memberName,//成员名称
			TranslateStructType type = SYS
		) {
		std::stringstream Get, Set;
		if (isPtr || isArry)//非值类型需要拷贝
		{
			//对于类型简化,如果是数组指针,则一定是指针,需要申请数组内存后拷贝,否则为数组仅需要拷贝
			bool isArryPtr = (isArry && isPtr);

			switch (type)
			{
			case EplTranslator::SYS: {
				if (isArryPtr)
				{
					//GetRealPtr 内部会申请
					Get << "realtype->" << memberName << " = this->" << memberName << ".GetRealPtr();\n";
					//CopyFromReal 内部会释放
					Set << "this->CopyFromReal" << memberName << "(" << "realtype->" << memberName << ");\n";
				}
				else if (isArry) {
					//拷贝
					Get << "memcpy(realtype->" << memberName << ",this->" << memberName << ".data(),std::min(this->" << memberName << ".size(),std::size(realtype->" << memberName << "));\n";
					//回归
					Set << "this->CopyFromReal" << memberName << "(" << "realtype->" << memberName << ");\n";
				}
				else//普通指针
				{
					Get << "realtype->" << memberName << " = new " << typeName << "{0};\n";
					Get << "*realtype->" << memberName << " = this->" << memberName << ";\n";
					Set << "this->" << memberName << " = *realtype->" << memberName << ";\n";
					Set << "delete realtype->" << memberName << ";\n";
				}

			}break;
			case EplTranslator::SYSPTR://字符串字节集需要拷贝,易语言中的字节集数组是不具有ABI兼容的,且一定为只读指针
			{
				if (isArry) {
					//GetRealPtr 内部会申请
					Get << "realtype->" << memberName << " = this->" << memberName << ".GetRealPtr();\n";
					Set << "this->" << memberName << ".CopyFromReal" << "(realtype->" << memberName << ");\n";
				}
				else
				{
					Get << "realtype->" << memberName << " = this->" << memberName << ".data();\n";
				}
			}break;
			case EplTranslator::USER: {
				if (isArryPtr)
				{
					//GetRealPtr 内部会申请
					Get << "realtype->" << memberName << " = this->" << memberName << ".GetRealPtr();\n";
					//CopyFromReal 内部会释放
					Set << "this->" << memberName << ".CopyFromReal" << "(" << "realtype->" << memberName << ");\n";
				}
				else if (isArry) {
					/*
					//申请内存
					for (size_t i = 0; i < std::size(realtype->memberName); i++)
					{
						auto realtype_memberName = this->memberName.GetRealPtr();
						realtype->memberName[i] = *realtype_memberName;
						delete realtype_memberName;
					}
					//释放,创建一个临时

					for (size_t i = 0; i < std::size(realtype->memberName); i++)
					{
						if (i < this->memberName.size())
							this->memberName.CopyFromReal(realtype->memberName[i], false);
						else{
						//创建一个临时的用于释放内存的
						 decltype(realtype->memberName[i]) p{};
						 //释放内存
						 p.CopyFromReal(realtype->memberName[i], false);
						}
					}


					*/
					//拷贝
					Get << "for (size_t i = 0; i < std::size(realtype->" << memberName << "); i++)\n";
					Get << "{\n";
					Get << "auto realtype_" << memberName << " = this->" << memberName << "[i].GetRealPtr();\n";
					Get << "realtype->" << memberName << "[i] = *realtype_" << memberName << ";\n";
					Get << "delete realtype_" << memberName << ";\n";
					Get << "}\n";
					//回归
					Set << "for (size_t i = 0; i < std::size(realtype->" << memberName << "); i++)\n";
					Set << "{\n";
					Set << "if (i < this->" << memberName << ".size())\n";
					Set << "this->" << memberName << "[i].CopyFromReal(realtype->" << memberName << "[i], false);\n";
					Set << "else{\n";
					Set << "decltype(realtype->" << memberName << "[i]) p{};\n";
					Set << "p.CopyFromReal(realtype->" << memberName << "[i], false);\n";
					Set << "}\n";
					Set << "}\n";
				}
				else//普通指针
				{
					//用户自定义即为结构体,需要调用其导出和回归函数,且为指针
					Get << "realtype->" << memberName << " = this->" << memberName << ".GetRealPtr();\n";
					Set << "this->" << memberName << ".CopyFromReal(realtype->" << memberName << ",true);\n";
				}
			}break;
			default:
				break;
			}


		}
		else
		{
			switch (type)
			{
			case EplTranslator::SYS:
				Get << "realtype->" << memberName << " = this->" << memberName << ";\n";
				Set << "this->" << memberName << " = realtype->" << memberName << ";\n";
				break;
			case EplTranslator::SYSPTR://字符串字节集需要拷贝,易语言中的字节集数组是不具有ABI兼容的,且一定为只读指针
				//申请内存
				Get << "realtype->" << memberName << " = this->" << memberName << ".data();\n";
				break;
			case EplTranslator::USER:
				//用户自定义即为结构体,需要调用其导出和回归函数,该出为类型实例
				Get //获取指针
					<< "auto realtype_" << memberName << " = this->" << memberName << ".GetRealPtr();\n"
					//赋值给实例
					<< "realtype->" << memberName << "= *realtype_" << memberName << ";\n"
					//释放内存
					<< "delete realtype_" << memberName << ";\n";
				//仅释放指针内部的内存
				Set << "this->" << memberName << ".CopyFromReal(realtype->" << memberName << ",false);\n";
				break;
			default:
				break;
			}

		}
		return { Get.str(),Set.str() };
	}


	/*翻译结构体*/
	/*对于易语言的结构体,基本数据类型中字节集和文本型一定传指针,其他类型则仅判断是否为引用或数组,同样都指的指针*/
	auto TranslateStruct(const std::shared_ptr<eplcltype::Struct>& s) -> std::string {
		//已经翻译过的结构体不再翻译
		static std::unordered_map<std::uint32_t, const std::shared_ptr<eplcltype::Struct>> s_translated;

		//将文本或字节集转为结构体
		auto translateRealType = [](const std::shared_ptr<eplcltype::Member>& m)->std::string {
			std::string ptr = m->isRef() ? "*" : "";
			auto type_id{ static_cast<wteplapi::DATA_TYPE>(m->getDataType()->getID()) };
			//仅支持系统数据类型和结构体
			if (wteplapi::GetDataTypeType(type_id) == wteplapi::DataTypeType::DTT_IS_SYS_DATA_TYPE)//系统数据类型
			{
				switch (type_id)
				{
				case wteplapi::DATA_TYPE::SDT_TEXT:
					return "char*";
				case wteplapi::DATA_TYPE::SDT_BIN:
					return "unsigned char*";
				case wteplapi::DATA_TYPE::SDT_BOOL:
					return "bool" + ptr;
				default:
					break;
				}
				return m->getDataType()->getName() + ptr;
			}
			return m->getDataType()->getName() + "::RealType" + ptr;
			};

		//结构体名称
		auto structname = s->getName();
		std::stringstream
			putout,
			out,//用于写入cpp文件
			realS,//用于写入真实结构体
			funcO,//用于导出真实结构体的函数
			funcI//用于回归真实结构体的函数
			;
		if (s_translated.contains(s->getID()))
		{
			return{};
		}


		//提前声明结构体
		decObj("struct", structname);



		out << "struct " << structname << "{\n";
		realS << "struct RealType" << "{\n";
		funcO << "RealType* GetRealPtr()\n{\n"
			<< "RealType* realtype = new RealType{};\n";
		;
		funcI << "void CopyFromReal(RealType* realtype,bool needFree)\n{\n";


		//构造运行时成员
		for (const auto& m : s->getMember()) {
			auto type = m->getDataType();
			//如果是自定义数据类型,则判断是否已经翻译过,如果没有则翻译
			if (type->getObjType() == eplcltype::BaseCodeObj::type::Struct)
			{
				auto p = std::dynamic_pointer_cast<eplcltype::Struct>(type);
				putout << TranslateStruct(p);
			}



			auto typeName = m->getDataType()->getName();
			auto memberName = m->getName();



			if (m->isArray())
			{
				//处理数组在机构体中的声明
				auto list_dim =
					[](const std::vector<std::uint32_t>& dimensions, const std::string& type, const std::string& name, bool isReal)
					->std::string {
					std::stringstream oss;
					if (isReal)
					{
						oss << type << " " << name;
						for (const auto& d : dimensions)
							oss << "[" << d << "]";
						oss << "{};\n";
					}
					else
					{
						oss << "epl_core_runtime::eplarray<" << type << "> " << name << "{";
						for (size_t i = 0; i < dimensions.size(); i++)
						{
							if (i != dimensions.size() - 1)
								oss << dimensions[i] << ",";
							else
								oss << dimensions[i];
						}
						oss << "};\n";
					};
					return oss.str();
					};
				//运行时结构体
				out << list_dim(m->getArrayDim(), typeName, memberName, false);
				//真实结构体
				realS << (m->isRef() ? (translateRealType(m) + memberName) + "{};\n" : list_dim(m->getArrayDim(), translateRealType(m), memberName, true));
			}
			else {


				out << typeName << " " << memberName << "{};\n";
				realS << translateRealType(m) << " " << memberName << "{};\n";
			}
			auto getdATATypeTYPE = [](const std::shared_ptr<eplcltype::Member>& m) ->TranslateStructType {
				auto type_id{ static_cast<wteplapi::DATA_TYPE>(m->getDataType()->getID()) };
				//仅支持系统数据类型和结构体
				if (wteplapi::GetDataTypeType(type_id) == wteplapi::DataTypeType::DTT_IS_SYS_DATA_TYPE)//系统数据类型
				{
					switch (type_id)
					{
					case wteplapi::DATA_TYPE::SDT_TEXT:
						return SYSPTR;
					case wteplapi::DATA_TYPE::SDT_BIN:
						return SYSPTR;
					default:
						break;
					}
					return SYS;
				}
				return USER;
				};
			auto [g, s] {TranslateStructTypeFunction(m->isRef(), m->isArray(), typeName, memberName, getdATATypeTYPE(m))};
			funcO << g; funcI << s;

		}


		//收尾
		realS << "};\n";
		funcO << "return realtype;\n};\n";
		funcI << "if(needFree && realtype!=nullptr)\n delete realtype; };\n";

		out << realS.str() << funcO.str() << funcI.str() << "};\n";
		//for (const auto& m : s->getMember())
		//{
		//	if (m->isArray())
		//	{
		//		auto MakeVector = 
		//			[](const std::vector<std::uint32_t>& dimensions, const std::string& type)
		//			->std::string {
		//			std::stringstream oss;
		//			oss << "epl_core_runtime::eplarray<" << type << ">";
		//			for (const auto& d : dimensions)
		//			{
		//				oss << "[" << d << "]";
		//			}
		//			return oss.str();
		//			};
		//		out << MakeVector(m->getArrayDim(), m->getDataType()->getName()) << " " << m->getName() << "{};\n";
		//	}
		//	else
		//		out << m->getDataType()->getName() << " " << m->getName() << "{};\n";
		//	
		//}
		////构建用于调用C共享库函数时的真实结构体,例如EplString对应的为char* 类型 EplMemBin对应的为void*类型对其处理为真实结构体引用处理为指针等,数组同样转为指针,且大小一定和声明时相同
		//{
		//	out << "struct RealType"  << "{\n";
		//	for (const auto& m : s->getMember())
		//		out << translateRealType(m->getDataType()) << " " << (m->getDataType()->getID() == (int)wteplapi::DATA_TYPE::SDT_TEXT ? "" : (m->isRef() ? "*" : "")) << m->getName() << (!m->isRef() ? translateArray(m->getArrayDim()) : "") << "{};\n";
		//	out << "};\n";
		//}
		////构建复制到C结构体以及回归结构体的函数
		//{
		//	std::stringstream Get, Set;
		//	Get << "RealType* GetRealPtr()\n{";
		//	Set << "void CopyFromReal(RealType* realtype,bool needFree)\n{";
		//	Get << "RealType* realtype = new RealType{};\n";
		//	{
		//	//for (const auto& m : s->getMember())
		//	//{
		//	//	if (static_cast<wteplapi::DATA_TYPE> (m->getDataType()->getID()) == wteplapi::DATA_TYPE::SDT_TEXT)
		//	//	{
		//	//		//任何情况都是传递指针,当前不能直接传递string的指针,拷贝后再传递,再回归再释放
		//	//		Get << "realtype->" << m->getName() << " = new char[this->" << m->getName() << ".size()+1]{'\\0'};\n";
		//	//		Get << "memcpy(realtype->" << m->getName() << ", this->" << m->getName() << ".data(), this->" << m->getName() << ".size());\n";
		//	//		Set << "this->" << m->getName() << " = realtype->" << m->getName() << ";\n";
		//	//		Set << "delete []realtype->" << m->getName() << ";\n";
		//	//		//Get << "realtype->" << m->getName() << " = const_cast<char*>(this->" << m->getName() << ".data());\n";
		//	//	}
		//	//	else {
		//	//		//如果为传指则需要申请并拷贝
		//	//		if (m->isArray())
		//	//		{
		//	//			//如果是引用则代表传递指针,传递前需要申请内存,回归后需要释放
		//	//			//如果不为引用则代表传递实体,传递前需要拷贝,回归后需要拷贝回原始std::vector数组中
		//	//			auto type_id{ static_cast<wteplapi::DATA_TYPE> (m->getDataType()->getID()) };
		//	//			auto memName = m->getName();
		//	//			auto memthisName = "this->" + m->getName(), memNameReal = "realtype->" + memName;
		//	//			auto memTypeName = m->getDataType()->getName();
		//	//			auto memTypeNameReal = translateRealType(m->getDataType());
		//	//			switch (wteplapi::GetDataTypeType(type_id))
		//	//			{
		//	//			case wteplapi::DataTypeType::DTT_IS_SYS_DATA_TYPE: {
		//	//				Get << memNameReal << " = new " << m->getDataType()->getName() << translateArray(m->getArrayDim()) << "{};\n";
		//	//				Get << "memcpy(" << memNameReal << ", " << memthisName << (m->isArray() ? ".data()" : "") << ", sizeof(" << m->getDataType()->getName() << translateArray(m->getArrayDim()) << "));\n";
		//	//				Set << "memcpy(this->" << m->getName() << ", realtype." << m->getName() << ", sizeof(" << m->getDataType()->getName() << translateArray(m->getArrayDim()) << "));\n";
		//	//				Set << "delete realtype->" << m->getName() << ";\n";
		//	//			}break;//如果是用户自定义结构体
		//	//			case wteplapi::DataTypeType::DTT_IS_USER_DATA_TYPE: {
		//	//				//构造
		//	//				Get << "realtype->" << m->getName() << " = this->" << m->getName() << ".GetRealPtr();\n";
		//	//				//释放
		//	//				Set << m->getName() << ".CopyFromReal(realtype->" << m->getName() << ",false);\n";
		//	//			}break;
		//	//			case wteplapi::DataTypeType::DTT_IS_LIB_DATA_TYPE: {
		//	//				//构造
		//	//				Get << "realtype->" << m->getName() << " = this->" << m->getName() << ";\n";
		//	//				Set << "this->" << m->getName() << " = realtype->" << m->getName() << ";\n";
		//	//			}break;
		//	//			default:
		//	//				debug_throw("[自定义数据类型中存在未定义类型]-来自结构体:" + s->getName() + "-对应ID:" + std::to_string(s->getID())
		//	//					+ "\n[-成员:" + ansi2u8(m->getName()) + "-对应ID:" + std::to_string(m->getID()) + "]");
		//	//				break;
		//	//			}
		//	//		}
		//	//		if (m->isRef())
		//	//		{
		//	//			auto type_id{ static_cast<wteplapi::DATA_TYPE> (m->getDataType()->getID()) };
		//	//			switch (wteplapi::GetDataTypeType(type_id))
		//	//			{
		//	//			case wteplapi::DataTypeType::DTT_IS_SYS_DATA_TYPE: {
		//	//				Get << "realtype->" << m->getName() << " = new " << m->getDataType()->getName() << translateArray(m->getArrayDim()) << "{};\n";
		//	//				Get << "memcpy(realtype->" << m->getName() << ", " << m->getName() << (m->isArray() ? ".data()" : "") << ", sizeof(" << m->getDataType()->getName() << translateArray(m->getArrayDim()) << "));\n";
		//	//				Set << "memcpy(this->" << m->getName() << ", realtype." << m->getName() << ", sizeof(" << m->getDataType()->getName() << translateArray(m->getArrayDim()) << "));\n";
		//	//				/*
		//	//				Get << "*realtype->" << m->getName() << " = *this->" << m->getName() << ";\n";
		//	//				Set << "*this->" << m->getName() << " = *realtype->" << m->getName() << ";\n";
		//	//				*/
		//	//				Set << "delete realtype->" << m->getName() << ";\n";
		//	//			}break;//如果是用户自定义结构体
		//	//			case wteplapi::DataTypeType::DTT_IS_USER_DATA_TYPE: {
		//	//				//构造
		//	//				Get << "realtype->" << m->getName() << " = this->" << m->getName() << ".GetRealPtr();\n";
		//	//				//释放
		//	//				Set << m->getName() << ".CopyFromReal(realtype->" << m->getName() << ",false);\n";
		//	//			}break;
		//	//			case wteplapi::DataTypeType::DTT_IS_LIB_DATA_TYPE: {
		//	//				//构造
		//	//				Get << "realtype->" << m->getName() << " = this->" << m->getName() << ";\n";
		//	//				Set << "this->" << m->getName() << " = realtype->" << m->getName() << ";\n";
		//	//			}break;
		//	//			default:
		//	//				debug_throw("[自定义数据类型中存在未定义类型]-来自结构体:" + s->getName() + "-对应ID:" + std::to_string(s->getID())
		//	//					+ "\n[-成员:" + ansi2u8(m->getName()) + "-对应ID:" + std::to_string(m->getID()) + "]");
		//	//				break;
		//	//			}
		//	//		}
		//	//		else
		//	//		{
		//	//			auto type_id{ static_cast<wteplapi::DATA_TYPE> (m->getDataType()->getID()) };
		//	//			switch (wteplapi::GetDataTypeType(type_id))
		//	//			{
		//	//			case wteplapi::DataTypeType::DTT_IS_SYS_DATA_TYPE: {
		//	//				Get << "memcpy(&realtype->" << m->getName() << ", " << m->getName() << (m->isArray() ? ".data()" : "") << ", sizeof(" << m->getDataType()->getName() << translateArray(m->getArrayDim()) << "));\n";
		//	//				Set << "memcpy(&this->" << m->getName() << ", realtype." << m->getName() << ", sizeof(" << m->getDataType()->getName() << translateArray(m->getArrayDim()) << "));\n";
		//	//			}break;//如果是用户自定义结构体
		//	//			case wteplapi::DataTypeType::DTT_IS_USER_DATA_TYPE: {
		//	//				//构造
		//	//				Get << "auto p" << m->getName() << " = " << m->getName() << ".GetRealPtr();\n";
		//	//				//赋值
		//	//				Get << "realtype->" << m->getName() << " = *p" << m->getName() << "; \n";
		//	//				//仅释放指针,保证成员有效
		//	//				Get << "delete p" << m->getName() << ";\n";
		//	//				//回归数据且仅释放内部成员
		//	//				Set << "this->" << m->getName() << ".CopyFromReal(&realtype->" << m->getName() << ",false);\n";
		//	//			}break;
		//	//			case wteplapi::DataTypeType::DTT_IS_LIB_DATA_TYPE: {
		//	//				//构造
		//	//				Get << "realtype->" << m->getName() << " = this->" << m->getName() << ";\n";
		//	//				Set << "this->" << m->getName() << " = realtype->" << m->getName() << ";\n";
		//	//			}break;
		//	//			default:
		//	//				debug_throw("[自定义数据类型中存在未定义类型]-来自结构体:" + s->getName() + "-对应ID:" + std::to_string(s->getID())
		//	//					+ "\n[-成员:" + ansi2u8(m->getName()) + "-对应ID:" + std::to_string(m->getID()) + "]");
		//	//				break;
		//	//			}
		//	//		}
		//	//	}
		//	//}
		//	}
		//	Get << "};\n";
		//	Set <<R"(if(needFree) delete realtype;)" << "};\n";
		//	out << Get.str();
		//	out << Set.str();
		//}

		putout << out.str();
		s_translated.emplace(s->getID(), s);
		return putout.str();
	}




	auto TranslateFolder() -> void {
		std::filesystem::path allclassfile = m_root / "classobj.h";
		m_allclass.open(allclassfile);
		std::filesystem::path allstructfile = m_root / "structobj.h";
		std::ofstream allstruct(allstructfile);

		std::filesystem::path file = m_root / "declare.h";
		//获取绝对目录
		m_headfile = std::filesystem::absolute(file);
		std::ofstream out(file);
		out << "#pragma once\n" << R"(#include"krnln.hpp"
)";
		//提前导入类声明文件
		out << "#include\"classobj.h\"\n";
		out << "#include\"structobj.h\"\n";
		if (!out.is_open())
		{
			debug_throw("创建文件失败");
		}
		putnewline(L"\n正在生成所有结构体" );
		//翻译结构体
		for (const auto& [id, p] : m_allsturct) {
			allstruct << TranslateStruct(p);
		}
		
		//翻译共享库



		/*子目录*/
		TranslateFolderChirld(out, m_tree, m_root);
		out.close();
		m_allclass.close();
		allstruct.close();
	}
	/*递归翻译*/
	auto TranslateFolderChirld(std::ofstream& out, const std::shared_ptr<eplcltype::FolderTree>& t, const std::filesystem::path& path) -> void {
		std::filesystem::path file = t->getNmae();
		std::filesystem::create_directories(path / file);
		if (!out.is_open())
		{
			debug_throw("创建文件失败");
		}
		/*根目录*/
		for (const auto& c : t->m_assembly)
		{
			out << TranslateFunctionFile(c, path / file);
		}
		/*子目录*/
		for (const auto& c : t->m_chirld)
		{
			TranslateFolderChirld(out, c, path / file);
		}
	}


private:
	std::ofstream m_allclass;
	std::filesystem::path m_headfile;
};



#endif


