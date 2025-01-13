#include "EplObjMap.hpp"

#define NAMESPACE_EPLCLTYPE_BEGIN namespace eplcltype{
#define NAMESPACE_EPLCLTYPE_END }
NAMESPACE_EPLCLTYPE_BEGIN


ShareLibSpace::ShareLibSpace()
	:BaseSpace("eplsharelib", space_type::SPACE_SHARELIB) {};


FianlShareFunction::FianlShareFunction(const ::std::string& name, const std::uint32_t id, const ::std::string& lib_name, const ::std::string& lib_func_name, const ::std::string& comment)
	:BaseCodeObj(name, id, comment), m_space(std::make_shared<ShareLibSpace>()), m_lib_name(lib_name), m_lib_func_name(lib_func_name) {};



/*系统数据类型*/
FinalSysDataType::FinalSysDataType(const::std::string& name, const std::uint32_t id)
	:BaseDataType(name, id, std::make_shared<SysSpace>()) {}


/*类对象*/
FinalClass::FinalClass(const::std::string& name, const std::uint32_t id, const std::vector<std::shared_ptr<Member>>& member, const::std::string& comment)
	:BaseAssembly(name, id, comment), BaseDataType(name, id, std::make_shared<BaseSpace>("eplclass", BaseSpace::space_type::SPACE_CLASS), comment) {};
FinalClass::FinalClass(const::std::string& name, const std::uint32_t id, const::std::string& comment)
	:BaseAssembly(name, id, comment), BaseDataType(name, id, std::make_shared<BaseSpace>("eplclass", BaseSpace::space_type::SPACE_CLASS), comment) {};




NAMESPACE_EPLCLTYPE_END

