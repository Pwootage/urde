#ifndef __URDE_CSAVEUITOUCHBAR_HPP__
#define __URDE_CSAVEUITOUCHBAR_HPP__

#include <utility>
#include <memory>
#include <string>

namespace urde
{
namespace MP1
{

class CSaveGameScreenTouchBar
{
public:
    virtual ~CSaveGameScreenTouchBar() = default;
    virtual int PopOption();
    virtual void SetUIOpts(const std::u16string& opt0,
                           const std::u16string& opt1,
                           const std::u16string& opt2);
};

std::unique_ptr<CSaveGameScreenTouchBar> NewSaveUITouchBar();

}
}

#endif // __URDE_CSAVEUITOUCHBAR_HPP__
