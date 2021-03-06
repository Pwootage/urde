#ifndef URDE_LOCALE_HPP
#define URDE_LOCALE_HPP

#include <specter/Translator.hpp>

namespace urde
{

std::vector<std::pair<const std::string*, const std::string*>> ListLocales();
const specter::Locale* LookupLocale(const std::string& name);
const specter::Locale* SystemLocaleOrEnglish();

}

#endif // URDE_LOCALE_HPP
