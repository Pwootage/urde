#include "CXRayBlurFilter.hpp"
#include "Graphics/CGraphics.hpp"
#include "Graphics/CTexture.hpp"
#include "GameGlobalObjects.hpp"

namespace urde
{

CXRayBlurFilter::CXRayBlurFilter(TLockedToken<CTexture>& tex)
: m_paletteTex(tex), m_booTex(tex->GetPaletteTexture())
{
    m_token = CGraphics::g_BooFactory->commitTransaction([&](boo::IGraphicsDataFactory::Context& ctx) -> bool
    {
        struct Vert
        {
            zeus::CVector2f m_pos;
            zeus::CVector2f m_uv;
        } verts[4] =
        {
        {{-1.0, -1.0}, {0.0, 0.0}},
        {{-1.0,  1.0}, {0.0, 1.0}},
        {{ 1.0, -1.0}, {1.0, 0.0}},
        {{ 1.0,  1.0}, {1.0, 1.0}},
        };
        m_vbo = ctx.newStaticBuffer(boo::BufferUse::Vertex, verts, 32, 4);
        m_uniBuf = ctx.newDynamicBuffer(boo::BufferUse::Uniform, sizeof(Uniform), 1);
        m_dataBind = TShader<CXRayBlurFilter>::BuildShaderDataBinding(ctx, *this);
        return true;
    });
}

void CXRayBlurFilter::draw(float amount)
{
    CGraphics::ResolveSpareTexture(CGraphics::g_CroppedViewport);

    float blurL = amount * 0.0014f * 0.25f;
    float blurQ = amount * 0.0000525f * 0.25f;
    //float blurL = amount * g_tweakGui->GetXrayBlurScaleLinear() * 0.25f;
    //float blurQ = amount * g_tweakGui->GetXrayBlurScaleQuadratic() * 0.25f;

    for (int i=0 ; i<4 ; ++i)
    {
        float iflt = i;
        float uvScale = (1.f - (blurL * iflt + blurQ * iflt * iflt));
        float uvOffset = uvScale * -0.5f + 0.5f;
        m_uniform.m_uv[i][0][0] = uvScale;
        m_uniform.m_uv[i][1][1] = uvScale;
        m_uniform.m_uv[i][3][0] = uvOffset;
        m_uniform.m_uv[i][3][1] = uvOffset;
    }
    m_uniBuf->load(&m_uniform, sizeof(m_uniform));

    CGraphics::g_BooMainCommandQueue->setShaderDataBinding(m_dataBind);
    CGraphics::g_BooMainCommandQueue->draw(0, 4);
}

void CXRayBlurFilter::Shutdown() {}

URDE_SPECIALIZE_SHADER(CXRayBlurFilter)

}
