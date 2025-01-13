#ifndef EPL_RES_H
#define EPL_RES_H
#include"tace.hpp"
#include"utils.hpp"


/*从旧版项目迁移,因为没有问题,怕出现兼容问题而增加命名空间*/
namespace eplconstold {
	struct EplBase
	{
		union
		{
			/*数据来源*/
			struct DataFrom
			{
				::std::uint16_t index;
				::std::uint16_t libId;
			}type;

			struct {
				::std::uint16_t  ID;
				::std::uint8_t type;
				::std::uint8_t type2;
			}unknow;
			int id;
		};
	};
	enum class epp_id_type : ::std::uint32_t
	{
		_const,/*常量*/
		_res,/*资源*/
	};
	struct epp_struct_base
	{
		::std::uint32_t  id{};
		epp_id_type id_type{};
	};
	/*值常量类型*/
	enum class ConstValType : ::std::uint8_t
	{
		Void = 0x16,
		Num = 0x17,/*双精度*/
		Bool = 0x18,/*短整数*/
		Date = 0x19,/*日期*/
		Str = 0x1A/*文本型*/
	};
	/*常量类型*/
	enum class ConstType : ::std::uint8_t
	{
		ConstVal = 0x18,/*值常量*/
		Pic = 0x28,/*图片*/
		Music = 0x38/*音乐*/
	};
	/*常量信息*/
	struct EplConst :public EplBase
	{

		///*此为读易语言数据*/
		//::std::uint32_t offest;/*起始偏移*/
		//::std::uint32_t size;/*数据长度*/
		//::std::uint16_t type;/*内部类型*/
		bool Unexamined{ true }; /*未审查*/;
		bool Public{ true };/*公开*/
		bool  Hidden{ true };/*隐藏*/
		bool LongText{ true };/*长文本*/
		/*真实数据*/
		::std::string name;/*常量名*/
		::std::string remak;/*备注*/
		/*以下为数据*/
		::std::vector<::std::uint8_t> data;/*字节集*/
		::std::string str;/*文本型*/
		union
		{
			double numVal{};/*数值型*/
			bool bVal;/*逻辑型*/
		};


	};

	struct Control :public EplBase
	{
		::std::string Name;/*名称*/
		::std::string Remark;/*备注*/
		::std::uint32_t Left{ 0 };/*左边*/
		::std::uint32_t Top{ 0 };/*顶边*/
		::std::uint32_t Width{ 0 };/*宽度*/
		::std::uint32_t Height{ 0 };/*高度*/
		::std::vector<::std::uint8_t> Cursor;/*光标*/
		::std::string Mark{ 0 };/*标记*/
		bool Visible{ false };/*可视*/
		bool Display{ false };/*禁止*/
		bool TabStop{ true };/*不存在焦点权重*/
		bool Locked{ true };/*组件锁定,编辑模式似乎无意义*/
		::std::uint32_t TabIndex{};/*焦点权重*/
		//::std::vector<EKeyValPair> Events;/*事件*/
		::std::vector<::std::uint8_t> OtherData;/*剩余库定义数据*/
		::std::uint32_t CWndAddress{};/*编辑模式下无意义的CWnd组件指针*/
		::std::uint32_t UnknownBeforeParent{};

		::std::uint32_t Parent{};/*父组件ID*/
		::std::vector<::std::uint32_t> Children{};/*子组件数组*/
	};

	/*窗口页面*/
	class EplWindowRes :public EplBase
	{
	public:
		::std::uint32_t type{ 0 };/**/
		::std::uint32_t AssemblyID{ 0 };/*对应程序集ID*/
		::std::string name;/*名称*/
		::std::string remark;/*备注*/
		::std::vector<Control> control;/*包含一个框架和一堆组件*/
	public:
		::std::uint32_t unkonw33{};
		EplWindowRes() = default;
		void init(eprogramutils::MemArchiveStream& _codes) {
			_codes >> this->type >> AssemblyID;
			/*窗口名称*/
			this->name = _codes.get_mfc_string();
			/*窗口备注*/
			this->remark = _codes.get_mfc_string();
			::std::uint32_t
				count{ 0 },/*必定大于0,因为其包含窗口本身*/
				length{ 0 };/*数据流长度*/
			_codes >> count >> length;	/*组件数量和数据长度*/
			auto tempwindow = _codes.memstream(length);/*获取本个窗口的数据流*/;


			if (count <= 0 || _codes.size() < 0)
				return;
			control.resize(count);
			get_elements(tempwindow, count);

		}
		/*获取组件元素*/
		void get_elements(eprogramutils::MemArchiveStream& _codes, const ::std::uint32_t count) {

			for (auto& c : control) {

				_codes >> this->unknow;/*长度*/

			}

			auto GetContolPropertyDataStream = [](eprogramutils::MemArchiveStream& _codes, const ::std::uint32_t count) {
				::std::vector<eprogramutils::MemArchiveStream> os(count);
				::std::vector<::std::uint32_t> sizes(count);
				/*获取组件属性偏移*/
				for (auto& size : sizes)
					_codes >> size;
				/*获取组件属性流长度*/
				for (size_t i = 0; i < count; i++)
				{
					if (i == sizes.size() - 1)/*为最后一个*/
					{
						sizes[i] = static_cast<std::uint32_t>(_codes.size() - sizes[i]);
						continue;
					}
					sizes[i] = sizes[i + 1] - sizes[i];
				}
				/*获取组件数据流*/
				for (size_t i = 0; i < count; i++) {
					os[i] = _codes.memstream(sizes[i]);
				};
				return os;
				};
			::std::vector<eprogramutils::MemArchiveStream> os = GetContolPropertyDataStream(_codes, count);/*组件属性对应的数据流,尾部指针并不一定正确,但一定会访问安全*/
			for (size_t i = 0; auto & sre :control)
			{
				auto debug__ = [](::std::uint32_t unkonw_int) {
					if (unkonw_int != 0)
					{
#ifdef _DEBUG
						__debugbreak();
#endif // DEBUG
						debug_put(unkonw_int);
					}
					};

				auto& contolos = os[i];
				::std::uint32_t
					lenth,/*数据长度*/
					unkonw_int;/*未知数据占用*/

				contolos >> lenth >> sre.type;

				auto end = contolos.now_ptr() + lenth;
				contolos >> unkonw_int;
				debug__(unkonw_int);
				contolos >> unkonw_int;
				debug__(unkonw_int);
				contolos >> unkonw_int;
				debug__(unkonw_int);
				contolos >> unkonw_int;
				debug__(unkonw_int);
				contolos >> unkonw_int;
				debug__(unkonw_int);

				/*对于string,仅用于获取标准字符串,即'\0'结尾,如果想获取易特有的定长字符串则需要使用get_mfc_string()*/
				contolos
					>> sre.Name
					>> sre.Remark
					>> sre.CWndAddress
					>> sre.Left
					>> sre.Top
					>> sre.Width
					>> sre.Height
					>> sre.UnknownBeforeParent
					>> sre.Parent
					>> sre.Children
					>> sre.Cursor
					>> sre.Mark;


				::std::uint32_t
					Check,/*必定为0*/
					Flag;

				contolos
					>> Check
					>> Flag;


				{
					sre.Visible = Flag & 0x01;
					sre.Display = Flag & 0x02;
					sre.TabStop = Flag & 0x4;
					sre.Locked = Flag & 0x10;
				}


				contolos >> sre.TabIndex;

				::std::uint32_t size;/*事件数量*/
				contolos >> size;
				debug__(size);
				contolos >> size;
				debug__(size);
				contolos >> unkonw_int;
				debug__(unkonw_int);
				contolos >> unkonw_int;
				debug__(unkonw_int);
				contolos >> unkonw_int;
				debug__(unkonw_int);
				if (end > contolos.now_ptr())
					sre.OtherData = ::std::vector<unsigned char>(contolos.now_ptr(), end);

				i++;
			}


			return;

		}
	};

	enum class epp_const_type
	{
		cvoid,/*空值*/
		cdata,/*日期*/
		cnum,/*数值*/
		cstr,/*文本*/
		cbool,/*逻辑*/
	};
	enum class epp_res_type
	{
		pic,/*图片*/
		music,/*音乐*/
	};
	/*epp数据*/
	struct epp_res : public epp_struct_base
	{
		bool is_public{ true };/*公开*/

		::std::string name;/*名称*/
		::std::string remark;/*备注*/

		epp_res_type type{};
		::std::vector<unsigned char> data;

	};


	/*epp常量*/
	struct epp_const : public epp_struct_base
	{
		bool is_public{ true };/*公开*/
		epp_const_type type{ epp_const_type::cvoid };
		::std::string  name;
		::std::string remark;
		double  numval{ 0 };
		bool bval{ false };
		::std::string str;
		std::string getVal() const{
			::std::stringstream ss;
			switch (type)
			{
			case epp_const_type::cvoid:
				ss << "nullptr";
				break;
			case epp_const_type::cdata:
				ss << numval;
				break;
			case epp_const_type::cnum:
				ss << numval;
				break;
			case epp_const_type::cstr: {
				ss << str;
			}break;
			case epp_const_type::cbool:
				ss << ::std::boolalpha << bval;
				break;
			default:
				break;
			}
			return ss.str();
		}

		auto to_string() const {
			::std::stringstream ss;
			if (type == epp_const_type::cvoid)
			{
				ss << "constexpr auto " << name << " =";
			}
			else if (type == epp_const_type::cstr)
			{
				ss << "constexpr char " << name << "[] =";
			}
			else
			{
				ss << "constexpr auto " << name << " =";
			}

			switch (type)
			{
			case epp_const_type::cvoid:
				ss << "nullptr";
				break;
			case epp_const_type::cdata:
				ss << numval;
				break;
			case epp_const_type::cnum:
				ss << numval;
				break;
			case epp_const_type::cstr: {
				::std::string now = str;
				auto cf = [&]() {
					auto tempstr = now.substr(0, 5000);
					ss << "R\"myeppstr(" << tempstr << ")myeppstr\"" << "\n";
					now = now.substr(5000, now.size() - 5000);

					};

				while (now.size() >= 5000)
				{
					cf();
				}

				ss << "R\"myeppstr(" << now << ")myeppstr\"";
			}break;
			case epp_const_type::cbool:
				ss << ::std::boolalpha << bval;
				break;
			default:
				break;
			}
			ss << ";";
			return ss.str();


		}

	};

	/*窗口资源常量等静态资源段*/
	class EplResourceSection :public eprogramutils::EplStreamObj
	{
	public:
		auto GetAllWindow() -> const ::std::vector<EplWindowRes>& {
			return window;
		}
		auto GetAllConst() -> const ::std::vector<EplConst>& {
			return const_data;
		}

	public:
		auto to_string() {

			::std::stringstream ss, eppconst, hid;

			if (!g_epp_const_data.empty())
			{
				bool is_hiden{ false };
				eppconst << "namespace eppconst{\n";

				hid << "namespace eppconst::hide{\n";
				for (const auto c : g_epp_const_data) {
					if (!c.is_public)
					{
						is_hiden = true;
						hid << " " << c.to_string() << "\n";
					}
					else
					{
						eppconst << " " << c.to_string() << "\n";
					}

				}
				hid << "}\n";

				eppconst << "}\n";

				if (is_hiden)
				{
					eppconst << hid.str();
				}
			}
			ss << eppconst.str();
			return ss.str();

		};
		EplResourceSection() = default;
		virtual void Serialize(eprogramutils::MemArchiveStream& s) override {
			if (s.size() < 0)
				return;
			InitAllWindows(s);
			init_const(s);
		}
	private:
		void InitAllWindows(eprogramutils::MemArchiveStream& _codes) {
			::std::uint32_t length{ 0 };
			_codes >> length;
			auto count = length / (sizeof(::std::uint32_t) + sizeof(::std::uint32_t));
			window.resize(count);

			/*代码类型*/
			for (auto& form : window) {
				_codes >> form.unknow;/*代码类型-窗口类*/
			}
			/*一个未知4字节数据,似乎是抽象语法树的ID*/
			for (auto& form : window) {
				_codes >> form.unkonw33;
			}
			/*分析*/
			for (auto& form : window) {
				form.init(_codes);
			}

		}
		/*获取常量资源*/
		void init_const(eprogramutils::MemArchiveStream& _codes) {
			::std::uint32_t count, datalenth;
			_codes >> count >> datalenth;
			if (count < 0 || _codes.size() - _codes.get_offset() < datalenth)
			{
				return;
			}
			::std::vector<EplConst> arr(count);
			for (auto& pc : arr)
			{
				_codes >> pc.unknow;

			}
			for (auto& pc : arr)
				_codes.skip<::std::uint32_t>();


			auto begin = _codes.now_ptr();
			for (auto& pc : arr)
			{
				
				/*处理epp数据*/
				epp_const temp_epp_const;
				temp_epp_const.id_type = epp_id_type::_const;
				temp_epp_const.id = pc.id;
				epp_res temp_epp_data;
				temp_epp_data.id = pc.id;
				temp_epp_data.id_type = epp_id_type::_res;
				/*截取指定长度流*/
				auto temp = _codes.memstream(_codes.read<::std::uint32_t>());
				/*获取属性*/
				::std::uint16_t Flags;
				temp >> Flags >> pc.name >> pc.remak;
				{
					pc.Unexamined = Flags & 0x1;
					pc.Hidden = Flags & 0x4;
					pc.Public = Flags & 0x2;
					pc.LongText = Flags & 0x10;
				}
				temp_epp_data.is_public = temp_epp_const.is_public = pc.Public;


				switch ((ConstType)pc.unknow.type2)
				{
				case ConstType::ConstVal: {
					ConstValType type;
					temp >> type;
					switch (type)
					{
					case ConstValType::Date: {
						temp >> pc.numVal;
						temp_epp_const.type = epp_const_type::cdata;
						temp_epp_const.numval = pc.numVal;
					}break;
					case ConstValType::Num:
					{
						temp >> pc.numVal;
						temp_epp_const.type = epp_const_type::cnum;
						temp_epp_const.numval = pc.numVal;
					}break;
					case ConstValType::Bool:
					{
						::std::uint32_t pBool;
						temp >> pBool;
						pc.bVal = pBool == 1 ? true : false;

						temp_epp_const.type = epp_const_type::cbool;
						temp_epp_const.bval = pc.bVal;
					}break;
					case ConstValType::Str: {
						pc.str = temp.get_mfc_string();
						temp_epp_const.type = epp_const_type::cstr;
						temp_epp_const.str = pc.str;

					} break;
					default:
						break;
					}
					temp_epp_const.name = pc.name;
					temp_epp_const.remark = pc.remak;
				}break;
				default:
				{
					if (pc.unknow.type2 == 0x28)/*图片*/
					{
						temp_epp_data.type = epp_res_type::pic;
					}
					else
					{
						temp_epp_data.type = epp_res_type::music;
					}

					temp >> pc.data;
					temp_epp_data.data = pc.data;
					temp_epp_data.name = pc.name;
					temp_epp_data.remark = pc.remak;
				}break;
				}
				if (!pc.name.empty())
				{
					const_data.push_back(pc);
					if (!temp_epp_data.name.empty())
					{

						no_same_name_push_back(g_epp_res_data, temp_epp_data);
					}

					if (!temp_epp_const.name.empty())
					{
						no_same_name_push_back(g_epp_const_data, temp_epp_const);
					}
				}
			}

			return;
		}
	public:
		/*窗口数据*/
		::std::vector<EplWindowRes> window;
		/*常量数据*/
		::std::vector<EplConst> const_data;


		::std::vector<epp_const> g_epp_const_data;
		::std::vector<epp_res> g_epp_res_data;
	};
}
#endif