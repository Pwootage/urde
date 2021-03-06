"Generates TCastTo source files for constexpr-enabled CEntity casts"

class Namespace:
    def __init__(self, name):
        self.name = name

class EndNamespace:
    pass

CENTITY_TYPES = (
    # Class, Visit, Header
    ('CActor', 'World/CActor.hpp'),
    ('CBallCamera', 'Camera/CBallCamera.hpp'),
    ('CBomb', 'Weapon/CBomb.hpp'),
    ('CCinematicCamera', 'Camera/CCinematicCamera.hpp'),
    ('CCollisionActor', 'Collision/CCollisionActor.hpp'),
    ('CDestroyableRock', 'World/CDestroyableRock.hpp'),
    ('CEnergyProjectile', 'Weapon/CEnergyProjectile.hpp'),
    ('CEntity', 'World/CEntity.hpp'),
    ('CExplosion', 'Weapon/CExplosion.hpp'),
    ('CFirstPersonCamera', 'Camera/CFirstPersonCamera.hpp'),
    ('CFishCloud', 'World/CFishCloud.hpp'),
    ('CGameCamera', 'Camera/CGameCamera.hpp'),
    ('CGameLight', 'World/CGameLight.hpp'),
    ('CGameProjectile', 'Weapon/CGameProjectile.hpp'),
    ('CHUDBillboardEffect', 'World/CHUDBillboardEffect.hpp'),
    Namespace('MP1'),
    ('CMetroidPrimeRelay', 'MP1/World/CMetroidPrimeRelay.hpp', 'MP1'),
    EndNamespace(),
    ('CPathCamera', 'Camera/CPathCamera.hpp'),
    ('CAi', 'World/CAi.hpp'),
    ('CPhysicsActor', 'World/CPhysicsActor.hpp'),
    ('CPlayer', 'World/CPhysicsActor.hpp'),
    ('CRepulsor', 'World/CRepulsor.hpp'),
    ('CScriptActor', 'World/CScriptActor.hpp'),
    ('CScriptActorKeyframe', 'World/CScriptActorKeyframe.hpp'),
    ('CScriptAiJumpPoint', 'World/CScriptAiJumpPoint.hpp'),
    ('CScriptCameraHint', 'World/CScriptCameraHint.hpp'),
    ('CScriptCameraPitchVolume', 'World/CScriptCameraPitchVolume.hpp'),
    ('CScriptCameraWaypoint', 'World/CScriptCameraWaypoint.hpp'),
    ('CScriptCoverPoint', 'World/CScriptCoverPoint.hpp'),
    ('CScriptDebugCameraWaypoint', 'World/CScriptDebugCameraWaypoint.hpp'),
    ('CScriptDistanceFog', 'World/CScriptDistanceFog.hpp'),
    ('CScriptDock', 'World/CScriptDock.hpp'),
    ('CScriptDoor', 'World/CScriptDoor.hpp'),
    ('CScriptEffect', 'World/CScriptEffect.hpp'),
    ('CScriptGrapplePoint', 'World/CScriptGrapplePoint.hpp'),
    ('CScriptGunTurret', 'World/CScriptGunTurret.hpp'),
    ('CScriptMazeNode', 'World/CScriptMazeNode.hpp'),
    ('CScriptPickup', 'World/CScriptPickup.hpp'),
    ('CScriptPlatform', 'World/CScriptPlatform.hpp'),
    ('CScriptPlayerHint', 'World/CScriptPlayerHint.hpp'),
    ('CScriptPointOfInterest', 'World/CScriptPointOfInterest.hpp'),
    ('CScriptRoomAcoustics', 'World/CScriptRoomAcoustics.hpp'),
    ('CScriptSound', 'World/CScriptSound.hpp'),
    ('CScriptMidi', 'World/CScriptMidi.hpp'),
    ('CScriptSpawnPoint', 'World/CScriptSpawnPoint.hpp'),
    ('CScriptSpecialFunction', 'World/CScriptSpecialFunction.hpp'),
    ('CScriptSpiderBallAttractionSurface', 'World/CScriptSpiderBallAttractionSurface.hpp'),
    ('CScriptSpiderBallWaypoint', 'World/CScriptSpiderBallWaypoint.hpp'),
    ('CScriptTargetingPoint', 'World/CScriptTargetingPoint.hpp'),
    ('CTeamAiMgr', 'World/CTeamAiMgr.hpp'),
    ('CScriptTimer', 'World/CScriptTimer.hpp'),
    ('CScriptTrigger', 'World/CScriptTrigger.hpp'),
    ('CScriptVisorFlare', 'World/CScriptVisorFlare.hpp'),
    ('CScriptWater', 'World/CScriptWater.hpp'),
    ('CScriptWaypoint', 'World/CScriptWaypoint.hpp'),
    ('CSnakeWeedSwarm', 'World/CSnakeWeedSwarm.hpp'),
    ('CScriptSpindleCamera', 'World/CScriptSpindleCamera.hpp'),
    ('CWallCrawlerSwarm', 'World/CWallCrawlerSwarm.hpp'),
    ('CWeapon', 'Weapon/CWeapon.hpp'),
)

def getqualified(tp):
    if len(tp) >= 3:
        return tp[2] + '::' + tp[0]
    else:
        return tp[0]

headerf = open('TCastTo.hpp', 'w')
sourcef = open('TCastTo.cpp', 'w')

headerf.write('''#ifndef __TCASTTO_HPP__
#define __TCASTTO_HPP__

namespace urde
{
class CEntity;
''')

for tp in CENTITY_TYPES:
    if type(tp) == tuple:
        headerf.write('class %s;\n' % tp[0])
    elif isinstance(tp, Namespace):
        headerf.write('namespace %s\n{\n' % tp.name)
    elif isinstance(tp, EndNamespace):
        headerf.write('}\n')

headerf.write('\nclass IVisitor\n{\npublic:\n')

for tp in CENTITY_TYPES:
    if type(tp) == tuple:
        headerf.write('    virtual void Visit(%s* p)=0;\n' % getqualified(tp))

headerf.write('''};

template <class T>
class TCastToPtr : public IVisitor
{
protected:
    T* ptr = nullptr;
public:
    TCastToPtr(CEntity* p);
    TCastToPtr(CEntity& p);

''')

for tp in CENTITY_TYPES:
    if type(tp) == tuple:
        headerf.write('    void Visit(%s* p);\n' % getqualified(tp))

headerf.write('''
    T* GetPtr() const { return ptr; }
    operator T*() const { return GetPtr(); }
    T* operator->() const { return GetPtr(); }
    operator bool() const { return ptr != nullptr; }
};

template <class T>
class TCastToConstPtr : TCastToPtr<T>
{
public:
    TCastToConstPtr(const CEntity* p) : TCastToPtr<T>(const_cast<CEntity*>(p)) {}
    TCastToConstPtr(const CEntity& p) : TCastToPtr<T>(const_cast<CEntity&>(p)) {}
    const T* GetPtr() const { return TCastToPtr<T>::ptr; }
    operator const T*() const { return GetPtr(); }
    const T* operator->() const { return GetPtr(); }
    operator bool() const { return TCastToPtr<T>::ptr != nullptr; }
};

}

#endif // __TCASTTO_HPP__
''')

headerf.close()

sourcef.write('#include "TCastTo.hpp"\n\n')

for tp in CENTITY_TYPES:
    if type(tp) == tuple:
        sourcef.write('#include "%s"\n' % tp[1])

sourcef.write('''
namespace urde
{

template <class T>
TCastToPtr<T>::TCastToPtr(CEntity* p) { p->Accept(*this); }

template <class T>
TCastToPtr<T>::TCastToPtr(CEntity& p) { p.Accept(*this); }

''')

for tp in CENTITY_TYPES:
    if type(tp) == tuple:
        qual = getqualified(tp)
        sourcef.write('''template <class T>
void TCastToPtr<T>::Visit(%s* p)
{
    ptr = reinterpret_cast<T*>(std::is_convertible<%s*, T*>::value ? p : nullptr);
}

''' % (qual, qual))

for tp in CENTITY_TYPES:
    if type(tp) == tuple:
        sourcef.write('template class TCastToPtr<%s>;\n' % getqualified(tp))

sourcef.write('\n}\n')

sourcef.close()
