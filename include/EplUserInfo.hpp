#ifndef EPL_USER_INFO_HPP
#define EPL_USER_INFO_HPP
#include"tace.hpp"
#include"utils.hpp"






struct EplUserInfo :public eprogramutils::EplStreamObj
{
	EplUserInfo() = default;
	virtual void Serialize(eprogramutils::MemArchiveStream& s) override {
		this->ProjectName = s.get_mfc_string();
		this->Remark = s.get_mfc_string();
		this->Author = s.get_mfc_string();
		this->PostalCode = s.get_mfc_string();
		this->Address = s.get_mfc_string();
		this->PhoneNumber = s.get_mfc_string();
		this->Fax = s.get_mfc_string();
		this->Email = s.get_mfc_string();
		this->HomePage = s.get_mfc_string();
		this->Copyright = s.get_mfc_string();
		s >> this->Major >> this->Minor >> this->Build >> this->Revision;
		this->isWriteInApp = (s.read<std::uint32_t>() == 0);
		this->PluginName = s.get_fixelenth_string(20);
		this->m_ExportPublicClassMethod = (s.read<std::uint32_t>() == 0);
	}
	auto to_string() const {
		std::stringstream ss;
		if (isWriteInApp)
		{
			ss
				<< "程序名称: " << ProjectName
				<< "\n程序描述: " << Remark
				<< "\n作者: " << Author
				<< "\n邮政编码: " << PostalCode
				<< "\n地址: " << Address
				<< "\n电话: " << PhoneNumber
				<< "\n传真: " << Fax
				<< "\n电子邮箱: " << Email
				<< "\n主页: " << HomePage
				<< "\n版权声明: " << Copyright
				<< "\n版本号: " << Major << "." << Minor
				<< "\n创建号: " << Build << "." << Revision
				<< "\n是否将星号信息写入程序" << isWriteInApp
				<< "\n插件名称: " << PluginName;
			;

		}


		return ss.str();
	}
	std::string ProjectName;/*程序名称*/
	std::string Remark;/*程序描述*/
	std::string Author;/*作者*/
	std::string PostalCode;/*邮政编码*/
	std::string Address;/*地址*/
	std::string PhoneNumber;/*电话*/
	std::string Fax;/*传真*/
	std::string Email;/*电子邮箱*/
	std::string HomePage;/*主页*/
	std::string Copyright;/*版权声明*/
	std::uint32_t Major{ 0 };/*主版本号*/
	std::uint32_t Minor{ 0 };/*次版本号*/
	std::uint32_t Build{ 0 };/*主创建号*/
	std::uint32_t Revision{ 0 };/*次创建号*/
	bool isWriteInApp{ false };/*是否将星号信息写入程序*/
	std::string PluginName;/*插件名称*/
	bool m_ExportPublicClassMethod{ false };/*是否导出公共类方法*/
};

#endif