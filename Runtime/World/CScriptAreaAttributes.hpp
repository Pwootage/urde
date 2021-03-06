#ifndef __URDE_CSCRIPTAREAATTRIBUTES_HPP__
#define __URDE_CSCRIPTAREAATTRIBUTES_HPP__

#include "CEntity.hpp"
#include "CEnvFxManager.hpp"

namespace urde
{
class CScriptAreaAttributes : public CEntity
{
    bool x34_24_showSkybox : 1;
    EEnvFxType x38_envFx;
    float x3c_envFxDensity;
    float x40_thermalHeat;
    float x44_xrayFogDistance;
    float x48_worldLightingLevel;
    ResId x4c_skybox;
    EPhazonType x50_phazon;

public:
    CScriptAreaAttributes(TUniqueId uid, const CEntityInfo& info, bool showSkybox, EEnvFxType fxType,
                          float envFxDensity, float thermalHeat, float xrayFogDistance, float worldLightingLevel,
                          ResId skybox, EPhazonType phazonType);

    void Accept(IVisitor& visitor);
    void AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId objId, CStateManager& stateMgr);


    bool GetNeedsSky() const { return x34_24_showSkybox; }
    bool GetNeedsEnvFx() const { return x38_envFx != EEnvFxType::None; }
    float GetEnvFxDensity() const { return x3c_envFxDensity; }
    float GetThermalHeat() const { return x40_thermalHeat; }
    float GetXRayFogDistance() const { return x44_xrayFogDistance; }
    float GetWorldLightingLevel() const { return x48_worldLightingLevel; }
};
}

#endif // __URDE_CSCRIPTAREAATTRIBUTES_HPP__
