#include "CCameraBlurFilter.hpp"
#include "Graphics/CBooRenderer.hpp"
#include "GameGlobalObjects.hpp"

namespace urde
{

static const char* VS =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertData\n"
"{\n"
"    float4 posIn [[ attribute(0) ]];\n"
"    float4 uvIn [[ attribute(1) ]];\n"
"};\n"
"\n"
"struct CameraBlurUniform\n"
"{\n"
"    float4 uv0;\n"
"    float4 uv1;\n"
"    float4 uv2;\n"
"    float4 uv3;\n"
"    float4 uv4;\n"
"    float4 uv5;\n"
"    float opacity;\n"
"};\n"
"\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float2 uvReg;\n"
"    float2 uv0;\n"
"    float2 uv1;\n"
"    float2 uv2;\n"
"    float2 uv3;\n"
"    float2 uv4;\n"
"    float2 uv5;\n"
"    float opacity;\n"
"};\n"
"\n"
"vertex VertToFrag vmain(VertData v [[ stage_in ]], constant CameraBlurUniform& cbu [[ buffer(2) ]])\n"
"{\n"
"    VertToFrag vtf;\n"
"    vtf.uvReg = v.uvIn.xy;\n"
"    vtf.uvReg.y = -vtf.uvReg.y;\n"
"    vtf.uv0 = cbu.uv0.xy + v.uvIn.xy;\n"
"    vtf.uv0.y = -vtf.uv0.y;\n"
"    vtf.uv1 = cbu.uv1.xy + v.uvIn.xy;\n"
"    vtf.uv1.y = -vtf.uv1.y;\n"
"    vtf.uv2 = cbu.uv2.xy + v.uvIn.xy;\n"
"    vtf.uv2.y = -vtf.uv2.y;\n"
"    vtf.uv3 = cbu.uv3.xy + v.uvIn.xy;\n"
"    vtf.uv3.y = -vtf.uv3.y;\n"
"    vtf.uv4 = cbu.uv4.xy + v.uvIn.xy;\n"
"    vtf.uv4.y = -vtf.uv4.y;\n"
"    vtf.uv5 = cbu.uv5.xy + v.uvIn.xy;\n"
"    vtf.uv5.y = -vtf.uv5.y;\n"
"    vtf.opacity = cbu.opacity;\n"
"    vtf.position = float4(v.posIn.xyz, 1.0);\n"
"    return vtf;\n"
"}\n";

static const char* FS =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"constexpr sampler samp(address::repeat, filter::linear);\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float2 uvReg;\n"
"    float2 uv0;\n"
"    float2 uv1;\n"
"    float2 uv2;\n"
"    float2 uv3;\n"
"    float2 uv4;\n"
"    float2 uv5;\n"
"    float opacity;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]], texture2d<float> sceneTex [[ texture(0) ]])\n"
"{\n"
"    float4 colorSample = sceneTex.sample(samp, vtf.uvReg) * 0.14285715;\n"
"    colorSample += sceneTex.sample(samp, vtf.uv0) * 0.14285715;\n"
"    colorSample += sceneTex.sample(samp, vtf.uv1) * 0.14285715;\n"
"    colorSample += sceneTex.sample(samp, vtf.uv2) * 0.14285715;\n"
"    colorSample += sceneTex.sample(samp, vtf.uv3) * 0.14285715;\n"
"    colorSample += sceneTex.sample(samp, vtf.uv4) * 0.14285715;\n"
"    colorSample += sceneTex.sample(samp, vtf.uv5) * 0.14285715;\n"
"    return float4(colorSample.rgb, vtf.opacity);\n"
"}\n";

URDE_DECL_SPECIALIZE_SHADER(CCameraBlurFilter)

static boo::IVertexFormat* s_VtxFmt = nullptr;
static boo::IShaderPipeline* s_Pipeline = nullptr;

struct CCameraBlurFilterMetalDataBindingFactory : TShader<CCameraBlurFilter>::IDataBindingFactory
{
    boo::IShaderDataBinding* BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                                                    CCameraBlurFilter& filter)
    {
        boo::MetalDataFactory::Context& cctx = static_cast<boo::MetalDataFactory::Context&>(ctx);

        boo::IGraphicsBuffer* bufs[] = {filter.m_uniBuf};
        boo::ITexture* texs[] = {CGraphics::g_SpareTexture};
        return cctx.newShaderDataBinding(s_Pipeline, s_VtxFmt,
                                         filter.m_vbo, nullptr, nullptr, 1, bufs,
                                         nullptr, nullptr, nullptr, 1, texs);
    }
};

TShader<CCameraBlurFilter>::IDataBindingFactory* CCameraBlurFilter::Initialize(boo::MetalDataFactory::Context& ctx)
{
    const boo::VertexElementDescriptor VtxVmt[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4},
        {nullptr, nullptr, boo::VertexSemantic::UV4}
    };
    s_VtxFmt = ctx.newVertexFormat(2, VtxVmt);
    s_Pipeline = ctx.newShaderPipeline(VS, FS, s_VtxFmt, CGraphics::g_ViewportSamples, boo::BlendFactor::SrcAlpha,
                                       boo::BlendFactor::InvSrcAlpha, boo::Primitive::TriStrips, false, false,
                                       boo::CullMode::None);
    return new CCameraBlurFilterMetalDataBindingFactory;
}

}
