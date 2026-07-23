#include "i18n.h"


I18n::I18n(): langs{
    { "en", { L"English", {
        {"avoid", L"Avoid"},
    }}},
    { "it", { L"Italiano", {
        {"avoid", L"Evita"},
    }}},
    }, deflt(&langs["en"]), curr(deflt)
{}


void I18n::set(const std::string &l) {
    auto it = langs.find(l);
    curr = it != langs.end() ? &it->second : deflt;
}


std::wstring I18n::operator()(const std::string &s) const {
    auto it = curr->tr.find(s), it2 = deflt->tr.find(s);
    return it != curr->tr.end() ? it->second : it2 != deflt->tr.end() ? it2->second : std::wstring();
}
