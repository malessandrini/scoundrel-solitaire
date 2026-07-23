#ifndef I18N_H
#define I18N_H

#include <map>
#include <string>


class I18n {
public:
    I18n();
    void set(const std::string &lang);
    std::wstring operator()(std::string const&) const;
protected:
    typedef std::map<std::string, std::wstring> Tr;
    struct Lang {
        std::wstring langName;
        Tr tr;
    };
    std::map<std::string, Lang> langs;
    const Lang *deflt, *curr;
};


#endif // I18N_H
