#include "MREA.hpp"
#include "SCLY.hpp"
#include "PATH.hpp"
#include "DeafBabe.hpp"
#include "../DNACommon/BabeDead.hpp"
#include "zeus/Math.hpp"
#include "zeus/CAABox.hpp"
#include "DataSpec/DNACommon/AROTBuilder.hpp"
#include "ScriptObjects/ScriptTypes.hpp"

extern hecl::SystemString ExeDir;

namespace DataSpec
{
namespace DNAMP1
{

void MREA::ReadBabeDeadToBlender_1_2(hecl::BlenderConnection::PyOutStream& os,
                                     athena::io::IStreamReader& rs)
{
    atUint32 bdMagic = rs.readUint32Big();
    if (bdMagic != 0xBABEDEAD)
        Log.report(logvisor::Fatal, "invalid BABEDEAD magic");
    os << "bpy.context.scene.render.engine = 'CYCLES'\n"
          "bpy.context.scene.world.use_nodes = True\n"
          "bpy.context.scene.render.engine = 'BLENDER_GAME'\n"
          "bg_node = bpy.context.scene.world.node_tree.nodes['Background']\n";
    for (atUint32 s=0 ; s<2 ; ++s)
    {
        atUint32 lightCount = rs.readUint32Big();
        for (atUint32 l=0 ; l<lightCount ; ++l)
        {
            BabeDeadLight light;
            light.read(rs);
            ReadBabeDeadLightToBlender(os, light, s, l);
        }
    }
}

void MREA::AddCMDLRigPairs(PAKEntryReadStream& rs,
                           PAKRouter<PAKBridge>& pakRouter,
                           std::unordered_map<UniqueID32, std::pair<UniqueID32, UniqueID32>>& addTo)
{
    /* Do extract */
    Header head;
    head.read(rs);
    rs.seekAlign32();

    /* Skip to SCLY */
    atUint32 curSec = 0;
    atUint64 secStart = rs.position();
    while (curSec != head.sclySecIdx)
        secStart += head.secSizes[curSec++];
    rs.seek(secStart, athena::Begin);
    SCLY scly;
    scly.read(rs);
    scly.addCMDLRigPairs(pakRouter, addTo);
}

bool MREA::Extract(const SpecBase& dataSpec,
                   PAKEntryReadStream& rs,
                   const hecl::ProjectPath& outPath,
                   PAKRouter<PAKBridge>& pakRouter,
                   const PAK::Entry& entry,
                   bool force,
                   hecl::BlenderToken& btok,
                   std::function<void(const hecl::SystemChar*)>)
{
    using RigPair = std::pair<CSKR*, CINF*>;
    RigPair dummy(nullptr, nullptr);

    if (!force && outPath.isFile())
        return true;

    /* Do extract */
    Header head;
    head.read(rs);
    rs.seekAlign32();

    hecl::BlenderConnection& conn = btok.getBlenderConnection();
    if (!conn.createBlend(outPath, hecl::BlenderConnection::BlendType::Area))
        return false;

    /* Open Py Stream and read sections */
    hecl::BlenderConnection::PyOutStream os = conn.beginPythonOut(true);
    os.format("import bpy\n"
              "import bmesh\n"
              "from mathutils import Vector\n"
              "\n"
              "bpy.context.scene.name = '%s'\n",
              pakRouter.getBestEntryName(entry, false).c_str());
    DNACMDL::InitGeomBlenderContext(os, dataSpec.getMasterShaderPath(), true);
    MaterialSet::RegisterMaterialProps(os);
    os << "# Clear Scene\n"
          "for ob in bpy.data.objects:\n"
          "    if ob.type != 'CAMERA':\n"
          "        bpy.context.scene.objects.unlink(ob)\n"
          "        bpy.data.objects.remove(ob)\n"
          "bpy.types.Lamp.retro_layer = bpy.props.IntProperty(name='Retro: Light Layer')\n"
          "bpy.types.Lamp.retro_origtype = bpy.props.IntProperty(name='Retro: Original Type')\n"
          "bpy.types.Object.retro_disable_enviro_visor = bpy.props.BoolProperty(name='Retro: Disable in Combat/Scan Visor')\n"
          "bpy.types.Object.retro_disable_thermal_visor = bpy.props.BoolProperty(name='Retro: Disable in Thermal Visor')\n"
          "bpy.types.Object.retro_disable_xray_visor = bpy.props.BoolProperty(name='Retro: Disable in X-Ray Visor')\n"
          "bpy.types.Object.retro_thermal_level = bpy.props.EnumProperty(items=[('COOL', 'Cool', 'Cool Temperature'),"
                                                                               "('HOT', 'Hot', 'Hot Temperature'),"
                                                                               "('WARM', 'Warm', 'Warm Temperature')],"
                                                                        "name='Retro: Thermal Visor Level')\n"
          "\n";

    /* One shared material set for all meshes */
    os << "# Materials\n"
          "materials = []\n"
          "\n";
    MaterialSet matSet;
    atUint64 secStart = rs.position();
    matSet.read(rs);
    matSet.readToBlender(os, pakRouter, entry, 0);
    rs.seek(secStart + head.secSizes[0], athena::Begin);
    std::vector<DNACMDL::VertexAttributes> vertAttribs;
    DNACMDL::GetVertexAttributes(matSet, vertAttribs);

    /* Read meshes */
    atUint32 curSec = 1;
    for (atUint32 m=0 ; m<head.meshCount ; ++m)
    {
        MeshHeader mHeader;
        secStart = rs.position();
        mHeader.read(rs);
        rs.seek(secStart + head.secSizes[curSec++], athena::Begin);
        curSec += DNACMDL::ReadGeomSectionsToBlender<PAKRouter<PAKBridge>, MaterialSet, RigPair, DNACMDL::SurfaceHeader_1>
                      (os, rs, pakRouter, entry, dummy, true,
                       true, vertAttribs, m, head.secCount, 0, &head.secSizes[curSec]);
        os.format("obj.retro_disable_enviro_visor = %s\n"
                  "obj.retro_disable_thermal_visor = %s\n"
                  "obj.retro_disable_xray_visor = %s\n"
                  "obj.retro_thermal_level = '%s'\n",
                  mHeader.visorFlags.disableEnviro() ? "True" : "False",
                  mHeader.visorFlags.disableThermal() ? "True" : "False",
                  mHeader.visorFlags.disableXray() ? "True" : "False",
                  mHeader.visorFlags.thermalLevelStr());
    }

    /* Skip AROT */
    rs.seek(head.secSizes[curSec++], athena::Current);

    /* Read SCLY layers */
    secStart = rs.position();
    SCLY scly;
    scly.read(rs);
    scly.exportToLayerDirectories(entry, pakRouter, force);
    rs.seek(secStart + head.secSizes[curSec++], athena::Begin);

    /* Read collision meshes */
    DeafBabe collision;
    secStart = rs.position();
    collision.read(rs);
    DeafBabe::BlenderInit(os);
    collision.sendToBlender(os);
    rs.seek(secStart + head.secSizes[curSec++], athena::Begin);

    /* Skip unknown section */
    rs.seek(head.secSizes[curSec++], athena::Current);

    /* Read BABEDEAD Lights as Cycles emissives */
    secStart = rs.position();
    ReadBabeDeadToBlender_1_2(os, rs);
    rs.seek(secStart + head.secSizes[curSec++], athena::Begin);

    /* Dump VISI entities */
    if (head.secSizes[curSec] && rs.readUint32Big() == 'VISI')
    {
        athena::io::YAMLDocWriter visiWriter("VISI");
        if (auto __vec = visiWriter.enterSubVector("entities"))
        {
            rs.seek(18, athena::Current);
            uint32_t entityCount = rs.readUint32Big();
            rs.seek(8, athena::Current);
            for (int i=0 ; i<entityCount ; ++i)
            {
                uint32_t entityId = rs.readUint32Big();
                visiWriter.writeUint16(nullptr, entityId & 0xffff);
            }
        }
        hecl::ProjectPath visiMetadataPath(outPath.getParentPath(), _S("!visi.yaml"));
        athena::io::FileWriter visiMetadata(visiMetadataPath.getAbsolutePath());
        visiWriter.finish(&visiMetadata);
    }

    /* Origins to center of mass */
    os << "bpy.context.scene.layers[1] = True\n"
          "bpy.ops.object.select_by_type(type='MESH')\n"
          "bpy.ops.object.origin_set(type='ORIGIN_CENTER_OF_MASS')\n"
          "bpy.ops.object.select_all(action='DESELECT')\n"
          "bpy.context.scene.layers[1] = False\n";

    /* Link MLVL scene as background */
    os.linkBackground("//../!world.blend", "World");

    os.centerView();
    os.close();
    return conn.saveBlend();
}

void MREA::Name(const SpecBase& dataSpec,
                PAKEntryReadStream& rs,
                PAKRouter<PAKBridge>& pakRouter,
                PAK::Entry& entry)
{
    /* Do extract */
    Header head;
    head.read(rs);
    rs.seekAlign32();

    /* One shared material set for all meshes */
    atUint64 secStart = rs.position();
    MaterialSet matSet;
    matSet.read(rs);
    matSet.nameTextures(pakRouter, hecl::Format("MREA_%s", entry.id.toString().c_str()).c_str(), -1);
    rs.seek(secStart + head.secSizes[0], athena::Begin);

    /* Skip to SCLY */
    atUint32 curSec = 1;
    secStart = rs.position();
    while (curSec != head.sclySecIdx)
        secStart += head.secSizes[curSec++];
    rs.seek(secStart, athena::Begin);
    SCLY scly;
    scly.read(rs);
    scly.nameIDs(pakRouter);

    /* Skip to PATH */
    while (curSec != head.pathSecIdx)
        secStart += head.secSizes[curSec++];
    rs.seek(secStart, athena::Begin);

    UniqueID32 pathID(rs);
    const nod::Node* node;
    PAK::Entry* pathEnt = (PAK::Entry*)pakRouter.lookupEntry(pathID, &node);
    pathEnt->name = entry.name + "_path";
}

void MREA::MeshHeader::VisorFlags::setFromBlenderProps(const std::unordered_map<std::string, std::string>& props)
{
    auto search = props.find("retro_disable_enviro_visor");
    if (search != props.cend() && !search->second.compare("True"))
        setDisableEnviro(true);
    search = props.find("retro_disable_thermal_visor");
    if (search != props.cend() && !search->second.compare("True"))
        setDisableThermal(true);
    search = props.find("retro_disable_xray_visor");
    if (search != props.cend() && !search->second.compare("True"))
        setDisableXray(true);
    search = props.find("retro_thermal_level");
    if (search != props.cend())
    {
        if (!search->second.compare("COOL"))
            setThermalLevel(ThermalLevel::Cool);
        else if (!search->second.compare("HOT"))
            setThermalLevel(ThermalLevel::Hot);
        else if (!search->second.compare("WARM"))
            setThermalLevel(ThermalLevel::Warm);
    }
}

bool MREA::Cook(const hecl::ProjectPath& outPath,
                const hecl::ProjectPath& inPath,
                const std::vector<DNACMDL::Mesh>& meshes,
                const ColMesh& cMesh,
                const std::vector<Light>& lights)
{
    return false;
}

bool MREA::PCCook(const hecl::ProjectPath& outPath,
                  const hecl::ProjectPath& inPath,
                  const std::vector<DNACMDL::Mesh>& meshes,
                  const ColMesh& cMesh,
                  const std::vector<Light>& lights,
                  hecl::BlenderToken& btok)
{
    /* Discover area layers */
    hecl::ProjectPath areaDirPath = inPath.getParentPath();
    std::vector<hecl::ProjectPath> layerScriptPaths;
    {
        hecl::DirectoryEnumerator dEnum(inPath.getParentPath().getAbsolutePath(),
                                        hecl::DirectoryEnumerator::Mode::DirsSorted,
                                        false, false, true);
        for (const hecl::DirectoryEnumerator::Entry& ent : dEnum)
        {
            hecl::ProjectPath layerScriptPath(areaDirPath, ent.m_name + _S("/!objects.yaml"));
            if (layerScriptPath.isFile())
                layerScriptPaths.push_back(std::move(layerScriptPath));
        }
    }

    size_t secCount = 1 + meshes.size() * 5; /* (materials, 5 fixed model secs) */

    /* tally up surfaces */
    for (const DNACMDL::Mesh& mesh : meshes)
        secCount += mesh.surfaces.size();

    /* Header */
    Header head = {};
    head.magic = 0xDEADBEEF;
    head.version = 0x1000F;
    head.localToWorldMtx[0].vec[0] = 1.f;
    head.localToWorldMtx[1].vec[1] = 1.f;
    head.localToWorldMtx[2].vec[2] = 1.f;
    head.meshCount = meshes.size();
    head.geomSecIdx = 0;
    head.arotSecIdx = secCount++;
    head.sclySecIdx = secCount++;
    head.collisionSecIdx = secCount++;
    head.unkSecIdx = secCount++;
    head.lightSecIdx = secCount++;
    head.visiSecIdx = secCount++;
    head.pathSecIdx = secCount++;
    head.secCount = secCount;

    std::vector<std::vector<uint8_t>> secs;
    secs.reserve(secCount + 2);

    /* Header section */
    {
        secs.emplace_back(head.binarySize(0), 0);
        athena::io::MemoryWriter w(secs.back().data(), secs.back().size());
        head.write(w);
        int i = w.position();
        int end = ROUND_UP_32(i);
        for (; i<end ; ++i)
            w.writeByte(0);
    }

    /* Sizes section */
    secs.emplace_back();
    std::vector<uint8_t>& sizesSec = secs.back();

    /* Pre-emptively build full AABB and mesh AABBs in world coords */
    zeus::CAABox fullAabb;
    std::vector<zeus::CAABox> meshAabbs;
    meshAabbs.reserve(meshes.size());

    /* Models */
    if (!DNACMDL::WriteHMDLMREASecs<HMDLMaterialSet, DNACMDL::SurfaceHeader_2, MeshHeader>
            (secs, inPath, meshes, fullAabb, meshAabbs))
        return false;

    /* AROT */
    {
        AROTBuilder arotBuilder;
        arotBuilder.build(secs, fullAabb, meshAabbs, meshes);
    }

    /* SCLY */
    DNAMP1::SCLY sclyData = {};
    {
        sclyData.fourCC = 'SCLY';
        sclyData.version = 1;
        for (const hecl::ProjectPath& layer : layerScriptPaths)
        {
            athena::io::FileReader freader(layer.getAbsolutePath());
            if (!freader.isOpen())
                continue;
            if (!BigYAML::ValidateFromYAMLStream<DNAMP1::SCLY::ScriptLayer>(freader))
                continue;

            athena::io::YAMLDocReader reader;
            if (!reader.parse(&freader))
                continue;

            sclyData.layers.emplace_back();
            sclyData.layers.back().read(reader);
            sclyData.layerSizes.push_back(sclyData.layers.back().binarySize(0));
        }
        sclyData.layerCount = sclyData.layers.size();

        secs.emplace_back(sclyData.binarySize(0), 0);
        athena::io::MemoryWriter w(secs.back().data(), secs.back().size());
        sclyData.write(w);
    }


    /* Collision */
    {        
        DeafBabe collision = {};
        DeafBabeBuildFromBlender(collision, cMesh);

        secs.emplace_back(collision.binarySize(0), 0);
        athena::io::MemoryWriter w(secs.back().data(), secs.back().size());
        collision.write(w);
    }

    /* Unk */
    {
        secs.emplace_back(8, 0);
        athena::io::MemoryWriter w(secs.back().data(), secs.back().size());
        w.writeUint32Big(1);
    }

    /* Lights */
    std::vector<atVec3f> lightsVisi;
    {
        int actualCount = 0;
        for (const Light& l : lights)
            if (l.layer == 0 || l.layer == 1)
                ++actualCount;
        lightsVisi.reserve(actualCount);

        secs.emplace_back(12 + 65 * actualCount, 0);
        athena::io::MemoryWriter w(secs.back().data(), secs.back().size());
        w.writeUint32Big(0xBABEDEAD);

        for (int lay=0 ; lay<2 ; ++lay)
        {
            int lightCount = 0;
            for (const Light& l : lights)
            {
                if (l.layer == lay)
                    ++lightCount;
            }
            w.writeUint32Big(lightCount);
            for (const Light& l : lights)
            {
                if (l.layer == lay)
                {
                    BabeDeadLight light = {};
                    WriteBabeDeadLightFromBlender(light, l);
                    light.write(w);
                    lightsVisi.push_back(light.position);
                }
            }
        }
    }

    /* VISI */
    hecl::ProjectPath visiMetadataPath(areaDirPath, _S("!visi.yaml"));
    bool visiGood = false;
    if (visiMetadataPath.isFile())
    {
        athena::io::FileReader visiReader(visiMetadataPath.getAbsolutePath());
        athena::io::YAMLDocReader r;
        if (r.parse(&visiReader))
        {
            size_t entityCount;
            std::vector<std::pair<uint16_t, zeus::CAABox>> entities;
            if (auto __vec = r.enterSubVector("entities", entityCount))
            {
                entities.reserve(entityCount);
                uint16_t entityId = r.readUint16(nullptr);
                for (const SCLY::ScriptLayer& layer : sclyData.layers)
                {
                    for (const std::unique_ptr<IScriptObject>& obj : layer.objects)
                    {
                        if ((obj->id & 0xffff) == entityId)
                        {
                            zeus::CAABox entAABB = obj->getVISIAABB(btok);
                            if (entAABB.min.x < entAABB.max.x)
                                entities.emplace_back(entityId, entAABB);
                        }
                    }
                }
            }

            hecl::ProjectPath visiIntOut = outPath.getWithExtension(_S(".visiint"));
            hecl::ProjectPath visiIn = outPath.getWithExtension(_S(".visi"));
            athena::io::FileWriter w(visiIntOut.getAbsolutePath());
            w.writeUint32Big(meshes.size());
            for (const DNACMDL::Mesh& mesh : meshes)
            {
                w.writeUint32Big(uint32_t(mesh.topology));

                w.writeUint32Big(mesh.pos.size());
                for (const auto& v : mesh.pos)
                {
                    atVec3f xfPos = hecl::BlenderConnection::DataStream::MtxVecMul4RM(mesh.sceneXf, v);
                    w.writeVec3fBig(xfPos);
                }

                w.writeUint32Big(mesh.surfaces.size());
                for (const DNACMDL::Mesh::Surface& surf : mesh.surfaces)
                {
                    w.writeUint32Big(surf.verts.size());
                    for (const DNACMDL::Mesh::Surface::Vert& vert : surf.verts)
                        w.writeUint32Big(vert.iPos);
                    const DNACMDL::Mesh::Material& mat = mesh.materialSets[0][surf.materialIdx];
                    w.writeBool(mat.transparent);
                }
            }

            w.writeUint32Big(entities.size());
            for (const auto& ent : entities)
            {
                w.writeUint32Big(ent.first);
                w.writeVec3fBig(ent.second.min);
                w.writeVec3fBig(ent.second.max);
            }

            w.writeUint32Big(lightsVisi.size());
            for (const auto& light : lightsVisi)
                w.writeVec3fBig(light);

            w.close();

            hecl::SystemString VisiGenPath = ExeDir + _S("/visigen");
#if _WIN32
            VisiGenPath += _S(".exe");
#endif
            const hecl::SystemChar* args[] = {VisiGenPath.c_str(),
                                              visiIntOut.getAbsolutePath().c_str(),
                                              visiIn.getAbsolutePath().c_str(),
                                              nullptr};
            if (0 == hecl::RunProcess(VisiGenPath.c_str(), args))
            {
                athena::io::FileReader r(visiIn.getAbsolutePath());
                size_t length = r.length();
                secs.emplace_back(length, 0);
                r.readBytesToBuf(secs.back().data(), length);
                visiGood = true;
            }
        }
    }
    if (!visiGood)
        secs.emplace_back(4, 0);

    /* PATH */
    {
        UniqueID32 pathId = inPath.ensureAuxInfo(_S("PATH"));
        secs.emplace_back(4, 0);
        athena::io::MemoryWriter w(secs.back().data(), secs.back().size());
        pathId.write(w);
    }

    /* Assemble sizes and add padding */
    {
        sizesSec.assign((((head.secCount) + 7) & ~7) * 4, 0);
        athena::io::MemoryWriter w(sizesSec.data(), sizesSec.size());
        for (auto it = secs.begin() + 2 ; it != secs.end() ; ++it)
        {
            std::vector<uint8_t>& sec = *it;
            int i = sec.size();
            int end = ROUND_UP_32(i);
            sec.resize(end);
            w.writeUint32Big(end);
        }
    }

    /* Output all padded sections to file */
    athena::io::FileWriter writer(outPath.getAbsolutePath());
    for (const std::vector<uint8_t>& sec : secs)
        writer.writeUBytes(sec.data(), sec.size());

    return true;
}

bool MREA::CookPath(const hecl::ProjectPath& outPath,
                    const hecl::ProjectPath& inPath)
{
    PATH path = {};
    path.version = 4;
    path.unkStructCount = 1;
    path.unkStructs.emplace_back();
    PATH::UnknownStruct& s = path.unkStructs.back();
    s.unk1 = 1;
    s.unk2[0] = {FLT_MAX, FLT_MAX, FLT_MAX};
    s.unk2[1] = {FLT_MIN, FLT_MIN, FLT_MIN};
    s.unk2[2] = {0.f, 0.f, 0.f};
    for (int i=0 ; i<8 ; ++i)
        s.unk3[i] = ~0;
    s.unk4 = 0;
    s.unk5 = 0;

    athena::io::FileWriter w(outPath.getAbsolutePath());
    path.write(w);
    int64_t rem = w.position() % 32;
    if (rem)
        for (int64_t i=0 ; i<32-rem ; ++i)
            w.writeBytes((atInt8*)"\xff", 1);

    return true;
}

}
}
