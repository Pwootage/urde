#ifndef __URDE_CGAMEHINTINFO_HPP__
#define __URDE_CGAMEHINTINFO_HPP__

#include "RetroTypes.hpp"
#include "IFactory.hpp"

namespace urde
{
class CGameHintInfo
{
public:
    struct SHintLocation
    {
        ResId x0_mlvlId = -1;
        ResId x4_mreaId = -1;
        TAreaId x8_areaId = kInvalidAreaId;
        ResId xc_stringId = -1;
        SHintLocation(CInputStream&, s32);
    };

    class CGameHint
    {
        std::string x0_name;
        float x10_immediateTime;
        float x14_normalTime;
        ResId x18_stringId;
        float x1c_textTime;
        std::vector<SHintLocation> x20_locations;
    public:
        CGameHint(CInputStream&, s32);

        float GetNormalTime() const { return x14_normalTime; }
        float GetImmediateTime() const { return x10_immediateTime; }
        float GetTextTime() const { return x1c_textTime; }
        const std::string& GetName() const { return x0_name; }
        ResId GetStringID() const { return x18_stringId; }
        const std::vector<SHintLocation>& GetLocations() const { return x20_locations; }
    };

private:
    std::vector<CGameHint> x0_hints;
public:
    CGameHintInfo(CInputStream&, s32);
    const std::vector<CGameHint>& GetHints() const { return x0_hints; }
    static int FindHintIndex(const char* str);
};

CFactoryFnReturn FHintFactory(const SObjectTag&, CInputStream&, const CVParamTransfer, CObjectReference*);
}
#endif // __URDE_CGAMEHINTINFO_HPP__
