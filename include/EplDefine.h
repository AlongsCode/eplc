#ifndef EPL_DEFINE_H
#define EPL_DEFINE_H



/*暂不支持窗口组件*/
#include <cstddef>
#include <cstdint>
#include<span>
/*基础类型MAKEDWORD(MAKEWORD(a,b)),起始对于x64x86是不同的，但是吴涛不考虑x64,所以x64我想咋定义都行*/

/*自定义数据类型*/
//constexpr auto CustomType(uint16_t a, uint16_t b) {
//	return (static_cast<uint32_t>(a) & 0xffff) | (static_cast<uint32_t>(b) & 0xffff) << 16);
//}

namespace wteplapi {

	/*确保不同编译器不同基础是数据类型长度不同时，同平台下值相同*/
	constexpr auto BaseType(std::uint16_t a, std::uint16_t b)
	{
		std::uint16_t merged = static_cast<std::uint16_t>((static_cast<std::uint8_t>(a) & 0xff) | (static_cast<std::uint16_t>(static_cast<std::uint8_t>(b) & 0xff) << 8));
		std::uint32_t result = (static_cast<std::uint32_t>(merged) & 0xffff) | (static_cast<std::uint32_t>(0x8000) << 16);
		return static_cast<std::int32_t>(result);
	}
	/*自定义数据类型*/
	constexpr auto UserType(std::uint16_t a, std::uint16_t b)
	{
		return static_cast<std::uint32_t>((static_cast<std::uint16_t>(a) & 0xFFFF) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(b) & 0xFFFF) << 16));
	}

	// 基本数据类型
	enum class DATA_TYPE : std::int32_t {
		_SDT_NULL = 0,  // 空
		_SDT_ALL = BaseType(0, 0),  // 通用型
		SDT_BYTE = BaseType(1, 1),  // uint8
		SDT_SHORT = BaseType(1, 2),  // int16
		SDT_INT = BaseType(1, 3),  // int32
		SDT_INT64 = BaseType(1, 4),  // int64
		SDT_FLOAT = BaseType(1, 5),  // float
		SDT_DOUBLE = BaseType(1, 6),  // double
		SDT_BOOL = BaseType(2, 0),  // int32，但是 C++ 中更好使用 bool
		SDT_DATE_TIME = BaseType(3, 0),  // double 时间类型
		SDT_TEXT = BaseType(4, 0),  // text 文本型
		SDT_BIN = BaseType(5, 0),  // 字节集 struct ebin
		SDT_SUB_PTR = BaseType(6, 0),  // 子程序指针
		SDT_STATMENT = BaseType(8, 0),
		SDT_UTEXT = BaseType(9, 0),  // text 文本型
	};
	enum class DataTypeType
	{
		DTT_IS_SYS_DATA_TYPE = 1,	// 系统数据类型
		DTT_IS_USER_DATA_TYPE = 2,	// 用户自定义数据类型
		DTT_IS_LIB_DATA_TYPE = 3,	// 库定义数据类型
		DTT_IS_NULL_DATA_TYPE = 4,	// 空数据类型
	};
	inline DataTypeType GetDataTypeType(DATA_TYPE dtDataType)
	{
		if (dtDataType == DATA_TYPE::_SDT_NULL)
			return DataTypeType::DTT_IS_NULL_DATA_TYPE;
#define	DTM_SYS_DATA_TYPE_MASK		0x80000000
#define	DTM_USER_DATA_TYPE_MASK		0x40000000
#define	DTM_LIB_DATA_TYPE_MASK		0x00000000
		std::uint32_t dw = static_cast<int>(dtDataType) & 0xC0000000;
		return dw == DTM_SYS_DATA_TYPE_MASK ? DataTypeType::DTT_IS_SYS_DATA_TYPE :
			dw == DTM_USER_DATA_TYPE_MASK ? DataTypeType::DTT_IS_USER_DATA_TYPE :
			DataTypeType::DTT_IS_LIB_DATA_TYPE;
#undef	DTM_SYS_DATA_TYPE_MASK
#undef	DTM_USER_DATA_TYPE_MASK
#undef	DTM_LIB_DATA_TYPE_MASK

	}
	// 操作系统类别：
	//#define	__OS_WIN  0x80000000;
	//#define	__OS_LINUX  0x40000000;
	//#define	__OS_UNIX  0x20000000;
	//#define	OS_ALL  (__OS_WIN | __OS_LINUX | __OS_UNIX)


	////数据类型标识
	//enum class DTM_DATA_TYPE {
	//	//系统类型
	//	DTM_SYS_DATA_TYPE_MASK = 0x80000000,
	//	//用户自定义类型
	//	DTM_USER_DATA_TYPE_MASK = 0x40000000,
	//	//库定义数据类型
	//	DTM_LIB_DATA_TYPE_MASK = 0x00000000,
	//};

	// 程序的版本类型宏
	enum class PT_VER {
		PT_EDIT_VER = 1,	// 为用作编辑的版本
		PT_DEBUG_RUN_VER = 2,// 为DEBUG调试运行版本
		PT_RELEASE_RUN_VER = 3,	// 为RELEASE最终运行版本
	};

	// 命令的学习难度级别
	enum class LVL_TYPE :short {
		LVL_SIMPLE = 1,	// 初级命令
		LVL_SECONDARY = 2,	// 中级命令
		LVL_HIGH = 3,	// 高级命令
	};

	//参数标志
	enum class ARG_MARK :unsigned long
	{
		AS_HAS_DEFAULT_VALUE = (1 << 0),//本参数有默认值，默认值在m_nDefault中说明。本参数在编辑程序时已被处理，编译时不需再处理。
		AS_DEFAULT_VALUE_IS_EMPTY = (1 << 1),//本参数有默认值，默认值为空，与AS_HAS_DEFAULT_VALUE标志互斥，运行时所传递过来的参数数据类型可能为_SDT_NULL。
		AS_RECEIVE_VAR = (1 << 2),//为本参数提供数据时只能提供单一变量，而不能提供整个变量数组、立即数或命令返回值。运行时所传递过来的参数数据肯定是内容不为数组的变量地址。
		AS_RECEIVE_VAR_ARRAY = (1 << 3),// 为本参数提供数据时只能提供整个变量数组，而不能提供单一变量、立即数或命令返回值。
		AS_RECEIVE_VAR_OR_ARRAY = (1 << 4),// 为本参数提供数据时只能提供单一变量或整个变量数组，而不能提供立即数 或命令返回值。如果具有此标志，则传递给库命令参数的数据类型将会通过DT_IS_ARY来标志其是否为数组。
		AS_RECEIVE_ARRAY_DATA = (1 << 5),//为本参数提供数据时可以同时提供非数组或数组数据，与上标志互斥。如指定了本标志，运行时所传递过来的参数数据肯定为数组。
		AS_RECEIVE_ALL_TYPE_DATA = (1 << 6), //为本参数提供数据时可以同时提供非数组或数组数据，与上标志互斥。 如果具有此标志，则传递给库命令参数的数据类型将会通过DT_IS_ARY来标志其是否为数组。
		AS_RECEIVE_VAR_OR_OTHER = (1 << 9),// 为本参数提供数据时可以提供单一变量或立即数或命令返回值，不能提供数组。 如果具有此标志，则传递给库命令参数的数据类型将会通过DT_IS_VAR来标志其是否为变量地址。
	};

	//函数标志
	enum class CMD_MARK : unsigned short
	{
		CT_IS_HIDED = (1 << 2),//   本命令是否为隐含命令（即不需要由用户直接插入的命令，如循环结束命令、被废弃但为了保持兼容性又要存在的命令）。
		CT_IS_ERROR = (1 << 3),// 本命令在本库中不能使用，具有此标志一定隐含，主要用作在不同语言版本的相同库中使用，即：A命令在A语言版本库中可能需要实现并使用，但在B语言版本库中可能就不需要。如果程序中使用了具有此标志的命令，则只能支持该程序调入和编译，而不能支持运行。如具有此标志，本命令可以不实现其执行部分。
		CT_DISABLED_IN_RELEASE = (1 << 4),// 调试版可用函数,不会编译到程序内部。
		CT_ALLOW_APPEND_NEW_ARG = (1 << 5),//无限参数,最少一个参数。
		CT_RETRUN_ARY_TYPE_DATA = (1 << 6),//返回数组。
		/***********************************对象类*****************************/
		/*复制函数
		1、此命令必须仅接受一个同数据类型参数而且不返回任何数据。
		  2、执行本命令时对象的内容数据为未初始化状态，命令内必须负责初始化其全部成员数据。
		  3、所提供过来的待复制数据类型参数数据可能为全零状态（由编译器自动生成的对象初始代码设置)*/
		CT_IS_OBJ_COPY_CMD = (1 << 7),

		/*析构函数
		1、此命令必须没有任何参数而且不返回任何数据。
		2、此命令被执行时对象的内容数据可能为全零状态（由编译器自动生成的对象初始代码设置)*/
		CT_IS_OBJ_FREE_CMD = (1 << 8),

		/*构造函数
		1、此命令必须没有任何参数而且不返回任何数据。
		2、此命令被执行时对象的内容数据为全零状态。
		3、指定类型成员（复合数据类型成员、数组成员），必须按照对应格式继续进行下一步初始化。*/
		CT_IS_OBJ_CONSTURCT_CMD = (1 << 9)
	};

	//结构体成员标志
	enum class STRUCT_ELEMENT_MARK : unsigned long
	{
		LES_HAS_DEFAULT_VALUE = (1 << 0),//存在默认值,枚举成员一定附带,在m_nDefault中指明
		LES_HIDED = (1 << 1)// 本子数据成员被隐藏。
	};

	//结构体标志
	enum class STRUCT_MARK : unsigned long
	{
		//   本类型是否为隐含类型（即不能由用户直接用作定义的类型，如被废弃
		// 但为了保持兼容性又要存在的类型）。
		LDT_IS_HIDED = (1 << 0),
		// 本类型在本库中不能使用，具有此标志一定隐含。
		// 即使具有此标志，本类型的类型数据也必须完整定义。
		LDT_IS_ERROR = (1 << 1),
		//   是否为窗口或可在窗口内使用的组件，如此标志置位则m_nElementCount必为0，
		LDT_WIN_UNIT = (1 << 6),
		// 是否为容器组件，如有此标志，LDT_WIN_UNIT必置位。
		LDT_IS_CONTAINER = (1 << 7),
		//是否为TAB控件(选择夹) 4.0新增
		LDT_IS_TAB_UNIT = (1 << 8),
		// 仅用作提供功能的窗口组件（如时钟），如此标志置位则LDT_WIN_UNIT必置位。
		// 具有此标志的组件尺寸固定为32*32，并且在运行时无可视外形。
		LDT_IS_FUNCTION_PROVIDER = (1 << 15),
		// 仅用作窗口组件，如此标志置位则表示此组件不能接收输入焦点，不能TAB键停留。
		LDT_CANNOT_GET_FOCUS = (1 << 16),
		// 仅用作窗口组件，如此标志置位则表示此组件默认不停留TAB键，使用本标志必须上标志置位。
		LDT_DEFAULT_NO_TABSTOP = (1 << 17),
		// 是否为枚举数据类型。
		// 3.7新增。
		LDT_ENUM = (1 << 22),
		// 是否为消息过滤组件。
		LDT_MSG_FILTER_CONTROL = (1 << 5),
		//!!! 注意高位包含 __OS_xxxx 宏用于指定本数据类型所支持的操作系统。
	};



	/*参数结构体*/
	struct ARG_INFO
	{
		char* m_szName;             // 参数名称
		char* m_szExplain;          // 参数详细解释
		unsigned short m_shtBitmapIndex;    // 指定图像索引，从1开始，0表示无
		unsigned short m_shtBitmapCount;    // 图像数目（用作动画）
		DATA_TYPE m_dtType;         //参数数据类型

		// 系统基本类型参数的默认指定值（在编辑程序时已被处理，编译时不需再处理）：
		// 1. 数字型：直接为数字值长度不超过size_t，使用静态转化
		// 2. 逻辑型：1等于真，0等于假；
		// 3. 文本型：本变量此时为char*指针，指向默认文本串；
		// 4. 其它所有类型参数（包括自定义类型）一律无默认指定值。
		const void* m_nDefault;

		ARG_MARK m_dwState;         // 参数标志
	};
	using PARG_INFO = ARG_INFO*;

	/*命令结构体*/
	struct CMD_INFO
	{
		char* m_szName;// 命令中文名称			
		char* m_szEgName;// 命令英文名称，可以为空或NULL。			
		char* m_szExplain;// 命令详细解释		
		short		m_shtCategory;// 全局命令的所属类别，从1开始。对象成员命令的此值为-1。		
		CMD_MARK		m_wState;//参数标志
		DATA_TYPE	m_dtRetValType;/*返回值类型，为 _SDT_ALL 绝对不能返回数组和符合类型*/
		unsigned short		m_wReserved;
		LVL_TYPE		m_shtUserLevel;	// 命令的用户学习难度级别，本变量的值为级别宏。
		short		m_shtBitmapIndex;	// 指定图像索引,从1开始,0表示无.
		short		m_shtBitmapCount;	// 图像数目(用作动画).
		unsigned int			m_nArgCount;		// 命令的参数数目
		PARG_INFO	m_pBeginArgInfo;


		/*是否是对象方法*/
		inline bool is_obj_fun() const
		{
			return m_shtCategory == -1;
		}
	};
	using PCMD_INFO = CMD_INFO*;

	/*结构体(符合数据类型)成员定义结构体*/
	struct LIB_DATA_TYPE_ELEMENT
	{

		DATA_TYPE m_dtType;//成员数据类型, 若枚举中恒为SDT_INT。
		void* m_pArySpec;//数组成员,枚举数据类型中为nullptr，不为数组为nullptr,注意绝对不能指定某维数上限为0的数组。
		const char* m_szName;//子数据组件的中文变量名称，如果所属的数据类型只有一个子数据组件，则此值应该为NULL。
		const char* m_szEgName;// 子数据组件的英文变量名称，可以为空或NULL。
		const char* m_szExplain;//成员解释
		STRUCT_ELEMENT_MARK m_dwState;//结构体标识
		const void* m_nDefault;//结构体存在默认值时的默认值
	};
	using PLIB_DATA_TYPE_ELEMENT = LIB_DATA_TYPE_ELEMENT*;



	// 注意首部必须完全包含 EVENT_ARG_INFO 。
	struct EVENT_ARG_INFO
	{
#define EAS_BY_REF (1 << 1)     // 不使用 (1 << 0)
		const char* m_szName;            // 参数名称
		const char* m_szExplain;         // 参数详细解释
		unsigned long m_dwState;           // EAS_BY_REF, 是否需要以参考方式传值,如果置位,则支持库中抛出事件的代码必须确保其能够被系统所访问(即分配内存的方法和数据的格式必须符合要求)。
		DATA_TYPE m_dtDataType;     // 类型
	};

	using PEVENT_ARG_INFO = EVENT_ARG_INFO*;


	//组件事件结构体
	struct EVENT_INFO
	{
		const char* m_szName;            // 事件名称
		const char* m_szExplain;         // 事件详细解释
		// 以下基本状态值宏与 EVENT_INFO 中的定义相同。
		// #define EV_IS_HIDED        (1 << 0)    // 本事件是否为隐含事件(即不能被一般用户所使用或被废弃但为了保持兼容性又要存在的事件)。
		// #define EV_IS_KEY_EVENT    (1 << 2)    
		// 注意高位包含 __OS_xxxx 宏用于指定本事件所支持的操作系统。
		// #define _EVENT_OS(os)    ((os) >> 1)  // 用作转换os类型以便加入到m_dwState。
		// #define _TEST_EVENT_OS(m_dwState,os)    ((_EVENT_OS (os) & m_dwState) != 0) // 用作测试指定事件是否支持指定操作系统。
#define EV_IS_VER2 (1 << 31)    // 表示本结构为EVENT_INFO2,!!!使用本结构时必须加上此状态值。

		unsigned long   m_dwState;           // 返回值类型, 最好带上 EV_IS_VER2, EV_ 开头常量, _EVENT_OS() 绝对不能定义成返回文本、字节集、复合类型等需要空间释放代码的数据类型。
		int    m_nArgCount;         // 事件的参数数目
		PEVENT_ARG_INFO m_pEventArgInfo;   // 事件参数
		DATA_TYPE m_dtRetDataType;          //! 如果该数据类型有额外的数据需要释放,需要由支持库中抛出事件的代码负责将其释放。
	};

	using PEVENT_INFO = EVENT_INFO*;


	// 库定义数据类型结构
	struct LIB_DATA_TYPE_INFO
	{
		const char* m_szName;  // 数据类型的中文名称（如：“整数”，“高精度数”等等）。
		const char* m_szEgName;// 数据类型的英文名称（如：“int”，“double”等等），可为空或NULL。
		const char* m_szExplain;   // 数据类型的详细解释，如无则可为NULL。
		int m_nCmdCount;	// 本类型中提供的成员命令的数目（可为0）。
		unsigned int* m_pnCmdsIndex;	// 顺序记录本类型中所有成员命令在库的命令表中的索引值，可为NULL。
		STRUCT_MARK m_dwState;
		////////////////////////////////////////////
		// 以下变量只有在为窗口、菜单组件且不为枚举数据类型时才有效。也就是说暂不支持
		unsigned long m_dwUnitBmpID;		// 指定在库中的组件图像资源ID，0为无。// 在OCX包装库中，m_dwUnitBmpID指定备用图像资源ID。						
		int m_nEventCount;//属性数量
		PEVENT_INFO m_pEventBegin;	// 定义本组件的所有事件。PEVENT_INFO2
		int m_nPropertyCount;//属性数量
		void* m_pPropertyBegin;//属性索引 PUNIT_PROPERTY
		void* m_pfnGetInterface;//窗口对象消息函数 PFN_GET_INTERFACE
		////////////////////////////////////////////
		// 以下变量只有在不为窗口、菜单组件或为枚举数据类型时才有效。


		int	m_nElementCount;// 本数据类型中子数据组件的数目。如为窗口、菜单组件，此变量值必为0。
		PLIB_DATA_TYPE_ELEMENT m_pElementBegin;  // 指向子数据成员数组。
	};
	using PLIB_DATA_TYPE_INFO = LIB_DATA_TYPE_INFO*;

	/*易库参数数据结构体*/
#pragma pack (push, old_value) 
#pragma pack (1)
	struct MDATA_INF
	{
		union
		{
			unsigned char m_byte;         // SDT_BYTE
			short	      m_short;        // SDT_SHORT
			int	      m_int;          // SDT_INT
			unsigned int	      m_uint;         // (unsigned long)SDT_INT
			__int64      m_int64;        // SDT_INT64
			float	      m_float;        // SDT_FLOAT
			double	      m_double;       // SDT_DOUBLE
			double          m_date;         // SDT_DATE_TIME OLE时间
			bool          m_bool;         // SDT_BOOL #define BOOL
			char* m_pText;        // SDT_TEXT，不可能为NULL。 // !!!为了避免修改到常量段(m_pText有可能指向常量段区域)中的数据，  // 只可读取而不可更改其中的内容，下同。								
			unsigned char* m_pBin;         // SDT_BIN，不可能为NULL，!!!只可读取而不可更改其中的内容。

			size_t        m_dwSubCodeAdr; // SDT_SUB_PTR，记录子程序代码地址。
			void* m_pCompoundData;// 复合数据类型数据指针，指针所指向数据的格式请见 run.h   // 可以直接更改其中的数据成员，但是如果需要必须首先释放该成员。。						
			void* m_pAryData;     // 数组数据指针，指针所指向数据的格式请见 run.h 。// 注意如果为文本或字节集数组，则成员数据指针可能为NULL。// !!! 只可读取而不可更改其中的内容。						  



			// 为指向变量地址的指针，仅当传入参数到库命令实现函数时才有用。
			unsigned char* m_pByte;         // SDT_BYTE*
			short* m_pShort;        // SDT_SHORT*
			int* m_pInt;          // SDT_INT*
			unsigned int* m_pUInt;         // ((unsigned long)SDT_INT)*
			__int64* m_pInt64;        // SDT_INT64*
			float* m_pFloat;        // SDT_FLOAT*
			double* m_pDouble;       // SDT_DOUBLE*
			double* m_pDate;         // SDT_DATE_TIME*
			bool* m_pBool;         // SDT_BOOL*
			char** m_ppText;        // SDT_TEXT，*m_ppText可能为NULL。// 注意写入新值之前必须释放前值，即：MFree (*m_ppText)。 // !!!不可直接更改*m_ppText所指向的内容，只能释放原指针后换入新指针.		 
			unsigned char** m_ppBin;         // SDT_BIN，*m_ppBin可能为NULL。 // 注意写入新值之前必须释放前值，即：MFree (*m_ppBin)。 // !!!不可直接更改*m_ppBin所指向的内容，只能释放原指针后换入新指针。		
			size_t* m_pdwSubCodeAdr; // SDT_SUB_PTR，子程序代码地址变量。
			void** m_ppCompoundData;   // 复合数据类型变量。// 可以直接更改其中的数据成员，但是如果需要必须首先释放该成员。				
			void** m_ppAryData;        // 数组数据变量，注意：// 1、写入新值之前必须释放原值（使用NRS_FREE_VAR通知）。// 2、变量如果为文本或字节集数组，则成员数据指针可能为NULL。// !!!不可直接更改*m_ppAryData所指向的内容，只能释放原指针后换入新指针。

		};


		// 1、当用作传入参数时，如果该参数具有 AS_RECEIVE_VAR_OR_ARRAY 或
		//    AS_RECEIVE_ALL_TYPE_DATA 标志，且为数组数据，则包含数组标志 DT_IS_ARY ，
		//    这也是 DT_IS_ARY 标志的唯一使用场合。
		// 2、当用作传递参数数据时，如果为空白数据，则为 _SDT_NULL 。
		DATA_TYPE m_dtDataType{ DATA_TYPE::_SDT_NULL };
	};
#pragma pack (pop, old_value) 
	using PMDATA_INF = MDATA_INF*;




	struct EVENT_ARG_VALUE
	{
		MDATA_INF m_inf{};

		// m_inf中是否为指针数据。
		//!!! 注意如果 m_inf.m_dtDataType 为文本型、字节集型、库定义数据类型（除开窗口单元及菜单数据类型），
		// 必须传递指针，即本标记必须置位。
#define EAV_IS_POINTER  (1 << 0)
#define EAV_IS_WINUNIT  (1 << 1)    // 补充说明m_inf.m_dtDataType数据类型是否为窗口单元，
									// !!!注意如果m_inf.m_dtDataType为窗口单元，此标记必须置位。
		unsigned long m_dwState{};
	};
	using  PEVENT_ARG_VALUE = EVENT_ARG_VALUE*;


	//消息通知事件
	struct EVENT_NOTIFY2
	{
		// 记录事件的来源
		unsigned long m_dwFormID;  // 窗口 ID
		unsigned long m_dwUnitID;  // 窗口组件 ID
		int m_nEventIndex;  // 事件索引
		int m_nArgCount{ 0 };  // 事件所传递的参数数目，最多 MAX_EVENT2_ARG_COUNT 个。
		EVENT_ARG_VALUE m_arg[12];  // 记录各参数值。

		//!!! 逻辑型注意下面成员在没有定义返回值的事件处理中无效。
		// 用户事件处理子程序处理完毕事件后是否有返回值
		int m_blHasRetVal{ 0 };
		// 记录用户事件处理子程序处理完毕事件后的返回值，注意其中的m_infRetData.m_dtDataType成员未被使用。
		MDATA_INF m_infRetData{};

		EVENT_NOTIFY2(unsigned long dwFormID, unsigned long dwUnitID, int nEventIndex)
			:m_dwFormID(dwFormID), m_dwUnitID(dwUnitID), m_nEventIndex(nEventIndex) {};

	};

	enum class CONST_TYPE
	{
		CT_NULL = 0,
		CT_NUM = 1,   // value sample: 3.1415926
		CT_BOOL = 2,  // value sample: 1
		CT_TEXT = 3,   // value sample: "abc"
	};

	struct  LIB_CONST_INFO // 库常量数据结构
	{

		const char* m_szName;    // 常量名
		const char* m_szEgName;  // 英文名
		const char* m_szExplain; // 说明
		short  m_shtLayout; // 必须为 1
		CONST_TYPE  m_shtType;   // 常量类型, CT_NULL=空常量, CT_NUM=数值型,double保存值, CT_BOOL=逻辑型, CT_TEXT=文本型
		const char* m_szText;    // CT_TEXT
		double m_dbValue;   // CT_NUM、CT_BOOL
	};
	using PLIB_CONST_INFO = LIB_CONST_INFO*;



	enum class MFC_WND_INTERFACE
	{
		// 窗口单元指定的接口。
		ITF_CREATE_UNIT = 1,     // 创建组件
		ITF_PROPERTY_UPDATE_UI = 2,  // 指定属性目前可否被修改
		ITF_DLG_INIT_CUSTOMIZE_DATA = 3,   // 使用对话框设置附加定制数据
		ITF_NOTIFY_PROPERTY_CHANGED = 4,   // 通知某属性数据被用户修改
		ITF_GET_ALL_PROPERTY_DATA = 5,   // 取全部属性数据
		ITF_GET_PROPERTY_DATA = 6,   // 取某属性数据
		ITF_GET_ICON_PROPERTY_DATA = 7,    // 取窗口的图标属性数据(仅用于窗口)
		ITF_IS_NEED_THIS_KEY = 8,    // 询问组件是否需要指定的按键信息,用作组件截获处理默认为系统处理的按键,如TAB、SHIFT+TAB、UP、DOWN等。
		ITF_LANG_CNV = 9,    // 组件数据语言转换
		ITF_MSG_FILTER = 11,    // 消息过滤
		ITF_GET_NOTIFY_RECEIVER = 12,   // 取组件的附加通知接收者(PFN_ON_NOTIFY_UNIT)
	};



	enum class EMSG :unsigned int {
		// NES_ 宏为仅被易编辑环境处理的通知。
		NES_GET_MAIN_HWND = 1,
		// 取易编辑环境主窗口的句柄，用作支持支持库的AddIn。
		NES_RUN_FUNC = 2,
		// 通知易编辑环境运行指定的功能，返回一个BOOL值。
		// dwParam1为功能号。
		// dwParam2为一个双DWORD数组指针,分别提供功能参数1和2。
		NES_PICK_IMAGE_INDEX_DLG = 7,
		// 通知易编辑环境显示一个对话框列出指定图片组内的所有图片，并返回用户所选择图片的索引号。
		// dwParam1为所欲浏览的有效的图片组句柄。
		//   dwParam2如果不为NULL，则系统认为此为一个编辑框HWND窗口句柄，当用户做出有效选择后，
		// 系统将自动更改此编辑框的内容并将焦点转移上去。
		// 返回用户所选择图片的索引号(-1表示用户选择无图片)，如果用户未选择，则返回-2。

	// NAS_ 宏为既被易编辑环境又被易运行环境处理的通知。
		NAS_GET_APP_ICON = 1000,
		// 通知系统创建并返回程序的图标。
		// dwParam1为PAPP_ICON指针。
		NAS_GET_LIB_DATA_TYPE_INFO = 1002,
		// 返回指定库定义数据类型的PLIB_DATA_TYPE_INFO定义信息指针。
		// dwParam1为欲检查的数据类型。
		// 如果该数据类型无效或者不为库定义数据类型，则返回NULL，否则返回PLIB_DATA_TYPE_INFO指针。
		NAS_GET_HBITMAP = 1003,
		// dwParam1为图片数据指针，dwParam2为图片数据尺寸。
		// 如果成功返回非NULL的HBITMAP句柄（注意使用完毕后释放），否则返回NULL。
		NAS_GET_LANG_ID = 1004,
		// 返回当前系统或运行环境所支持的语言ID，具体ID值请见lang.h
		NAS_GET_VER = 1005,
		// 返回当前系统或运行环境的版本号，LOWORD为主版本号，HIWORD为次版本号。
		NAS_GET_PATH = 1006,
		/* 返回当前开发或运行环境的某一类目录或文件名，目录名以“\”结束。
		   dwParam1: 指定所需要的目录，可以为以下值：
			 A、开发及运行环境下均有效的目录:
				1: 开发或运行环境系统所处的目录；
			 B、开发环境下有效的目录(仅开发环境中有效):
				1001: 系统例程和支持库例程所在目录名
				1002: 系统工具所在目录
				1003: 系统帮助信息所在目录
				1004: 保存所有登记到系统中易模块的目录
				1005: 支持库所在的目录
				1006: 安装工具所在目录
			 C、运行环境下有效的目录(仅运行环境中有效):
				2001: 用户EXE文件所处目录；
				2002: 用户EXE文件名；
		   dwParam2: 接收缓冲区地址，尺寸必须为MAX_PATH。
		*/

		// 通过指定HWND句柄创建一个CWND对象，返回其指针，记住此指针必须通过调用NRS_DELETE_CWND_OBJECT来释放
		// dwParam1为HWND句柄
		// 成功返回CWnd*指针，失败返回NULL
		NAS_CREATE_CWND_OBJECT_FROM_HWND = 1007,

		NAS_DELETE_CWND_OBJECT = 1008,
		// 删除通过NRS_CREATE_CWND_OBJECT_FROM_HWND创建的CWND对象
		// dwParam1为欲删除的CWnd对象指针
		NAS_DETACH_CWND_OBJECT = 1009,
		// 取消通过NRS_CREATE_CWND_OBJECT_FROM_HWND创建的CWND对象与其中HWND的绑定
		// dwParam1为CWnd对象指针
		// 成功返回HWND,失败返回0
		NAS_GET_HWND_OF_CWND_OBJECT = 1010,
		// 获取通过NRS_CREATE_CWND_OBJECT_FROM_HWND创建的CWND对象中的HWND
		// dwParam1为CWnd对象指针
		// 成功返回HWND,失败返回0
		NAS_ATTACH_CWND_OBJECT = 1011,
		// 将指定HWND与通过NRS_CREATE_CWND_OBJECT_FROM_HWND创建的CWND对象绑定起来
		// dwParam1为HWND
		// dwParam2为CWnd对象指针
		// 成功返回1,失败返回0
		NAS_IS_EWIN = 1014,
		// 如果指定窗口为易语言窗口或易语言组件，返回真，否则返回假。
		// dwParam1为欲测试的HWND.

		// NRS_ 宏为仅能被易运行环境处理的通知。
		NRS_UNIT_DESTROIED = 2000,
		// 通知系统指定的组件已经被销毁。
		// dwParam1为dwFormID
		// dwParam2为dwUnitID
		NRS_CONVERT_NUM_TO_INT = 2001,
		// 转换其它数值格式到整数。
		// dwParam1为 PMDATA_INF 指针，其 m_dtDataType 必须为数值型。
		// 返回转换后的整数值。
		NRS_GET_CMD_LINE_STR = 2002,
		// 取当前命令行文本
		// 返回命令行文本指针，有可能为空串。
		NRS_GET_EXE_PATH_STR = 2003,
		// 取当前执行文件所处目录名称
		// 返回当前执行文件所处目录文本指针。
		NRS_GET_EXE_NAME = 2004,
		// 取当前执行文件名称
		// 返回当前执行文件名称文本指针。
		NRS_GET_UNIT_PTR = 2006,
		// 取组件对象指针
		// dwParam1为WinForm的ID
		// dwParam2为WinUnit的ID
		// 成功返回有效的组件对象CWnd*指针，失败返回0。
		NRS_GET_AND_CHECK_UNIT_PTR = 2007,
		// 取组件对象指针
		// dwParam1为WinForm的ID
		// dwParam2为WinUnit的ID
		// 成功返回有效的组件对象CWnd*指针，失败报告运行时错误并退出程序。
		NRS_EVENT_NOTIFY = 2008,
		// 以第一类方式通知系统产生了事件。
		// dwParam1为PEVENT_NOTIFY指针。
		//   如果返回 0 ，表示此事件已被系统抛弃，否则表示系统已经成功传递此事件到用户
		// 事件处理子程序。
		NRS_DO_EVENTS = 2018,
		// 通知系统发送所有待处理事件。
		NRS_GET_UNIT_DATA_TYPE = 2022,
		// dwParam1为WinForm的ID
		// dwParam2为WinUnit的ID
		// 成功返回有效的 DATA_TYPE ，失败返回 0 。
		NRS_FREE_ARY = 2023,
		// 释放指定数组数据。
		// dwParam1为该数据的DATA_TYPE，只能为系统数据类型。
		// dwParam2为指向该数组数据的指针。
		NRS_MALLOC = 2024,
		// 分配指定空间的内存，所有与易程序交互的内存都必须使用本通知分配。
		//   dwParam1为欲需求内存字节数。
		//   dwParam2如为0，则如果分配失败就自动报告运行时错并退出程序。
		// 如不为0，则如果分配失败就返回NULL。
		//   返回所分配内存的首地址。
		NRS_MFREE = 2025,
		// 释放已分配的指定内存。
		// dwParam1为欲释放内存的首地址。
		NRS_MREALLOC = 2026,
		// 重新分配内存。
		//   dwParam1为欲重新分配内存尺寸的首地址。
		//   dwParam2为欲重新分配的内存字节数。
		// 返回所重新分配内存的首地址，失败自动报告运行时错并退出程序。
		NRS_RUNTIME_ERR = 2027,
		// 通知系统已经产生运行时错误。
		// dwParam1为char*指针，说明错误文本。
		NRS_EXIT_PROGRAM = 2028,
		// 通知系统退出用户程序。
		// dwParam1为退出代码，该代码将被返回到操作系统。
		NRS_GET_PRG_TYPE = 2030,
		// 返回当前用户程序的类型，为PT_DEBUG_RUN_VER（调试版）或PT_RELEASE_RUN_VER（发布版）。
		NRS_EVENT_NOTIFY2 = 2031,
		// 以第二类方式通知系统产生了事件。
		// dwParam1为PEVENT_NOTIFY2指针。
		//   如果返回 0 ，表示此事件已被系统抛弃，否则表示系统已经成功传递此事件到用户
		// 事件处理子程序。
		NRS_GET_WINFORM_COUNT = 2032,
		// 返回当前程序的窗体数目。
		NRS_GET_WINFORM_HWND = 2033,
		// 返回指定窗体的窗口句柄，如果该窗体尚未被载入，返回NULL。
		// dwParam1为窗体索引。
		NRS_GET_BITMAP_DATA = 2034,
		// 返回指定HBITMAP的图片数据，成功返回包含BMP图片数据的HGLOBAL句柄，失败返回NULL。
		// dwParam1为欲获取其图片数据的HBITMAP。
		NRS_FREE_COMOBJECT = 2035,
		// 通知系统释放指定的DTP_COM_OBJECT类型COM对象。
		// dwParam1为该COM对象的地址指针。
		NRS_CHK_TAB_VISIBLE = 2039,
		// 当选择夹子夹被切换后, 使用本消息通知易系统


		/*///////////////////////////////////////////////////////////////////*/
		// 系统可以通知库的码值：

		//   告知库通知系统用的函数指针，在装载支持库前通知，可能有多次，
		// 后通知的值应该覆盖前面所通知的值），忽略返回值。
		//   库可将此函数指针记录下来以便在需要时使用它通知信息到系统。
		//   dwParam1: (PFN_NOTIFY_SYS)
		NL_SYS_NOTIFY_FUNCTION = 1,
		NL_FREE_LIB_DATA = 6,// 通知支持库释放资源准备退出及释放指定的附加数据。
		NL_GET_CMD_FUNC_NAMES = 14,// 返回所有命令实现函数的的函数名称数组(char*[]), 支持静态编译的动态库必须处理
		NL_GET_NOTIFY_LIB_FUNC_NAME = 15,// 返回处理系统通知的函数名称(PFN_NOTIFY_LIB函数名称), 支持静态编译的动态库必须处理
		NL_GET_DEPENDENT_LIBS = 16,
	};



	//C原型接口函数
	/* 所有命令和方法实现函数的原型。
	   1、必须是 CDECL 调用方式；
	   2、pRetData 用作返回数据；
	   3、!!!如果指定库命令返回数据类型不为 _SDT_ALL ，可以
		  不填充 pRetData->m_dtDataType，如果为 _SDT_ALL ，则必须填写；
	   4、pArgInf 提供参数数据本身，所指向的 MDATA_INF 描述每个输入参数，数目等同于 nArgCount 。*/
	using PFN_EXECUTE_CMD = void(__cdecl*) (PMDATA_INF pRetData, size_t nArgCount, PMDATA_INF pArgInf);

	// 此函数用作系统通知库有关事件。
	using PFN_NOTIFY_LIB = ptrdiff_t(__stdcall*) (EMSG nMsg, size_t dwParam1, size_t dwParam2);

	// 此函数用作库通知系统有关事件。
	using PFN_NOTIFY_SYS = ptrdiff_t(__stdcall*) (EMSG nMsg, size_t dwParam1, size_t dwParam2);




	enum class LIB_MARK :unsigned long
	{
		LBS_FUNC_NO_RUN_CODE = (1 << 2),    // 本库仅为声明库,没有对应功能的支持代码,因此不能运行。
		LBS_NO_EDIT_INFO = (1 << 3),  // 本库内无供编辑用的信息(编辑信息主要为：各种名称、解释字符串等)。
		LBS_IS_DB_LIB = (1 << 5),  // 本库是否为数据库操作支持库。
		LBS_LIB_INFO2 = (1 << 7),  // 本信息结构实际上是否为LIB_INFO2
		LBS_IDE_PLUGIN = (1 << 8),  // 本支持库是否为易语言IDE插件. 注意: 只有设置了此标志的支持库,才会接收到来自易语言IDE的NL_IDE_READY和NL_RIGHT_POPUP_MENU_SHOW等相关通知.
		__OS_WIN = 0x80000000,
		__OS_LINUX = 0x40000000,
		__OS_UNIX = 0x20000000,
		OS_ALL = (__OS_WIN | __OS_LINUX | __OS_UNIX)
	};

	//易语言原始库字节集传递结构
	struct ebin
	{
		using size_type = size_t;
		static constexpr auto nop{ 0 };
		std::uint32_t m_check = 1;
		size_type m_size{ nop };
		unsigned char m_pdata[nop];
	};
	//易语言原始库数组传递结构
	template <typename T>
	struct  earry
	{
		std::uint32_t m_check = 1;
		size_t m_size;
		T m_pdata[0];
		/*span并不拥有指针，所以拷贝span实际上只是添加了一个引用，所以基本不会损失效率，但是对于迭代器和可操作性会大幅增加*/
		std::span<T> get_ary_elementInf() {
			return std::span <T>(m_pdata, m_pdata + m_size);
		}
	};
	struct LIB_INFO
	{
		unsigned long  m_dwLibFormatVer;        // 库格式号,应该等于LIB_FORMAT_VER(20000101)。主要用途如下：譬如 krnln.fnX 库,在做其它与此同名但功能完全不一致的库时,应当改变此格式号,以防止错误装载。
		const char* m_szGuid;                // 对应于本库的唯一GUID串,不能为NULL或空,库的所有版本此串都应相同。如果为ActiveX控件,此串记录其CLSID。
		int    m_nMajorVersion;         // 本库的主版本号,必须大于0。
		int    m_nMinorVersion;         // 本库的次版本号。
		int    m_nBuildNumber;          // 构建版本号,无需对此版本号作任何处理。本版本号仅用作区分相同正式版本号的系统软件(譬如仅仅修改了几个 BUG,不值得升级正式版本的系统软件)。任何公布过给用户使用的版本其构建版本号都应该不一样。赋值时应该顺序递增。
		int    m_nRqSysMajorVer;        // 所需要的易语言系统的主版本号。
		int    m_nRqSysMinorVer;        // 所需要的易语言系统的次版本号。
		int    m_nRqSysKrnlLibMajorVer; // 所需要的系统核心支持库的主版本号。
		int    m_nRqSysKrnlLibMinorVer; // 所需要的系统核心支持库的次版本号。

		const char* m_szName;    // 库名,不能为NULL或空。
		int    m_nLanguage;    // 库所支持的语言。
		const char* m_szExplain;    // 库详细解释


		unsigned long  m_dwState;           // _LIB_OS() | LBS_ 开头常量, 如果是插件,必须包含 LBS_IDE_PLUGIN

		const char* m_szAuthor;          // 作者
		const char* m_szZipCode;         // 邮编
		const char* m_szAddress;         // 地址
		const char* m_szPhoto;           // 电话
		const char* m_szFax;             // 传真
		const char* m_szEmail;           // 邮箱
		const char* m_szHomePage;        // 主页
		const char* m_szOther;           // 其他信息

		//////////////////
		int                 m_nDataTypeCount;   // 本库中自定义数据类型的数目。
		PLIB_DATA_TYPE_INFO m_pDataType;        // 本库中所有的自定义数据类型。可以参考使用系统核心支持库中的自定义数据类型,系统核心支持库在程序的库登记数组中的索引值加1后的值为1。
		int                 m_nCategoryCount;   // 全局命令类别数目,可为0。
		const char* m_szzCategory;      // 全局命令类别说明表


		int                 m_nCmdCount;        // 本库中提供的所有命令(全局命令及对象命令)的数目(可为0)。
		PCMD_INFO           m_pBeginCmdInfo;    // 可为NULL
		PFN_EXECUTE_CMD* m_pCmdsFunc;        // 指向每个命令的实现代码首地址,可为NULL


		// 有关AddIn功能的说明,两个字符串说明一个功能。第一个为功能名称(仅限一行20字符,如果希望自行初始位置而不被自动加入到工具菜单,
		// 则名称应该以@开始,此时会接收到值为 -(nAddInFnIndex + 1) 的调用通知),
		// 第二个为功能详细介绍(仅限一行60字符),最后由两个空串结束。
		void* m_pfnRunAddInFn;    // 不可用
		const char* m_szzAddInFnInfo;   // 功能详细介绍（仅限一行60字符），最后由两个空串结束。

		PFN_NOTIFY_LIB      m_pfnNotify;        // 不能为NULL,和易语言通讯的函数

		// 超级模板暂时保留不用。
		// 有关SuperTemplate的说明,两个字符串说明一个SuperTemplate。
		// 第一个为SuperTemplate名称(仅限一行30字符),第二个为详细介绍(不限),最后由两个空串结束。
		void* m_pfnSuperTemplate;   // 可为NULL
		const char* m_szzSuperTemplateInfo;              // 详细介绍(不限长度), 最后由两个空串结束。

		int                 m_nLibConstCount;            // 本库预先定义的所有常量数量。
		PLIB_CONST_INFO     m_pLibConst;    // 本库预先定义的所有常量。

		const char* m_szzDependFiles;        // 可为NULL, 本库正常运行所需要依赖的其他支持文件

	};
	using PLIB_INFO = LIB_INFO*;
	using PFN_GET_LIB_INFO = PLIB_INFO(__stdcall*) ();    // 取支持库信息函数











	enum class PROPERTY_TYPE :short
	{

		UD_PICK_SPEC_INT = 1000,// 数据为INT值,用户只能选择,不能编辑。
		UD_INT = 1001,// 数据为INT值
		UD_DOUBLE = 1002,// 数据为DOUBLE值
		UD_BOOL = 1003,// 数据为BOOL值
		UD_DATE_TIME = 1004,// 数据为DATE值
		UD_TEXT = 1005,// 数据为字符串
		UD_PICK_INT = 1006,// 数据为INT值,用户只能选择,不能编辑。
		UD_PICK_TEXT = 1007,// 数据为字符串,用户只能选择,不能编辑。
		UD_EDIT_PICK_TEXT = 1008,// 数据为字符串,用户可以编辑。
		UD_PIC = 1009,// 为图片文件内容
		UD_ICON = 1010,// 为图标文件内容
		UD_CURSOR = 1011,// 第一个INT记录鼠标指针类型,具体值见LoadCursor函数。如为-1,则为自定义鼠标指针,此时后跟相应长度的鼠标指针文件内容。
		UD_MUSIC = 1012,// 为声音文件内容
		UD_FONT = 1013,// 为一个LOGFONT数据结构,不能再改。
		UD_COLOR = 1014,// 数据为COLORREF值。
		UD_COLOR_TRANS = 1015,// 数据为COLORREF值,允许透明颜色(用CLR_DEFAULT代表)。
		UD_FILE_NAME = 1016,// 数据为文件名字符串。此时m_szzPickStr中的数据为: 对话框标题\0 + 文件过滤器串\0 + 默认后缀\0 + "1"(取保存文件名)或"0"(取读入文件名)\0
		UD_COLOR_BACK = 1017,// 数据为COLORREF值,允许系统默认背景颜色(用CLR_DEFAULT代表)。
		UD_IMAGE_LIST = 1023,// 图片组,数据结构为：DWORD: 标志数据：为 IMAGE_LIST_DATA_MARK, COLORREF: 透明颜色(可以为CLR_DEFAULT), 后面为图片组数据.用CImageList::Read和CImageList::Write读写。
		IMAGE_LIST_DATA_MARK = static_cast<short>((short)UserType('IM', 'LT')),
		UD_CUSTOMIZE = 1024,

		UD_BEGIN = UD_PICK_SPEC_INT,
		UD_END = UD_CUSTOMIZE,
	};
	//
	//
	//#include<string>
	//typedef struct
	//{
	//	const char* m_szName;    // 属性名称,注意为利于在属性表中同时设置多对象的属性,属性名称必须高度一致。
	//	const char* m_szEgName;    // 属性英文名
	//	const char* m_szExplain;    // 属性解释。
	//
	//
	//
	//	PROPERTY_TYPE m_shtType;    // 属性的数据类型。
	//
	//#define UW_HAS_INDENT           (1 << 0)    // 在属性表中显示时向外缩进一段,一般用于子属性。
	//#define UW_GROUP_LINE           (1 << 1)    // 在属性表中本属性下显示分组底封线。
	//#define UW_ONLY_READ            (1 << 2)    // 只读属性,设计时不可用,运行时不能写。
	//#define UW_CANNOT_INIT          (1 << 3)    // 设计时不可用,但运行时可以正常读写。与上标志互斥。
	//#define UW_IS_HIDED             (1 << 4)    // 3.2 新增 隐藏但可用。
	////!!! 注意高位包含 __OS_xxxx 宏用于指定本属性所支持的操作系统。
	//#define _PROP_OS(os)            ((os) >> 16)// 用作转换os类型以便加入到m_wState。
	//#define _TEST_PROP_OS(m_wState,os)    ((_PROP_OS (os) & m_wState) != 0) // 用作测试指定属性是否支持指定操作系统。
	//	WORD m_wState;        // UW_开头常量
	//
	//	// 顺序记录所有的备选文本(除开UD_FILE_NAME),以一个空串结束。
	//	// 当m_nType为UP_PICK_INT、UP_PICK_TEXT、UD_EDIT_PICK_TEXT、UD_FILE_NAME时不为NULL。
	//	// 当m_nType为UD_PICK_SPEC_INT时,每一项备选文本的格式为 数值文本 + "\0" + 说明文本 + "\0" 。
	//	LPCSTR m_szzPickStr;
	//} UNIT_PROPERTY, * PUNIT_PROPERTY;
	//
	//
	//#include<map>
	//std::string getUW(WORD value) {
	//	std::string result;
	//	value &= 0xFF;
	//	if (value & UW_HAS_INDENT)
	//		result += "UW_HAS_INDENT | ";
	//	if (value & UW_IS_HIDED)
	//		result += "UW_IS_HIDED | ";
	//	if (value & UW_CANNOT_INIT)
	//		result += "UW_CANNOT_INIT | ";
	//
	//	if (result.empty())
	//	{
	//		return "_PROP_OS (OS_ALL)";
	//	}
	//	// Remove the trailing " | " from the result
	//
	//	result = result.substr(0, result.length() - 3);
	//	result += "|_PROP_OS (OS_ALL)";
	//	return result;
	//}
	//
	//std::string get_defint(PROPERTY_TYPE value) {
	//	std::map<PROPERTY_TYPE, std::string> definition_map = {
	//		{PROPERTY_TYPE::UD_PICK_SPEC_INT, "UD_PICK_SPEC_INT"},
	//		{PROPERTY_TYPE::UD_INT, "UD_INT"},
	//		{PROPERTY_TYPE::UD_DOUBLE, "UD_DOUBLE"},
	//		{PROPERTY_TYPE::UD_BOOL, "UD_DOUBLE"},
	//		{PROPERTY_TYPE::UD_DATE_TIME, "UD_DATE_TIME"},
	//		{PROPERTY_TYPE::UD_TEXT, "UD_TEXT"},
	//		{PROPERTY_TYPE::UD_PICK_INT, "UD_PICK_INT"},
	//		{PROPERTY_TYPE::UD_PICK_TEXT, "UD_PICK_TEXT"},
	//		{PROPERTY_TYPE::UD_EDIT_PICK_TEXT, "UD_EDIT_PICK_TEXT"},
	//		{PROPERTY_TYPE::UD_PIC, "UD_PIC"},
	//		{PROPERTY_TYPE::UD_ICON, "UD_ICON"},
	//		{PROPERTY_TYPE::UD_CURSOR, "UD_CURSOR"},
	//		{PROPERTY_TYPE::UD_MUSIC, "UD_MUSIC"},
	//		{PROPERTY_TYPE::UD_FONT, "UD_FONT"},
	//		{PROPERTY_TYPE::UD_COLOR, "UD_COLOR"},
	//		{PROPERTY_TYPE::UD_COLOR_TRANS, "UD_COLOR_TRANS"},
	//		{PROPERTY_TYPE::UD_FILE_NAME, "UD_FILE_NAME"},
	//		{PROPERTY_TYPE::UD_COLOR_BACK, "UD_COLOR_BACK"},
	//		{PROPERTY_TYPE::UD_IMAGE_LIST, "UD_IMAGE_LIST"},
	//		{PROPERTY_TYPE::UD_CUSTOMIZE, "UD_CUSTOMIZE"}
	//	};
	//
	//	auto it = definition_map.find(value);
	//	if (it != definition_map.end()) {
	//		return it->second;
	//	}
	//	else {
	//		return "未知定义";
	//	}
	//}
	//
	//
	//std::string get_old_string(const char* text) {
	//	if (!text || *text == '\0')
	//	{
	//		return "NULL";
	//	}
	//	std::vector<std::string> options;
	//
	//	const char* start = text;
	//	const char* end = text;
	//
	//	while (true) {
	//		if (*end == '\0') {
	//			options.emplace_back(start, end);
	//			start = ++end;
	//			if (*end == '\0') {
	//				break;
	//			}
	//		}
	//		else {
	//			++end;
	//		}
	//	}
	//
	//	std::string ret;
	//	for (const std::string& option : options) {
	//		ret += "\"" + option + "\\0\"";
	//	}
	//	ret += "\"\\0\"";
	//	return ret;
	//}
	//typedef void(__stdcall* PFN_INTERFACE) ();
	//typedef PFN_INTERFACE(__stdcall* PFN_GET_INTERFACE) (MFC_WND_INTERFACE nInterfaceNO);


	//class TypeBase
	//{
	//public:
	//
	//
	//private:
	//
	//};
	//
	//class TypeWnd :public TypeBase
	//{
	//public:
	//	TypeWnd();
	//	~TypeWnd();
	//
	//private:
	//	LIB_DATA_TYPE_INFO  m_data_info = { 0 };
	//};
	//
	//TypeWnd::TypeWnd()
	//{
	//}
	//
	//TypeWnd::~TypeWnd()
	//{
	//}
	//
	///*创建函数原型*/
	//using PFN_CREATE_UNIT = DWORD_PTR(__stdcall*) (LPBYTE pAllData, INT nAllDataSize,
	//	DWORD dwStyle, HWND hParentWnd, UINT uID, HMENU hMenu, INT x, INT y, INT cx, INT cy,
	//	DWORD dwWinFormID, DWORD dwUnitID,            // 用作通知到系统
	//	HWND hDesignWnd, BOOL blInDesignMode);

};
#endif // EPL_DEFINE_H