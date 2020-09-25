#include <phonenumbers/phonenumberutil.h>
#include "EnumHelper.h"

extern "C" {
#include <postgres.h>
#include <fmgr.h>
#include <utils/elog.h>
#include <utils/array.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(is_valid_number);
Datum is_valid_number(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(is_valid_number_with_region);
Datum is_valid_number_with_region(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(is_valid_number_for_region);
Datum is_valid_number_for_region(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(format_number);
Datum format_number(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(format_number_with_region);
Datum format_number_with_region(PG_FUNCTION_ARGS);
}

namespace
{
i18n::phonenumbers::PhoneNumberUtil &
    phone_util(*i18n::phonenumbers::PhoneNumberUtil::GetInstance());

EnumHelper ENUM_HELPER;

std::string text2string(const text *txt)
{
    return std::string(VARDATA(txt), VARSIZE(txt) - VARHDRSZ);
}

text *string2text(const std::string &str)
{
    const auto size = str.size() + VARHDRSZ;
    auto dest = static_cast<text *>(palloc0(size));
    SET_VARSIZE(dest, size);
    memcpy(dest->vl_dat, str.data(), str.size());
    return dest;
}
} // namespace

Datum is_valid_number(PG_FUNCTION_ARGS)
{
    try {
        using namespace i18n::phonenumbers;
        PhoneNumber number;

        const std::string region;
        const auto numberStr = text2string(PG_GETARG_TEXT_P(0));

        auto err = phone_util.Parse(numberStr, region, &number);
        if (err != PhoneNumberUtil::NO_PARSING_ERROR) {
            PG_RETURN_BOOL(false);
        }

        PG_RETURN_BOOL(phone_util.IsValidNumber(number));
    }
    catch (const std::exception &e) {
        elog(ERROR, "catch exception, what %s", e.what());
    }
    catch (...) {
        elog(ERROR, "catch unknown exception");
    }

    PG_RETURN_BOOL(false);
}

Datum is_valid_number_with_region(PG_FUNCTION_ARGS)
{
    try {
        using namespace i18n::phonenumbers;
        PhoneNumber number;

        const auto numberStr = text2string(PG_GETARG_TEXT_P(0));
        const auto region = text2string(PG_GETARG_TEXT_P(1));
        auto err = phone_util.Parse(numberStr, region, &number);
        if (err != PhoneNumberUtil::NO_PARSING_ERROR) {
            PG_RETURN_BOOL(false);
        }

        PG_RETURN_BOOL(phone_util.IsValidNumber(number));
    }
    catch (const std::exception &e) {
        elog(ERROR, "phonenumber catch exception, what %s", e.what());
    }
    catch (...) {
        elog(ERROR, "phonenumber catch unknown exception");
    }

    PG_RETURN_BOOL(false);
}

Datum is_valid_number_for_region(PG_FUNCTION_ARGS)
{
    try {
        using namespace i18n::phonenumbers;
        PhoneNumber number;

        const auto numberStr = text2string(PG_GETARG_TEXT_P(0));
        const auto region = text2string(PG_GETARG_TEXT_P(1));
        auto err = phone_util.Parse(numberStr, region, &number);
        if (err != PhoneNumberUtil::NO_PARSING_ERROR) {
            PG_RETURN_BOOL(false);
        }

        PG_RETURN_BOOL(phone_util.IsValidNumberForRegion(number, region));
    }
    catch (const std::exception &e) {
        elog(ERROR, "phonenumber catch exception, what %s", e.what());
    }
    catch (...) {
        elog(ERROR, "phonenumber catch unknown exception");
    }

    PG_RETURN_BOOL(false);
}

Datum format_number(PG_FUNCTION_ARGS)
{
    auto input = PG_GETARG_TEXT_P(0);
    auto oid = PG_GETARG_OID(1);

    try {
        using namespace i18n::phonenumbers;
        PhoneNumber number;

        const std::string region;
        const auto numberStr = text2string(input);
        const auto format = ENUM_HELPER.oid2format(oid);

        auto err = phone_util.Parse(numberStr, region, &number);
        if (err != PhoneNumberUtil::NO_PARSING_ERROR) {
            elog(LOG, "phonenumber: failed to parse %s", numberStr.c_str());
            PG_RETURN_TEXT_P(input);
        }

        std::string formatted;
        phone_util.Format(number, format, &formatted);

        PG_RETURN_TEXT_P(string2text(formatted));
    }
    catch (const std::exception &e) {
        elog(ERROR, "phonenumber catch exception, what %s", e.what());
    }
    catch (...) {
        elog(ERROR, "phonenumber catch unknown exception");
    }
}

Datum format_number_with_region(PG_FUNCTION_ARGS)
{
    auto numberTxt = PG_GETARG_TEXT_P(0);
    auto regionTxt = PG_GETARG_TEXT_P(1);
    auto enumOid = PG_GETARG_OID(2);

    try {
        using namespace i18n::phonenumbers;
        PhoneNumber number;

        const auto numberStr = text2string(numberTxt);
        const auto region = text2string(regionTxt);
        const auto format = ENUM_HELPER.oid2format(enumOid);

        auto err = phone_util.Parse(numberStr, region, &number);
        if (err != PhoneNumberUtil::NO_PARSING_ERROR) {
            elog(LOG, "phonenumber: failed to parse %s", numberStr.c_str());
            PG_RETURN_TEXT_P(numberTxt);
        }

        std::string formatted;
        phone_util.Format(number, format, &formatted);

        PG_RETURN_TEXT_P(string2text(formatted));
    }
    catch (const std::exception &e) {
        elog(ERROR, "phonenumber catch exception, what %s", e.what());
    }
    catch (...) {
        elog(ERROR, "phonenumber catch unknown exception");
    }
}
