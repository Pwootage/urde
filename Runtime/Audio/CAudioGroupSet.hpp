#ifndef __URDE_CAUDIOGROUPSET_HPP__
#define __URDE_CAUDIOGROUPSET_HPP__

#include "RetroTypes.hpp"
#include "CFactoryMgr.hpp"
#include "IObj.hpp"
#include "CToken.hpp"
#include "IOStreams.hpp"
#include "amuse/AudioGroupData.hpp"

namespace urde
{

class CAudioGroupSet
{
    std::unique_ptr<u8[]> m_buffer;
    std::string x10_baseName;
    std::string x20_name;
    amuse::AudioGroupData m_data;
    amuse::AudioGroupData LoadData();
public:
    CAudioGroupSet(std::unique_ptr<u8[]>&& in);
    const amuse::AudioGroupData& GetAudioGroupData() const {return m_data;}
    const std::string& GetName() const { return x20_name; }
};

CFactoryFnReturn FAudioGroupSetDataFactory(const urde::SObjectTag& tag,
                                           std::unique_ptr<u8[]>&& in, u32 len,
                                           const urde::CVParamTransfer& vparms,
                                           CObjectReference* selfRef);

}

#endif // __URDE_CAUDIOGROUPSET_HPP__
