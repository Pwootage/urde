#include "CDamageInfo.hpp"
#include "CDamageVulnerability.hpp"
#include "DataSpec/DNACommon/Tweaks/ITweakPlayerGun.hpp"

namespace urde
{

CDamageInfo::CDamageInfo(const DataSpec::SShotParam& other)
: x0_weaponMode(CWeaponMode(EWeaponType(other.weaponType), other.Charged(), other.Comboed(), other.InstaKill()))
, x8_damage(other.damage)
, xc_radiusDamage(other.radiusDamage)
, x10_radius(other.radius)
, x14_knockback(other.knockback)
{
}

CDamageInfo& CDamageInfo::operator=(const DataSpec::SShotParam& other)
{
    x0_weaponMode = CWeaponMode(EWeaponType(other.weaponType), other.Charged(), other.Comboed(), other.InstaKill());
    x8_damage = other.damage;
    xc_radiusDamage = x8_damage;
    x10_radius = other.radius;
    x14_knockback = other.knockback;
    x18_ = false;
    return *this;
}

float CDamageInfo::GetDamage(const CDamageVulnerability& dVuln)
{
    EVulnerability vuln = dVuln.GetVulnerability(x0_weaponMode, false);
    if (vuln == EVulnerability::Reflect)
        return 0.f;
    else if (vuln == EVulnerability::DoubleDamage)
        return 2.f * x8_damage;

    return x8_damage;
}

float CDamageInfo::GetRadiusDamage(const CDamageVulnerability& dVuln)
{
    EVulnerability vuln = dVuln.GetVulnerability(x0_weaponMode, false);
    if (vuln == EVulnerability::Reflect)
        return 0.f;
    else if (vuln == EVulnerability::DoubleDamage)
        return 2.f * xc_radiusDamage;

    return xc_radiusDamage;
}

}
