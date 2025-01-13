#ifndef EPL_CLASSPUBLICITY_HPP
#define EPL_CLASSPUBLICITY_HPP
#include"tace.hpp"
#include"utils.hpp"
class EplClassPublicity :public eprogramutils::EplStreamObj {
    struct ClassPublicityInfo
    {
        int m_Class{};
        int m_Flags{};
        bool Public() const
        {
            return (m_Flags & 1) != 0;
        }
        void SetPublic(bool value)
        {
            m_Flags = (m_Flags & -2) | (value ? 1 : 0);
        }
        bool Hidden() const
        {
            return (m_Flags & 2) != 0;
        }
        void SetHidden(bool value)
        {
            m_Flags = (m_Flags & -3) | (value ? 2 : 0);
        }
    };
    std::vector<ClassPublicityInfo> m_ClassPublicities;
public:
    EplClassPublicity() = default;
    virtual void Serialize(eprogramutils::MemArchiveStream& s) override {
        this->m_ClassPublicities.resize(s.size() / 8);
        for (auto& i : this->m_ClassPublicities)
            s >> i.m_Class >> i.m_Flags;

    }
};

#endif