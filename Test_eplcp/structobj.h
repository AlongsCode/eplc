struct 数据类型1{
int 整数型{};
epl_core_runtime::eplarray<int> 测试文本数组{97};
EplMemBin 字节集{};
struct RealType{
int 整数型{};
int 测试文本数组[97]{};
unsigned char* 字节集{};
};
RealType* GetRealPtr()
{
RealType* realtype = new RealType{};
realtype->整数型 = this->整数型;
memcpy(realtype->测试文本数组,this->测试文本数组.data(),std::min(this->测试文本数组.size(),std::size(realtype->测试文本数组));
realtype->字节集 = this->字节集.data();
return realtype;
};
void CopyFromReal(RealType* realtype,bool needFree)
{
this->整数型 = realtype->整数型;
this->CopyFromReal测试文本数组(realtype->测试文本数组);
if(needFree && realtype!=nullptr)
 delete realtype; };
};
