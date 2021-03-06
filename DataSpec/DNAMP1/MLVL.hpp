#ifndef __DNAMP1_MLVL_HPP__
#define __DNAMP1_MLVL_HPP__

#include "../DNACommon/PAK.hpp"
#include "../DNACommon/MLVL.hpp"
#include "DNAMP1.hpp"

namespace DataSpec
{
namespace DNAMP1
{

struct MLVL : BigYAML
{
    DECL_YAML
    Value<atUint32> magic;
    Value<atUint32> version;
    UniqueID32 worldNameId;
    UniqueID32 saveWorldId;
    UniqueID32 worldSkyboxId;

    Value<atUint32> memRelayLinkCount;
    struct MemRelayLink : BigYAML
    {
        DECL_YAML
        Value<atUint32> memRelayId;
        Value<atUint32> targetId;
        Value<atUint16> msg;
        Value<bool> active;
    };
    Vector<MemRelayLink, DNA_COUNT(memRelayLinkCount)> memRelayLinks;

    Value<atUint32> areaCount;
    Value<atUint32> unknown1;
    struct Area : BigYAML
    {
        DECL_YAML
        UniqueID32 areaNameId;
        Value<atVec4f> transformMtx[3];
        Value<atVec3f> aabb[2];
        UniqueID32 areaMREAId;
        Value<atUint32> areaId;

        Value<atUint32> attachedAreaCount;
        Vector<atUint16, DNA_COUNT(attachedAreaCount)> attachedAreas;
        Value<atUint32> padding;

        Value<atUint32> depCount;
        struct Dependency : BigYAML
        {
            DECL_YAML
            UniqueID32 id;
            DNAFourCC type;

            Dependency() = default;
            Dependency(const UniqueID32& idin, const hecl::FourCC& fcc)
            : id(idin), type(fcc) {}
        };
        Vector<Dependency, DNA_COUNT(depCount)> deps;

        Value<atUint32> depLayerCount;
        Vector<atUint32, DNA_COUNT(depLayerCount)> depLayers;

        Value<atUint32> dockCount;
        struct Dock : BigYAML
        {
            DECL_YAML
            Value<atUint32> endpointCount;
            struct Endpoint : BigYAML
            {
                DECL_YAML
                Value<atUint32> areaIdx;
                Value<atUint32> dockIdx;
            };
            Vector<Endpoint, DNA_COUNT(endpointCount)> endpoints;

            Value<atUint32> planeVertCount;
            Vector<atVec3f, DNA_COUNT(planeVertCount)> planeVerts;
        };
        Vector<Dock, DNA_COUNT(dockCount)> docks;
    };
    Vector<Area, DNA_COUNT(areaCount)> areas;

    void finishLastArea()
    {
        if (areas.size())
        {
            MLVL::Area& areaLast = areas.back();
            areaLast.attachedAreaCount = areaLast.attachedAreas.size();
            areaLast.depCount = areaLast.deps.size();
            areaLast.depLayerCount = areaLast.depLayers.size();
            areaLast.dockCount = areaLast.docks.size();
        }
    }

    UniqueID32 worldMap;
    Value<atUint8> unknown2;
    Value<atUint32> unknown3;

    Value<atUint32> audioGroupCount;
    struct AudioGroup : BigYAML
    {
        DECL_YAML
        Value<atUint32> groupId;
        UniqueID32 agscId;
    };
    Vector<AudioGroup, DNA_COUNT(audioGroupCount)> audioGroups;
    String<-1> unkString;

    Value<atUint32> layerFlagCount;
    struct LayerFlags : BigYAML
    {
        DECL_YAML
        Value<atUint32> layerCount;
        Value<atUint64> flags;
    };
    Vector<LayerFlags, DNA_COUNT(layerFlagCount)> layerFlags;

    Value<atUint32> layerNameCount;
    Vector<String<-1>, DNA_COUNT(layerNameCount)> layerNames;

    Value<atUint32> layerNameOffsetCount;
    Vector<atUint32, DNA_COUNT(layerNameOffsetCount)> layerNameOffsets;

    void readMeta(athena::io::YAMLDocReader& __dna_docin)
    {
        /* worldSkyboxId */
        __dna_docin.enumerate("worldSkyboxId", worldSkyboxId);
        /* audioGroupCount squelched */
        /* audioGroups */
        audioGroupCount = __dna_docin.enumerate("audioGroups", audioGroups);
    }

    void writeMeta(athena::io::YAMLDocWriter& __dna_docout) const
    {
        /* worldSkyboxId */
        __dna_docout.enumerate("worldSkyboxId", worldSkyboxId);
        /* audioGroupCount squelched */
        /* audioGroups */
        __dna_docout.enumerate("audioGroups", audioGroups);
    }

    static bool Extract(const SpecBase& dataSpec,
                        PAKEntryReadStream& rs,
                        const hecl::ProjectPath& outPath,
                        PAKRouter<PAKBridge>& pakRouter,
                        const PAK::Entry& entry,
                        bool force,
                        hecl::BlenderToken& btok,
                        std::function<void(const hecl::SystemChar*)> fileChanged);

    using World = hecl::BlenderConnection::DataStream::World;

    static bool Cook(const hecl::ProjectPath& outPath,
                     const hecl::ProjectPath& inPath,
                     const World& wld,
                     hecl::BlenderToken& btok);

    static bool CookMAPW(const hecl::ProjectPath& outPath,
                         const World& wld,
                         hecl::BlenderToken& btok);

    static bool CookSAVW(const hecl::ProjectPath& outPath,
                         const World& wld);
};

}
}

#endif // __DNAMP1_MLVL_HPP__
