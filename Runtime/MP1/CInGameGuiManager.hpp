#ifndef __URDE_CINGAMEGUIMANAGER_HPP__
#define __URDE_CINGAMEGUIMANAGER_HPP__

#include "CToken.hpp"
#include "CRandom16.hpp"
#include "CPlayerVisor.hpp"
#include "CFaceplateDecoration.hpp"
#include "CSamusFaceReflection.hpp"
#include "CMessageScreen.hpp"
#include "CSaveGameScreen.hpp"
#include "Camera/CCameraFilter.hpp"
#include "CStateManager.hpp"
#include "DataSpec/DNACommon/Tweaks/ITweakGui.hpp"
#include "CInventoryScreen.hpp"

namespace urde
{
class CStateManager;
class CArchitectureQueue;
class CDependencyGroup;
class CModelData;
class CActorLights;
class CGuiModel;
class CGuiCamera;
class CAutoMapper;

namespace MP1
{
class CPauseScreen;
class CSamusHud;
class CInventoryScreen;

enum class EInGameGuiState
{
    Zero,
    InGame,
    MapScreen,
    PauseGame,
    PauseLogBook,
    PauseSaveGame,
    PauseHUDMessage
};

class CInGameGuiManager
{
    enum class ELoadPhase
    {
        LoadDepsGroup = 0,
        PreLoadDeps,
        LoadDeps,
        Done
    };

    struct SGuiProfileInfo
    {

    };

    TLockedToken<CDependencyGroup> x0_iggmPreLoad;
    std::vector<CToken> x8_preLoadDeps;
    ELoadPhase x18_loadPhase = ELoadPhase::LoadDepsGroup;
    CRandom16 x1c_rand;
    CFaceplateDecoration x20_faceplateDecor;
    std::unique_ptr<CPlayerVisor> x30_playerVisor;
    std::unique_ptr<CSamusHud> x34_samusHud;
    std::unique_ptr<CAutoMapper> x38_autoMapper;
    std::unique_ptr<CPauseScreen> x3c_pauseScreen;
    std::unique_ptr<CSamusFaceReflection> x40_samusReflection;
    std::unique_ptr<CMessageScreen> x44_messageScreen;
    std::unique_ptr<CInventoryScreen> x48_inventoryScreen;
    std::unique_ptr<CSaveGameScreen> x4c_saveUI;
    TLockedToken<CTexture> x50_deathDot;
    std::vector<TLockedToken<CDependencyGroup>> x5c_pauseScreenDGRPs;
    std::vector<TLockedToken<CDependencyGroup>> xc8_inGameGuiDGRPs;
    std::vector<u32> xd8_;
    std::vector<CToken> xe8_pauseResources;
    CCameraFilterPass xf8_camFilter;
    ResId x124_pauseGameHudMessage = -1;
    float x128_pauseGameHudTime = 0.f;
    std::list<CToken> x12c_;
    u32 x140_ = 0;
    CGuiWidget* x144_basewidget_automapper = nullptr;
    CGuiModel* x148_model_automapper = nullptr;
    CGuiCamera* x14c_basehud_camera = nullptr;
    CGuiWidget* x150_basewidget_functional = nullptr;
    zeus::CQuaternion x154_automapperRotate;
    zeus::CVector3f x164_automapperOffset;
    zeus::CQuaternion x170_camRotate;
    zeus::CVector3f x180_camOffset;
    zeus::CTransform x18c_camXf;
    EInGameGuiState x1bc_prevState = EInGameGuiState::Zero;
    EInGameGuiState x1c0_nextState = EInGameGuiState::Zero;
    SOnScreenTex x1c4_onScreenTex;
    float x1d8_ = 0.f;
    TLockedToken<CTexture> x1dc_onScreenTexTok; // Used to be heap-allocated
    DataSpec::ITweakGui::EHelmetVisMode x1e0_helmetVisMode;
    bool x1e4_enableTargetingManager;
    bool x1e8_enableAutoMapper;
    DataSpec::ITweakGui::EHudVisMode x1ec_hudVisMode;
    u32 x1f0_enablePlayerVisor;
    float x1f4_player74c;

    union
    {
        struct
        {
            bool x1f8_24_ : 1;
            bool x1f8_25_ : 1;
            bool x1f8_26_deferTransition : 1;
            bool x1f8_27_inSaveUI : 1;
        };
        u32 _dummy = 0;
    };

    static std::vector<TLockedToken<CDependencyGroup>> LockPauseScreenDependencies();
    bool CheckDGRPLoadComplete() const;
    void BeginStateTransition(EInGameGuiState state, CStateManager& stateMgr);
    void EnsureStates(CStateManager& stateMgr);
    void DoStateTransition(CStateManager& stateMgr);
    void DestroyAreaTextures(CStateManager& stateMgr);
    void TryReloadAreaTextures();

public:
    CInGameGuiManager(CStateManager& stateMgr, CArchitectureQueue& archQueue);
    bool CheckLoadComplete(CStateManager& stateMgr);
    void Update(CStateManager& stateMgr, float dt, CArchitectureQueue& archQueue, bool);
    void ProcessControllerInput(CStateManager& stateMgr, const CFinalInput& input,
                                CArchitectureQueue& archQueue);
    void PreDraw(CStateManager& stateMgr);
    void Draw(CStateManager& stateMgr);
    void ShowPauseGameHudMessage(CStateManager& stateMgr, ResId pauseMsg, float time);
    void PauseGame(CStateManager& stateMgr, EInGameGuiState state);
    void StartFadeIn();
    bool WasInGame() const { return x1bc_prevState >= EInGameGuiState::Zero && x1bc_prevState <= EInGameGuiState::InGame; }
    bool IsInGame() const { return x1c0_nextState >= EInGameGuiState::Zero && x1c0_nextState <= EInGameGuiState::InGame; }
    bool IsInSaveUI() const { return x1f8_27_inSaveUI; }
    bool GetIsGameDraw() const;
    static std::string GetIdentifierForMidiEvent(ResId world, ResId area, const std::string& midiObj);
};

}
}

#endif // __URDE_CINGAMEGUIMANAGER_HPP__
