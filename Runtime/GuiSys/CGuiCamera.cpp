#include "CGuiCamera.hpp"
#include "CGuiFrame.hpp"
#include "Graphics/CGraphics.hpp"
#include "CGuiWidgetDrawParms.hpp"

namespace urde
{

CGuiCamera::CGuiCamera(const CGuiWidgetParms& parms,
                       float left, float right,
                       float top, float bottom,
                       float znear, float zfar)
: CGuiWidget(parms), xf8_proj(EProjection::Orthographic),
  xfc_left(left), x100_right(right),
  x104_top(top), x108_bottom(bottom),
  x10c_znear(znear), x110_zfar(zfar)
{}

CGuiCamera::CGuiCamera(const CGuiWidgetParms& parms,
                       float fov, float aspect,
                       float znear, float zfar)
: CGuiWidget(parms), xf8_proj(EProjection::Perspective),
  xfc_fov(fov), x100_aspect(aspect),
  x104_znear(znear), x108_zfar(zfar)
{}

zeus::CVector3f CGuiCamera::ConvertToScreenSpace(const zeus::CVector3f& vec) const
{
    zeus::CVector3f local = RotateTranslateW2O(vec);
    if (local.isZero())
        return {-1.f, -1.f, 1.f};

    zeus::CMatrix4f mat = CGraphics::CalculatePerspectiveMatrix(xfc_fov, x100_aspect,
                                                                x104_znear, x108_zfar,
                                                                false);
    return mat.multiplyOneOverW(local);
}

void CGuiCamera::Draw(const CGuiWidgetDrawParms& parms) const
{
    if (xf8_proj == EProjection::Perspective)
        CGraphics::SetPerspective(xfc_fov, x100_aspect, x104_znear, x108_zfar);
    else
        CGraphics::SetOrtho(xfc_left, x100_right, x104_top, x108_bottom, x10c_znear, x110_zfar);
    CGraphics::SetViewPointMatrix(GetGuiFrame()->GetAspectTransform() *
                                  zeus::CTransform::Translate(parms.x4_cameraOffset) * x34_worldXF);
    CGuiWidget::Draw(parms);
}

std::shared_ptr<CGuiWidget> CGuiCamera::Create(CGuiFrame* frame, CInputStream& in, CSimplePool* sp)
{
    CGuiWidgetParms parms = ReadWidgetHeader(frame, in);
    EProjection proj = EProjection(in.readUint32Big());
    std::shared_ptr<CGuiCamera> ret = {};
    switch (proj)
    {
    case EProjection::Perspective:
    {
        float fov = in.readFloatBig();
        float aspect = in.readFloatBig();
        float znear = in.readFloatBig();
        float zfar = in.readFloatBig();
        ret = std::make_shared<CGuiCamera>(parms, fov, aspect, znear, zfar);
        break;
    }
    case EProjection::Orthographic:
    {
        float left = in.readFloatBig();
        float right = in.readFloatBig();
        float top = in.readFloatBig();
        float bottom = in.readFloatBig();
        float znear = in.readFloatBig();
        float zfar = in.readFloatBig();
        ret = std::make_shared<CGuiCamera>(parms, left, right, top, bottom, znear, zfar);
        break;
    }
    break;
    }
    frame->SetFrameCamera(ret->shared_from_this());
    ret->ParseBaseInfo(frame, in, parms);
    return ret;
}

}
