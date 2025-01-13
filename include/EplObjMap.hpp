//该页代码用于存放易语言IR对象,以及对象的生成代码用于最后翻译器使用
//构建器的最终目标是生成易语言IR对象,然后由翻译器进行翻译

//中间结构较多,为了方便,使用了大量的继承,这样的好处是可以方便的进行类型判断,但是也会造成大量的虚函数调用,影响性能
//对于命名风格,底层基类前缀为Base,最终层前缀为Final,中间层无前缀的名称,如DataType,Function等
//对于易语言中的对象,如变量,参数,都是继承自BaseCodeObj
//对于易语言中的域,如程序集,类,都是继承自BaseSpace
//对于易语言中的数据类型,如结构体,都是继承自BaseDataType
#ifndef EPL_OBJMAP_HPP
#define EPL_OBJMAP_HPP
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include"tace.hpp"
#include"utils.hpp"
#include"EplLibObjMap.hpp"


/*基于eplc重构的函数,解析后的代码才会传入*/
namespace eplcltype {
	/*声明类型*/
	class BaseCodeObj;//底层基类
	class BaseSpace;//域基类
	class BaseAssembly;//集基类
	class BaseDataType;//数据类型基类
	class FianlShareFunction;//共享库函数
	class ShareLibSpace;//共享库域
	class FinalClass;//类
	class Struct;//结构体
	class UserDefinedConst;//用户定义常量
	class UserDefinedRes;//用户定义资源
	class BaseAbstract;//抽象基类
	class Param;//参数
	class Member;//成员
	class SysSpace;//系统核心运行时
	class FinalSysDataType;//系统基础数据类型
	class LibDefinedDataType;//库数据类型
	class Var;//变量
	class GlobalVar;//全局变量
	class FinalAssembly;//程序集
	class FinalAssemblyFunction;//程序集方法
	class FinalLibFun;//库函数

	struct DataFrom/*数据来源*/
	{
		union
		{
			struct {
				std::uint16_t m_data_id;
				std::uint16_t m_lib_id;
			};
			std::uint32_t m_id{ 0 };
		};
		DataFrom() { memset(this, 0, sizeof(*this)); };
		DataFrom(const std::uint32_t id) :m_id(id) {};
		DataFrom(const std::uint16_t lib_id, const std::uint16_t data_id) :m_lib_id(lib_id), m_data_id(data_id) {};
	};

#define RUNTIMECLASSTYPEMAP(name)   virtual auto getObjType() -> const type override {return type::name;} ;virtual auto getObjTypeStr() -> const std::wstring override {return L#name;};
#define TYPENAMEMAP(name, ...)      enum class type : int {  name,  __VA_ARGS__  }; static constexpr const char* type_name[]{ #name, #__VA_ARGS__ };    

	/*易语言IR对象基类*/
	class BaseCodeObj
	{
	protected:
		using byte_type = unsigned char;
		using byte_array = std::vector<byte_type>;
	private:
		std::uint32_t m_id{ 0 };
		::std::string  m_name;
		::std::string m_comment;
	public:
		/*基类类型,用于映射转换*/
		enum class type : int { BaseCodeObj, BaseSpace, BaseAssembly, BaseDataType, FianlShareFunction, ShareLibSpace, FinalClass, Struct, UserDefinedConst, UserDefinedRes, BaseAbstract, Param, Member, SysSpace, FinalSysDataType, LibDefinedDataType, Var, GlobalVar, FinalAssembly, FinalAssemblyFunction, FinalLibFun };
		static constexpr const char* type_name[]{ "BaseCodeObj", "BaseSpace", "BaseAssembly", "BaseDataType", "FianlShareFunction", "ShareLibSpace", "FinalClass", "Struct", "UserDefinedConst", "UserDefinedRes", "BaseAbstract", "Param", "Member", "SysSpace", "FinalSysDataType", "LibDefinedDataType", "Var", "GlobalVar", "FinalAssembly", "FinalAssemblyFunction", "FinalLibFun" };



		BaseCodeObj() = default;
		BaseCodeObj(const::std::string& name, const std::uint32_t id, const ::std::string& comment = "") :m_name(name), m_id(id), m_comment(comment) {};
		virtual ~BaseCodeObj() = default;
		virtual auto getObjType() -> const type = 0;
		virtual auto getObjTypeStr() -> const std::wstring = 0;
		virtual auto getName() -> const ::std::string {
			if (m_name.empty())
			{
				debug_put(m_name, type_name[static_cast<int>(getObjType())]);
				m_name = type_name[static_cast<int>(getObjType())] + ("_" + std::to_string(m_id));
			}
			//return ansi2u8(m_name);
			return m_name;
		}

		virtual auto getComment() -> const ::std::string {
			return m_comment;
		}


		auto getID() const -> const std::uint32_t {
			return m_id;
		}

		template<typename ...Arg>
		bool isType(const Arg... t) {
			return (... || (static_cast<int>(this->getObjType()) == static_cast<int>(t)));
		}
	};
#undef TYPEMAP
	/*域基类,包括程序集/类/等*/
	class BaseSpace : public BaseCodeObj
	{
	public:
		enum class space_type {
			SPACE_EPLCONST = 0,/*来自常量*/
			SPACE_EPLVAR,
			SPACE_EPLFUNC,
			SPACE_SHARELIB,/*来自共享库*/
			SPACE_STRUCT,/*来自数据类型*/
			SPACE_CLASS
		};
	public:
		BaseSpace(const ::std::string& name, const space_type& id) :BaseCodeObj(name, static_cast<std::uint32_t>(id)) {};
		BaseSpace(const ::std::string& name, const std::uint32_t id, const ::std::string& comment = "") :BaseCodeObj(name, id, comment) {};
		RUNTIMECLASSTYPEMAP(BaseSpace)
	};

	/*系统核心运行时*/
	class SysSpace : public BaseSpace {
	public:
		SysSpace() :BaseSpace("epp_runtime_core", 0) {};
		RUNTIMECLASSTYPEMAP(SysSpace)
	};










	/*常量资源数据*/
	class UserDefinedRes :public  BaseCodeObj
	{
		::std::string m_name;
		byte_array m_data;
		bool m_is_public{ false };
	public:
		UserDefinedRes(const ::std::string& name, const std::uint32_t id, const byte_array& value, bool is_public, const ::std::string& comment) :BaseCodeObj(name, id, comment), m_data(value), m_is_public(is_public) {};
		auto getData() -> const byte_array {
			return m_data;
		}

		RUNTIMECLASSTYPEMAP(UserDefinedRes)
	};


	/*用户自定义常量*/
	class UserDefinedConst : public  BaseCodeObj
	{
	public:
		enum const_type
		{
			cvoid,/*空值*/
			cdata,/*日期*/
			cnum,/*数值*/
			cstr,/*文本*/
			cbool,/*逻辑*/
		};
	private:
		bool m_is_public{ false };
		const_type m_type{ cvoid };
		::std::string m_value;
	public:
		UserDefinedConst(const ::std::string& name, const std::uint32_t id, const ::std::string& value, bool is_public, const const_type type, const std::string& comment = "") : BaseCodeObj(name, id, comment), m_value(value), m_is_public(is_public), m_type(type) {};
		auto getConstType() -> const const_type {
			return m_type;
		}
		auto getVal() -> const ::std::string {
			return m_value;
		}
		RUNTIMECLASSTYPEMAP(UserDefinedConst)
	};
	/*抽象基类*/
	class BaseAbstract : public  BaseCodeObj
	{
		/*数据类型*/
		std::shared_ptr<BaseDataType> m_type;
	public:
		BaseAbstract(const ::std::string& name, const std::uint32_t id, const std::shared_ptr<BaseDataType>& type, const ::std::string& comment = "") :BaseCodeObj(name, id, comment), m_type(type) {};
		auto getDataType() -> const std::shared_ptr<BaseDataType> {
			return m_type;
		}
		RUNTIMECLASSTYPEMAP(BaseAbstract)
	};
	/*参数*/
	class Param : public  BaseAbstract
	{

		/*是否引用*/
		bool m_is_ref{ false };
		/*是否可空*/
		bool m_is_optional{ false };
		/*是否数组*/
		bool m_is_array{ false };
	public:
		Param(const ::std::string& name, const std::uint32_t id, const std::shared_ptr<BaseDataType>& type, const bool is_ref = false, const bool is_optional = false, const bool is_array = false, const ::std::string& comment = "") :BaseAbstract(name, id, type, comment), m_is_ref(is_ref), m_is_optional(is_optional), m_is_array(is_array) {};
		RUNTIMECLASSTYPEMAP(BaseAbstract)
	};
	/*变量*/
	class Var : public  BaseAbstract
	{
		/*数组维数*/
		std::vector<std::uint32_t> m_array_dim;
		/*是否静态*/
		bool m_is_static{ false };
		/*所在区域*/
	public:
		Var(const ::std::string& name, const std::uint32_t id, const std::shared_ptr<BaseDataType>& type, const std::vector<std::uint32_t>& array_dim = {}, const bool is_static = false, const ::std::string& comment = "") :BaseAbstract(name, id, type, comment), m_array_dim(array_dim), m_is_static(is_static) {};
		auto isArray() -> const bool {
			return !m_array_dim.empty();
		}
		auto isStatic() -> const bool {
			return m_is_static;
		}
		auto getArrayDim() -> const std::vector<std::uint32_t> {
			return m_array_dim;
		}

		RUNTIMECLASSTYPEMAP(Var)
	};
	class GlobalVar : public  Var
	{
	public:
		GlobalVar(const ::std::string& name, const std::uint32_t id, const std::shared_ptr<BaseDataType>& type, const std::vector<std::uint32_t>& array_dim = {}, const ::std::string& comment = "") :Var(name, id, type, array_dim, true, comment) {};
		RUNTIMECLASSTYPEMAP(GlobalVar)
	};


	/*成员*/
	class Member : public  BaseAbstract
	{
		/*是否引用*/
		bool m_is_ref{ false };
		/*数组维度*/
		std::vector<std::uint32_t> m_array_dim;
	public:
		Member(const ::std::string& name, const std::uint32_t id, const std::shared_ptr<BaseDataType>& type, const bool is_ref = false, const std::vector<std::uint32_t>& array_dim = {}, const ::std::string& comment = "") :BaseAbstract(name, id, type, comment), m_is_ref(is_ref), m_array_dim(array_dim) {};
		auto isArray() -> const bool {
			return !m_array_dim.empty();
		}
		auto isRef() -> const bool {
			return m_is_ref;
		}
		auto getArrayDim() -> const std::vector<std::uint32_t> {
			return m_array_dim;
		}

		RUNTIMECLASSTYPEMAP(Member)
	};

	/*数据类型*/
	class BaseDataType : public  BaseCodeObj
	{
		/*所在域,即C++命名空间*/
		::std::shared_ptr<BaseSpace> m_space;

	public:
		BaseDataType() = default;
		BaseDataType(const ::std::string& name, const std::uint32_t id, const ::std::shared_ptr<BaseSpace>& type, const std::string& comment = "") :BaseCodeObj(name, id, comment), m_space(type) {};
		RUNTIMECLASSTYPEMAP(BaseDataType)
	};




	/*系统基础数据类型*/
	class FinalSysDataType : public  BaseDataType
	{
	public:
		FinalSysDataType(const ::std::string& name, const std::uint32_t id);
		RUNTIMECLASSTYPEMAP(FinalSysDataType)
	};



	/*库数据类型*/
	class LibDefinedDataType : public  BaseDataType
	{
	public:
		LibDefinedDataType(const std::uint32_t id) :BaseDataType("epllib", id, {}) {};
		LibDefinedDataType(const std::uint16_t lib_id, const std::uint16_t data_id) :BaseDataType("epllib", DataFrom({ lib_id,data_id }).m_id, {}) {};
		virtual auto getName() -> const ::std::string override {
			DataFrom from(getID());
			std::stringstream ss;
			ss << "eplibobj_" << from.m_lib_id << "_" << from.m_data_id;
			return ss.str();
		}
		auto getMemberName(std::uint16_t memindex) {
			return ".member_" + std::to_string(memindex);
		}
		RUNTIMECLASSTYPEMAP(LibDefinedDataType)
	};

	/*结构体*/
	class Struct : public  BaseDataType
	{
		std::vector<std::shared_ptr<Member>> m_member;/*成员变量*/
	public:
		Struct(const ::std::string& name, const std::uint32_t id, const std::vector<std::shared_ptr<Member>>& member, const std::string& comment = "")
			:BaseDataType(name, id, std::make_shared<BaseSpace>("eplstruct", BaseSpace::space_type::SPACE_STRUCT), comment), m_member(member) {};
		Struct(const ::std::string& name, const std::uint32_t id, const std::string& comment = "")
			:BaseDataType(name, id, std::make_shared<BaseSpace>("eplstruct", BaseSpace::space_type::SPACE_STRUCT), comment) {};
		auto setMember(const std::vector<std::shared_ptr<Member>>& member) -> void {
			m_member = member;
		}
		auto getMember() -> const std::vector<std::shared_ptr<Member>>& {
			return m_member;
		}
		RUNTIMECLASSTYPEMAP(Struct)
	};

	/*extren"C" 共享库:dll,so等*/
	class FianlShareFunction : public  BaseCodeObj
	{
		/*所在域,即C++命名空间*/
		::std::shared_ptr<BaseSpace> m_space;
		/*参数*/
		::std::vector<::std::shared_ptr<Param>> m_params;/*默认无参数*/
		/*返回值类型*/
		::std::shared_ptr<BaseDataType> m_return_type;/*默认无返回值*/
		/*是否公开*/
		bool m_is_public{ false };
		std::string m_lib_name;
		std::string m_lib_func_name;
	public:
		FianlShareFunction(const ::std::string& name, const std::uint32_t id, const ::std::string& lib_name, const ::std::string& lib_func_name, const ::std::string& comment = "");
		auto setParam(const ::std::vector<::std::shared_ptr<Param>>& params) -> void {
			m_params = params;
		}
		auto setReturnType(const ::std::shared_ptr<BaseDataType>& type) -> void {
			m_return_type = type;
		}
		RUNTIMECLASSTYPEMAP(FianlShareFunction)
	};
	/*这是一个确定的域,及易语言中的动态链接库/共享库*/
	class ShareLibSpace : public BaseSpace
	{
	public:
		ShareLibSpace();
		RUNTIMECLASSTYPEMAP(ShareLibSpace)
	};
	/*集,是程序集和类的基类,存在域内变量,如程序集变量,*/
	class BaseAssembly : public  BaseSpace
	{
		std::vector<::std::shared_ptr<FinalAssemblyFunction>> m_funcs;
		std::vector<::std::shared_ptr<Var>> m_vars;/*程序集变量,对非集内成员匿名*/
	public:
		BaseAssembly(const ::std::string& name, const std::uint32_t id, const ::std::string& comment = "") :BaseSpace(name, id, comment) {};
		auto addFunc(const std::shared_ptr<FinalAssemblyFunction>& func) -> void {
			m_funcs.push_back(func);
		}
		auto getFunc() -> const std::vector<std::shared_ptr<FinalAssemblyFunction>>& {
			return m_funcs;
		}
		auto setVars(const std::vector<std::shared_ptr<Var>>& vars) -> void {
			m_vars = vars;
		}
		auto getVars() -> const std::vector<std::shared_ptr<Var>>& {
			return m_vars;
		}
		RUNTIMECLASSTYPEMAP(BaseAssembly)
	};
	/*程序集*/
	class FinalAssembly : public  BaseAssembly
	{
	public:
		FinalAssembly(const ::std::string& name, const std::uint32_t id, const ::std::string& comment = "") :BaseAssembly(name, id, comment) {};
		RUNTIMECLASSTYPEMAP(FinalAssembly)
	};

	/*类*/
	class FinalClass : public virtual BaseAssembly, public virtual  BaseDataType
	{
		std::shared_ptr<FinalClass> m_parent;/*基类对象*/
		bool is_public{ false };
	public:
		FinalClass(const ::std::string& name, const std::uint32_t id, const std::vector<std::shared_ptr<Member>>& member, const ::std::string& comment = "");
		FinalClass(const ::std::string& name, const std::uint32_t id, const ::std::string& comment = "");
		auto setParent(const std::shared_ptr<FinalClass>& parent) -> void {
			m_parent = parent;
		}
		auto getParent() -> const std::shared_ptr<FinalClass> {
			return m_parent;
		}

		RUNTIMECLASSTYPEMAP(FinalClass)
	};
	class FinalLibFun : public BaseCodeObj
	{
	public:
		FinalLibFun(const std::uint32_t id) :BaseCodeObj("", id, "") {};
		FinalLibFun(const std::uint16_t lib_id, const std::uint16_t data_id) :BaseCodeObj("", DataFrom(lib_id, data_id).m_id, "") {};
		virtual auto getName() -> const ::std::string override {
			DataFrom from(getID());
			std::stringstream ss;
			ss << "eplibFunc_" << from.m_lib_id << "_" << from.m_data_id;
			return ss.str();
		}
		auto getLibIndex() const {
			DataFrom from(getID());
			return from.m_lib_id;
		}
		auto getFuncIndex() const {
			DataFrom from(getID());
			return from.m_data_id;
		}
		RUNTIMECLASSTYPEMAP(FinalLibFun)
	};
	/*程序集方法*/
	class FinalAssemblyFunction : public  BaseCodeObj
	{
		/*所在域,即C++命名空间*/
		::std::shared_ptr<BaseAssembly> m_space;
		/*参数*/
		::std::vector<::std::shared_ptr<Param>> m_params;/*默认无参数*/
		/*返回值类型*/
		::std::shared_ptr<BaseDataType> m_return_type;/*默认无返回值*/
		/*是否公开*/
		bool m_is_public{ false };
		/*局部变量*/
		::std::vector<::std::shared_ptr<Var>> m_vars;
		std::string m_code;
	public:
		FinalAssemblyFunction(const ::std::string& name, const std::uint32_t id, const ::std::shared_ptr<BaseAssembly>& space, const bool is_public, const ::std::string& comment = "")
			:BaseCodeObj(name, id, comment), m_space(space), m_is_public(is_public) {};
		auto setParam(const ::std::vector<::std::shared_ptr<Param>>& params) -> void {
			m_params = params;
		}
		auto getReturnType() -> const std::shared_ptr<BaseDataType> {
			return m_return_type;
		}
		auto setReturnType(const ::std::shared_ptr<BaseDataType>& type) -> void {
			m_return_type = type;
		}
		auto addVar(const ::std::shared_ptr<Var>& var) -> void {
			m_vars.push_back(var);
		}
		auto setCode(const ::std::string& code) -> void {
			m_code = code;
		}
		auto getCode() -> const ::std::string {
			return m_code;
		}
		auto getVars() -> const std::vector<std::shared_ptr<Var>>& {
			return m_vars;
		}
		auto getSapce() -> const std::shared_ptr<BaseAssembly> {
			return m_space;
		}
		auto getParams() -> const std::vector<std::shared_ptr<Param>>& {
			return m_params;
		}
		RUNTIMECLASSTYPEMAP(FinalAssemblyFunction)
	};


	struct FolderTree
	{
		std::string m_name;
		std::vector <std::shared_ptr<BaseAssembly>> m_assembly;
		std::vector<std::shared_ptr<FolderTree>> m_chirld;
	public:
		FolderTree(const std::string& name) :m_name(name) {};
		/*递归获取所有数量成员,包括子文件夹和文件*/
		auto all_size()  const-> const std::size_t {
			std::size_t size = 0;
			for (const auto& child : m_chirld)
			{
				size += child->all_size();
			}
			size += m_assembly.size() + m_chirld.size();
			return size;
		}
		/*所有文件数*/
		auto all_assembly_size() const -> const std::size_t {
			std::size_t size = 0;
			for (const auto& child : m_chirld)
			{
				size += child->all_assembly_size();
			}
			size += m_assembly.size();
			return size;
		}
		/*所有文件夹数*/
		auto all_folder_size()  const-> const std::size_t {
			std::size_t size = 0;
			for (const auto& child : m_chirld)
			{
				size += child->all_folder_size();
			}
			size += m_chirld.size();
			return size;
		}

		/*本文件夹下的数量*/
		auto size()  const-> const std::size_t {
			return m_assembly.size() + m_chirld.size();
		}
		/*本文件夹下的文件数*/
		auto assembly_size()  const-> const std::size_t {
			return m_assembly.size();
		}
		/*本文件夹下的文件夹数*/
		auto folder_size()  const-> const std::size_t {
			return m_chirld.size();
		}

		auto getNmae() -> const std::string {
			return m_name;
		}
		auto getAssembly() -> const std::vector<std::shared_ptr<BaseAssembly>>& {
			return m_assembly;
		}
		auto getChirld() -> const std::vector<std::shared_ptr<FolderTree>>& {
			return m_chirld;
		}
		auto addChirld(const std::shared_ptr<FolderTree>& chirld) -> void {
			m_chirld.push_back(chirld);
		}
		auto addAssembly(const std::shared_ptr<BaseAssembly>& assembly) -> void {
			m_assembly.push_back(assembly);
		}
		auto setChirld(const std::vector<std::shared_ptr<FolderTree>>& chirld) -> void {
			m_chirld = chirld;
		}
		auto printTree(int depth) const -> std::wstring {
			std::wstringstream ss;
			ss << std::wstring(depth - 1, L' ') << L"+[" << depth << L"-Folder]" << ansi2wstr(m_name) << L"\n";
			for (const auto& assembly : m_assembly) {
				ss << std::wstring(depth, L' ') << L"-[" << assembly->getObjTypeStr() << L"]" << ansi2wstr(assembly->getName()) << L"\n";
				//获取函数
				for (const auto& f : assembly->getFunc())
				{
					//将函数同样打印到文件夹下用
					ss << std::wstring(depth + 1, L' ') << L"-[" << f->getObjTypeStr() << L"]" << ansi2wstr(f->getName()) << L"\n";
				}
			}
			for (const auto& child : m_chirld) {
				ss << child->printTree(depth + 1);
			}
			return ss.str();
		}
		auto printTree() const -> std::wstring {
			std::wstringstream ss;
			ss <<
				L"\n[printTree count:" <<
				all_size() <<
				L"]\n[file count:" <<
				all_assembly_size() <<
				L"]\n[folder count:" <<
				all_folder_size() <<
				L"]\n[root-dir]" <<
				ansi2wstr(m_name) <<
				L":\n------------------------------------------------------\n"
				<< printTree(1);;

			return ss.str();
		}
	};


#undef RUNTIMECLASSTYPEMAP


}





#endif