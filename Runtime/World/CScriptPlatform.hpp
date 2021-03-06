#ifndef __URDE_CSCRIPTPLATFORM_HPP__
#define __URDE_CSCRIPTPLATFORM_HPP__

#include "CPhysicsActor.hpp"
#include "optional.hpp"
#include "CToken.hpp"

namespace urde
{
class CCollidableOBBTreeGroup;

class CScriptPlatform : public CPhysicsActor
{
public:
    CScriptPlatform(TUniqueId, const std::string& name, const CEntityInfo& info, const zeus::CTransform& xf,
                    CModelData&& mData, const CActorParameters& actParms, const zeus::CAABox& aabb, float, bool, float,
                    bool, const CHealthInfo& hInfo, const CDamageVulnerability& dInfo,
                    const rstl::optional_object<TLockedToken<CCollidableOBBTreeGroup>>& dcln, bool, u32, u32);

    void Accept(IVisitor& visitor);
};
}

#endif // __URDE_CSCRIPTPLATFORM_HPP__
