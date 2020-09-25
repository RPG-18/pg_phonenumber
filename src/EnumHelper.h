#pragma once

#include <mutex>
#include <unordered_map>
#include <phonenumbers/phonenumberutil.h>

extern "C" {
#include <postgres.h>
}

class EnumHelper
{
public:
    i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat oid2format(Oid oid);

private:
    void load();

private:
    std::mutex m_mutex;
    std::once_flag m_flag;
    std::unordered_map<Oid,
    i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat>
        m_oid2format;
};
