#ifndef EPL_FOLDERSECTION_HPP
#define EPL_FOLDERSECTION_HPP
#include"tace.hpp"
#include"utils.hpp"

struct EplCodeFolderInfo
{
    bool Expand{};
    int Key{};;
    int ParentKey{};;
    std::string Name;
    std::vector<uint32_t> Children;/*子程序集页ID*/
    EplCodeFolderInfo() = default;
    EplCodeFolderInfo(int key) : Key(key) {}
};
class EplFolderSection :public eprogramutils::EplStreamObj {
	int allocatedKey = 0;//分配的key
    std::vector<EplCodeFolderInfo> codeFolderInfos;
public:
	EplFolderSection() = default;
	virtual void Serialize(eprogramutils::MemArchiveStream& s) override {
		s >> allocatedKey;
        while (!s.is_end())
        {
            bool expand = s.read<uint32_t>() != 0;
            EplCodeFolderInfo info(s.read<uint32_t>());
            info.Expand = expand;
            info.ParentKey = s.read<uint32_t>();
            info.Name = s.get_mfc_string();
            info.Children = s.get_mfc_byte_vec<uint32_t>();
            codeFolderInfos.push_back(info);
        }
	}
    auto getFolders()-> const std::vector<EplCodeFolderInfo> {
		return codeFolderInfos;
	}
};



#endif