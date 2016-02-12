#include "CGraphics.hpp"
#include <Math.hpp>

namespace Retro
{

u32 CGraphics::g_NumLightsActive = 0;
ERglLight CGraphics::g_LightActive = ERglLight::None;
ERglLight CGraphics::g_LightsWereOn = ERglLight::None;

void CGraphics::DisableAllLights()
{
    g_NumLightsActive = 0;
    g_LightActive = ERglLight::None;
    // TODO: turn lights off for real
}

void CGraphics::EnableLight(ERglLight light)
{
    if ((light & g_LightActive) == ERglLight::None)
    {
        g_LightActive |= light;
        ++g_NumLightsActive;
        // TODO: turn light on for real
    }
    g_LightsWereOn = g_LightActive;
}

void CGraphics::SetLightState(ERglLight lightState)
{
    // TODO: set state for real
    g_LightActive = lightState;
    g_NumLightsActive = Zeus::Math::PopCount(lightState);
}

}
