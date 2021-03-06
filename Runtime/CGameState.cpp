#include "CGameState.hpp"
#include "IOStreams.hpp"
#include "zeus/Math.hpp"
#include "GameGlobalObjects.hpp"
#include "CMemoryCardSys.hpp"
#include "CSimplePool.hpp"
#include "CSaveWorld.hpp"

namespace urde
{
union BitsToDouble
{
    struct
    {
#if BYTE_ORDER == __LITTLE_ENDIAN
        u32 high;
        u32 low;
#else
        u32 low;
        u32 high;
#endif
    };
    double doub;
};

CWorldLayerState::CWorldLayerState(CBitStreamReader& reader, const CSaveWorld& saveWorld)
{
    u32 bitCount = reader.ReadEncoded(10);
    x10_saveLayers.reserve(bitCount);

    for (u32 i=0 ; i<bitCount ; ++i)
    {
        bool bit = reader.ReadEncoded(1);
        if (bit)
            x10_saveLayers.setBit(i);
        else
            x10_saveLayers.unsetBit(i);
    }
}

void CWorldLayerState::PutTo(CBitStreamWriter& writer) const
{
    u32 totalLayerCount = 0;
    for (int i=0 ; i<x0_areaLayers.size() ; ++i)
        totalLayerCount += GetAreaLayerCount(i) - 1;

    writer.WriteEncoded(totalLayerCount, 10);

    for (int i=0 ; i<x0_areaLayers.size() ; ++i)
    {
        u32 count = GetAreaLayerCount(i);
        for (u32 l=1 ; l<count ; ++l)
            writer.WriteEncoded(IsLayerActive(i, l), 1);
    }
}

void CWorldLayerState::InitializeWorldLayers(const std::vector<CWorldLayers::Area>& layers)
{
    if (x0_areaLayers.size())
        return;
    x0_areaLayers = layers;
    if (x10_saveLayers.getBitCount() == 0)
        return;

    u32 a = 0;
    u32 b = 0;
    for (const CWorldLayers::Area& area : x0_areaLayers)
    {
        for (u32 l=0 ; l<area.m_layerCount ; ++l)
            SetLayerActive(a, l, x10_saveLayers.getBit(b++));
        ++a;
    }

    x10_saveLayers.clear();
}

CWorldState::CWorldState(ResId id)
: x0_mlvlId(id), x4_areaId(0)
{
    x8_relayTracker = std::make_shared<CRelayTracker>();
    xc_mapWorldInfo = std::make_shared<CMapWorldInfo>();
    x10_desiredAreaAssetId = -1;
    x14_layerState = std::make_shared<CWorldLayerState>();
}

CWorldState::CWorldState(CBitStreamReader& reader, ResId mlvlId, const CSaveWorld& saveWorld)
: x0_mlvlId(mlvlId)
{
    x4_areaId = reader.ReadEncoded(32);
    x10_desiredAreaAssetId = reader.ReadEncoded(32);
    x8_relayTracker = std::make_shared<CRelayTracker>(reader, saveWorld);
    xc_mapWorldInfo = std::make_shared<CMapWorldInfo>(reader, saveWorld, mlvlId);
    x14_layerState = std::make_shared<CWorldLayerState>(reader, saveWorld);
}

void CWorldState::PutTo(CBitStreamWriter& writer, const CSaveWorld& savw) const
{
    writer.WriteEncoded(x4_areaId, 32);
    writer.WriteEncoded(x10_desiredAreaAssetId, 32);
    x8_relayTracker->PutTo(writer, savw);
    xc_mapWorldInfo->PutTo(writer, savw, x0_mlvlId);
    x14_layerState->PutTo(writer);
}

CGameState::GameFileStateInfo CGameState::LoadGameFileState(const u8* data)
{
    CBitStreamReader stream(data, 4096);
    GameFileStateInfo ret;

    for (u32 i = 0; i < 128; i++)
        stream.ReadEncoded(8);
    ret.x14_timestamp = stream.ReadEncoded(32);

    ret.x20_hardMode = stream.ReadEncoded(1);
    stream.ReadEncoded(1);
    ResId origMLVL = stream.ReadEncoded(32);
    ret.x8_mlvlId = g_ResFactory->TranslateOriginalToNew(origMLVL);

    BitsToDouble conv;
    conv.low = stream.ReadEncoded(32);
    conv.high = stream.ReadEncoded(32);
    ret.x0_playTime = conv.doub;

    CPlayerState playerState(stream);
    ret.x10_energyTanks = playerState.GetItemCapacity(CPlayerState::EItemType::EnergyTanks);

    u32 itemPercent;
    if (origMLVL == 0x158EFE17)
        itemPercent = 0;
    else
        itemPercent = std::ceil(playerState.CalculateItemCollectionRate() * 100.f / playerState.GetPickupTotal());

    ret.x18_itemPercent = itemPercent;

    float somePercent;
    if (playerState.GetTotalLogScans() == 0)
        somePercent = 0.f;
    else
        somePercent = 100.f * playerState.GetLogScans() / float(playerState.GetTotalLogScans());
    ret.x1c_scanPercent = somePercent;

    return ret;
}

CGameState::CGameState()
{
    x98_playerState = std::make_shared<CPlayerState>();
    x9c_transManager = std::make_shared<CWorldTransManager>();
    x228_25_deferPowerupInit = true;
    if (g_MemoryCardSys)
        InitializeMemoryStates();
}

CGameState::CGameState(CBitStreamReader& stream, u32 saveIdx)
: x20c_saveFileIdx(saveIdx)
{
    x9c_transManager = std::make_shared<CWorldTransManager>();
    x228_25_deferPowerupInit = true;

    for (u32 i = 0; i < 128; i++)
        x0_[i] = stream.ReadEncoded(8);
    u32 tsSeconds = stream.ReadEncoded(32);

    x228_24_hardMode = stream.ReadEncoded(1);
    x228_25_deferPowerupInit = stream.ReadEncoded(1);
    x84_mlvlId = g_ResFactory->TranslateOriginalToNew(stream.ReadEncoded(32));
    EnsureWorldPakReady(x84_mlvlId);

    BitsToDouble conv;
    conv.low = stream.ReadEncoded(32);
    conv.high = stream.ReadEncoded(32);
    xa0_playTime = conv.doub;

    x98_playerState = std::make_shared<CPlayerState>(stream);
    float currentHealth = x98_playerState->GetHealthInfo().GetHP();

    x17c_gameOptions = CGameOptions(stream);
    x1f8_hintOptions = CHintOptions(stream);

    const auto& memWorlds = g_MemoryCardSys->GetMemoryWorlds();
    x88_worldStates.reserve(memWorlds.size());
    for (const auto& memWorld : memWorlds)
    {
        TLockedToken<CSaveWorld> saveWorld =
            g_SimplePool->GetObj(SObjectTag{FOURCC('SAVW'), memWorld.second.GetSaveWorldAssetId()});
        x88_worldStates.emplace_back(stream, memWorld.first, *saveWorld);
    }

    InitializeMemoryWorlds();
    WriteBackupBuf();
}

void CGameState::ReadPersistentOptions(CBitStreamReader& r)
{
    xa8_systemOptions = CPersistentOptions(r);
}

void CGameState::ImportPersistentOptions(const CPersistentOptions& opts)
{
    if (opts.xd0_24_fusionLinked)
        xa8_systemOptions.xd0_24_fusionLinked = true;
    if (opts.xd0_27_fusionBeat)
        xa8_systemOptions.xd0_27_fusionBeat = true;
    if (&opts != &xa8_systemOptions)
        memcpy(xa8_systemOptions.x0_, opts.x0_, 98);
    xa8_systemOptions.SetLogScanCount(opts.GetLogScanCount());
    xa8_systemOptions.SetAllItemsCollected(opts.GetAllItemsCollected());
    xa8_systemOptions.SetPlayerBeatNormalMode(opts.GetPlayerBeatNormalMode());
    xa8_systemOptions.SetPlayerBeatHardMode(opts.GetPlayerBeatHardMode());
}

void CGameState::ExportPersistentOptions(CPersistentOptions& opts) const
{
    if (xa8_systemOptions.xd0_24_fusionLinked)
        opts.xd0_24_fusionLinked = true;
    if (xa8_systemOptions.xd0_27_fusionBeat)
        opts.xd0_27_fusionBeat = true;
    if (&opts != &xa8_systemOptions)
        memcpy(opts.x0_, xa8_systemOptions.x0_, 98);
    opts.SetPlayerFusionSuitActive(xa8_systemOptions.GetPlayerFusionSuitActive());
}

void CGameState::WriteBackupBuf()
{
    x218_backupBuf.resize(940);
    CBitStreamWriter w(x218_backupBuf.data(), 940);
    PutTo(w);
}

void CGameState::PutTo(CBitStreamWriter& writer) const
{
    for (u32 i = 0; i < 128; i++)
        writer.WriteEncoded(x0_[i], 8);

    writer.WriteEncoded(CBasics::ToWiiTime(std::chrono::system_clock::now()) / CBasics::TICKS_PER_SECOND, 32);
    writer.WriteEncoded(x228_24_hardMode, 1);
    writer.WriteEncoded(x228_25_deferPowerupInit, 1);
    writer.WriteEncoded(g_ResFactory->TranslateNewToOriginal(x84_mlvlId), 32);

    BitsToDouble conv;
    conv.doub = xa0_playTime;
    writer.WriteEncoded(conv.low, 32);
    writer.WriteEncoded(conv.high, 32);

    x98_playerState->PutTo(writer);
    x17c_gameOptions.PutTo(writer);
    x1f8_hintOptions.PutTo(writer);

    const auto& memWorlds = g_MemoryCardSys->GetMemoryWorlds();
    for (const auto& memWorld : memWorlds)
    {
        TLockedToken<CSaveWorld> saveWorld =
            g_SimplePool->GetObj(SObjectTag{FOURCC('SAVW'), memWorld.second.GetSaveWorldAssetId()});
        const CWorldState& wld = const_cast<CGameState&>(*this).StateForWorld(memWorld.first);
        wld.PutTo(writer, *saveWorld);
    }
}

void CGameState::EnsureWorldPakReady(ResId mlvl)
{
    /* TODO: Schedule resource list load for World Pak containing mlvl */
}

void CGameState::SetCurrentWorldId(ResId id)
{
    StateForWorld(id);
    x84_mlvlId = id;
    EnsureWorldPakReady(x84_mlvlId);
}

void CGameState::SetTotalPlayTime(float time)
{
    xa0_playTime = zeus::clamp<double>(0.0, time, 359999.0);
}

CWorldState& CGameState::StateForWorld(ResId mlvlId)
{
    auto it = x88_worldStates.begin();
    for (; it != x88_worldStates.end() ; ++it)
    {
        if (it->GetWorldAssetId() == mlvlId)
            break;
    }

    if (it == x88_worldStates.end())
    {
        x88_worldStates.emplace_back(mlvlId);
        return x88_worldStates.back();
    }
    return *it;
}

float CGameState::GetHardModeDamageMultiplier() const
{
    return g_tweakGame->GetHardModeDamageMultiplier();
}

float CGameState::GetHardModeWeaponMultiplier() const
{
    return g_tweakGame->GetHardModeWeaponMultiplier();
}

void CGameState::InitializeMemoryWorlds()
{
    const auto& memoryWorlds = g_MemoryCardSys->GetMemoryWorlds();
    for (const auto& wld : memoryWorlds)
    {
        const auto& layerState = StateForWorld(wld.first).GetLayerState();
        layerState->InitializeWorldLayers(wld.second.GetDefaultLayerStates());
    }
}

void CGameState::InitializeMemoryStates()
{
    x98_playerState->InitializeScanTimes();
    x1f8_hintOptions.InitializeMemoryState();
    InitializeMemoryWorlds();
    WriteBackupBuf();
}

}
