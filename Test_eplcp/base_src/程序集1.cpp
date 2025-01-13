/*程序集1*/
#include"E:\MyProject\epl\eplcl\Test_eplcp\declare.h"
namespace 程序集1{
namespace {
epl_runtime_systype::eplstring 集_结果{};
};
//本子程序在程序启动后最先执行
int _启动子程序(){
epl_runtime_systype::eplstring 测试文本{};
(测试文本)=(epl_runtime_systype::eplstring(R"_eplcl_(用来测试一下gbk编码)_eplcl_"));
程序集1::子程序1();

krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(\\\R\R\r\n测试下)_eplcl_"))+(krnln::constant_0_3)+(epl_runtime_systype::eplstring(R"_eplcl_(你好世界!!)_eplcl_"))+(krnln::constant_0_3)+(测试文本), 20, 7, 8, 6.2000000000000002);
krnln::ioput(epl_runtime_systype::nooparg, krnln::constant_0_3);
程序集1::生成乘法表();
krnln::ioinput(epl_runtime_systype::nooparg);

return(0);//可以根据您的需要返回任意数值






};
void 生成乘法表(){
int i{};
int j{};
int 结果{};

krnln::ioput(epl_runtime_systype::nooparg, epl_runtime_systype::eplstring(R"_eplcl_(乘法表：)_eplcl_"));//输出乘法表的标题
krnln::ioput(epl_runtime_systype::nooparg, krnln::constant_0_3);

//打印表头
krnln::ioput(epl_runtime_systype::nooparg, epl_runtime_systype::eplstring(R"_eplcl_(    )_eplcl_"));//为表头留出空间
for (i = 1;i <= 9; i+=(1)) 
{krnln::ioput(epl_runtime_systype::nooparg, i);//打印表头的数字
krnln::ioput(epl_runtime_systype::nooparg, epl_runtime_systype::eplstring(R"_eplcl_(    )_eplcl_"));//给每个列标题间隔

}

krnln::ioput(epl_runtime_systype::nooparg, krnln::constant_0_3);

//打印乘法表
for (i = 1;i <= 9; i+=(1)) 
{krnln::ioput(epl_runtime_systype::nooparg, i);//打印行的数字（如1, 2, 3...）
krnln::ioput(epl_runtime_systype::nooparg, epl_runtime_systype::eplstring(R"_eplcl_(    )_eplcl_"));//每个值之间的间隔
for (j = 1;j <= 9; j+=(1)) 
{(结果)=((i)*(j));//计算乘积
krnln::ioput(epl_runtime_systype::nooparg, 结果);//输出乘积
krnln::ioput(epl_runtime_systype::nooparg, epl_runtime_systype::eplstring(R"_eplcl_(    )_eplcl_"));//每个值之间的间隔

}
krnln::ioput(epl_runtime_systype::nooparg, krnln::constant_0_3);//每一行结束后换行

}
};
void 子程序1(){
epl_runtime_systype::eplstring 局_测试一致性{};
epl_runtime_systype::eplstring 运算结果{};
程序集1::九九乘法表();
(局_测试一致性)=(krnln::to_string(程序集1::加((程序集1::加(1, 2))-((程序集1::加(3, 4))*(程序集1::加(5, 6))), 程序集1::加(2, 3))));
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(第一关: )_eplcl_"))+(krnln::constant_0_3));
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(正确答案 : -2148 运算答案 )_eplcl_"))+(局_测试一致性)+(krnln::constant_0_3));
krnln::ioput(epl_runtime_systype::nooparg, (局_测试一致性)+(krnln::constant_0_3));
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(第二关: )_eplcl_"))+(krnln::constant_0_3));
if(((true)==(false))||((true)==(false))) 
{
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(正确答案: 假 我的答案:)_eplcl_"))+(krnln::to_string(((true)==(false))||((true)==(false))))+(krnln::constant_0_3));

}
else {
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(正确答案: 假 我的答案:)_eplcl_"))+(krnln::to_string(((true)==(false))||((true)==(false))))+(krnln::constant_0_3));

}

if(((true)==(false))||((true)==(true))) 
{
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(正确答案: 真 我的答案:)_eplcl_"))+(krnln::to_string(((true)==(false))||((true)==(true))))+(krnln::constant_0_3));

}
else {
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(正确答案: 真 我的答案:)_eplcl_"))+(krnln::to_string(((true)==(false))||((true)==(true))))+(krnln::constant_0_3));

}
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(第二关: )_eplcl_"))+(krnln::constant_0_3));
(运算结果)=(krnln::to_string((krnln::to_int(程序集1::连连看(epl_runtime_systype::eplstring(R"_eplcl_(1)_eplcl_"), epl_runtime_systype::eplstring(R"_eplcl_(2)_eplcl_"))))+((krnln::to_int(程序集1::连连看(epl_runtime_systype::eplstring(R"_eplcl_(3)_eplcl_"), epl_runtime_systype::eplstring(R"_eplcl_(4)_eplcl_"))))*(krnln::to_int(程序集1::连连看(epl_runtime_systype::eplstring(R"_eplcl_(5)_eplcl_"), epl_runtime_systype::eplstring(R"_eplcl_(6)_eplcl_")))))));
krnln::ioput(epl_runtime_systype::nooparg, (epl_runtime_systype::eplstring(R"_eplcl_(正确答案2: 1 3 51916 运行答案 )_eplcl_"))+(集_结果)+(运算结果)+(krnln::constant_0_3));

};
int 加(int a,int b){
return((a)*(b));
};
epl_runtime_systype::eplstring 连连看(epl_runtime_systype::eplstring a,epl_runtime_systype::eplstring b){
(集_结果)=((集_结果)+(epl_runtime_systype::eplstring(R"_eplcl_( )_eplcl_"))+(a));
return((a)+(b));

};
void 九九乘法表(){
int I{};
int K{};
for (I = 1; I <= 9; I++) 
{for (K = 1; K <= I; K++) 
{krnln::ioput(epl_runtime_systype::nooparg, K, epl_runtime_systype::eplstring(R"_eplcl_(*)_eplcl_"), I, epl_runtime_systype::eplstring(R"_eplcl_(=)_eplcl_"), (K)*(I), epl_runtime_systype::eplstring(R"_eplcl_( )_eplcl_"));

}
krnln::ioput(epl_runtime_systype::nooparg, krnln::constant_0_3);

}
};
};
