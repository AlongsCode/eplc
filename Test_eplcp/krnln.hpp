#ifndef KRNLN_HPP
#define KRNLN_HPP


#pragma region import_stl
#include <optional>
#include <string_view>
#include <array>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <bitset>
#include <vector>
#include <numeric>
#include <filesystem>
#include <string>
#ifdef _WIN32
#include<conio.h>
#else
#include <unistd.h>
#endif // _WIN32

#pragma endregion






#ifdef CPP20
#define fn constexpr auto
#else
#define fn inline auto
#endif

#define __passnop 
#define __impeplthiself() this
#define EPL_GET_FUNCTION_NAME() \
   epl_runtime_systype::eplstring(_imp_proname_help::u82u16(__FUNCTION__))

#define EPL_GET_FILENAME_XC() \
   epl_runtime_systype::eplstring(std::filesystem::path(__FILEW__).filename().replace_extension(L"").wstring())

#define EPL_GET_PARENT_DIR_AND_PROJ() \
   epl_runtime_systype::eplstring(std::filesystem::path(__FILEW__).parent_path().parent_path().parent_path().parent_path().wstring() + L"\\" + \
           std::filesystem::path(__FILEW__).parent_path().parent_path().parent_path().parent_path().filename().wstring() + \
           L".xcproj")

#define EPL_GET_GRANDPARENT_DIR_PROJ() \
   epl_runtime_systype::eplstring(std::filesystem::path(__FILEW__).parent_path().parent_path().parent_path().parent_path().filename().wstring() + \
           L".xcproj")

#define EPL_GET_EMPTY() \
   epl_runtime_systype::eplstring(L"")

#define EPL_GET_GRANDPARENT_DIR_NAME() \
   epl_runtime_systype::eplstring(std::filesystem::path(__FILEW__).parent_path().parent_path().parent_path().parent_path().filename().wstring())

#define EPL_GET_UNKNOWN_TYPE() \
   epl_runtime_systype::eplstring(L"未知类型")

#define EPL_GET_PROGRAM_NAME(type) \
    (type == 1 ? EPL_GET_FUNCTION_NAME() : \
     type == 2 ? EPL_GET_FILENAME_XC() : \
     type == 3 ? EPL_GET_PARENT_DIR_AND_PROJ() : \
     type == 4 ? EPL_GET_GRANDPARENT_DIR_PROJ() : \
     type == 5 ? EPL_GET_EMPTY() : \
     type == 6 ? EPL_GET_GRANDPARENT_DIR_NAME() : EPL_GET_UNKNOWN_TYPE())









//------------------辅助函数
namespace krnln {
    void ErrorMsgBox(const std::wstring& filePath, const std::wstring& msg);
#ifdef _DEBUG
#define put_errmsg(x) krnln::ErrorMsgBox(__FILEW__,x)
#else
#define put_errmsg(x)
#endif // DEBUG
}




//-------------------这些是预定义数据类型
namespace krnln {


    namespace datetime {
        struct EPLDATATIME : public std::tm {};
    }
}
using EPLKRNLNDATATIME_T = krnln::datetime::EPLDATATIME;
namespace epl_runtime_systype {
    inline constexpr std::nullopt_t nooparg{ std::nullopt };
    /*对于C风格字符串的包装,而非C++字符串*/
    /*易语言字符串*/
    struct eplbool_t
    {
        int32_t bBool{ 0 }; // 0 表示 false，1 表示 true
        inline constexpr operator bool() const {
            return bBool != 0;  // 如果 bBool 为 0，则转换为 false，否则为 true
        }
    };

    // 常量 epltrue 和 eplfalse
    inline constexpr eplbool_t eplfalse{ 0 };
    inline constexpr eplbool_t epltrue{ 1 };

    inline bool operator==(const eplbool_t& lhs, const eplbool_t& rhs) {
        return lhs.bBool == rhs.bBool;
    }
    inline bool operator!=(const eplbool_t& lhs, const eplbool_t& rhs) {
        return lhs.bBool != rhs.bBool;
    }

    class eplstring {
        using CharType = char;
        using SizeType = std::size_t;

        //定义迭代器
        class eplstring_iterator
        {
            using value_type = CharType;
            using difference_type = ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;
        private:
            pointer m_p{ nullptr };
        public:
            eplstring_iterator(CharType* p) : m_p(p) {}
            eplstring_iterator(const eplstring_iterator& other) : m_p(other.m_p) {}
            [[nodiscard]] constexpr reference operator*() const noexcept {
                return *m_p;
            }

            [[nodiscard]] pointer operator->() const noexcept {
                auto c = **this;
                return std::pointer_traits<pointer>::pointer_to(**this);
            }
            eplstring_iterator& operator=(const eplstring_iterator& other) {
                m_p = other.m_p;
                return *this;
            }
            [[nodiscard]] constexpr operator pointer() const
            {
                return m_p;
            }
            constexpr eplstring_iterator& operator++() noexcept {
                ++m_p;
                return *this;
            }

            eplstring_iterator operator++(int) noexcept {
                eplstring_iterator _Tmp{ *this };
                ++*this;
                return _Tmp;
            }

            constexpr eplstring_iterator& operator--() noexcept {
                --m_p;
                return *this;
            }

            eplstring_iterator operator--(int) noexcept {
                eplstring_iterator _Tmp{ *this };
                --*this;
                return _Tmp;
            }


            constexpr eplstring_iterator& operator+=(const difference_type _Off) noexcept {
                m_p += _Off;
                return *this;
            }

            [[nodiscard]] eplstring_iterator operator+(const difference_type _Off) const noexcept {
                eplstring_iterator _Tmp{ *this };
                _Tmp += _Off;
                return _Tmp;
            }

            [[nodiscard]] friend  eplstring_iterator operator+(
                const difference_type _Off, eplstring_iterator _Next) noexcept {
                _Next += _Off;
                return _Next;
            }

            constexpr eplstring_iterator& operator-=(const difference_type _Off) noexcept {
                return *this += -_Off;
            }

            [[nodiscard]] eplstring_iterator operator-(const difference_type _Off) const noexcept {
                eplstring_iterator _Tmp{ *this };
                _Tmp -= _Off;
                return _Tmp;
            }


            [[nodiscard]] constexpr
                const difference_type operator-
                (const eplstring_iterator& other) const
            {
                return m_p - other.m_p;
            }
            /*下标引用*/
            [[nodiscard]] 
                reference operator[]
                (const difference_type _Off) const noexcept
            {
                return *(*this + _Off);
            }

            [[nodiscard]]
            constexpr std::strong_ordering operator<=>
                (const eplstring_iterator& _Right) const noexcept {
                return this->m_p <=> _Right.m_p;
            }
        };
    private:
        eplstring_iterator m_pData{ nullptr };//起始位置
        eplstring_iterator m_pDataEnd{ nullptr }; //结束位置
    public:

        eplstring() = default;
        /// <summary>
        /// 来自STL字符串的构造函数
        /// </summary>
        /// <param name="stlstring"></param>
        eplstring(const ::std::basic_string<CharType>& stlstring) {
            auto nSize{ stlstring.size() };
            m_pData = new CharType[nSize + 1]{ '\0' };
            m_pDataEnd = m_pData + nSize;
            ::std::copy(stlstring.begin(), stlstring.end(), m_pData);
        };
        /*构造函数*/
        eplstring(const CharType* text) {
            auto nSize{ std::strlen(text) };
            m_pData = new CharType[nSize + 1]{ '\0' };
            m_pDataEnd = m_pData + nSize;
            std::copy(text, text + nSize, m_pData);
        }
        /*构造函数*/
        eplstring(const CharType* text, const SizeType nSize) {
            m_pData = new CharType[nSize + 1]{ '\0' };
            m_pDataEnd = m_pData + nSize;
            std::copy(text, text + nSize, m_pData);
        }

        /*复制构造*/
        eplstring(const eplstring& other) {
            auto nSize{ other.size() };
            m_pData = new CharType[nSize + 1]{ '\0' };
            m_pDataEnd = m_pData + nSize;
            std::copy(other.begin(), other.end(), m_pData);
        }
        /*析构函数*/
        ~eplstring() {
            delete[] m_pData;
        }
        [[nodiscard]] fn operator=(const eplstring& other) -> eplstring& {
            auto nSize{ other.size() };
            auto pNewData = new CharType[nSize + 1]{ '\0' };
            std::copy(other.begin(), other.end(), pNewData);
            if (m_pData)
                delete[] m_pData;
            m_pData = pNewData;
            m_pDataEnd = m_pData + nSize;
            return *this;
        }
        [[nodiscard]] fn data()const -> const CharType* {
            return m_pData;
        }
        [[nodiscard]] fn empty()const -> bool {
            return !m_pData || !*m_pData;
        }
        [[nodiscard]] fn begin()const -> eplstring_iterator {
            return m_pData;
        }
        [[nodiscard]] fn end()const -> eplstring_iterator {
            return m_pDataEnd;
        }
        [[nodiscard]] fn begin() -> eplstring_iterator {
            return m_pData;
        }
        [[nodiscard]] fn end() -> eplstring_iterator {
            return m_pDataEnd;
        }
        [[nodiscard]] fn operator[](SizeType index) -> CharType& {
            return m_pData[index];
        }
        [[nodiscard]] fn operator[](SizeType index)const -> const CharType& {
            return m_pData[index];
        }
        [[nodiscard]] fn operator==(const eplstring& other)const -> bool {

        }
        [[nodiscard]] fn operator+=(const eplstring& other) -> eplstring& {
            auto nSize{ other.size() };
            auto pNewData = new CharType[size() + nSize];
            std::copy(m_pData, m_pDataEnd, pNewData);
            std::copy(other.begin(), other.end(), pNewData + size());
            delete[] m_pData;
            m_pData = pNewData;
            m_pDataEnd = m_pData + size() + nSize;
        }
        //是否相似
        fn like(const eplstring& other)->bool {
            // 获取str2的长度
            const auto& str2Length = other.size();
            const CharType* str2 = other.m_pData;
            // 用于遍历str1的指针
            auto pStr1 = m_pData;
            // 如果str2的长度小于或等于str1的长度
            if (str2Length <= size()) {
                // 如果str2为空，则返回true
                if (str2Length == 0)
                    return true;
                // 循环比较字符，直到str2的末尾
                while (*pStr1) {
                    // 如果字符不匹配，则返回false
                    if (*pStr1++ != *str2++)
                        return false;
                }
                // 比较两个字符串的最后一个字符
                if (*(--pStr1) == *(--str2))
                    return true;
            }
            return false;
        }
        fn size()const -> SizeType {
            return m_pDataEnd - m_pData;
        }
        fn c_str()const {
            return m_pData;
        }
        eplstring operator+(const eplstring& rhs) {
            // 计算新的大小
            auto newSize = size() + rhs.size();

            // 创建一个新的eplstring对象
            eplstring result;

            // 释放旧内存（如果有的话）
            if (result.m_pData) {
                delete[] result.m_pData;
            }

            // 分配新的内存
            result.m_pData = new CharType[newSize + 1]{ '\0' };  // +1 for null-terminator
            result.m_pDataEnd = result.m_pData + newSize;

            // 将lhs的数据复制到result
            std::copy(begin(), end(), result.m_pData);

            // 将rhs的数据追加到result
            std::copy(rhs.begin(), rhs.end(), result.m_pData + size());

            return result;
        }



    };
}



namespace krnln {



    inline void ioput(int) {
        // 函数出口
    }
    template<typename T, typename... Args>
    void ioput(std::optional<int> 输出方向_, T 输出内容, Args... args) {
        int 输出方向 = 输出方向_.value_or(1);
        // 判断输出内容的类型是否为宽字符
        if constexpr (std::is_same<T, wchar_t>::value
            || std::is_same<T, wchar_t*>::value
            || std::is_same<T, const wchar_t*>::value
            || std::is_same<T, std::wstring>::value
            || std::is_same<T, const std::wstring>::value) {
            // 输出内容是宽字符，使用 std::wcout 输出
            if (输出方向 == 2) {
                std::wcerr << 输出内容;
            }
            else {
                std::wcout << 输出内容;
            }
        }
        else if constexpr (std::is_same<T, epl_runtime_systype::eplstring>::value) {
            if (输出方向 == 2) {
                std::cerr << 输出内容.c_str();
            }
            else {
                std::cout << 输出内容.c_str();
            }
        }
        else {
            // 输出内容不是宽字符，使用 std::cout 输出
            if (输出方向 == 2) {
                std::cerr << 输出内容;
            }
            else {
                std::cout << 输出内容;
            }
        }
        // 继续处理其余的参数
        ioput(输出方向, args...);
    }


    inline  epl_runtime_systype::eplstring ioinput(std::optional<bool> 是否回显)
    {
        std::string result;
        if (是否回显.value_or(true)) {
            // 使用 std::getline 从标准输入流读取一行字符串
            std::getline(std::cin, result);
        }
        else {
#ifdef _WIN32
            char c;
            while ((c = (char)_getch()) != '\r' && c != '\0' && c != '\n') {
                result.push_back(c);
            }
#else
            char* c = getpass(nullptr);
            result = c ? c : "";
#endif // _WIN32

        }
        return result.c_str();
    }





    template <typename T>
    inline void formatText(const std::wstring& text, std::reference_wrapper<T> output) {
        using BaseType = std::remove_cv_t<std::remove_reference_t<T>>;

        if constexpr (std::is_same_v<BaseType, epl_runtime_systype::eplstring>) {
            output.get() = text;
        }
        else if constexpr (std::is_integral_v<BaseType>) {
            if constexpr (sizeof(BaseType) <= sizeof(std::int32_t)) {
                output.get() = static_cast<BaseType>(std::wcstol(text.c_str(), nullptr, 10));
            }
            else {
                output.get() = static_cast<BaseType>(std::wcstoll(text.c_str(), nullptr, 10));
            }
        }
        else if constexpr (std::is_floating_point_v<BaseType>) {
            output.get() = static_cast<BaseType>(std::wcstod(text.c_str(), nullptr));
        }
        else {
            static_assert(std::is_same_v<BaseType, BaseType>, "Unsupported type for text formatting");
        }
    }

    template<typename T, typename U>
    void assign_values(T&& value, U& variable) {
        variable = std::forward<T>(value);
    }
    template<typename T, typename First, typename... Rest>
    void assign_values(T&& value, First& first, Rest&... rest) {
        assign_values(std::forward<T>(value), first);
        assign_values(std::forward<T>(value), rest...);
    }
    //连续赋值
    template<typename T, typename... U>
    void assign_values(T&& value, U&... variables) {
        assign_values(std::forward<T>(value), variables...);
    }
    //多项选择
    template<typename T, typename... Args>
    T choose(int index, T firstOption, Args... otherOptions) {
        std::vector<T> options = { firstOption, otherOptions... };

        if (index < 1 || index > static_cast<int>(options.size())) {
            put_errmsg(L"多项选择的索引值参数小于一或超出了所提供参数表范围");
            throw std::out_of_range("多项选择的索引值参数小于一或超出了所提供参数表范围");
        }

        return options[index - 1];
    }
    // 检查类型T是否可以构造为指定字符类型CharType的basic_string_view
    template<typename CharType, typename Type>
    struct is_basic_string_constructible : std::is_constructible<std::basic_string_view<CharType>, Type> {};

    /// <summary>
    /// 是否可以构造为指定的字符类型
    /// </summary>
    /// <typeparam name="CharType">字符类型</typeparam>
    /// <typeparam name="Type">待检测的类型</typeparam>
    template<typename CharType, typename Type>
    constexpr bool is_be_text_v = is_basic_string_constructible<CharType, std::remove_cvref_t<Type>>::value || std::is_same_v<CharType, std::remove_cvref_t<Type>>;

    // 默认情况下，类型不能转换为文本
    template<typename Type, typename = void>
    struct is_to_text_impl : std::false_type {};

    // 检查类型T是否可以通过std::to_wstring转换为文本
    template<typename Type>
    struct is_to_text_impl<Type, std::void_t<decltype(std::to_string(std::declval<Type>()))>> : std::true_type {};

    /// <summary>
    /// 是否可以转换为文本类型
    /// </summary>
    /// <typeparam name="Type">待检测的类型</typeparam>
    template<typename Type>
    constexpr bool is_to_text_v = is_to_text_impl<Type>::value;

    // 转换为epl_runtime_systype::eplstring（字符串类）类型
    inline auto to_string() -> epl_runtime_systype::eplstring {
        return {};
    }
    inline auto to_string(const epl_runtime_systype::eplstring& text) -> epl_runtime_systype::eplstring {
        return text;
    }

    inline auto to_string(epl_runtime_systype::eplbool_t booleanValue) -> epl_runtime_systype::eplstring {
        return booleanValue ? "真" : "假";
    }
    inline auto to_string(bool booleanValue) -> epl_runtime_systype::eplstring {
        return booleanValue ? "真" : "假";
    }
    /// <summary>
    /// 将任意类型格式化为epl_runtime_systype::eplstring（字符串类）
    /// </summary>
    /// <typeparam name="Type">待转换的类型</typeparam>
    /// <param name="value">待转换的值</param>
    /// <returns>转换后的字符串</returns>
    template<typename Type>
    auto to_string(Type&& value) -> epl_runtime_systype::eplstring {
        using ValueType = std::remove_cvref_t<Type>;

        if constexpr (is_be_text_v<char, ValueType>) {
            if constexpr (std::is_same_v<char, ValueType>) {
                const char wc[]{ value, '\0' };
                return wc;
            }
            else {
                return std::string_view{ value }.data();
            }
        }
        //else if constexpr (std::is_same_v<krnln::datetime::EPLDATATIME, ValueType>)
        //{
        //    std::wostringstream oss;
        //    int year = value.tm_year + 1900;
        //    int month = value.tm_mon + 1;
        //    int day = value.tm_mday;
        //    int hour = value.tm_hour;
        //    int minute = value.tm_min;
        //    int second = value.tm_sec;
        //    oss << year << L"年" << month << L"月" << day << L"日";
        //    if (second != 0 || oss.str().empty()) {
        //        oss << hour << L"时" << minute << L"分" << second << L"秒";
        //    }
        //    else if (minute != 0) {
        //        oss << hour << L"时" << minute << L"分";
        //    }
        //    else if (hour != 0) {
        //        oss << hour << L"时";
        //    }
        //    return oss.str();
        //}
        //else if constexpr (is_be_text_v<wchar_t, ValueType>) {
        //    if constexpr (std::is_same_v<wchar_t, ValueType>) {
        //        const wchar_t wc[]{ value, L'\0' };
        //        return wc;
        //    }
        //    else {
        //        return std::wstring_view{ value }.data();
        //    }
        //}
        else if constexpr (is_to_text_v<ValueType>) {
            return std::to_string(value).c_str();
        }
        //else if constexpr (std::is_same_v<CXBytes, ValueType>) {
        //    epl_runtime_systype::eplstring result;
        //    result.resize(value.size() + 1);
        //    std::memcpy(std::addressof(result[0]), value.getPtr(), value.size());
        //    result.resize(wcslen(result.getPtr()));
        //    return result;
        //}
        else {
            put_errmsg(L"选择了无法转换为文本的类型");
            return {};
        }
    }




    inline int to_int(const epl_runtime_systype::eplstring& value) {
        if (value.empty())
        {
            return 0;
        }
        return atoi(value.c_str());
    }
    //template <typename T>
    //concept Arithmetic = std::is_arithmetic_v<T>; 

    template <typename T>
    int to_int(T value) {
        if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<int>(value);
        }
        else
        {
            return 0;
        }
    }

}


















//-------------------常量
namespace krnln {
    const epl_runtime_systype::eplstring constant_0_0 = R"(")";
    const epl_runtime_systype::eplstring constant_0_1 = R"(“)";
    const epl_runtime_systype::eplstring constant_0_2 = R"(”)";
    const epl_runtime_systype::eplstring constant_0_3 = "\n";
};



#endif // !KRNLN_HPP