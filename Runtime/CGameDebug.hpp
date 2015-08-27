#ifndef __RETRO_CGAMEDEBUG_HPP__
#define __RETRO_CGAMEDEBUG_HPP__

#include <string>

namespace Retro
{
class CFinalInput;

const char* StringForControlOption(int);

enum EDebugMenu
{
};

enum EDebugOptions
{
};

enum EDebugMainMenu
{
};

class CDebugOption
{
public:
    CDebugOption(EDebugMenu, EDebugOptions, const std::string&, bool);
    CDebugOption(EDebugMenu, EDebugOptions, const std::string&, float, float, float, float);
};

class CGameDebug
{
public:
    enum EReturnValue
    {
    };

    void DeactivateMenu();
    void AddDebugOption(EDebugMenu, EDebugOptions, const char*, bool);
    void AddDebugOption(EDebugMenu, EDebugOptions, const char*, float, float, float, float);
    void SetCaptureMovieTimeLeft(float);
    const std::string& GetCaptureMovieName();
    void SetCaptureMovieName(const std::string&);
    void AddDebugOptions();
    void CopyDebugToTweaks();
    void CopyTweaksToDebug();
    void ProcessControllerInput(const CFinalInput&);
    void Update(float);
    void Draw(void) const;
    void ActivateMenu(EDebugMainMenu, int);
    void AddDebugOption(const CDebugOption&);
};

}

#endif // __RETRO_CGAMEDEBUG_HPP__