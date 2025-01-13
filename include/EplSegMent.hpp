#ifndef EPL_SEGMENT_HPP
#define EPL_SEGMENT_HPP
#include"utils.hpp"

struct EplSegMent
{
private:
	U32EPLMAGIC m_Check;/*段魔术数*/
	U32EPLMAGIC m_CheckSum;/*段校验和*/
	std::uint8_t m_Key[4]{ 0 };/*用于决定如何对Name字段编码，同时也是易语言内部识别数据段的唯一标识（易语言内部并不靠Name字段识别数据段*/
	char m_Name[30]{ 0 };/*段名*/
	short m_ReserveFill{ 0 };/*对齐填充（确认于易语言V5.71）*/
	std::uint32_t m_Index{ 0 };/*段索引*/
	std::uint32_t m_IsOptional{ 0 };/*如果易系统版本过低，无法识别本数据段，或本数据段已损坏，此字段将指定是否可以直接跳过。对于重要数据段，此字段通常设置为false，否则设置为true。*/
	std::uint32_t m_DataCheckSum{ 0 };/*数据校验和*/
	std::uint32_t m_Size{ 0 };/*切片长度*/
	std::uint8_t Reserve[40]{ 0 };/*预留的40字节*/
public:
	bool CheckThis() {/*检测有效性*/
		return CheckIsEsegment(m_Check);
	}
	const std::uint32_t GetKey()  const {
		return *reinterpret_cast<const std::uint32_t*>(this->m_Key);
	}
	/*返回空则代表读到尾段或数据错误*/
	const  std::string GetName()   const{
		char Name[30]{ 0 };
		/*解密段名*/
		if (!CheckIsDefaulKey(this->m_Key)) {
			for (size_t i = 0, key_i = 1; i < sizeof(this->m_Name); i++, key_i++) {
				Name[i] = this->m_Name[i] ^ this->m_Key[key_i % sizeof(this->m_Key)];

			};
			return Name;
		};
		return {};
	}
public:
	std::size_t GetDataSize()
	{
		return this->m_Size;
	}
};
#endif