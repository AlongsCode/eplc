#ifndef EPL_CODE_SECTION_HPP
#define EPL_CODE_SECTION_HPP
#include <array>
#include <unordered_map>
#include "tace.hpp"
#include "utils.hpp"




// 基本变量/成员/信息结构体,易代码转为AST的中间层数据结构
struct BaseVariableInfo : public eprogramutils::EplIDBase
{
    using Stream = eprogramutils::MemArchiveStream;

    enum class Type
    {
        StructField,     // 结构体字段
        ClassField,      // 类字段和汇编变量
        GlobalVariable,  // 全局变量
        LocalVariable,   // 局部变量
        ShareLibParameter,// 共享库参数
        ShareLibReturn,   // 共享库返回值
        MethodParameter,  // 方法参数
        MethodReturn,     // 方法返回值
        ConstVariale,     // 常量
    };

    std::uint32_t m_size{ 0 };       // 长度
    std::uint32_t m_datatype{ 0 };   // 数据类型对应的ID
    std::string m_comment{};       // 注释
    std::uint16_t m_flags{ 0 };      // 标志
    std::vector<std::uint32_t> m_ubound;  // 上界

    std::shared_ptr<eprogramutils::EplIDBase> m_space;  // 命名空间，即C++命名空间

    BaseVariableInfo(Stream& stream, std::uint32_t id) : eprogramutils::EplIDBase(id)
    {
        stream >> m_size >> m_datatype >> m_flags;
        m_ubound = stream.get_vec<std::uint32_t>(stream.read<std::uint8_t>());
        stream >> m_name >> m_comment;
    }
    ~BaseVariableInfo() { };
    virtual Type get_type() const = 0;
    virtual auto getCompleteName() -> const std::string
    {
        auto space = m_space->getName();
        if (space.empty())
        {
            debug_put("命名空间为空");
            return eprogramutils::EplIDBase::getName();
        }
        return space + "::" + eprogramutils::EplIDBase::getName();
    }
};


// 类成员变量
struct ClassVariableInfo : public BaseVariableInfo
{
    ClassVariableInfo(Stream& stream, std::uint32_t id) : BaseVariableInfo(stream, id) {}

    Type get_type() const override
    {
        return Type::ClassField;
    }

    auto getName() -> const std::string override
    {
        return BaseVariableInfo::getName();
    }
};

// 局部变量
struct LocalVariable : public BaseVariableInfo
{
    LocalVariable(Stream& stream, std::uint32_t id) : BaseVariableInfo(stream, id) {}

    Type get_type() const override
    {
        return Type::LocalVariable;
    }

    auto getName() -> const std::string override
    {
        return BaseVariableInfo::getName();
    }
};

// 全局变量
struct GlobalVariable : public BaseVariableInfo
{
    GlobalVariable(Stream& stream, std::uint32_t id) : BaseVariableInfo(stream, id) {}

    Type get_type() const override
    {
        return Type::GlobalVariable;
    }

    auto getName() -> const std::string override
    {
        return BaseVariableInfo::getName();
    }
};

// 结构体成员
struct StructMembersInfo : public BaseVariableInfo
{
    StructMembersInfo(Stream& stream, std::uint32_t id) : BaseVariableInfo(stream, id) {}

    Type get_type() const override
    {
        return Type::StructField;
    }

    auto getName() -> const std::string override
    {
        return BaseVariableInfo::getName();
    }
};

// 共享库参数
struct ShareLibParameter : public BaseVariableInfo
{
    ShareLibParameter(Stream& stream, std::uint32_t id) : BaseVariableInfo(stream, id) {}

    Type get_type() const override
    {
        return Type::ShareLibParameter;
    }

    auto getName() -> const std::string override
    {
        return BaseVariableInfo::getName();
    }
};

// EplDll 结构体
struct EplShareLib : virtual public eprogramutils::EplIDBase
{
    std::uint32_t MemoryAddress{};   // 内存地址
    std::uint32_t Public{};
    std::uint32_t ReturnDataType{};
    std::string remark;              // 备注
    std::string EntryPoint;
    std::string LibraryName;
    std::vector<ShareLibParameter> Parameters;

    EplShareLib() = default;
    EplShareLib(eprogramutils::MemArchiveStream& codes, int id, int memoryAddress) : eprogramutils::EplIDBase(id)
    {
        this->MemoryAddress = memoryAddress;
        auto Flags = codes.read<std::uint32_t>();
        this->Public = (Flags & 0x2) != 0;
        this->ReturnDataType = codes.read<std::uint32_t>();
        this->m_name = codes.get_mfc_string();
        this->remark = codes.get_mfc_string();
        this->LibraryName = codes.get_mfc_string();
        this->EntryPoint = codes.get_mfc_string();
        this->Parameters = codes.read_blocks_have_Id_and_offset<ShareLibParameter>();
    }
};

// 方法参数
struct MethodParameter : public BaseVariableInfo
{
    MethodParameter(Stream& stream, std::uint32_t id) : BaseVariableInfo(stream, id) {}

    Type get_type() const override
    {
        return Type::MethodParameter;
    }

    auto getName() -> const std::string override
    {
        return BaseVariableInfo::getName();
    }
};

// 方法代码数据
struct MethodCodeData : public eprogramutils::EplStreamObj
{
    std::uint32_t m_formid{};               // 父方法ID
    using byte_type = unsigned char;
    std::vector<byte_type> LineOffset;
    std::vector<byte_type> BlockOffset;
    std::vector<byte_type> MethodReference;
    std::vector<byte_type> VariableReference;
    std::vector<byte_type> ConstantReference;
    std::vector<byte_type> ExpressionData;  // 代码表达式数据

    MethodCodeData() = default;

    virtual void Serialize(eprogramutils::MemArchiveStream& s) override
    {
        s >> LineOffset >> BlockOffset >> MethodReference >> VariableReference >> ConstantReference >> ExpressionData;
    }
};

// EplClass (模块) 类
class EplClass : virtual public eprogramutils::EplIDBase
{
public:
    std::uint32_t m_MemoryAddress{};       // 内存地址
    std::uint32_t m_FormID{};              // 关联的汇编ID
    std::uint32_t m_base_class_id{};       // 基类ID
    std::string m_remark{};                // 备注
    std::vector<std::uint32_t> m_methods{};  // 方法IDs


    std::vector<ClassVariableInfo> m_Variables{};  // 成员变量

    EplClass(eprogramutils::MemArchiveStream& stream, int id, int memoryAddress) : eprogramutils::EplIDBase(id)
    {
        this->m_MemoryAddress = memoryAddress;
        stream >> this->m_FormID >> this->m_base_class_id;
        this->m_name = stream.get_mfc_string();
        this->m_remark = stream.get_mfc_string();
        this->m_methods = stream.get_mfc_byte_vec<std::uint32_t>();
        this->m_Variables = stream.read_blocks_have_Id_and_offset<ClassVariableInfo>();

        for (auto& v : m_Variables)
        {
            v.m_space = std::make_shared<EplClass>(*this);
        }
    }
};

// EplMethod 结构体
struct EplMethod : virtual public eprogramutils::EplIDBase
{
    bool m_isHidden{ false };               // 隐藏
    bool m_isPublic{ false };               // 公开
    std::uint32_t m_ReturnDataType{};     // 返回值类型
    std::uint32_t m_MemoryAddress{};
    std::vector<LocalVariable> m_Variables;
    std::vector<MethodParameter> m_Parameters;
    std::string m_remark;
    MethodCodeData m_codedata;
    std::uint32_t m_Class_id{};

    EplMethod(eprogramutils::MemArchiveStream& codes, int id, int memoryAddress) : eprogramutils::EplIDBase(id)
    {
        std::uint32_t Flags;
        this->m_MemoryAddress = memoryAddress;
        codes >> this->m_Class_id >> Flags >> this->m_ReturnDataType;
        this->m_isHidden = (Flags & 0x8) != 0;
        this->m_isPublic = (Flags & 0x80) != 0;
        this->m_name = codes.get_mfc_string();
        this->m_remark = codes.get_mfc_string();
        this->m_Variables = codes.read_blocks_have_Id_and_offset<LocalVariable>();
        this->m_Parameters = codes.read_blocks_have_Id_and_offset<MethodParameter>();
        codes >> this->m_codedata;

        for (auto& l : m_Variables)
        {
            l.m_space = std::make_shared<EplMethod>(*this);
        }

        for (auto& l : m_Parameters)
        {
            l.m_space = std::make_shared<EplMethod>(*this);
        }

        m_codedata.m_formid = id;
    }
};

// 库引用信息
class LibraryRefInfo
{
public:
    std::string FileName;
    std::string Guid;
    int Major{};
    int Minor{};
    std::string Name;
    std::uint32_t MinRequiredCmd{};
    std::uint16_t MinRequiredDataType{};
    std::uint16_t MinRequiredConstant{};

    static void ApplyCompatibilityInfo(std::vector<LibraryRefInfo>& infos,
        std::vector<std::uint32_t>& minRequiredCmds,
        std::vector<std::uint16_t>& minRequiredDataTypes,
        std::vector<std::uint16_t>& minRequiredConstants)
    {
        for (size_t i = 0; i < infos.size(); i++)
        {
            infos[i].MinRequiredCmd = (!minRequiredCmds.empty() && i < minRequiredCmds.size())
                ? minRequiredCmds[i]
                : 0;
            infos[i].MinRequiredDataType = (!minRequiredDataTypes.empty() && i < minRequiredDataTypes.size())
                ? minRequiredDataTypes[i]
                : 0;
            infos[i].MinRequiredConstant = (!minRequiredConstants.empty() && i < minRequiredConstants.size())
                ? minRequiredConstants[i]
                : 0;
        }
    }

    LibraryRefInfo() = default;
};

// EplStruct 结构体
struct EplStruct : virtual public eprogramutils::EplIDBase
{
    std::uint32_t MemoryAddress{};  // 内存地址
    bool bPublic{};
    std::string remark;             // 备注
    std::vector<StructMembersInfo> m_Members;

    EplStruct(eprogramutils::MemArchiveStream& codes, int id, int memoryAddress) : eprogramutils::EplIDBase(id)
    {
        this->MemoryAddress = memoryAddress;
        std::uint32_t Flags;
        codes >> Flags;
        this->bPublic = (Flags & 0x1) != 0;
        this->m_name = codes.get_mfc_string();
        this->remark = codes.get_mfc_string();
        this->m_Members = codes.read_blocks_have_Id_and_offset<StructMembersInfo>();

        for (auto& m : m_Members)
        {
            std::vector<std::uint32_t> real_array_size;
            for (auto x : m.m_ubound)
            {
                if (x != 0)
                {
                    real_array_size.push_back(x);
                }
            }
            m.m_ubound = real_array_size;
        }

        for (auto& m : m_Members)
        {
            m.m_space = std::make_shared<EplStruct>(*this);
        }
        for (auto& m : m_Members)
        {
            //如果数据类型为NULL，则默认为INT
            m.m_datatype = m.m_datatype == 0 ? 2147484417 : m.m_datatype;

        }
    }
};



//代码段
class EplCodeSection :public eprogramutils::EplStreamObj
{
private:
	auto split(const std::string& s, const std::string& delim)
	{
		std::vector<std::string> elems;
		if (delim.empty()) return elems;
		size_t len{ s.size() }, delim_len{ delim.size() }, pos{ 0 };
		while (pos < len)
		{
			auto find_pos{ s.find(delim, pos) };
			if (find_pos == std::string::npos)
			{
				elems.push_back(s.substr(pos, len - pos));
				break;
			}
			elems.push_back(s.substr(pos, find_pos - pos));
			pos = find_pos + delim_len;
		}
		return elems;
	}
	auto makeFne(const std::vector<std::string> str_arr) {
		
		for (const auto& s : str_arr)
		{
			auto lib = split(s, "\r");
			LibraryRefInfo fne;
			if (lib.size() == 5)
			{
				fne.FileName = lib[0];
				fne.Guid = lib[1];
				fne.Major = atoi(lib[2].c_str());
				fne.Minor = atoi(lib[3].c_str());
				fne.Name = lib[4];
			}
			else
			{
				debug_throw("易语言支持库信息解析错误");
			}
			all_epl_fne.push_back(fne);
		}
	}

public:
	EplCodeSection() = default;
	virtual void Serialize(eprogramutils::MemArchiveStream& Stream) override {
		if (Stream.size() < 0)
			return;
		Stream >> AllocatedIdNum;
		Stream.skip<std::uint32_t>();//未知的一个整数
		minRequiredCmds = Stream.get_mfc_byte_vec<std::uint32_t>();
		minRequiredDataTypes = Stream.get_mfc_byte_vec<std::uint16_t>();
		minRequiredConstants = Stream.get_mfc_byte_vec<std::uint16_t>();
		auto str_arr{ Stream.get_mfc_string_arry() };
		makeFne(str_arr);
		Stream >> flag >> MainMethod;
		LibraryRefInfo::ApplyCompatibilityInfo(all_epl_fne, minRequiredCmds, minRequiredDataTypes, minRequiredConstants);
		if ((flag & 1) != 0)Stream.skip(16);/*16个未知字节*/
		Stream >> icon;/*获取图标*/
		debug_command = Stream.get_mfc_string();
		class_info = Stream.read_blocks_have_Id_and_memoryaddress<EplClass>();
		method_info = Stream.read_blocks_have_Id_and_memoryaddress<EplMethod>();
		global_variables = Stream.read_blocks_have_Id_and_offset<GlobalVariable>();
		epl_struct = Stream.read_blocks_have_Id_and_memoryaddress<EplStruct>();
		epl_share_lib = Stream.read_blocks_have_Id_and_memoryaddress<EplShareLib>();
	}
	auto buildHashMap() {
		std::unordered_map<std::uint32_t, std::shared_ptr<eprogramutils::EplIDBase>> objformidmap;
		std::unordered_map<std::uint32_t, std::shared_ptr<BaseVariableInfo>> varformidmap;

		/*类/程序集以及类成员/程序集变量*/
		for (auto& eplClass : class_info)
		{
			objformidmap[eplClass.m_id] = std::make_shared<EplClass>(eplClass);
			for (auto& v : eplClass.m_Variables)
			{
				varformidmap[v.m_id] = std::make_shared<ClassVariableInfo>(v);
			}
		}
		/*子程序*/
		for (auto& eplMethod : method_info)
		{
			objformidmap[eplMethod.m_id] = std::make_shared<EplMethod>(eplMethod);
			for (auto& v : eplMethod.m_Parameters)
			{
				varformidmap[v.m_id] = std::make_shared<MethodParameter>(v);
			}
			for (auto& v : eplMethod.m_Variables)
			{
				varformidmap[v.m_id] = std::make_shared<LocalVariable>(v);
			}
		}
		/*全局变量*/
		for (auto& v : global_variables)
		{
			varformidmap[v.m_id] = std::make_shared<GlobalVariable>(v);
		}
		/*结构体*/
		for (auto& eplStruct : epl_struct)
		{
			objformidmap[eplStruct.m_id] = std::make_shared<EplStruct>(eplStruct);
			for (auto& v : eplStruct.m_Members)
			{
				varformidmap[v.m_id] = std::make_shared<StructMembersInfo>(v);
			}
		}
		/*共享库*/
		for (auto& eplDll : epl_share_lib)
		{
			objformidmap[eplDll.m_id] = std::make_shared<EplShareLib>(eplDll);
			for (auto& v : eplDll.Parameters)
			{
				varformidmap[v.m_id] = std::make_shared<ShareLibParameter>(v);
			}
		}
		return objformidmap;
	}
    auto makeHashClass() {
        std::unordered_map<std::uint32_t, std::shared_ptr<EplClass>> objformidmap;

        /*类/程序集以及类成员/程序集变量*/
        for (auto& eplClass : class_info)
        {
            /*程序集*/
            if (eplClass.m_base_class_id==0)  continue; 
            objformidmap.emplace(eplClass.m_id, std::make_shared<EplClass>(eplClass));
        }
        return objformidmap;
    }
    auto makeHashAssembly() {
        std::unordered_map<std::uint32_t, std::shared_ptr<EplClass>> objformidmap;

        /*类/程序集以及类成员/程序集变量*/
        for (auto& eplClass : class_info)
        {
            /*程序集*/
            if (eplClass.m_base_class_id != 0)  continue;
            objformidmap.emplace(eplClass.m_id, std::make_shared<EplClass>(eplClass));
        }
        return objformidmap;
    
    }


	std::uint32_t AllocatedIdNum{ 0xFFFF };//已分配的Id的数值部分的最大值
	std::uint32_t flag{ 0 };//【仅用于不带有编辑信息的EC文件】“_启动子程序”，系统将在 初始模块段 保存的方法被调用完成后调用
	std::uint32_t MainMethod{ 0 };
	std::vector<std::uint16_t> minRequiredDataTypes{ 0 };
	std::vector<std::uint16_t> minRequiredConstants{ 0 };
	std::vector<std::uint32_t> minRequiredCmds;
	std::vector<std::uint8_t> icon;//图标
	std::string debug_command;//调试命令行
	std::vector<LibraryRefInfo> all_epl_fne;//所有易语言支持库
	std::vector<EplClass>  class_info;
	std::vector<EplMethod>  method_info;
	std::vector<GlobalVariable> global_variables;
	std::vector<EplStruct> epl_struct;
	std::vector<EplShareLib> epl_share_lib; 
};

#endif