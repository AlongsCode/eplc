//该页代码用于存放易语言IR对象,以及对象的生成代码用于最后翻译器使用
//构建器的最终目标是生成易语言IR对象,然后由翻译器进行翻译
#ifndef EPL_LIBOBJMAP_HPP
#define EPL_LIBOBJMAP_HPP
#include<unordered_map>
#include"tace.hpp"
#include"utils.hpp"


namespace elibcall {
	inline bool isFneDataType(const std::uint32_t id) {
		return (id & 0xFF000000) == 0 && id != 0;
	};

	/*易语言支持库数据类型*/
	class LibDataType
	{
		struct DataFrom/*数据来源*/
		{
			union
			{
				struct {
					std::uint16_t m_lib_id;
					std::uint16_t m_data_id;
				};
				std::uint32_t m_id;
			};
			DataFrom() { memset(this, 0, sizeof(*this)); };
		};
		DataFrom m_from{};
	public:
		LibDataType(const std::uint16_t lib_id, const std::uint16_t data_id) {
			m_from.m_data_id = data_id;
			m_from.m_lib_id = lib_id;
		};
		LibDataType(const std::uint32_t id) {
			m_from.m_id = id;
		};

		/*获取数据类型所在支持库中的索引*/
		auto getDataIndex()const->std::uint16_t {
			return m_from.m_data_id;
		};
		/*获取数据类型所在的支持库索引*/
		auto getLibIndex()const->std::uint16_t {
			return m_from.m_lib_id;
		};
	};

}



#endif