set(WEAPON_SOURCES
    WeaponCommon.hpp
    CWeaponMgr.hpp CWeaponMgr.cpp
    CGunController.hpp CGunController.cpp
    CGunMotion.hpp CGunMotion.cpp
    CGrappleArm.hpp CGrappleArm.cpp
    CWeaponMode.hpp CWeaponMode.cpp
    CPlayerGun.hpp CPlayerGun.cpp
    CGunWeapon.hpp CGunWeapon.cpp
    CAuxWeapon.hpp CAuxWeapon.cpp
    CPowerBeam.hpp CPowerBeam.cpp
    CIceBeam.hpp CIceBeam.cpp
    CWaveBeam.hpp CWaveBeam.cpp
    CPlasmaBeam.hpp CPlasmaBeam.cpp
    CPhazonBeam.hpp CPhazonBeam.cpp
    CGSFreeLook.hpp CGSFreeLook.cpp
    CGSComboFire.hpp CGSComboFire.cpp
    CGSFidget.hpp CGSFidget.cpp
    CFidget.hpp CFidget.cpp
    CWeapon.hpp CWeapon.cpp
    CGameProjectile.hpp CGameProjectile.cpp
    CBeamProjectile.hpp CBeamProjectile.cpp
    CBeamInfo.hpp CBeamInfo.cpp
    CPlasmaProjectile.hpp CPlasmaProjectile.cpp
    CEnergyProjectile.cpp CEnergyProjectile.cpp
    CExplosion.hpp CExplosion.cpp
    CBomb.hpp CBomb.cpp
    CPowerBomb.hpp CPowerBomb.cpp)

runtime_add_list(Weapon WEAPON_SOURCES)
