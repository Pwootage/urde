#ifndef __URDE_CTARGETINGMANAGER_HPP__
#define __URDE_CTARGETINGMANAGER_HPP__

#include "CCompoundTargetReticle.hpp"
#include "COrbitPointMarker.hpp"

namespace urde
{
class CStateManager;
class CTargetingManager
{
public:
    CTargetingManager(const CStateManager&);
    bool CheckLoadComplete();
    void Update(float, const CStateManager&);
    void Draw(const CStateManager&) const;
    void Touch() const;
};
}

#endif // __URDE_CTARGETINGMANAGER_HPP__
