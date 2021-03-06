#ifndef __URDE_CSPACEWARPFILTER_HPP__
#define __URDE_CSPACEWARPFILTER_HPP__

#include "TShader.hpp"
#include "zeus/CMatrix4f.hpp"
#include "zeus/CColor.hpp"

namespace urde
{

class CSpaceWarpFilter
{
    friend struct CSpaceWarpFilterGLDataBindingFactory;
    friend struct CSpaceWarpFilterVulkanDataBindingFactory;
    friend struct CSpaceWarpFilterMetalDataBindingFactory;
    friend struct CSpaceWarpFilterD3DDataBindingFactory;

    struct Uniform
    {
        zeus::CMatrix4f m_matrix;
        zeus::CMatrix4f m_indXf;
        zeus::CVector3f m_strength;
    };
    u8 m_shiftTexture[4][8][4] = {};
    boo::GraphicsDataToken m_token;
    boo::ITexture* m_warpTex;
    boo::IGraphicsBufferS* m_vbo;
    boo::IGraphicsBufferD* m_uniBuf;
    boo::IShaderDataBinding* m_dataBind = nullptr;
    Uniform m_uniform;
    float m_strength = 1.f;

    void GenerateWarpRampTex(boo::IGraphicsDataFactory::Context& ctx);

public:
    CSpaceWarpFilter();
    void setStrength(float strength) { m_strength = strength; }
    void draw(const zeus::CVector3f& pt);

    using _CLS = CSpaceWarpFilter;
#include "TShaderDecl.hpp"
};

}

#endif // __URDE_CSPACEWARPFILTER_HPP__
