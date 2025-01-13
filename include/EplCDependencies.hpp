#ifndef EPL_CDEPENDENCIES_HPP
#define EPL_CDEPENDENCIES_HPP
#include"tace.hpp"
#include"utils.hpp"

class ECDependencyInfo {
public:
    struct PackedIds {
        int Start{};
        int Count{};
        PackedIds() = default;
        PackedIds(int start, int count) : Start(start), Count(count) {}
    };

    ECDependencyInfo() : InfoVersion(2), FileLastModifiedDate(0) {}

    int InfoVersion{ 2 };
    int FileSize{};
    std::time_t FileLastModifiedDate;
    bool ReExport{};
    std::string Name;
    std::string Path;
    std::vector<PackedIds> DefinedIds;
};




class EplCDependencies :public eprogramutils::EplStreamObj
{
    uint32_t m_Num{}; /*模块数量*/
    std::vector<ECDependencyInfo> m_Dependencies;/*模块信息*/
public:
    EplCDependencies() = default;
    virtual void Serialize(eprogramutils::MemArchiveStream& s) override {
        s >> m_Num;
        m_Dependencies.resize(m_Num);
        for (auto& eCDependencyInfo : m_Dependencies)
        {
            s >> eCDependencyInfo.InfoVersion;
            if (eCDependencyInfo.InfoVersion > 2)
            {
                //put_error();
            }
            s >> eCDependencyInfo.FileSize >> eCDependencyInfo.FileLastModifiedDate;
            if (eCDependencyInfo.InfoVersion >= 2)
            {
                eCDependencyInfo.ReExport = s.read<uint32_t>() != 0;
            }
            eCDependencyInfo.Name = s.get_mfc_string();
            eCDependencyInfo.Path = s.get_mfc_string();
            auto array1 = s.get_mfc_byte_vec<uint32_t>();
            auto array2 = s.get_mfc_byte_vec<uint32_t>();

            for (size_t j = 0; j < array1.size(); j++)
            {
                auto start = array1[j];
                auto count = array2[j];
                ECDependencyInfo::PackedIds temp(start, count);
                eCDependencyInfo.DefinedIds.push_back(temp);
            }

        }

    }
};





#endif