#include <utility>

#include "SpecBase.hpp"
#include "DNAMP2/DNAMP2.hpp"

#include "DNAMP2/MLVL.hpp"
#include "DNAMP2/STRG.hpp"
#include "DNAMP2/AGSC.hpp"
#include "DNAMP1/CSNG.hpp"

#include "hecl/ClientProcess.hpp"

#include "Runtime/RetroTypes.hpp"

namespace DataSpec
{

static logvisor::Module Log("urde::SpecMP2");
extern hecl::Database::DataSpecEntry SpecEntMP2;
extern hecl::Database::DataSpecEntry SpecEntMP2ORIG;

struct SpecMP2 : SpecBase
{
    bool checkStandaloneID(const char* id) const
    {
        if (!memcmp(id, "G2M", 3))
            return true;
        return false;
    }

    std::vector<const nod::Node*> m_nonPaks;
    std::vector<DNAMP2::PAKBridge> m_paks;
    std::map<std::string, DNAMP2::PAKBridge*, hecl::CaseInsensitiveCompare> m_orderedPaks;

    hecl::ProjectPath m_workPath;
    hecl::ProjectPath m_cookPath;
    PAKRouter<DNAMP2::PAKBridge> m_pakRouter;

    SpecMP2(const hecl::Database::DataSpecEntry* specEntry, hecl::Database::Project& project, bool pc)
    : SpecBase(specEntry, project, pc),
      m_workPath(project.getProjectWorkingPath(), _S("MP2")),
      m_cookPath(project.getProjectCookedPath(SpecEntMP2), _S("MP2")),
      m_pakRouter(*this, m_workPath, m_cookPath) {}

    void buildPaks(nod::Node& root,
                   const std::vector<hecl::SystemString>& args,
                   ExtractReport& rep)
    {
        m_nonPaks.clear();
        m_paks.clear();
        for (const nod::Node& child : root)
        {
            bool isPak = false;
            const std::string& name = child.getName();
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), tolower);
            if (name.size() > 4)
            {
                std::string::iterator extit = lowerName.end() - 4;
                if (!std::string(extit, lowerName.end()).compare(".pak"))
                {
                    /* This is a pak */
                    isPak = true;
                    std::string lowerBase(lowerName.begin(), extit);

                    /* Needs filter */
                    bool good = true;
                    if (args.size())
                    {
                        good = false;
                        if (!lowerName.compare(0, 7, "metroid"))
                        {
                            hecl::SystemChar idxChar = lowerName[7];
                            for (const hecl::SystemString& arg : args)
                            {
                                if (arg.size() == 1 && iswdigit(arg[0]))
                                    if (arg[0] == idxChar)
                                        good = true;
                            }
                        }
                        else
                            good = true;

                        if (!good)
                        {
                            for (const hecl::SystemString& arg : args)
                            {
                                std::string lowerArg = hecl::SystemUTF8View(arg).str();
                                std::transform(lowerArg.begin(), lowerArg.end(), lowerArg.begin(), tolower);
                                if (!lowerArg.compare(0, lowerBase.size(), lowerBase))
                                    good = true;
                            }
                        }
                    }

                    m_paks.emplace_back(m_project, child, good);
                }
            }

            if (!isPak)
                m_nonPaks.push_back(&child);
        }

        /* Sort PAKs alphabetically */
        m_orderedPaks.clear();
        for (DNAMP2::PAKBridge& dpak : m_paks)
            m_orderedPaks[dpak.getName()] = &dpak;

        /* Assemble extract report */
        for (const std::pair<std::string, DNAMP2::PAKBridge*>& item : m_orderedPaks)
        {
            if (!item.second->m_doExtract)
                continue;
            rep.childOpts.emplace_back();
            ExtractReport& childRep = rep.childOpts.back();
            hecl::SystemStringView nameView(item.first);
            childRep.name = nameView;
            childRep.desc = item.second->getLevelString();
        }
    }

    bool checkFromStandaloneDisc(nod::DiscBase& disc,
                                 const hecl::SystemString& regstr,
                                 const std::vector<hecl::SystemString>& args,
                                 std::vector<ExtractReport>& reps)
    {
        nod::Partition* partition = disc.getDataPartition();
        std::unique_ptr<uint8_t[]> dolBuf = partition->getDOLBuf();
        const char* buildInfo = (char*)memmem(dolBuf.get(), partition->getDOLSize(), "MetroidBuildInfo", 16) + 19;
        if (!buildInfo)
            return false;

        /* Root Report */
        reps.emplace_back();
        ExtractReport& rep = reps.back();
        rep.name = _S("MP2");
        rep.desc = _S("Metroid Prime 2 ") + regstr;
        std::string buildStr(buildInfo);
        hecl::SystemStringView buildView(buildStr);
        rep.desc += _S(" (") + buildView + _S(")");

        /* Iterate PAKs and build level options */
        nod::Node& root = partition->getFSTRoot();
        buildPaks(root, args, rep);

        return true;
    }

    bool checkFromTrilogyDisc(nod::DiscBase& disc,
                              const hecl::SystemString& regstr,
                              const std::vector<hecl::SystemString>& args,
                              std::vector<ExtractReport>& reps)
    {
        std::vector<hecl::SystemString> mp2args;
        bool doExtract = false;
        if (args.size())
        {
            /* Needs filter */
            for (const hecl::SystemString& arg : args)
            {
                hecl::SystemString lowerArg = arg;
                hecl::ToLower(lowerArg);
                if (!lowerArg.compare(0, 3, _S("mp2")))
                {
                    doExtract = true;
                    mp2args.reserve(args.size());
                    size_t slashPos = arg.find(_S('/'));
                    if (slashPos == hecl::SystemString::npos)
                        slashPos = arg.find(_S('\\'));
                    if (slashPos != hecl::SystemString::npos)
                        mp2args.emplace_back(hecl::SystemString(arg.begin() + slashPos + 1, arg.end()));
                }
            }
        }
        else
            doExtract = true;

        if (!doExtract)
            return false;

        nod::Partition* partition = disc.getDataPartition();
        nod::Node& root = partition->getFSTRoot();
        nod::Node::DirectoryIterator dolIt = root.find("rs5mp2_p.dol");
        if (dolIt == root.end())
            return false;

        std::unique_ptr<uint8_t[]> dolBuf = dolIt->getBuf();
        const char* buildInfo = (char*)memmem(dolBuf.get(), dolIt->size(), "MetroidBuildInfo", 16) + 19;

        /* Root Report */
        reps.emplace_back();
        ExtractReport& rep = reps.back();
        rep.name = _S("MP2");
        rep.desc = _S("Metroid Prime 2 ") + regstr;
        if (buildInfo)
        {
            std::string buildStr(buildInfo);
            hecl::SystemStringView buildView(buildStr);
            rep.desc += _S(" (") + buildView + _S(")");
        }

        /* Iterate PAKs and build level options */
        nod::Node::DirectoryIterator mp2It = root.find("MP2");
        if (mp2It == root.end())
            return false;
        buildPaks(*mp2It, mp2args, rep);

        return true;
    }

    bool extractFromDisc(nod::DiscBase&, bool force, FProgress progress)
    {
        nod::ExtractionContext ctx = {true, force, nullptr};

        m_workPath.makeDir();

        progress(_S("Indexing PAKs"), _S(""), 2, 0.0);
        m_pakRouter.build(m_paks, [&progress](float factor)
        {
            progress(_S("Indexing PAKs"), _S(""), 2, factor);
        });
        progress(_S("Indexing PAKs"), _S(""), 2, 1.0);

        hecl::ProjectPath outPath(m_project.getProjectWorkingPath(), _S("out"));
        outPath.makeDir();
        hecl::ProjectPath mp2OutPath(outPath, _S("MP2"));
        mp2OutPath.makeDir();
        progress(_S("MP2 Root"), _S(""), 3, 0.0);
        int prog = 0;
        ctx.progressCB = [&](const std::string& name) {
            hecl::SystemStringView nameView(name);
            progress(_S("MP2 Root"), nameView.c_str(), 3, prog / (float)m_nonPaks.size());
        };
        for (const nod::Node* node : m_nonPaks)
        {
            node->extractToDirectory(mp2OutPath.getAbsolutePath(), ctx);
            prog++;
        }
        progress(_S("MP2 Root"), _S(""), 3, 1.0);

        std::mutex msgLock;
        hecl::ClientProcess process;
        int compIdx = 4;
        prog = 0;
        for (std::pair<const std::string, DNAMP2::PAKBridge*>& pair : m_orderedPaks)
        {
            DNAMP2::PAKBridge& pak = *pair.second;
            if (!pak.m_doExtract)
                continue;

            const std::string& name = pak.getName();
            hecl::SystemStringView sysName(name);

            {
                std::unique_lock<std::mutex> lk(msgLock);
                progress(sysName.c_str(), _S(""), compIdx, 0.0);
            }
            hecl::SystemString pakName = sysName.sys_str();
            process.addLambdaTransaction([&, pakName](hecl::BlenderToken& btok)
            {
                m_pakRouter.extractResources(pak, force, btok,
                [&](const hecl::SystemChar* substr, float factor)
                {
                    std::unique_lock<std::mutex> lk(msgLock);
                    progress(pakName.c_str(), substr, compIdx, factor);
                });
            });
        }

        process.waitUntilComplete();

        return true;
    }

    const hecl::Database::DataSpecEntry& getOriginalSpec() const
    {
        return SpecEntMP2;
    }

    const hecl::Database::DataSpecEntry& getUnmodifiedSpec() const
    {
        return SpecEntMP2ORIG;
    }

    hecl::ProjectPath getWorking(class UniqueID32& id)
    {
        return m_pakRouter.getWorking(id);
    }

    bool checkPathPrefix(const hecl::ProjectPath& path) const
    {
        return path.getRelativePath().compare(0, 4, _S("MP2/")) == 0;
    }

    bool validateYAMLDNAType(athena::io::IStreamReader& fp) const
    {
        athena::io::YAMLDocReader reader;
        yaml_parser_set_input(reader.getParser(), (yaml_read_handler_t*)athena::io::YAMLAthenaReader, &fp);
        return reader.ClassTypeOperation([](const char* classType)
        {
            if (!strcmp(classType, DNAMP2::MLVL::DNAType()))
                return true;
            else if (!strcmp(classType, DNAMP2::STRG::DNAType()))
                return true;
            else if (!strcmp(classType, "ATBL"))
                return true;
            return false;
        });
    }

    urde::SObjectTag BuildTagFromPath(const hecl::ProjectPath& path, hecl::BlenderToken& btok) const
    {
        return {};
    }

    void cookMesh(const hecl::ProjectPath& out, const hecl::ProjectPath& in,
                  BlendStream& ds, bool fast, hecl::BlenderToken& btok,
                  FCookProgress progress)
    {
    }

    void cookActor(const hecl::ProjectPath& out, const hecl::ProjectPath& in,
                   BlendStream& ds, bool fast, hecl::BlenderToken& btok,
                   FCookProgress progress)
    {
    }

    void cookArea(const hecl::ProjectPath& out, const hecl::ProjectPath& in,
                  BlendStream& ds, bool fast, hecl::BlenderToken& btok,
                  FCookProgress progress)
    {
    }

    void cookWorld(const hecl::ProjectPath& out, const hecl::ProjectPath& in,
                   BlendStream& ds, bool fast, hecl::BlenderToken& btok,
                   FCookProgress progress)
    {
    }

    void cookGuiFrame(const hecl::ProjectPath& out, const hecl::ProjectPath& in,
                      BlendStream& ds, hecl::BlenderToken& btok,
                      FCookProgress progress)
    {
    }

    void cookYAML(const hecl::ProjectPath& out, const hecl::ProjectPath& in,
                  athena::io::IStreamReader& fin, FCookProgress progress)
    {
    }

    void flattenDependenciesYAML(athena::io::IStreamReader& fin, std::vector<hecl::ProjectPath>& pathsOut)
    {
    }

    void cookAudioGroup(const hecl::ProjectPath& out, const hecl::ProjectPath& in,
                        FCookProgress progress)
    {
        DNAMP2::AGSC::Cook(in, out);
        progress(_S("Done"));
    }

    void cookSong(const hecl::ProjectPath& out, const hecl::ProjectPath& in,
                  FCookProgress progress)
    {
        DNAMP1::CSNG::Cook(in, out);
        progress(_S("Done"));
    }
};

hecl::Database::DataSpecEntry SpecEntMP2
(
    _S("MP2"),
    _S("Data specification for original Metroid Prime 2 engine"),
    [](hecl::Database::Project& project, hecl::Database::DataSpecTool)
    -> hecl::Database::IDataSpec* {return new struct SpecMP2(&SpecEntMP2, project, false);}
);

hecl::Database::DataSpecEntry SpecEntMP2PC =
{
    _S("MP2-PC"),
    _S("Data specification for PC-optimized Metroid Prime 2 engine"),
    [](hecl::Database::Project& project, hecl::Database::DataSpecTool tool)
    -> hecl::Database::IDataSpec*
    {
        if (tool != hecl::Database::DataSpecTool::Extract)
            return new struct SpecMP2(&SpecEntMP2PC, project, true);
        return nullptr;
    }
};

hecl::Database::DataSpecEntry SpecEntMP2ORIG =
{
    _S("MP2-ORIG"),
    _S("Data specification for unmodified Metroid Prime 2 resources"),
    {}
};

}
