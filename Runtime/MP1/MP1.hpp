#ifndef __URDE_MP1_HPP__
#define __URDE_MP1_HPP__

#define MP1_USE_BOO 0

#include "IMain.hpp"
#include "CTweaks.hpp"
#include "CPlayMovie.hpp"
#include "IOStreams.hpp"
#include "CBasics.hpp"
#include "CMemoryCardSys.hpp"
#include "CResFactory.hpp"
#include "CSimplePool.hpp"
#include "Character/CAssetFactory.hpp"
#include "World/CAi.hpp"
#include "CGameState.hpp"
#include "CInGameTweakManager.hpp"
#include "Particle/CElementGen.hpp"
#include "Character/CAnimData.hpp"
#include "Particle/CDecalManager.hpp"
#include "Particle/CGenDescription.hpp"
#include "Graphics/CBooRenderer.hpp"
#include "Audio/CAudioSys.hpp"
#include "Input/CInputGenerator.hpp"
#include "GuiSys/CGuiSys.hpp"
#include "CIOWinManager.hpp"
#include "GuiSys/CSplashScreen.hpp"
#include "CMainFlow.hpp"
#include "GuiSys/CConsoleOutputWindow.hpp"
#include "GuiSys/CErrorOutputWindow.hpp"
#include "GuiSys/CTextParser.hpp"
#include "CAudioStateWin.hpp"
#include "GameGlobalObjects.hpp"
#include "CArchitectureQueue.hpp"
#include "CTimeProvider.hpp"
#include "GuiSys/CTextExecuteBuffer.hpp"
#include "DataSpec/DNAMP1/Tweaks/CTweakPlayer.hpp"
#include "DataSpec/DNAMP1/Tweaks/CTweakGame.hpp"

namespace urde
{
class IFactory;
class IObjectStore;

namespace MP1
{
class CMain;

class CGameGlobalObjects
{
    friend class CMain;

    std::unique_ptr<CMemoryCardSys> x0_memoryCardSys;
    IFactory& x4_resFactory;
    CSimplePool& xcc_simplePool;
    CCharacterFactoryBuilder xec_charFactoryBuilder;
    CAiFuncMap x110_aiFuncMap;
    std::unique_ptr<CGameState> x134_gameState;
    TLockedToken<CStringTable> x13c_mainStringTable;
    CInGameTweakManager x150_tweakManager;
    std::unique_ptr<IRenderer> m_renderer;

    void LoadStringTable()
    {
        x13c_mainStringTable = g_SimplePool->GetObj("STRG_Main");
        g_MainStringTable = x13c_mainStringTable.GetObj();
    }
    static IRenderer*
    AllocateRenderer(IObjectStore& store, IFactory& resFactory)
    {
        g_Renderer = new CBooRenderer(store, resFactory);
        return g_Renderer;
    }

public:
    CGameGlobalObjects(IFactory& resFactory,
                       CSimplePool& objStore)
    : x4_resFactory(resFactory), xcc_simplePool(objStore)
    {
        g_ResFactory = &x4_resFactory;
        g_SimplePool = &xcc_simplePool;
        g_CharFactoryBuilder = &xec_charFactoryBuilder;
        g_AiFuncMap = &x110_aiFuncMap;
        x134_gameState.reset(new CGameState());
        g_GameState = x134_gameState.get();
        g_TweakManager = &x150_tweakManager;
    }

    void PostInitialize()
    {
        LoadStringTable();
        m_renderer.reset(AllocateRenderer(xcc_simplePool, x4_resFactory));
    }

    void ResetGameState()
    {
        x134_gameState.reset(new CGameState());
        g_GameState = x134_gameState.get();
    }
};

#if MP1_USE_BOO
class CGameArchitectureSupport : public boo::IWindowCallback
#else
class CGameArchitectureSupport
#endif
{
    CMain& m_parent;
    CArchitectureQueue x4_archQueue;
    CAudioSys x0_audioSys;
    CInputGenerator x30_inputGenerator;
    CGuiSys x44_guiSys;
    CIOWinManager x58_ioWinManager;
    s32 x78_gameFrameCount;

    enum class EAudioLoadStatus
    {
        Loading,
        Loaded,
        Uninitialized
    };
    EAudioLoadStatus x88_audioLoadStatus = EAudioLoadStatus::Uninitialized;
    std::vector<TToken<CAudioGroupSet>> x8c_pendingAudioGroups;

    boo::SWindowRect m_windowRect;
    bool m_rectIsDirty;

    void destroyed() { x4_archQueue.Push(MakeMsg::CreateApplicationExit(EArchMsgTarget::ArchitectureSupport)); }

    void resized(const boo::SWindowRect &rect)
    {
        m_windowRect = rect;
        m_rectIsDirty = true;
    }

public:
    CGameArchitectureSupport(CMain& parent,
                             boo::IAudioVoiceEngine* voiceEngine,
                             amuse::IBackendVoiceAllocator& backend);
    ~CGameArchitectureSupport();

    void mouseDown(const boo::SWindowCoord &coord, boo::EMouseButton button, boo::EModifierKey mods)
    { x30_inputGenerator.mouseDown(coord, button, mods); }
    void mouseUp(const boo::SWindowCoord &coord, boo::EMouseButton button, boo::EModifierKey mods)
    { x30_inputGenerator.mouseUp(coord, button, mods); }
    void mouseMove(const boo::SWindowCoord &coord)
    { x30_inputGenerator.mouseMove(coord); }
    void scroll(const boo::SWindowCoord &coord, const boo::SScrollDelta &scroll)
    { x30_inputGenerator.scroll(coord, scroll); }
    void charKeyDown(unsigned long charCode, boo::EModifierKey mods, bool isRepeat)
    { x30_inputGenerator.charKeyDown(charCode, mods, isRepeat); }
    void charKeyUp(unsigned long charCode, boo::EModifierKey mods)
    { x30_inputGenerator.charKeyUp(charCode, mods); }
    void specialKeyDown(boo::ESpecialKey key, boo::EModifierKey mods, bool isRepeat)
    { x30_inputGenerator.specialKeyDown(key, mods, isRepeat); }
    void specialKeyUp(boo::ESpecialKey key, boo::EModifierKey mods)
    { x30_inputGenerator.specialKeyUp(key, mods); }
    void modKeyDown(boo::EModifierKey mod, bool isRepeat)
    { x30_inputGenerator.modKeyDown(mod, isRepeat);}
    void modKeyUp(boo::EModifierKey mod)
    { x30_inputGenerator.modKeyUp(mod); }

    void PreloadAudio();
    bool LoadAudio();
    void UnloadAudio();
    void UpdateTicks();
    void Update();
    void Draw();

    bool isRectDirty() { return m_rectIsDirty; }
    const boo::SWindowRect& getWindowRect()
    {
        m_rectIsDirty = false;
        return m_windowRect;
    }

    CIOWinManager& GetIOWinManager() { return x58_ioWinManager; }
};

#if MP1_USE_BOO
class CMain : public boo::IApplicationCallback, public IMain
#else
class CMain : public IMain
#endif
{
    friend class CGameArchitectureSupport;
#if MP1_USE_BOO
    boo::IWindow* mainWindow;
    int appMain(boo::IApplication* app);
    void appQuitting(boo::IApplication*)
    { xe8_b24_finished = true; }
    void appFilesOpen(boo::IApplication*, const std::vector<std::string>& paths)
    {
        fprintf(stderr, "OPENING: ");
        for (const std::string& path : paths)
            fprintf(stderr, "%s ", path.c_str());
        fprintf(stderr, "\n");
    }
#endif
private:

    struct BooSetter
    {
        BooSetter(boo::IGraphicsDataFactory* factory,
                  boo::IGraphicsCommandQueue* cmdQ,
                  boo::ITextureR* spareTex);
    } m_booSetter;

    //CMemorySys x6c_memSys;
    CTweaks x70_tweaks;
    EGameplayResult xe4_gameplayResult;

    /* urde addition: these are simply initialized along with everything else */
    CGameGlobalObjects x128_globalObjects;

    EFlowState x12c_flowState = EFlowState::Five;

    u32 x130_[10] = { 1000000 };

    union
    {
        struct
        {
            bool x160_24_finished : 1;
            bool x160_25_mfGameBuilt : 1;
            bool x160_26_screenFading : 1;
            bool x160_27_ : 1;
            bool x160_28_manageCard : 1;
            bool x160_29_ : 1;
            bool x160_30_ : 1;
            bool x160_31_cardBusy : 1;
            bool x161_24_gameFrameDrawn : 1;
        };
        u16 _dummy = 0;
    };

    std::unique_ptr<CGameArchitectureSupport> x164_archSupport;

    boo::IWindow* m_mainWindow = nullptr;

    void InitializeSubsystems(const hecl::Runtime::FileStoreManager& storeMgr);

public:
    CMain(IFactory& resFactory, CSimplePool& resStore,
          boo::IGraphicsDataFactory* gfxFactory,
          boo::IGraphicsCommandQueue* cmdQ,
          boo::ITextureR* spareTex);
    void RegisterResourceTweaks();
    void ResetGameState();
    void StreamNewGameState(CBitStreamReader&, u32 idx);
    void CheckTweakManagerDebugOptions() {}
    void SetMFGameBuilt(bool b) { x160_25_mfGameBuilt = b; }
    void SetScreenFading(bool b) { x160_26_screenFading = b; }

    //int RsMain(int argc, const boo::SystemChar* argv[]);
    void Init(const hecl::Runtime::FileStoreManager& storeMgr,
              boo::IWindow* window,
              boo::IAudioVoiceEngine* voiceEngine,
              amuse::IBackendVoiceAllocator& backend);
    bool Proc();
    void Draw();
    void Shutdown();
    boo::IWindow* GetMainWindow() const;

    void MemoryCardInitializePump()
    {
        if (!g_MemoryCardSys)
        {
            std::unique_ptr<CMemoryCardSys>& memSys = x128_globalObjects.x0_memoryCardSys;
            if (!memSys)
                memSys.reset(new CMemoryCardSys());
            if (memSys->InitializePump())
            {
                g_MemoryCardSys = memSys.get();
                g_GameState->InitializeMemoryStates();
            }
        }
    }

    bool CheckReset() { return false; }
    bool CheckTerminate() { return false; }
    void DrawDebugMetrics(double, CStopWatch&) {}
    void DoPredrawMetrics() {}
    void FillInAssetIDs();
    bool LoadAudio();
    void ShutdownSubsystems();
    EGameplayResult GetGameplayResult() const { return xe4_gameplayResult; }
    void SetGameplayResult(EGameplayResult wl) { xe4_gameplayResult = wl; }
    void SetManageCard(bool v) { x160_28_manageCard = v; }
    bool GetCardBusy() const { return x160_31_cardBusy; }
    void SetCardBusy(bool v) { x160_31_cardBusy = v; }
    void SetGameFrameDrawn() { x161_24_gameFrameDrawn = true; }

    EFlowState GetFlowState() const { return x12c_flowState; }
    void SetFlowState(EFlowState s) { x12c_flowState = s; }

    void SetX30(bool v) { x160_30_ = v; }

    CGameArchitectureSupport* GetArchSupport() const { return x164_archSupport.get(); }
};

}
}

#endif // __URDE_MP1_HPP__
