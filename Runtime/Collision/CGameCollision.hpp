#ifndef __URDE_CGAMECOLLISION_HPP__
#define __URDE_CGAMECOLLISION_HPP__
#include "zeus/CVector3f.hpp"

namespace urde
{

class CInternalCollisionStructure
{
};

class CActor;
class CCollisionInfo;
class CCollisionInfoList;
class CMaterialList;
class CStateManager;
class CGameCollision
{
public:
    static bool NullMovingCollider(const CInternalCollisionStructure&, const zeus::CVector3f&, double&, CCollisionInfo&)
    {
        return false;
    }
    static bool NullBooleanCollider(const CInternalCollisionStructure&) { return false; }
    static bool NullCollisionCollider(const CInternalCollisionStructure&, CCollisionInfoList&) { return false; }
    static void InitCollision();

    static bool CanBlock(const CMaterialList&, const zeus::CVector3f&);
    static bool IsFloor(const CMaterialList&, const zeus::CVector3f&);
    void SendMaterialMessage(CStateManager&, const CMaterialList&, CActor&);
};
}

#endif // __URDE_CGAMECOLLISION_HPP__
