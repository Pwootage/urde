#ifndef __URDE_CTEXTURE_HPP__
#define __URDE_CTEXTURE_HPP__

#include "GCNTypes.hpp"
#include "CFactoryMgr.hpp"
#include "IObj.hpp"
#include "IOStreams.hpp"
#include "Graphics/CGraphics.hpp"
#include "boo/graphicsdev/IGraphicsDataFactory.hpp"

namespace urde
{
class CVParamTransfer;

class CTexture
{
public:
    enum class EFontType
    {
        None = -1,
        OneLayer = 0, /* Fill bit0 */
        OneLayerOutline = 1, /* Fill bit0, Outline bit1 */
        FourLayers = 2,
        TwoLayersOutlines = 3, /* Fill bit0/2, Outline bit1/3 */
        TwoLayers = 4, /* Fill bit0/1 and copied to bit2/3 */
        TwoLayersOutlines2 = 8 /* Fill bit2/3, Outline bit0/1 */
    };

private:
    ETexelFormat x0_fmt;
    u16 x4_w;
    u16 x6_h;
    u32 x8_mips;
    boo::GraphicsDataToken m_booToken;
    boo::ITexture* m_booTex;
    boo::ITexture* m_paletteTex;
    std::unique_ptr<u8[]> m_otex;
    EFontType m_ftype = EFontType::None;

    size_t ComputeMippedTexelCount();
    size_t ComputeMippedBlockCountDXT1();
    void BuildI4FromGCN(CInputStream& in);
    void BuildI8FromGCN(CInputStream& in);
    void BuildIA4FromGCN(CInputStream& in);
    void BuildIA8FromGCN(CInputStream& in);
    void BuildC4FromGCN(CInputStream& in);
    void BuildC8FromGCN(CInputStream& in);
    void BuildC14X2FromGCN(CInputStream& in);
    void BuildRGB565FromGCN(CInputStream& in);
    void BuildRGB5A3FromGCN(CInputStream& in);
    void BuildRGBA8FromGCN(CInputStream& in);
    void BuildDXT1FromGCN(CInputStream& in);
    void BuildRGBA8(const void* data, size_t length);
    void BuildC8(const void* data, size_t length);
    void BuildC8Font(const void* data, EFontType ftype);

public:
    CTexture(ETexelFormat, s16, s16, s32);
    CTexture(std::unique_ptr<u8[]>&& in, u32 length, bool otex);
    enum class EClampMode
    {
        None,
        One
    };
    ETexelFormat GetTexelFormat() const {return x0_fmt;}
    ETexelFormat GetMemoryCardTexelFormat() const
    {return x0_fmt == ETexelFormat::C8PC ? ETexelFormat::C8 : ETexelFormat::RGB5A3;}
    u16 GetWidth() const {return x4_w;}
    u16 GetHeight() const {return x6_h;}
    void Load(int slot, EClampMode clamp) const;
    boo::ITexture* GetBooTexture() {return m_booTex;}
    boo::ITexture* GetPaletteTexture() {return m_paletteTex;}
    std::unique_ptr<u8[]> BuildMemoryCardTex(u32& sizeOut, ETexelFormat& fmtOut,
                                             std::unique_ptr<u8[]>& paletteOut) const;
    boo::ITexture* GetFontTexture(EFontType tp);
};

CFactoryFnReturn FTextureFactory(const urde::SObjectTag& tag,
                                 std::unique_ptr<u8[]>&& in, u32 len,
                                 const urde::CVParamTransfer& vparms,
                                 CObjectReference* selfRef);

}

#endif // __URDE_CTEXTURE_HPP__
