#include "CScriptSteam.hpp"

namespace urde
{

CScriptSteam::CScriptSteam(TUniqueId uid, const std::string& name, const CEntityInfo& info, const zeus::CVector3f& pos,
                           const zeus::CAABox& aabb, const CDamageInfo& dInfo, const zeus::CVector3f& orientedForce,
                           ETriggerFlags flags, bool active, ResId, float, float, float, float, bool)
: CScriptTrigger(uid, name, info, pos, aabb, dInfo, orientedForce, flags, active, false, false)
{

}

}
