#ifndef EPL_AST_HPP
#define EPL_AST_HPP

#include <functional>
#include <stack>
#include <variant>
#include <iomanip>
#include"Tace.hpp"
#include "utils.hpp"





#define debug_throw_this(x) debug_throw("[域: " + std::string(ansi2u8(m_fun->getSapce()->getName()).data()) + " | 函数:" + std::string(ansi2u8(m_fun->getName()).data())+ " | id:" + std::string(std::to_string(m_fun->getID()).data()).append("]").append(x));
/*易语言抽象语法树,无论是转译还是编译,都可以直接通过该语法树构建*/
namespace eplast {
    template <typename T, typename... Args>
    inline auto newsp(Args&&... args) -> std::shared_ptr<T> {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    enum class
        EnumEplTokenType
        :std::uint8_t {
        //参数列表结尾标注
        END_OF_ARGUMENTS = 1,
        //空参std::opional
        EMPTY_ARGUMENTS = 22,
        //数值字面量
        NUMBER = 23,
        //逻辑字面量
        BOOLEAN = 24,
        //日期字面量
        DATE_TIME = 25,
        //字符串字面量
        STRING = 26,
        //自定义常量
        CONSTANT = 27,
        //库定义常量
        LIBRARY_CONSTANT = 28,
        //变量参数
        VARIABLE_PARAMETER = 29,
        //子程序指针
        METHOD_POINTER = 30,
        //数组表达式
        ARRAY_EXPRESSION = 31,
        //数组结束标志
        ARRAY_END = 32,
        //函数调用表达式
        FUNCTION_EXPRESSION1 = 33,
        //函数调用表达式
        FUNCTION_EXPRESSION2 = 34,
        //枚举常量,仅存在于支持库运行时,会转为字面量
        ENUM_CONSTANT = 35,
        //静态函数调用
        FUNCTION_EXPRESSION_STATIC = 54,
        //访问成员函数结束
        FUNCTION_EXPRESSION_CLASS_END = 55,
        //访问成员函数
        FUNCTION_EXPRESSION_CLASS = 56,
        //访问局部变量
        FUNCTION_EXPRESSION_LOCAL = 56,
        //访问变量成员
        VARIABLE_MEMBER = 57,
        //数组下标
        VARIABLE_ARRAY_INDEX = 58,
        //整数字面量下标
        NUM_OF_VARIABLE_ARRAY_INDEX = 59,
        //否则
        ELSE = 80,
        //如果块结尾
        ENDIF = 81,
        //如果真结尾
        ENDIF_TRUE = 82,
        //case结束标志
        CASE_END = 83,
        //switch结束标志
        SWITCH_END = 84,
        //循环结束标志
        LOOP_END = 85,
        //函数调用语句
        FUNCTION_CALL = 106,
        //如果语句
        IF = 107,
        //如果真语句
        IF_TRUE = 108,
        //switch语句
        SWITCH = 109,
        //case语句
        CASE = 110,
        //default语句
        DEFAULT = 111,
        //循环语句，XX循环首(参数...)
        LOOP_BEGIN = 112,
        //循环结束语句，XX循环尾(参数...)
        LOOP_BEGIN_OF_LAST = 113,
        //整个如果块结束
        IF_END_OF_STATEMENT = 114,
        //整个如果真块结束
        IFTRUE_END_OF_STATEMENT = 115,
        //整个判断段结束
        SWITCH_END_OF_STATEMENT = 116,
        //函数结束标志
        END_OF_METHOD = 255
    };

    /*如果进行测法分析将会使用到*/
    //struct Token {
    //    Enum_EplTokenType type{};
    //    std::string value{};
    //    Token() = default;
    //    Token(Enum_EplTokenType t, const std::string& v) : type(t), value(v) {}
    //};

    /*抽象语法树节点*/
    struct ASTNode {
        virtual ~ASTNode() = default;
        virtual std::string generateCode() const = 0;
        virtual bool isNoop() const { return false; };
    };
    /*空块,区别于空参数*/
    struct ASTNoop :virtual public ASTNode {
        ASTNoop() = default;
        std::string generateCode() const override {
            return {};
        }
        bool isNoop()const override { return true; };
    };
#define push_real_as(v,c)  auto __ctemp{c} ;if(!__ctemp->isNoop()) v.push_back(__ctemp);
    /*语句段节点基类,包含注释语句*/
    class ASTStatement :virtual public ASTNode {
        std::string comment;//语句注释
    public:
        ASTStatement() = default;
        ASTStatement(const std::string& comment) : comment(comment) {}
        virtual ~ASTStatement() = default;
        std::string generateCode() const override {
            return comment.empty() ? "" : "//" + comment;
        }
    };


    /*语句组即为块/段*/
    using ASTBlock = std::vector<std::shared_ptr<ASTNode>>;
    /*空参数*/
    struct ASTNoopArg :virtual  public ASTNode
    {
        ASTNoopArg() = default;
        std::string generateCode() const override {
            return "epl_runtime_systype::nooparg";
        }
    };




    /*日期时间型字面量*/
    struct ASTOleDateTimeLiteral :virtual public ASTNode {
        double value;
        ASTOleDateTimeLiteral(const double& v) :value(v) {}
        std::string generateCode() const override {
            return std::to_string(value);
        }
    };
    /*字符串字面量*/
    struct ASTStringLiteral :virtual public ASTNode {
        std::string value;
        ASTStringLiteral(const std::string& v) :value(v) {}
        std::string generateCode() const override {
            return "epl_runtime_systype::eplstring(R\"_eplcl_(" + value + ")_eplcl_\")";
            //return value;
        }
    };
    /*常量节点*/
    struct ASTConstant :virtual public ASTNode {
        /*所在域,-2为自定义常量*/
        std::int16_t space;
        std::int32_t index;
        /*自定义常量*/
        ASTConstant(std::int32_t index) :index(index) {
            space = -2;
        }
        /*自库常量*/
        ASTConstant(std::int16_t libid, std::int16_t index) :index(index), space(libid) {}
        std::string generateCode() const override {
            if (index==0)//核心库直接使用定义,其他从库中读取常量
            {
                return "krnln::constant_" + std::to_string(index) + "_" + std::to_string(space);
            }
            return "constant_" + std::to_string(index) + std::to_string(space);
        }
    };
    struct ASTEnum :virtual public ASTNode {
        /*所在域,-2为自定义常量*/
        std::int16_t lib;
        std::int16_t objindex;
        std::int32_t enumindex;
        /*自库常量*/
        ASTEnum(std::int16_t libid, std::int16_t objindex, std::int32_t index) :lib(libid), objindex(objindex), enumindex(index) {}
        std::string generateCode() const override {
            return "enum" + std::to_string(lib) + "_" + std::to_string(objindex) + "_" + std::to_string(enumindex);
        }
    };


    /*子程序指针*/
    struct ASTMethodPtr :virtual public ASTNode {
        /*子程序id*/
        std::int32_t id;
        ASTMethodPtr(std::int32_t index) :id(index) {
        }
        std::string generateCode() const override {
            return "&method" + std::to_string(id);
        }
    };

    /*数值*/
    struct ASTNumberLiteral :virtual public ASTNode {
        double value;
        ASTNumberLiteral(double v) :value(v) {}
        std::string generateCode() const override {
            std::ostringstream oss;
            // 判断是否是整数
            if (std::floor(value) == value) {
                // 如果是整数，直接输出整数部分
                oss << static_cast<int>(value);
            }
            else {
                // 否则，根据数值的绝对值大小决定使用 fixed 还是 scientific
                if (std::abs(value) >= 1e-3 && std::abs(value) < 1e6) {
                    oss << std::fixed;
                }
                else {
                    oss << std::scientific;
                }

                oss << std::setprecision(16) << value;
            }

            return oss.str();
        }
    };
    //数组字面量
    class ASTArrayLiteral : public ASTNode {
        ASTBlock elements;
    public:
        ASTArrayLiteral(const ASTBlock& elements) : elements(elements) {};
        std::string generateCode() const override {
            std::string code = "{";
            for (const auto& node : elements) {
                code += node->generateCode() + ", ";
            }
            if (!elements.empty()) {
                code.pop_back();
                code.pop_back();
            }
            code += "}";
            return code;
        }
    };
    /*逻辑字面量节点*/
    struct ASTBoolLiteral :virtual public ASTNode {
        int value;
        ASTBoolLiteral(int v) :value(v) {}
        std::string generateCode() const override {
            return value == 0 ? "false" : "true";
        }
    };
    /*变量节点*/
    struct ASTVariable :virtual public ASTNode {
        std::shared_ptr<eplcltype::BaseAbstract> m_var{ nullptr };
        ASTVariable() = default;
        ASTVariable(const std::shared_ptr<eplcltype::BaseAbstract>& var) : m_var(var) {}
        std::string generateCode() const override {
            return  m_var->getName();
        }
        auto getDataType() const -> std::shared_ptr<eplcltype::BaseDataType> {
            return m_var->getDataType();
        }
    };
    /*数组下标访问表达式*/
    struct ASTVariableIndex : public ASTNode {
        std::shared_ptr <ASTNode> m_var{ nullptr };//表达式
        std::shared_ptr<ASTNode> m_node{ nullptr };//访问下标
        ASTVariableIndex(const std::shared_ptr<ASTNode>& var, std::shared_ptr<ASTNode> node) :m_var(var), m_node(node) {}
        std::string generateCode() const override {
            return m_var->generateCode() + "[" + m_node->generateCode() + "]";
        }
    };

    /*判断循环语句*/
    class ASTWhileDoLoopStatement : public ASTStatement {
        std::shared_ptr<ASTNode> condition{ nullptr };//条件表达式
        ASTBlock loopBlock{ nullptr };/*循环语句块*/
    public:
        ASTWhileDoLoopStatement(const std::string& comment, const std::shared_ptr<ASTNode>& cond, const ASTBlock& loopB) : ASTStatement(comment), condition(cond), loopBlock(loopB) {};
        std::string generateCode() const override {
            std::string code = "while (" + condition->generateCode() + ") " + ASTStatement::generateCode() + "\n{";
            for (const auto& node : loopBlock) {
                code += node->generateCode() + "\n";
            }
            code += "}";
            return code;
        }
    };
    /*循环判断语句*/
    class ASTDoWhileLoopStatement : public ASTStatement {
        std::shared_ptr<ASTNode> condition{ nullptr };//条件表达式
        ASTBlock loopBlock{ nullptr };/*循环语句块*/
    public:
        ASTDoWhileLoopStatement(const std::string& comment, const std::shared_ptr<ASTNode>& cond, const ASTBlock& loopB) : ASTStatement(comment), condition(cond), loopBlock(loopB) {};
        std::string generateCode() const override {
            std::string code = "do " + ASTStatement::generateCode() + "\n{";
            for (const auto& node : loopBlock) {
                code += node->generateCode() + "\n";
            }
            code += "} while (" + condition->generateCode() + ");";
            return code;
        }
    };
    /*计次循环语句 : for(欲循环次数,记录次数的变量)*/
    class ASTCountLoopStatement : public ASTStatement {
        std::shared_ptr<ASTNode> end_var{ nullptr }; //欲循环次数
        std::shared_ptr<ASTNode> count_var{ nullptr }; //记录次数的变量
        ASTBlock loopBlock{ nullptr };/*循环语句块*/
    public:
        ASTCountLoopStatement(const std::string& comment, const std::shared_ptr<ASTNode>& cond, const std::shared_ptr<ASTNode>& count, const ASTBlock& loopB) : ASTStatement(comment), end_var(cond), count_var(count), loopBlock(loopB) {};
        std::string generateCode() const override {
            auto val_name = (count_var->generateCode() != "epl::nooparg") ? count_var->generateCode() : "i";

            std::string code = "for (" + val_name + " = 1; " + val_name + " <= " + end_var->generateCode() + "; " + val_name + "++) " + ASTStatement::generateCode() + "\n{";
            for (const auto& node : loopBlock) {
                code += node->generateCode() + "\n";
            }
            code += "}";
            return code;
        }
    };
    /*变量循环首*/
    class ASTForLoopStatement : public ASTStatement {
        //变量起始值,初始值为“1”。定义循环变量的起始数值。
        std::shared_ptr<ASTNode> begin_var{ nullptr };
        //“变量目标值”，。定义循环变量的目标数值，在循环首部如果发现变量值已超出此目标值时将跳出循环。
        std::shared_ptr<ASTNode> end_var{ nullptr };
        //“变量递增值”，初始值为“1”。每次执行到循环尾部时都将把此值加入到循环变量中去，此值可为正数或负数，如为正数则递增，为负数则递减。
        std::shared_ptr<ASTNode> step_var{ nullptr };
        //“循环变量”，可以被省略，提供参数数据时只能提供变量。本数值型变量将用作控制循环执行次数，在循环尾部将自动递增或递减该变量内的数值。如果被省略，命令将自动使用内部临时变量。
        std::shared_ptr<ASTNode> count_var{ nullptr };
        ASTBlock loopBlock{ nullptr };/*循环语句块*/
    public:
        ASTForLoopStatement(const std::string& comment, const std::shared_ptr<ASTNode>& begin_var, const std::shared_ptr<ASTNode>& end_var, const std::shared_ptr<ASTNode>& step_var, const std::shared_ptr<ASTNode>& count_var, const ASTBlock& loopB) : ASTStatement(comment), begin_var(begin_var), end_var(end_var), step_var(step_var), count_var(count_var), loopBlock(loopB) {};
        std::string generateCode() const override {
            auto val_name = (count_var->generateCode() != "epl::nooparg") ? count_var->generateCode() : "i";

            std::string code = "for (" + val_name + " = " + begin_var->generateCode() + ";"+ val_name +" <= " + end_var->generateCode() + "; " + val_name + "+=(" + step_var->generateCode() + ")) " + ASTStatement::generateCode() + "\n{";
            for (const auto& node : loopBlock) {
                code += node->generateCode() + "\n";
            }
            code += "}";
            return code;
        }
    };
    /*函数调用语句表达式*/
    class ASTFunctionCall : public virtual ASTNode {
    private:
        /*位于核心库中的运算符逻辑*/
                //运算符
        enum class OperatorType :std::uint16_t
        {
            Unary,
            Binary,
            Multi,
            Spec
        };
        struct OperatorInfo {
            std::string Operator;/*运算符*/
            int Precedence;/*表达式优先级*/
            OperatorType Type;/*运算符类型*/

            OperatorInfo(const std::string& op, int prec, OperatorType t)
                : Operator(op), Precedence(prec), Type(t) {}
        };

        /*Token和运算符映射表,也就是其在核心运行时中的位置*/
        static inline const std::unordered_map<int, OperatorInfo> OperatorMap{
        { 13,  OperatorInfo("return", 6, OperatorType::Spec) },/*返回*/
        { 15,  OperatorInfo("*", 2, OperatorType::Multi) },
        { 16,  OperatorInfo("/", 2, OperatorType::Multi) },
        { 17,  OperatorInfo("\\", 3, OperatorType::Multi) },//整除
        { 18,  OperatorInfo("%", 4, OperatorType::Multi) },
        { 19, OperatorInfo("+", 5, OperatorType::Multi) },
        { 20, OperatorInfo("-", 5, OperatorType::Multi) /* 相减 */},
        { 21, OperatorInfo("-", 1, OperatorType::Unary) /* 负*/ },
        { 38,  OperatorInfo("==", 6, OperatorType::Binary) /* 等于 */},
        { 39, OperatorInfo("!=", 6, OperatorType::Binary) },
        { 40,  OperatorInfo("<", 6, OperatorType::Binary) },
        { 41,  OperatorInfo(">", 6, OperatorType::Binary) },
        { 42,  OperatorInfo("<=", 6, OperatorType::Binary) },
        { 43,  OperatorInfo(">=", 6, OperatorType::Binary) },
        { 44,  OperatorInfo("fnLike", 6, OperatorType::Spec) },/*近似等于*/
        { 45,  OperatorInfo("&&", 7, OperatorType::Multi) },
        { 46,  OperatorInfo("||", 8, OperatorType::Multi) },/*或者*/
        { 47,  OperatorInfo("!", 8, OperatorType::Unary) }, /* 取反 */
        { 48,  OperatorInfo("~", 8, OperatorType::Unary) }, /* 按位取反 */
        { 49, OperatorInfo("&", 4, OperatorType::Multi) /* 按位与 */},
        { 50, OperatorInfo("|", 6, OperatorType::Multi) /* 按位或 */},
        { 51, OperatorInfo("^", 5, OperatorType::Multi) /* 按位异或 */},
        { 52,  OperatorInfo("=", 9, OperatorType::Binary) /* 赋值 */},
        //{ 52,  OperatorInfo("fnStore", 9, OperatorType::Binary) /* 连续赋值 */},
        { 54,  OperatorInfo("&", 9, OperatorType::Unary) /* 取原始指针/参考 */},
        { 90,  OperatorInfo("krnln::to_string", 6, OperatorType::Spec) /* 到文本 */},
         { 577,  OperatorInfo("krnln::ioput", 6, OperatorType::Spec) },/*标准输出*/
        { 578,  OperatorInfo("krnln::ioinput", 6, OperatorType::Spec) },/*标准输入*/
        { 631,  OperatorInfo("krnln::to_int", 6, OperatorType::Spec) },/*标准输入*/
        };

    private:/*处理运算符*/
        auto isOperator() const -> bool {


            if (space_id == 0 && method_id->getObjType() == eplcltype::BaseCodeObj::type::FinalLibFun)
            {
                return OperatorMap.contains(std::dynamic_pointer_cast<eplcltype::FinalLibFun>(method_id)->getFuncIndex());
            }
            return false;
        }
        auto getOperator() const -> OperatorInfo {
            return OperatorMap.at(std::dynamic_pointer_cast<eplcltype::FinalLibFun>(method_id)->getFuncIndex());
        }
        auto removeExtraParentheses(std::string s)const -> const std::string {
            return s.data();
        }
    private:
        // 函数在对应域的索引
        std::shared_ptr<eplcltype::BaseCodeObj> method_id;
        //函数在对应域 >0：支持库索引，0：核心运行时，-1：表达式语句，-2：用户定义子程序，-3：外部DLL命令
        std::int16_t space_id{ 0 };
        std::int16_t flag{ 0 };
        std::vector<std::shared_ptr<ASTNode>> arguments;
    public:
        ASTFunctionCall() = default;
        ASTFunctionCall(const std::shared_ptr< eplcltype::BaseCodeObj>& _method_id, const std::int16_t& _space_id, const std::int16_t& _flag, const std::vector<std::shared_ptr<ASTNode>>& args)
            :method_id(_method_id), space_id(_space_id), flag(_flag), arguments(args) {};



        std::string generateCode() const override {

            if (isOperator())
            {
                std::string code;// { "(" };
                const auto thisOperator{ getOperator() };
                //thisOperator.Type == OperatorType::Multi;
                switch (thisOperator.Type)
                {
                case OperatorType::Multi:
                    for (const auto& arg : arguments) {
                        code += "(" + arg->generateCode() + ")" + thisOperator.Operator;
                    }
                    if (!arguments.empty())
                    {
                        //移除最后一个运算符
                        code = code.substr(0, code.size() - thisOperator.Operator.size());
                    }
                    break;
                case OperatorType::Binary:
                    if (arguments.size() > 2)
                    {
                        debug_throw("对于二元运算符表达式出现多余两个的情况!");
                    }
                    else
                    {
                        code += "(" + arguments[0]->generateCode() + ")" + thisOperator.Operator + "(" + arguments[1]->generateCode() + ")";
                    }
                    break;
                case OperatorType::Spec:
                {

                    code += thisOperator.Operator + "(";
                    for (const auto& arg : arguments) {
                        code += arg->generateCode() + ", ";
                    }
                    if (!arguments.empty()) {
                        code.pop_back();
                        code.pop_back();
                    }
                    code.push_back(')');


                }break;
                default:
                    if (arguments.size() > 1)
                    {
                        debug_throw("对于单一运算符表达式出现负数的情况!");
                    }
                    else
                    {
                        for (const auto& arg : arguments) {
                            code += thisOperator.Operator + "(" + arg->generateCode() + ")";
                        }
                    }
                    break;
                }
                //  code.push_back(')');
                return removeExtraParentheses(code);

            }
            std::string code = method_id->getName();
            if (method_id->getObjType() == eplcltype::BaseCodeObj::type::FinalAssemblyFunction)
            {
                auto fun = std::dynamic_pointer_cast<eplcltype::FinalAssemblyFunction>(method_id);
                
                //如果不为类成员函数则添加命名空间

                if(fun->getSapce()->getObjType() != eplcltype::BaseCodeObj::type::FinalClass)
				{
                    code = fun->getSapce()->getName() + "::" + code;
				}
            }
            else if (method_id->getObjType() == eplcltype::BaseCodeObj::type::FinalLibFun)
            {
                if (code.find("eplibFunc_0") == 0)//核心库代码
                {
                    code = "krnln::" + code;
                }
                else {
                    code = "epl_fne_runtime::" + code;
                }
                

            }
            code.push_back('(');
            for (const auto& arg : arguments) {
                code += arg->generateCode() + ", ";
            }
            if (!arguments.empty()) {
                code.pop_back();
                code.pop_back();
            }
            code.push_back(')');
            return removeExtraParentheses(code);
        }
    };
    /*访问变量成员节点*/
    class ASTVariableMember :public ASTNode
    {
        std::shared_ptr<ASTNode> m_obj{ nullptr };//节点对象
        //访问成员
        std::int16_t libid;
        std::variant<std::int16_t, std::shared_ptr<eplcltype::Struct>>structid;

        std::int32_t memberid;
    public:
        ASTVariableMember(const std::shared_ptr<ASTNode>& obj, std::int16_t libid, std::int16_t structid, std::int32_t memberid)
            :m_obj(obj), libid(libid), structid(structid), memberid(memberid) {};
        ASTVariableMember(const std::shared_ptr<ASTNode>& obj, const std::shared_ptr<eplcltype::Struct>& structid, std::int32_t memberid)
            :m_obj(obj), libid(-2), structid(structid), memberid(memberid) {};

        std::string generateCode() const override {
            if (libid != -2)/*库类型*/
            {
                auto lib = eplcltype::LibDefinedDataType(libid, std::get<std::int16_t>(structid));
                return lib.getName() + lib.getMemberName(memberid);
            }
            /*判断是否为数据类型*/
            auto obj = std::get<std::shared_ptr<eplcltype::Struct>>(structid);
            if (obj->getObjType() == eplcltype::BaseCodeObj::type::Struct)
            {
                auto structobj = std::dynamic_pointer_cast<eplcltype::Struct>(obj);
                auto members = structobj->getMember();
                for (const auto& m : members) {
                    if (m->getID() == memberid)
                        return m_obj->generateCode() + "." + m->getName();
                }
            }
            debug_throw("未知的成员变量");
        }
    };

    /*数据类型成员函数访问语句*/
    class ASTClassFunctionCall : public ASTFunctionCall {
        std::shared_ptr<ASTNode> m_obj;
    public:
        ASTClassFunctionCall() = default;
        ASTClassFunctionCall(const std::shared_ptr<ASTNode>& obj, const std::shared_ptr< eplcltype::BaseCodeObj>& _method_id, const std::int16_t& _space_id, const std::int16_t& _flag, const std::vector<std::shared_ptr<ASTNode>>& args)
            :ASTFunctionCall(_method_id, _space_id, _flag, args), m_obj(obj) {};
        std::string generateCode() const override {
            return  m_obj->generateCode() + "." + ASTFunctionCall::generateCode();
        }
    };
    /*函数调用语句段*/
    class ASTFunctionCallStatement : public  ASTStatement, public ASTFunctionCall {
    public:
        ASTFunctionCallStatement() = default;
        ASTFunctionCallStatement(const std::shared_ptr< eplcltype::BaseCodeObj>& _method_id, const std::int16_t& _space_id, const std::string& comment, const std::int16_t& _flag, const std::vector<std::shared_ptr<ASTNode>>& args)
            :ASTFunctionCall(_method_id, _space_id, _flag, args), ASTStatement(comment) {};
        std::string generateCode() const override {
            return ASTFunctionCall::generateCode() + ";" + ASTStatement::generateCode();;
        }
    };
    /*变量成员函数调用语句段*/
    class ASTClassFunctionCallStatement :public ASTFunctionCallStatement {
        std::shared_ptr<ASTNode> m_obj;
    public:
        ASTClassFunctionCallStatement(const std::shared_ptr<ASTNode>& obj, const std::shared_ptr< eplcltype::BaseCodeObj>& _method_id, const std::string& comment, const std::int16_t& _space_id, const std::int16_t& _flag, const std::vector<std::shared_ptr<ASTNode>>& args)
            :ASTFunctionCallStatement(_method_id, _space_id, comment, _flag, args), m_obj(obj) {};
        std::string generateCode() const override {
            return m_obj->generateCode() + "." + ASTFunctionCallStatement::generateCode();
        }
    };
    /*如果和如果真语句*/
    class ASTIfStatement : virtual public ASTStatement {
    protected:
        std::shared_ptr<ASTNode> condition{ nullptr };//条件表达式
        ASTBlock ifBlock{ nullptr };/*if语句块*/
        ASTBlock elseBlock{ nullptr };/*else语句块*/

    public:
        ASTIfStatement() = default;
        ASTIfStatement(const std::string& comment, const std::shared_ptr<ASTNode>& cond,
            const ASTBlock& ifB,
            const ASTBlock& elseB)
            : condition(cond), ifBlock(ifB), elseBlock(elseB), ASTStatement(comment) {};

        std::string generateCode() const override {

            std::string code = "if (" + condition->generateCode() + ") " + ASTStatement::generateCode() + "\n{\n";
            for (const auto& node : ifBlock) {
                code += node->generateCode() + "\n";
            }
            if (!elseBlock.empty())
            {
                code += "} else {\n";
                for (const auto& node : elseBlock) {
                    code += node->generateCode() + "\n";
                }
            }
            code += "}";
            return code;
        }

    };
    /*分支语句*/
    class ASTElseIfStatement : virtual public ASTStatement {
    protected:
        std::shared_ptr<ASTNode> condition{ nullptr };//条件表达式
        ASTBlock Block{ nullptr };/*if语句块*/
    public:
        ASTElseIfStatement() = default;
        ASTElseIfStatement(const std::string& comment, const std::shared_ptr<ASTNode>& cond,
            const ASTBlock& ifB)
            : condition(cond), Block(ifB), ASTStatement(comment) {};

        std::string generateCode() const override {
            std::string code = "(" + condition->generateCode() + ") ";
            code = code + ASTStatement::generateCode() + "\n{\n";
            for (const auto& node : Block) {
                code += node->generateCode() + "\n";
            }
            code += "}";
            return code;
        }
    };
    /*判断语句*/
    class ASTIFElseIfStatement : virtual public ASTStatement {
    protected:
        std::shared_ptr <ASTElseIfStatement> ifBlock{ nullptr };/*if语句块*/
        std::vector<std::shared_ptr<ASTElseIfStatement>> elseIfBlock{ nullptr };/*else if语句块,因存在else段,所以else段必须尾空*/
        ASTBlock elseBlock{ nullptr };/*else语句块*/
    public:
        ASTIFElseIfStatement(const std::string& comment,
            const  std::shared_ptr <ASTElseIfStatement>& ifB,
            const std::vector<std::shared_ptr<ASTElseIfStatement>>& elseIfB,
            const  ASTBlock& elseB)
            : ifBlock(ifB), elseIfBlock(elseIfB), elseBlock(elseB), ASTStatement(comment) {};
        std::string generateCode() const override {
            std::string code = "if" + ifBlock->generateCode() + "\n";
            for (const auto& node : elseIfBlock) {
                code.append("else if");
                code += node->generateCode() + "\n";
            }
            if (!elseBlock.empty()) {
                code.append("else {\n");
                for (const auto& node : elseBlock) {
                    code += node->generateCode() + "\n";
                }
                code.append("}");
            }
            return code;
        }
    };



    /*解析器*/
    class Parser {
        using MemStream = eprogramutils::MemArchiveStream;
        using byte_type = MemStream::byte_type;
        using buffer_type = MemStream::buffer_type;
        using AST = std::vector<std::shared_ptr<ASTStatement>>;/*树*/
    private:
        eprogramutils::MemArchiveStream m_ms;//流,类似TOKEN组
        EnumEplTokenType m_CurrToken{ EnumEplTokenType::END_OF_METHOD };

        //当前token
        auto getCurrentToken() -> EnumEplTokenType const {
            return m_CurrToken;

        }
        //移到下一个token
        auto nextTokenType() -> void {
            if (m_ms.is_end())
            {
                m_CurrToken = EnumEplTokenType::END_OF_METHOD;
                return;
            }
            m_ms >> m_CurrToken;
        }
        //移到下一个token并获取
        auto getNextToken() -> EnumEplTokenType {
            nextTokenType();
            return getCurrentToken();
        }

        //基本表达式
        auto parseExpression() -> std::shared_ptr<ASTNode> {
            auto currentToken{ getCurrentToken() };
            switch (currentToken)
            {
            case EnumEplTokenType::EMPTY_ARGUMENTS: // 空参数
                return std::make_shared<ASTNoopArg>();
            case EnumEplTokenType::NUM_OF_VARIABLE_ARRAY_INDEX: //整数字面量下标
                return std::make_shared<ASTNumberLiteral>(m_ms.read<std::int32_t>());
            case EnumEplTokenType::NUMBER://数值字面量
                return std::make_shared<ASTNumberLiteral>(m_ms.read<double>());
            case EnumEplTokenType::BOOLEAN://逻辑字面量
                return std::make_shared<ASTBoolLiteral>(m_ms.read<short>());
            case EnumEplTokenType::DATE_TIME:
                return std::make_shared<ASTOleDateTimeLiteral>(m_ms.read<double>());
            case EnumEplTokenType::STRING: //字符串字面量
                return std::make_shared<ASTStringLiteral>(m_ms.get_mfc_string());
            case EnumEplTokenType::CONSTANT:
                return std::make_shared<ASTConstant>(m_ms.read<std::int32_t>());
            case EnumEplTokenType::LIBRARY_CONSTANT:
                return std::make_shared<ASTConstant>(m_ms.read<std::int16_t>() - 1, m_ms.read<std::int16_t>() - 1);
            case EnumEplTokenType::METHOD_POINTER:
                return std::make_shared<ASTMethodPtr>(m_ms.read<std::int32_t>());
            case EnumEplTokenType::ARRAY_EXPRESSION://状数组表达式
            {
                ASTBlock elements;
                while (getNextToken() != EnumEplTokenType::ARRAY_END)
                {
                    elements.push_back(parseExpression());
                }
                return std::make_shared<ASTArrayLiteral>(elements);
            }
            case EnumEplTokenType::VARIABLE_PARAMETER: {
                /*变量数据类型*/
                if (getNextToken() == EnumEplTokenType::FUNCTION_EXPRESSION_LOCAL)
                {
                    return parseExpression();
                }
                debug_throw("未知变量数据");
            }break;
            case EnumEplTokenType::FUNCTION_EXPRESSION_LOCAL: // 访问局部变量
            {
                auto variable{ m_ms.read<int>() };
                if (variable == 0x0500FFFE) // ThisCall
                {
                    nextTokenType();
                    nextTokenType();
                    return parseExpression();
                }
                else  // 访问成员变量
                {

                    std::shared_ptr<ASTNode> obj = std::make_shared<ASTVariable>(findVar(variable));
                    while (true)
                    {
                        auto _currentToken{ getNextToken() };
                        if (_currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_CLASS_END)
                        {
                            break;
                        }
                        std::function<std::shared_ptr<ASTNode>()> thiscall = [&]()->std::shared_ptr<ASTNode> {
                            if (_currentToken == EnumEplTokenType::VARIABLE_MEMBER)//变量成员访问
                            {
                                auto memberId{ m_ms.read<std::uint32_t>() }, structId{ m_ms.read<std::uint32_t>() };
                                if (EplSystemId::IsLibDataType(structId))
                                {
                                    std::int16_t  libId, structIdInLibId;
                                    EplSystemId::DecomposeLibDataTypeId(structId, libId, structIdInLibId);
                                    return  std::make_shared<ASTVariableMember>(obj, libId, structIdInLibId, memberId - 1);
                                }
                                else
                                {
                                    return std::make_shared<ASTVariableMember>(obj, findDataType(structId), memberId);
                                }
                            }
                            else if (_currentToken == EnumEplTokenType::VARIABLE_ARRAY_INDEX)//变量数组下标访问
                            {
                                nextTokenType();
                                auto index{ parseExpression() };
                                return std::make_shared<ASTVariableIndex>(obj, index);
                            }
                            debug_throw_this("连续的变量访问出现未知的结尾");
                            };
                        obj = thiscall();

                    }

                    return  obj;
                };
            }break;
            case EnumEplTokenType::FUNCTION_EXPRESSION1:
            case EnumEplTokenType::FUNCTION_EXPRESSION2:
                return parseFunctionCall();
            case EnumEplTokenType::ENUM_CONSTANT:
                return std::make_shared<ASTEnum>(m_ms.read<std::int16_t>() - 1, m_ms.read<std::int16_t>() - 1, m_ms.read<std::uint32_t>() - 1);
                break;
            case EnumEplTokenType::FUNCTION_CALL:
                break;
            case EnumEplTokenType::END_OF_METHOD:
                break;
            case EnumEplTokenType::FUNCTION_EXPRESSION_CLASS_END:
                return std::make_shared<ASTNoop>();
                break;
            default:
                break;
            }
            debug_throw_this("未知的表达式:" + std::to_string((int)currentToken));
            return  {};
        }


        auto  parseStatement() -> std::shared_ptr<ASTStatement> {
            auto currentToken{ getNextToken() };
            switch (currentToken)
            {
            case EnumEplTokenType::END_OF_ARGUMENTS:
                return  parseStatement();
            case EnumEplTokenType::ELSE:
            case EnumEplTokenType::ENDIF:
            case EnumEplTokenType::ENDIF_TRUE:
            case EnumEplTokenType::LOOP_END:
            case EnumEplTokenType::CASE_END:
            case EnumEplTokenType::SWITCH_END:
                return std::make_shared<ASTStatement>();
            case EnumEplTokenType::FUNCTION_CALL://函数调用语句
                return  parseFunctionCallStatement();
            case EnumEplTokenType::IF: //如果
                return parseIfStatement(true);
            case EnumEplTokenType::IF_TRUE: //如果真
                return parseIfStatement();
            case EnumEplTokenType::SWITCH:/*判断*/
                return parseSwitchStatement();
            case EnumEplTokenType::LOOP_BEGIN_OF_LAST:/*循环判断尾*/
            case EnumEplTokenType::LOOP_BEGIN:/*循环判断首*/
                return paseLoopStatement();
            default:
                break;
            }
            debug_throw_this("未知的表达式:" + std::to_string(static_cast<int>(currentToken)));
        }
        //所有的表达式,块基本都是以函数形式调用,所以这里统一处理,并以结构化绑定的形式返回
        auto ParseFunStatement() {

            //应该用这种方式
            auto methodId = m_ms.read<std::uint32_t>();
            auto spaceId{ m_ms.read<std::int16_t>() }, flag{ m_ms.read<std::int16_t>() };;
            auto code{ m_ms.get_mfc_string() }, comment{ m_ms.get_mfc_string() };
            return std::make_tuple(methodId, spaceId, flag, code, comment);
        }
        /*调试token*/
        auto debug_token() -> void {
            while (true)
            {
                auto currentToken{ getCurrentToken() };
                debug_put("当前Token:" + std::to_string((int)currentToken));
                nextTokenType();
            }
        }

        /*实际产生if&else if&else*/
        auto parseSwitchStatement() -> std::shared_ptr<ASTStatement> {
            auto getcase = [&] {
                auto [methodId, spaceId, flag, code, comment] { ParseFunStatement()};
                if (!code.empty())debug_throw_this("出现未预编译的代码:" + ansi2u8(code));

                if (getNextToken() != EnumEplTokenType::FUNCTION_EXPRESSION_STATIC)debug_throw_this("条件表达式存在意外的结尾:NO FUNCTION_EXPRESSION_STATIC");
                nextTokenType();

                //条件表达式
                auto condition{ parseExpression() };
                if (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS)debug_throw_this("条件表达式存在意外的结尾:NO ARG_END");
                ASTBlock Block;/*IF块*/
                while (getCurrentToken() != EnumEplTokenType::CASE_END) {
                    Block.push_back(parseStatement());
                }
                return std::make_shared<ASTElseIfStatement>(comment, condition, Block);
                };

            if (getNextToken() != EnumEplTokenType::CASE)debug_throw_this("switch 后没有跟随分支代码");
            /*首段分支处理,必定会有一段case分支才符合预期行为*/
            auto ifBlock{ getcase() };
            std::vector<std::shared_ptr<ASTElseIfStatement>> elseifBlock;
            /*处理else if*/
            while (getNextToken() == EnumEplTokenType::CASE)
                elseifBlock.push_back(getcase());
            /*处理else*/
            if (getCurrentToken() != EnumEplTokenType::DEFAULT)debug_throw_this("switch 没有默认分支?");
            ASTBlock elseBlock;/*else块*/
            while (getCurrentToken() != EnumEplTokenType::SWITCH_END) {
                elseBlock.push_back(parseStatement());
            }
            if (getNextToken() != EnumEplTokenType::SWITCH_END_OF_STATEMENT)debug_throw_this("判断段不存在结尾?");
            return std::make_shared<ASTIFElseIfStatement>("这是一个易语言判断文本!", ifBlock, elseifBlock, elseBlock);
        }

        /*普通调用段,包含thiscall和静态call*/
        auto   parseFunctionCallStatement() -> std::shared_ptr<ASTStatement> {
            /*处理表达式*/
            auto [methodId, spaceId, flag, code, comment] { ParseFunStatement()};
            // 解析函数参数（如果有）
            std::vector<std::shared_ptr<ASTNode>> arguments;
            auto currentToken{ getNextToken() };


            /*普通静态函数*/
            if (currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_STATIC) {
                while (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS && getCurrentToken() != EnumEplTokenType::END_OF_METHOD)
                {
                    arguments.push_back(parseExpression());
                }
                if (arguments.empty())
                {
                    /*空行或者注释行*/
                    return (methodId == 0) ? std::make_shared<ASTStatement>(comment) : std::make_shared<ASTFunctionCallStatement>(findFun(methodId, spaceId), spaceId, comment, flag, arguments);
                }
                return std::make_shared<ASTFunctionCallStatement>(findFun(methodId, spaceId), spaceId, comment, flag, arguments);
            }
            /*变量成员函数和变量成员*/
            else if (currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_CLASS) {
                auto variable{ m_ms.read<int>() };
                std::shared_ptr<ASTNode> obj{ nullptr };
                if (variable == 0x0500FFFE) // ThisCall
                {

                    nextTokenType(); // 跳过一个必定为0x3A的字节
                    nextTokenType(); //读取下一个token
                    auto tempobj{ parseExpression() };//必定不为空
                    auto _currentToken{ getNextToken() };
                    //回退
                    m_ms.back(2);
                    nextTokenType();

                    if (_currentToken == EnumEplTokenType::VARIABLE_MEMBER)
                    {
                        while (true)
                        {
                            auto _currentToken{ getNextToken() };
                            if (_currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_CLASS_END)
                                break;

                            std::function<std::shared_ptr<ASTNode>()> thiscall = [&]()->std::shared_ptr<ASTNode> {
                                if (_currentToken == EnumEplTokenType::VARIABLE_MEMBER)//变量成员访问
                                {
                                    auto memberId{ m_ms.read<std::uint32_t>() }, structId{ m_ms.read<std::uint32_t>() };
                                    if (EplSystemId::IsLibDataType(structId))
                                    {
                                        std::int16_t  libId, structIdInLibId;
                                        EplSystemId::DecomposeLibDataTypeId(structId, libId, structIdInLibId);
                                        return  std::make_shared<ASTVariableMember>(tempobj, libId, structIdInLibId, memberId - 1);
                                    }
                                    else
                                    {
                                        return std::make_shared<ASTVariableMember>(tempobj, findDataType(structId), memberId);
                                    }
                                }
                                else if (_currentToken == EnumEplTokenType::VARIABLE_ARRAY_INDEX)//变量数组下标访问
                                {
                                    nextTokenType();
                                    auto index{ parseExpression() };
                                    return std::make_shared<ASTVariableIndex>(tempobj, index);
                                }
                                debug_throw_this("连续的变量访问出现未知的结尾");
                                };
                            tempobj = thiscall();

                        }
                    }
                    else
                    {
                        while (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS && getCurrentToken() != EnumEplTokenType::END_OF_METHOD)
                        {
                            push_real_as(arguments, parseExpression());
                        }
                    }
                    return std::make_shared<ASTClassFunctionCallStatement>(tempobj, findFun(methodId, spaceId), comment, spaceId, flag, arguments);
                }
                else
                {

                    obj = std::make_shared<ASTVariable>(findVar(variable));
                    while (true)
                    {
                        auto _currentToken{ getNextToken() };
                        if (_currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_CLASS_END)
                        {
                            break;
                        }
                        std::function<std::shared_ptr<ASTNode>()> thiscall = [&]()->std::shared_ptr<ASTNode> {
                            if (_currentToken == EnumEplTokenType::VARIABLE_MEMBER)//变量成员访问
                            {
                                auto memberId{ m_ms.read<std::uint32_t>() }, structId{ m_ms.read<std::uint32_t>() };
                                if (EplSystemId::IsLibDataType(structId))
                                {
                                    std::int16_t  libId, structIdInLibId;
                                    EplSystemId::DecomposeLibDataTypeId(structId, libId, structIdInLibId);
                                    return  std::make_shared<ASTVariableMember>(obj, libId, structIdInLibId, memberId - 1);
                                }
                                else
                                {
                                    return std::make_shared<ASTVariableMember>(obj, findDataType(structId), memberId);
                                }
                            }
                            else if (_currentToken == EnumEplTokenType::VARIABLE_ARRAY_INDEX)//变量数组下标访问
                            {
                                nextTokenType();
                                auto index{ parseExpression() };
                                return std::make_shared<ASTVariableIndex>(obj, index);
                            }
                            debug_throw_this("连续的变量访问出现未知的结尾");
                            };
                        obj = thiscall();

                    }
                }
                while (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS && getCurrentToken() != EnumEplTokenType::END_OF_METHOD)
                {
                    arguments.push_back(parseExpression());
                }
                if (!obj)
                {
                    debug_throw_this("无效的对象!");
                }
                return std::make_shared<ASTClassFunctionCallStatement>(obj, findFun(methodId, spaceId), comment, spaceId, flag, arguments);
            }

            debug_throw_this("未知的调用函数!");
        }

        auto parseIfStatement(bool bHaseElse = false) -> std::shared_ptr<ASTStatement> {
            /*其实也是类似普通call的语句,但是为了更标准的语法树不采用易语言的继承规则*/
            auto [methodId, spaceId, flag, code, comment] { ParseFunStatement()};
            if (!code.empty())debug_throw_this("出现未预编译的代码");

            if (bHaseElse)
            {
                ASTBlock ifBlock, elseBlock;
                if (getCurrentToken() != EnumEplTokenType::IF)debug_throw_this("条件表达式存在意外的结尾:NO IF");


                if (getNextToken() != EnumEplTokenType::FUNCTION_EXPRESSION_STATIC)debug_throw_this("条件表达式存在意外的结尾:NO FUNCTION_EXPRESSION_STATIC");
                nextTokenType();
                //条件表达式
                auto condition{ parseExpression() };
                if (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS)debug_throw_this("条件表达式存在意外的结尾:NO ARG_END");

                //TRUE语句块
                while (getCurrentToken() != EnumEplTokenType::ELSE)
                {
                    ifBlock.push_back(parseStatement());
                }

                // if (getNextToken() != Enum_EplTokenType::ELSE)debug_throw_this("ELSE语句存在意外的结尾:NO ELSE");
                while (getCurrentToken() != EnumEplTokenType::ENDIF)
                {
                    elseBlock.push_back(parseStatement());
                }
                if (getNextToken() != EnumEplTokenType::IF_END_OF_STATEMENT)debug_throw_this("条件表达式存在意外的结尾:NO ARG_END");
                return std::make_shared<ASTIfStatement>(comment, condition, ifBlock, elseBlock);
            }
            else {

                ASTBlock ifBlock, elseBlock;
                if (getCurrentToken() != EnumEplTokenType::IF_TRUE)debug_throw_this("条件表达式存在意外的结尾:NO IF");


                if (getNextToken() != EnumEplTokenType::FUNCTION_EXPRESSION_STATIC)debug_throw_this("条件表达式存在意外的结尾:NO FUNCTION_EXPRESSION_STATIC");
                nextTokenType();
                //条件表达式
                auto condition{ parseExpression() };
                if (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS)debug_throw_this("条件表达式存在意外的结尾:NO ARG_END");

                //TRUE语句块
                while (getCurrentToken() != EnumEplTokenType::ENDIF_TRUE && getCurrentToken() != EnumEplTokenType::ELSE)
                {
                    ifBlock.push_back(parseStatement());
                }
                if (getNextToken() != EnumEplTokenType::IFTRUE_END_OF_STATEMENT)debug_throw_this("条件表达式存在意外的结尾:NO ARG_END");
                return std::make_shared<ASTIfStatement>(comment, condition, ifBlock, elseBlock);

            }
            debug_throw_this("出现BUG块");
        }

        auto  paseLoopStatement() -> std::shared_ptr<ASTStatement> {
            auto [methodId, spaceId, flag, code, comment] { ParseFunStatement()};
            if (!code.empty())debug_throw_this("出现未预编译的代码");
            std::vector< std::shared_ptr<ASTNode>> arguments;
            ASTBlock loopBlock;
            auto currentToken{ getNextToken() };
            /*循环条件表达式*/
            if (currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_STATIC) {
                while (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS)
                {
                    arguments.push_back(parseExpression());
                }
            }
            while (getCurrentToken() != EnumEplTokenType::LOOP_END)
            {
                loopBlock.push_back(parseStatement());
            }
            if (getNextToken() != EnumEplTokenType::LOOP_BEGIN_OF_LAST)debug_throw_this("循环尾条件表达式存在意外的结尾:NO LOOP_BEGIN_OF_LAST");

            /*尾部*/
            auto methodid2 = m_ms.read<std::uint32_t>();
            auto spaceId2{ m_ms.read<std::int16_t>() }, flag2{ m_ms.read<std::int16_t>() };;
            auto code2{ m_ms.get_mfc_string() }, comment2{ m_ms.get_mfc_string() };
            if (!code2.empty())debug_throw_this("出现未预编译的代码");

            std::vector< std::shared_ptr<ASTNode>> arguments2;
            auto currentToken2{ getNextToken() };
            /*循环条件表达式*/
            if (currentToken2 == EnumEplTokenType::FUNCTION_EXPRESSION_STATIC) {
                while (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS)
                {
                    arguments2.push_back(parseExpression());
                }
            }

            switch (methodId)
            {
            case 3://判断循环首
                if (methodid2 != 4 && arguments.size() < 1)debug_throw_this("循环尾条件表达式存在意外的结尾:无判断循环尾");
                return std::make_shared<ASTWhileDoLoopStatement>(comment, arguments[0], loopBlock);
                break;
            case 5://循环判断尾
                if (methodid2 != 6 && arguments.size() < 1)debug_throw_this("循环尾条件表达式存在意外的结尾:无循环判断尾");
                return std::make_shared<ASTDoWhileLoopStatement>(comment, arguments2[0], loopBlock);
                break;
            case 7://计次循环
                if (methodid2 != 8 && arguments.size() < 2)debug_throw_this("循环尾条件表达式存在意外的结尾:无计次循环尾");
                return std::make_shared<ASTCountLoopStatement>(comment, arguments[0], arguments[1], loopBlock);
            case 9://变量循环
                if (methodid2 != 10 && arguments.size() < 4)debug_throw_this("循环尾条件表达式存在意外的结尾:无变量循环尾");
                return std::make_shared<ASTForLoopStatement>(comment, arguments[0], arguments[1], arguments[2], arguments[3], loopBlock);
            default:
                break;
            }
            debug_throw_this("循环内部判定表达式有误?");
        }
    public://语句表达式解析

        //函数调用语句
        auto parseFunctionCall() -> std::shared_ptr<ASTNode> {
            /*处理表达式*/
            auto [methodId, spaceId, flag, code, comment] { ParseFunStatement()};
            // 解析函数参数（如果有）
            std::vector<std::shared_ptr<ASTNode>> arguments;
            auto currentToken{ getNextToken() };


            /*普通静态函数*/
            if (currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_STATIC) {
                while (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS && getCurrentToken() != EnumEplTokenType::END_OF_METHOD)
                {
                    arguments.push_back(parseExpression());
                }
                return std::make_shared<ASTFunctionCall>(findFun(methodId, spaceId), spaceId, flag, arguments);
            }
            /*变量成员函数和变量成员*/
            else if (currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_CLASS) {
                auto variable{ m_ms.read<int>() };
                std::shared_ptr<ASTNode> obj{ nullptr };
                if (variable == 0x500fffe) // ThisCall
                {

                    nextTokenType(); // 跳过一个必定为0x3A的字节
                    nextTokenType(); //读取下一个token
                    auto tempobj{ parseExpression() };//必定不为空
                    while (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS && getCurrentToken() != EnumEplTokenType::END_OF_METHOD)
                    {
                        push_real_as(arguments, parseExpression());
                    }

                    return std::make_shared<ASTClassFunctionCall>(tempobj, findFun(methodId, spaceId), spaceId, flag, arguments);
                }
                else
                {
                    obj = std::make_shared<ASTVariable>(findVar(variable));
                    while (true)
                    {
                        auto _currentToken{ getNextToken() };
                        if (_currentToken == EnumEplTokenType::FUNCTION_EXPRESSION_CLASS_END)
                        {
                            break;
                        }
                        std::function<std::shared_ptr<ASTNode>()> thiscall = [&]()->std::shared_ptr<ASTNode> {
                            if (_currentToken == EnumEplTokenType::VARIABLE_MEMBER)//变量成员访问
                            {
                                auto memberId{ m_ms.read<std::uint32_t>() }, structId{ m_ms.read<std::uint32_t>() };
                                if (EplSystemId::IsLibDataType(structId))
                                {
                                    std::int16_t  libId, structIdInLibId;
                                    EplSystemId::DecomposeLibDataTypeId(structId, libId, structIdInLibId);
                                    return  std::make_shared<ASTVariableMember>(obj, libId, structIdInLibId, memberId - 1);
                                }
                                else
                                {
                                    return std::make_shared<ASTVariableMember>(obj, findDataType(structId), memberId);
                                }
                            }
                            else if (_currentToken == EnumEplTokenType::VARIABLE_ARRAY_INDEX)//变量数组下标访问
                            {
                                nextTokenType();
                                auto index{ parseExpression() };
                                return std::make_shared<ASTVariableIndex>(obj, index);
                            }
                            debug_throw_this("连续的变量访问出现未知的结尾");
                            };
                        obj = thiscall();

                    }

                }
                while (getNextToken() != EnumEplTokenType::END_OF_ARGUMENTS && getCurrentToken() != EnumEplTokenType::END_OF_METHOD)
                {
                    arguments.push_back(parseExpression());
                }
                if (!obj)
                {
                    debug_throw_this("无效的对象!");
                }
                return std::make_shared<ASTClassFunctionCall>(obj, findFun(methodId, spaceId), spaceId, flag, arguments);
            }

            debug_throw_this("未知的调用函数!");
        }


    public:
        Parser(const std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::GlobalVar>>& GlobalVarMap,
            const std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::FinalAssemblyFunction>>& GlobalFunMap,
            const std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::FianlShareFunction>>& ShareFunMap,
            std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::Struct>> EplStructMap

        ) :m_GlobalVarMap(GlobalVarMap),
            m_GlobalFunMap(GlobalFunMap),
            m_ShareFunMap(ShareFunMap),
            m_EplStructMap(EplStructMap)
        {};

        auto parse(const std::vector<unsigned char>& inputTokens,
            const std::shared_ptr<eplcltype::FinalAssemblyFunction>& fun
        ) -> AST {
            m_ms = MemStream(inputTokens);
            m_fun = fun;
            AST ast;
#ifdef _DEBUG
            auto chick_name = [&](const std::uint32_t& name) {

                if (m_fun->getID() == name) {
                    eprogramutils::appio::ioput("find");
                }
                };
            chick_name(67287201);
#endif // _DEBUG
            //  eprogramutils::appio::ioput(L"正在转译子程序:",  ansi2wstr(fun->getSapce()->getName()),L"::", ansi2wstr(fun->getName()), L" id:", fun->getID(), "\n\n");
            while (!m_ms.is_end()) {
                ast.emplace_back(parseStatement());
                //  debug_put(ast.back()->generateCode());
            }
            return ast;
        }
    private:
        std::shared_ptr<eplcltype::FinalAssemblyFunction> m_fun;
        std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::GlobalVar>> m_GlobalVarMap;
        std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::FinalAssemblyFunction>> m_GlobalFunMap;
        std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::FianlShareFunction>> m_ShareFunMap;
        std::unordered_map<std::uint32_t, std::shared_ptr<eplcltype::Struct>> m_EplStructMap;

        auto findVar(const std::uint32_t& id) -> std::shared_ptr<eplcltype::BaseAbstract> {
            using param = std::shared_ptr<eplcltype::Param>;
            using var = std::shared_ptr<eplcltype::Var>;
            using var_arry = std::vector<var>;
            /*搜索深度为参数->局部变量->集变量->全局变量*/
            auto params = m_fun->getParams();
            var_arry
                local_var = m_fun->getVars(),
                assembly_var = m_fun->getSapce()->getVars();
            /*开始使用find_if搜索数组*/
            /*首先搜索参数*/
            auto params_it = std::find_if(params.begin(), params.end(), [&](const param& v) {return v->getID() == id; });
            if (params_it != params.end()) return *params_it;
            /*然受搜索局部变量*/
            auto findvar = [&](const var& v) {return v->getID() == id; };
            auto local_var_it = std::find_if(local_var.begin(), local_var.end(), findvar);
            if (local_var_it != local_var.end()) return *local_var_it;
            /*其次搜索集变量*/
            auto assembly_var_it = std::find_if(assembly_var.begin(), assembly_var.end(), findvar);
            if (assembly_var_it != assembly_var.end()) return *assembly_var_it;
            /*最后搜索全局*/
            if (m_GlobalVarMap.contains(id)) return m_GlobalVarMap[id];
            debug_throw_this("未知的变量");
        }
        auto findFun(const std::uint32_t& id, std::int16_t spec_id) -> std::shared_ptr<eplcltype::BaseCodeObj> {
            //函数在对应域 >0：支持库索引，0：核心运行时，-1：表达式语句，-2：用户定义子程序，-3：外部DLL命令
            using fun = std::shared_ptr<eplcltype::FinalAssemblyFunction>;
            if (spec_id == -3) {
                if (m_ShareFunMap.contains(id)) return  m_ShareFunMap[id];
            }
            else if (spec_id == -2) {
                debug_put(id);
                if (m_GlobalFunMap.contains(id)) return m_GlobalFunMap[id];
            }
            else if (spec_id == -1) {
                return std::make_shared<eplcltype::FinalLibFun>(id);
            }
            else if (spec_id >= 0) {
                return std::make_shared<eplcltype::FinalLibFun>(id);
            }
            debug_throw_this("未知的函数");
        }
        /*主要用于寻找自定义数据类型*/
        std::shared_ptr<eplcltype::Struct> findDataType(const std::uint32_t& id) {
            if (m_EplStructMap.contains(id)) return m_EplStructMap[id];
            debug_throw_this("未知的数据类型");
        }
    };
}

#undef debug_throw_this
#endif // !EPL_AST_HPP