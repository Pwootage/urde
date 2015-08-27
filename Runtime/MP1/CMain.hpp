#ifndef __RETRO_CMAIN_HPP__
#define __RETRO_CMAIN_HPP__

#include "COsContext.hpp"
#include "CMemory.hpp"
#include "CTweaks.hpp"
#include "CPlayMovie.hpp"

namespace Retro
{
class CStopwatch;

namespace MP1
{

enum EGameplayResult
{
    GameplayResultNone,
    GameplayResultWin,
    GameplayResultLose,
    GameplayResultPlaying
};

class CMain : public COsContext
{
    CMemorySys x6c_memSys;
    CTweaks x70_tweaks;
    EGameplayResult xe4_gameplayResult;
    bool xe8_finished = false;
public:
    CMain();
    void RegisterResourceTweaks();
    void ResetGameState();
    void StreamNewGameState(CInputStream&);
    void CheckTweakManagerDebugOptions() {}
    void AddWorldPaks();
    int RsMain(int argc, const char* argv[]);
    bool CheckReset();
    bool CheckTerminate() {return false;}
    void DrawDebugMetrics(double, CStopwatch&) {}
    void DoPredrawMetrics() {}
    void FillInAssetIDs();
    void LoadAudio();
    void ShutdownSubsystems();
    void InitializeSubsystems();
    EGameplayResult GetGameplayResult() const {return xe4_gameplayResult;}
    void SetGameplayResult(EGameplayResult wl) {xe4_gameplayResult = wl;}
};

extern CMain* g_main;
}
}

#endif // __RETRO_CMAIN_HPP__