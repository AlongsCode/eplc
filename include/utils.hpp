#ifndef ALMEMSTREAM_HPP
#define ALMEMSTREAM_HPP
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <cstring>
#include <list>
#include<sstream>


class EplSystemId {
public:
    static constexpr int Id_NaV = 0x0500FFFE;

    static constexpr int Type_Method = 0x04000000;
    static constexpr int Type_Global = 0x05000000;
    static constexpr int Type_StaticClass = 0x09000000;
    static constexpr int Type_Dll = 0x0A000000;
    static constexpr int Type_ClassMember = 0x15000000;
    static constexpr int Type_Constant = 0x18000000;
    static constexpr int Type_FormClass = 0x19000000;
    static constexpr int Type_Local = 0x25000000;
    static constexpr int Type_ImageResource = 0x28000000;
    static constexpr int Type_StructMember = 0x35000000;
    static constexpr int Type_SoundResource = 0x38000000;
    static constexpr int Type_Struct = 0x41000000;
    static constexpr int Type_DllParameter = 0x45000000;
    static constexpr int Type_Class = 0x49000000;
    static constexpr int Type_Form = 0x52000000;
    static constexpr int Type_FormSelf = 0x06000000;
    static constexpr int Type_FormControl = 0x16000000;
    static constexpr int Type_FormMenu = 0x26000000;

    static constexpr int Mask_Num = 0x00FFFFFF;
    static constexpr int Mask_Type = 0xFF000000;

    static constexpr int DataType_Void = 0x00000000;
    static constexpr int DataType_Any = 0x80000000;
    static constexpr int DataType_Byte = 0x80000101;
    static constexpr int DataType_Short = 0x80000201;
    static constexpr int DataType_Int = 0x80000301;
    static constexpr int DataType_Long = 0x80000401;
    static constexpr int DataType_Float = 0x80000501;
    static constexpr int DataType_Double = 0x80000601;
    static constexpr int DataType_Bool = 0x80000002;
    static constexpr int DataType_DateTime = 0x80000003;
    static constexpr int DataType_String = 0x80000004;
    static constexpr int DataType_Bin = 0x80000005;
    static constexpr int DataType_MethodPtr = 0x80000006;
    static constexpr int DataType_Lambda = 0x80000008;

    static int GetType(int id) { return id & Mask_Type; }

    static int MakeSureIsSpecifiedType(int id, const int* type, int count) {
        for (int i = 0; i < count; ++i) {
            if (GetType(id) == type[i]) {
                return id;
            }
        }
        throw std::exception("不是指定类型的Id");
    }

    /*获取库索引和自身索引*/

    inline static  auto getlibandindex(const std::uint32_t id) {
        struct ret
        {
            union
            {
                /*数据来源*/
                struct DataFrom
                {
                    std::uint16_t index;
                    std::uint16_t libId;
                }type;
                int m_id{};
            };
            ret() = default;
            ret(const std::uint32_t id) : m_id(id) {}
        };

        return ret{ id }.type;
    }

    static bool IsLibDataType(int id) { return (id & 0xFF000000) == 0 && id != DataType_Void; }

    static int MakeLibDataTypeId(short lib, short type) {
        return ((lib + 1) << 16) | (type + 1);
    }

    static void DecomposeLibDataTypeId(int id, short& lib, short& type) {
        if (!IsLibDataType(id)) throw std::exception("DecomposeLibDataTypeId只能处理库类型Id");
        lib = static_cast<short>(id >> 16);
        lib--;
        type = static_cast<short>(id);
        type--;
    }
};


namespace {

  

    /*32位魔术数*/
    using U32EPLMAGIC = std::uint32_t;
    /*64位魔术数*/
    using U64EPLMAGIC = std::int64_t;
    /*使用4位标识符构造魔术数*/
    constexpr auto __make_epl_u32magic(const char a, const char b, const char c, const char d) -> U32EPLMAGIC {
        return (d << 24) | (c << 16) | (b << 8) | a;
    }


    /*编译时数据魔术数为"中国吴涛易语言项目"的英文缩写*/
    constexpr auto  __eprogramheader() -> U64EPLMAGIC {
        constexpr char magic[] = { 'C', 'N', 'W', 'T','E', 'P', 'R', 'G' };
        std::uint64_t result{ 0 };
        for (size_t i = 0; i < 8; ++i) {
            result = (result << 8) | magic[7 - i];
        }
        return result;
    }

    inline bool CheckIsEfile(U64EPLMAGIC Magic) {
        return Magic == __eprogramheader();
    }
    inline bool CheckIsEsegment(U32EPLMAGIC Magic) {
        return Magic == __make_epl_u32magic(0x19, 0x73, 0x11, 0x15);
    }
    inline bool CheckIsDefaulKey(const std::uint8_t key[4]) {
        return __make_epl_u32magic(key[1], key[2], key[3], key[4]) == __make_epl_u32magic(0x19, 0x73, 0x11, 0x15);
    }

    template<typename T>
    inline void no_same_name_push_back(std::vector<T>& vec, const  T& data) {
        if (std::ranges::find_if(vec, [&data](const T& elem) {
            return elem.name == data.name;
            }) == vec.end()) {
            vec.push_back(data);
        }
    }
}







/*内存数据流处理*/
namespace eprogramutils {
    class appio {
        template<typename CharType, typename T>
        static constexpr auto __is_be_xxstring() {
            using RType = std::remove_reference_t<T>;
            using NoConstType = std::remove_const_t<RType>;
            using EType = std::remove_extent_t<NoConstType>;
            if constexpr (std::is_same_v<CharType, wchar_t>)
            {
                return (std::is_array_v<std::remove_reference_t<EType>> && std::is_same_v<std::remove_extent_t<EType>, wchar_t>)
                    || std::is_same_v<EType, wchar_t>
                    || std::is_same_v<EType, const wchar_t>
                    || std::is_same_v<EType, wchar_t*>
                    || std::is_same_v<EType, const wchar_t*>
                    || std::is_same_v<EType, std::wstring>
                    || std::is_same_v<EType, const std::wstring>
                    || std::is_same_v<EType, std::wstring_view>
                    || std::is_same_v<EType, const std::wstring_view>;

            }
            return (std::is_array_v<std::remove_reference_t<EType>> && std::is_same_v<std::remove_extent_t<EType>, char>)
                || std::is_same_v<EType, char>
                || std::is_same_v<EType, const char>
                || std::is_same_v<EType, char*>
                || std::is_same_v<EType, const char*>
                || std::is_same_v<EType, std::string>
                || std::is_same_v<EType, const std::string>
                || std::is_same_v<EType, std::string_view>
                || std::is_same_v<EType, const std::string_view>;
        };
        static void ioput() {

        }
    public:
        template<typename T, typename... Args>
        static void ioput(const T& thing, Args... args) {
            //// 判断输出内容的类型是否为宽字符
            if constexpr (
                __is_be_xxstring<wchar_t, T>()
                ) {
                // 输出内容是宽字符，使用 std::wcout 输出
                
                std::wcout << thing;
            }
            else {
                // 输出内容不是宽字符，使用 std::cout 输出
                std::wcout << thing;
            }

            // 继续处理其余的参数
            ioput(args...);

        }
    };
    
    class MemArchiveStream;



    /*继承易语言带唯一ID的基类*/
    struct EplIDBase
    {
        using Stream = MemArchiveStream;
        std::uint32_t m_id{};/*对应映射id,易语言中应该唯一*/
        std::string m_name;/*域名*/
        EplIDBase() = default;
        EplIDBase(std::uint32_t id) : m_id(id) {}
        EplIDBase(const std::string& name, std::uint32_t id) : m_id(id), m_name(name) {}
        auto getID() const {
            return  m_id;
        }
        virtual auto getName() -> const std::string {
            return m_name;
        }
        virtual ~EplIDBase() = default;
    };
    /*继承流*/
    class EplStreamObj {
    public:
        EplStreamObj() {}
        virtual ~EplStreamObj() = default;
        /*必须实现流输入*/
        virtual void Serialize(MemArchiveStream& s) = 0;
    };
    struct MFCString
    {
        std::string str;
        MFCString() = default;
        MFCString(std::string _s) :str(_s) {};
        operator std::string() const& {
            return str;
        }
    };

    class MemArchiveStream {
    public:
#define EPL_NODISCARD [[nodiscard]]
#if _HAS_CXX20
#define EPL_CONSTEXPR20 inline //constexpr
        template <typename CharType>
        using basic_str = std::basic_string_view<CharType>;
#else
#define EPL_CONSTEXPR20 inline
        template <typename CharType>
        using basic_str = std::basic_string<CharType>;
#endif
        using byte_type = unsigned char;
        using buffer_type = std::vector<byte_type>;
        using size_type = buffer_type::size_type;
        using this_type = MemArchiveStream;

        static constexpr auto eof{ static_cast<size_type>(-1) };
        static constexpr auto bof{ static_cast<size_type>(0) };

    private:
        buffer_type data;
        size_type _position{ eof }; // 当前位置

    public:

        /*用于监视debug视图*/
        EPL_CONSTEXPR20 size_type& position();
        /*用于监视debug视图*/
        EPL_CONSTEXPR20 size_type position() const;

        /// <summary>
        /// 默认构造函数
        /// </summary>
        EPL_CONSTEXPR20  MemArchiveStream() = default;

        /// <summary>
        /// 使用文件路径构造函数
        /// </summary>
        /// <param name="path">文件路径</param>
        EPL_CONSTEXPR20  MemArchiveStream(const std::string_view& path);

        /// <summary>
        /// 使用C风格字符串构造函数
        /// </summary>
        /// <param name="path">C风格字符串路径</param>
        EPL_CONSTEXPR20  MemArchiveStream(const char* path);

        /// <summary>
        /// 使用数据构造函数
        /// </summary>
        /// <param name="_data">数据</param>
        EPL_CONSTEXPR20 MemArchiveStream(const buffer_type& _data);

        /// <summary>
        /// 设置输入数据
        /// </summary>
        /// <param name="_data">输入数据</param>
        EPL_CONSTEXPR20 auto input(const buffer_type& _data) -> void;

        /// <summary>
        /// 打开文件
        /// </summary>
        /// <param name="path">文件路径</param>
        /// <returns>打开是否成功</returns>
        EPL_CONSTEXPR20 auto open(const std::string_view& path) -> bool;

        /// <summary>
        /// 读取指定类型的数据
        /// </summary>
        /// <typeparam name="T">数据类型</typeparam>
        /// <returns>读取到的数据</returns>
        template <typename T>
        EPL_NODISCARD EPL_CONSTEXPR20 auto read() -> T;

        /// <summary>
        /// 读取指定类型的数据到引用
        /// </summary>
        /// <typeparam name="T">数据类型</typeparam>
        /// <param name="value">接收数据的引用</param>
        template <typename T>
        EPL_CONSTEXPR20 auto read(T& value) -> void;

        /// <summary>
        /// 创建切片
        /// </summary>
        /// <param name="size">切片的大小</param>
        /// <returns>新的切片</returns>
        EPL_NODISCARD EPL_CONSTEXPR20  auto memstream(size_t size) -> this_type;

        /// <summary>
        /// 判断是否到达流尾
        /// </summary>
        /// <returns>是否到达流尾</returns>
        EPL_NODISCARD EPL_CONSTEXPR20 auto is_end() const -> bool;

        /// <summary>
        /// 获取定长文本
        /// </summary>
        /// <returns>是否到达流尾</returns>
        EPL_NODISCARD EPL_CONSTEXPR20 auto get_fixelenth_string(size_t length) -> std::string;

        EPL_NODISCARD EPL_CONSTEXPR20 auto get_bstring() -> std::string;

        /// <summary>
        /// 获取由CArchive序列化的CString
        /// </summary>
        /// <returns>序列化的CString</returns>
        EPL_NODISCARD EPL_CONSTEXPR20  auto get_mfc_string() -> std::string;

        /// <summary>
        /// 获取普通字符串
        /// </summary>
        /// <returns>普通字符串</returns>
        EPL_NODISCARD EPL_CONSTEXPR20 std::string get_string();

        EPL_NODISCARD EPL_CONSTEXPR20 auto now_ptr() -> const unsigned char* const;
        /// <summary>
        /// 重载操作符>>，用于读取数据
        /// </summary>
        /// <typeparam name="T">数据类型</typeparam>
        /// <param name="value">接收数据的引用</param>
        /// <returns>当前流</returns>
        template <typename T>
        EPL_CONSTEXPR20 auto operator>>(T& value)->MemArchiveStream&;


        /// <summary>
        /// 重载操作符>>，用于跳过指定长度
        /// </summary>
        /// <param name="value">跳过的长度</param>
        /// <returns>当前流</returns>
        EPL_CONSTEXPR20 auto operator>>(const int&& value)->MemArchiveStream&;

        /// <summary>
        /// 重载操作符>>，用于读取字符串
        /// </summary>
        /// <param name="value">接收字符串的引用</param>
        /// <returns>当前流</returns>
        EPL_CONSTEXPR20 MemArchiveStream& operator>>(std::string& value);

        /// <summary>
        /// 重载操作符>>，用于读取MFCString
        /// </summary>
        /// <param name="value">接收MFCString的引用</param>
        /// <returns>当前流</returns>
        EPL_CONSTEXPR20 MemArchiveStream& operator>>(MFCString& value);


        /// <summary>
        /// 获取指定数量的向量数据
        /// </summary>
        /// <typeparam name="T">数据类型</typeparam>
        /// <param name="size">向量大小</param>
        /// <returns>向量数据</returns>
        template <typename T>
        EPL_NODISCARD EPL_CONSTEXPR20 auto get_vec(size_t size) -> std::vector<T>;

        /// <summary>
        /// 获取由MFC数组序列化的指定类型数组
        /// </summary>
         /// <returns>指定类型数组</returns>
        template <typename T>
        EPL_NODISCARD EPL_CONSTEXPR20 auto get_mfc_vec() -> std::vector<T>;

        /// <summary>
        /// 获取由MFC字节集序列化的指定类型数组
        /// </summary>
        /// <returns>指定类型数组</returns>
        template <typename T>
        EPL_NODISCARD EPL_CONSTEXPR20 auto get_mfc_byte_vec() -> std::vector<T>;

        /// <summary>
        /// 获取由CArchive序列化的字符串数组
        /// </summary>
        /// <returns>字符串数组</returns>
        EPL_NODISCARD EPL_CONSTEXPR20  auto get_mfc_string_arry() -> std::vector<std::string>;
        /// <summary>
        /// 按字节移动指针
        /// </summary>
        /// <returns>移动长度</returns>
        EPL_NODISCARD EPL_CONSTEXPR20 auto move_pointer(size_t offset) -> void;

        /// <summary>
        /// 读取具有ID和内存地址的块
        /// </summary>
        /// <typeparam name="T">数据类型</typeparam>
        /// <param name="readFunction">读取块的函数指针</param>
        /// <returns>ID和内存地址块的列表</returns>
        template <typename T>
        EPL_NODISCARD EPL_CONSTEXPR20 auto read_blocks_have_Id_and_memoryaddress() -> std::vector<T>;

        /// <summary>
        /// 获取流的大小
        /// </summary>
        /// <returns>流的大小</returns>
        EPL_NODISCARD EPL_CONSTEXPR20 auto size() const->size_type;

        /// <summary>
        /// 跳过指定偏移量
        /// </summary>
        /// <param name="offset">偏移量</param>
        /// <returns>新的位置</returns>
        EPL_CONSTEXPR20  auto skip(size_type offset) -> size_type;

        /// <summary>
       /// 跳过指定类型长度的偏移量
       /// </summary>
       /// <returns>新的位置</returns>
         template<typename T>
        EPL_CONSTEXPR20 auto skip() -> size_type;

        ///回退
        EPL_CONSTEXPR20    auto back(size_type offset) -> size_type;

        /// <summary>
        /// 获取当前偏移位置
        /// </summary>
        /// <returns>当前偏移位置</returns>
        EPL_CONSTEXPR20 size_type get_offset() const;

        EPL_CONSTEXPR20 void set_offset(MemArchiveStream::size_type __position);

        template<typename T>
        EPL_CONSTEXPR20 std::vector<T> read_blocks_have_Id_and_offset();

        /// <summary>
        /// MFC风格数组重载
        /// </summary>
        template<typename T>
        EPL_CONSTEXPR20 this_type& operator>>(std::vector<T>& vec);
};

}

namespace eprogramutils {
    EPL_NODISCARD EPL_CONSTEXPR20 MemArchiveStream::size_type& MemArchiveStream::position()
    {
        return _position;
    }
    EPL_CONSTEXPR20 MemArchiveStream::size_type MemArchiveStream::position() const
    {
        return _position;
    }
    EPL_CONSTEXPR20 MemArchiveStream::MemArchiveStream(const std::string_view& path) { open(path); }

    EPL_CONSTEXPR20 MemArchiveStream::MemArchiveStream(const char* path) : MemArchiveStream(std::string_view(path)) {}

    EPL_CONSTEXPR20 MemArchiveStream::MemArchiveStream(const buffer_type& _data) : data(_data) {
        if (!data.empty())
            position() = bof;
    }

    EPL_CONSTEXPR20 auto MemArchiveStream::input(const buffer_type& _data) -> void
    {
        data = _data;
        if (!data.empty())
            position() = bof;
    }

    EPL_CONSTEXPR20 auto MemArchiveStream::open(const std::string_view& path) -> bool
    {
        std::ifstream file(path.data(), std::ios::binary);
        if (file.is_open()) {
            file.seekg(0, std::ios::end);
            size_t length = static_cast<size_t>(file.tellg());
            file.seekg(0, std::ios::beg);
            data.resize(length);
            file.read(reinterpret_cast<char*>(data.data()), length);
            if (!data.empty())
                position() = bof;
            file.close();
            return true;
        }
        return false;
    }
    template<typename T>
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::read() -> T
    {

        T value{};
        if constexpr (std::is_base_of_v< EplStreamObj, T>)
        {
            value.Serialize(*this);
        }
        else {
            if (position() == eof)
            {

            }
            else if ((int)position() + sizeof(T) >= (int)data.size()) {
                const unsigned char* ptr = &data[position()];
                std::memcpy(&value, ptr, data.size() - position());
                position() = eof;
            }
            else {
                
                const unsigned char* ptr = &data[position()];
                position() += sizeof(T);
                std::memcpy(&value, ptr, sizeof(T));
            }
        }
#ifdef _DEBUG
        //        std::cout << " {";
        //        for (const auto&& b : data) {
        //s           std::cout << std::hex << (int)b << ",";
        //        }
        //            for (size_t i = 0; i < s.size(); i++)
        //            {
        //                if (i == 0) {
        //                    str.append(" 字节集 :" + std::to_string((int)s.size()) + " {");
        //                }
        //                str.append(std::to_string((int)s.data()[i]));
        //                str.append(",");
        //            }
        //            str.pop_back();
        //            str.append("} | ");
#endif 
        return value;
    }
    template<typename T>
    EPL_CONSTEXPR20 auto MemArchiveStream::read(T& value) -> void
    {
        try
        {
            value = read<T>();
        }
        catch (const std::exception&)
        {
            value = {};
        }
        
    }

    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::memstream(size_t size) -> this_type
    {
        if (size == 0)
            return {};
        if (is_end())
            return {};
        this_type ms;
        if (position() + size >= data.size())/*到结尾了*/
        {
            ms = this_type(buffer_type(data.data() + position(), data.data() + data.size()));
            position() = eof;
        }
        else
        {
            ms = this_type(buffer_type(data.data() + position(), data.data() + position() + size));
            position() += size;
        }

        return  ms;
    }

    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::is_end() const -> bool
    {
        if (data.empty()) return true;
        if (position() == eof) return true;
        if (position() >= data.size()) return true;
        return false;
    }
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::get_fixelenth_string(size_t length)-> std::string {
        auto buffer = get_vec<char>(length);
        auto nullPos = std::find(buffer.begin(), buffer.end(), '\0');
        if (nullPos != buffer.end()) {
            buffer.resize(std::distance(buffer.begin(), nullPos));
        }
        return std::string(buffer.begin(), buffer.end());
    }
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::get_bstring()->std::string {
        if (is_end())
            return {};
        auto size{ read<std::uint32_t>() };
        if (size == 0)
            return {};
        auto str{ get_fixelenth_string(size) };
        skip(1);
        return str;
    };

    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::get_mfc_string() -> std::string
    {
        if (is_end())
            return {};
        std::string str;
        std::uint32_t size{ 0 };
        *this >> size;
        if (size == 0)
            return {};
        const unsigned char* ptr = &data[position()];
        std::string result(size, '\0');
        if (position() + size >= data.size())/*到结尾了*/
        {
            std::memcpy(result.data(), ptr, data.size() - position());
            position() = eof;

        }
        else {
            std::memcpy(result.data(), ptr, size);
            position() += size;
        }
        return  (result + "").data();
    }

    EPL_NODISCARD EPL_CONSTEXPR20 std::string MemArchiveStream::get_string()
    {
        std::string str;
        char ch{ 0 };
        while (!is_end())
        {
            *this >> ch;
            if (ch == '\0')
                break;
            str.push_back(ch);
        }
        return str;
    }

    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::now_ptr()->const unsigned char* const
    {
        return &data[position()];
    }
    template<typename T>
    EPL_CONSTEXPR20 auto MemArchiveStream::operator>>(T& value) -> MemArchiveStream&
    {
        read(value);
        return *this;
    }

    EPL_CONSTEXPR20 auto MemArchiveStream::operator>>(const int&& value) -> MemArchiveStream&
    {
        skip(value);
        return *this;
    }
    EPL_CONSTEXPR20 MemArchiveStream& MemArchiveStream::operator>>(std::string& value)
    {
        value = this->get_string();
        return *this;
    }

    EPL_CONSTEXPR20 MemArchiveStream& MemArchiveStream::operator>>(MFCString& value)
    {
        value = this->get_mfc_string();
        return *this;
    }



    template<typename T>
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::get_vec(size_t size) -> std::vector<T>
    {
        std::vector<T> ret;
        for (size_t i = 0; i < size; ++i) {
            T value;
            read(value);
            ret.push_back(value);
        }
        return ret;
    }
    template<typename T>
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::get_mfc_vec() -> std::vector<T>
    {
        std::uint32_t size{ 0 };
        *this >> size;
        std::vector<T> ret(size);
        for (size_t i = 0; i < size; ++i) {
            T value;
            read(value);
            ret.push_back(value);
        }
        return ret;
    }

    template<typename T>
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::get_mfc_byte_vec() -> std::vector<T>
    {
        std::uint32_t size{ 0 };
        *this >> size;
        const auto count = size / sizeof(T);
        std::vector<T> ret(count);
        for (size_t i = 0; i < count; ++i) {
            T value;
            read(value);
            ret.push_back(value);
        }
        return ret;
    }
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::get_mfc_string_arry() -> std::vector<std::string>
    {
        std::uint32_t count{ 0 };
        uint16_t count_16bit = read<uint16_t>();
        if (count_16bit != static_cast<uint16_t>(0xFFFFU)) {
            count = count_16bit;
        }
        else {
            count = read<std::uint32_t>();
        }
        std::vector<std::string> result(count);
        for (auto& s : result)
        {
            s = get_mfc_string();
        }
        return result;
    }
 
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::move_pointer(size_t offset)->void {
        if (position() + offset >= data.size())/*到结尾了*/
        {
            position() = eof;
            return;
        }
        position() += offset;
    }

    template <typename T>
    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::read_blocks_have_Id_and_memoryaddress(
    ) -> std::vector<T> {
        auto headerSize = read<std::uint32_t>();
        int count = headerSize / 8;
        std::vector<std::uint32_t>
            ids = get_vec<std::uint32_t>(count),
            memoryAddresses = get_vec<std::uint32_t>(count);
        std::vector<T> result;
        for (int i = 0; i < count; i++)
        {
            result.push_back(T(*this, ids[i], memoryAddresses[i]));

        }
        return result;
    }

    EPL_NODISCARD EPL_CONSTEXPR20 auto MemArchiveStream::size() const -> size_type
    {
        return data.size();
    }

    EPL_CONSTEXPR20 auto MemArchiveStream::skip(size_type offset) -> size_type
    {
        return position() += offset;
    }

    template<typename T>
    EPL_CONSTEXPR20 auto MemArchiveStream::skip() -> size_type
    {
        return position() += sizeof(T);
    }
    EPL_CONSTEXPR20 auto MemArchiveStream::back(size_type offset) -> size_type
    {
        return position() -= offset;
    }
    EPL_CONSTEXPR20  MemArchiveStream::size_type MemArchiveStream::get_offset() const {
        return position();
    }
    EPL_CONSTEXPR20  void MemArchiveStream::set_offset(MemArchiveStream::size_type __position) {
        position() = __position;
    }

    template<typename T>
    /*读取具有ID和偏移的的块*/
    EPL_CONSTEXPR20 std::vector<T>   MemArchiveStream::read_blocks_have_Id_and_offset() {
        std::uint32_t count, size;
        *this >> count >> size;
        auto endposition = position() + size;
        std::vector<T> result;
        auto ids = get_vec<std::uint32_t>(count),
            offsets = get_vec<std::uint32_t>(count);
        auto startposition = position();
        for (std::uint32_t i = 0; i < count; i++)
        {
            position() = startposition + offsets[i];
            result.push_back(T(*this, ids[i]));
        }
        position() = endposition;
        return result;
    }



    template<typename T>
    EPL_CONSTEXPR20 MemArchiveStream& MemArchiveStream::operator>>(std::vector<T>& vec)
    {
        std::uint32_t size{ 0 };
        *this >> size;
        for (size_t i = 0; i < size; ++i) {
            T value;
            read(value);
            vec.push_back(value);
        }
        return *this;
    }
}


#endif // !ALMEMSTREAM


