#include "CTexturedQuadFilter.hpp"
#include "Graphics/CTexture.hpp"

namespace urde
{

CTexturedQuadFilter::CTexturedQuadFilter(boo::ITexture* tex)
: m_booTex(tex)
{
}

CTexturedQuadFilter::CTexturedQuadFilter(CCameraFilterPass::EFilterType type, boo::ITexture* tex)
: m_booTex(tex)
{
    m_token = CGraphics::g_BooFactory->commitTransaction([&](boo::IGraphicsDataFactory::Context& ctx) -> bool
    {
        m_vbo = ctx.newDynamicBuffer(boo::BufferUse::Vertex, 32, 4);
        m_uniBuf = ctx.newDynamicBuffer(boo::BufferUse::Uniform, sizeof(Uniform), 1);
        m_dataBind = TMultiBlendShader<CTexturedQuadFilter>::BuildShaderDataBinding(ctx, type, *this);
        return true;
    });
}

CTexturedQuadFilter::CTexturedQuadFilter(CCameraFilterPass::EFilterType type,
                                         TLockedToken<CTexture> tex)
: CTexturedQuadFilter(type, (tex ? tex->GetBooTexture() : nullptr))
{
    m_tex = tex;
}

void CTexturedQuadFilter::draw(const zeus::CColor& color, float uvScale, const zeus::CRectangle& rect)
{
    Vert verts[4] =
    {
        {{0.f, 0.f}, {0.f, 0.f}},
        {{0.f, 1.f}, {0.f, 1.f}},
        {{1.f, 0.f}, {1.f, 0.f}},
        {{1.f, 1.f}, {1.f, 1.f}},
    };
    m_vbo->load(verts, sizeof(verts));

    m_uniform.m_matrix[0][0] = rect.size.x * 2.f;
    m_uniform.m_matrix[1][1] = rect.size.y * 2.f;
    m_uniform.m_matrix[3][0] = rect.position.x * 2.f - 1.f;
    m_uniform.m_matrix[3][1] = rect.position.y * 2.f - 1.f;
    m_uniform.m_color = color;
    m_uniform.m_uvScale = uvScale;
    m_uniBuf->load(&m_uniform, sizeof(m_uniform));

    CGraphics::g_BooMainCommandQueue->setShaderDataBinding(m_dataBind);
    CGraphics::g_BooMainCommandQueue->draw(0, 4);
}

void CTexturedQuadFilter::drawCropped(const zeus::CColor& color, float uvScale)
{
    float xFac = CGraphics::g_CroppedViewport.xc_width / float(g_Viewport.x8_width);
    float yFac = CGraphics::g_CroppedViewport.x10_height / float(g_Viewport.xc_height);
    float xBias = CGraphics::g_CroppedViewport.x4_left / float(g_Viewport.x8_width);
    float yBias = CGraphics::g_CroppedViewport.x8_top / float(g_Viewport.xc_height);

    Vert verts[4] =
    {
        {{-1.0, -1.0}, {xBias, yBias}},
        {{-1.0,  1.0}, {xBias, yBias + yFac}},
        {{ 1.0, -1.0}, {xBias + xFac, yBias}},
        {{ 1.0,  1.0}, {xBias + xFac, yBias + yFac}},
    };
    m_vbo->load(verts, sizeof(verts));

    m_uniform.m_color = color;
    m_uniform.m_uvScale = uvScale;
    m_uniBuf->load(&m_uniform, sizeof(m_uniform));

    CGraphics::g_BooMainCommandQueue->setShaderDataBinding(m_dataBind);
    CGraphics::g_BooMainCommandQueue->draw(0, 4);
}

const zeus::CRectangle CTexturedQuadFilter::DefaultRect = {0.f, 0.f, 1.f, 1.f};

void CTexturedQuadFilter::Shutdown() {}

URDE_SPECIALIZE_MULTI_BLEND_SHADER(CTexturedQuadFilter)

CTexturedQuadFilterAlpha::CTexturedQuadFilterAlpha(CCameraFilterPass::EFilterType type, boo::ITexture* tex)
: CTexturedQuadFilter(tex)
{
    m_token = CGraphics::g_BooFactory->commitTransaction([&](boo::IGraphicsDataFactory::Context& ctx) -> bool
    {
        m_vbo = ctx.newDynamicBuffer(boo::BufferUse::Vertex, 32, 4);
        m_uniBuf = ctx.newDynamicBuffer(boo::BufferUse::Uniform, sizeof(Uniform), 1);
        m_dataBind = TMultiBlendShader<CTexturedQuadFilterAlpha>::BuildShaderDataBinding(ctx, type, *this);
        return true;
    });
}

CTexturedQuadFilterAlpha::CTexturedQuadFilterAlpha(CCameraFilterPass::EFilterType type,
                                                   TLockedToken<CTexture> tex)
: CTexturedQuadFilterAlpha(type, (tex ? tex->GetBooTexture() : nullptr))
{
    m_tex = tex;
}

void CTexturedQuadFilterAlpha::Shutdown() {}

URDE_SPECIALIZE_MULTI_BLEND_SHADER(CTexturedQuadFilterAlpha)

}
