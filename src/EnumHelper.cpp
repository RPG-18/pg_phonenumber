#include "EnumHelper.h"

extern "C" {
#include <catalog/namespace.h>
#include <catalog/pg_enum.h>
#include <catalog/pg_proc.h>

#include <utils/syscache.h>
#include <utils/catcache.h>

#include <access/htup_details.h>
}

namespace
{

struct EnumLabel
{
    int index;
    const char *label;
};

int enum_label_cmp(const void *left, const void *right)
{
    const char *l = ((EnumLabel *) left)->label;
    const char *r = ((EnumLabel *) right)->label;
    return strcmp(l, r);
}

static inline Oid XProcTupleGetOid(HeapTuple proc_tup)
{
#if PG_VERSION_NUM >= 120000
    Form_pg_proc proc_struct;
    proc_struct = (Form_pg_proc) GETSTRUCT(proc_tup);
    return proc_struct->oid;
#else
    return HeapTupleGetOid(proc_tup);
#endif
}

/*-------------------------------------------------------------------------
 * enum code
 * Copyright (c) 2010, PostgreSQL Global Development Group
 * Written by Joey Adams <joeyadams3.14159@gmail.com>.
 * https://www.postgresql.org/message-id/AANLkTimKGJgoY+03aFPwyzxGKoECvMQG1q_s8tCXX1aO@mail.gmail.com
 *-------------------------------------------------------------------------
 * getEnumLabelOids
 *    Look up the OIDs of enum labels.  Enum label OIDs are needed to
 *    return values of a custom enum type from a C function.
 *
 *    Callers should typically cache the OIDs produced by this function
 *    using fn_extra, as retrieving enum label OIDs is somewhat expensive.
 *
 *    Every labels[i].index must be between 0 and count, and oid_out
 *    must be allocated to hold count items.  Note that getEnumLabelOids
 *    sorts the labels[] array passed to it.
 *
 *    Any labels not found in the enum will have their corresponding
 *    oid_out entries set to InvalidOid.
 *
 *    Sample usage:
 *
 *    -- SQL --
 *    CREATE TYPE colors AS ENUM ('red', 'green', 'blue');
 *
 *    -- C --
 *    enum Colors {RED, GREEN, BLUE, COLOR_COUNT};
 *
 *    static EnumLabel enum_labels[COLOR_COUNT] =
 *    {
 *        {RED,   "red"},
 *        {GREEN, "green"},
 *        {BLUE,  "blue"}
 *    };
 *
 *    Oid *label_oids = palloc(COLOR_COUNT * sizeof(Oid));
 *    getEnumLabelOids("colors", enum_labels, label_oids, COLOR_COUNT);
 *
 *    PG_RETURN_OID(label_oids[GREEN]);
 */
void getEnumLabelOids(const char *typname, EnumLabel labels[], Oid oid_out[],
                      int count)
{
    CatCList *list;
    Oid enumtypoid;
    int total;
    int i;
    EnumLabel key;
    EnumLabel *found;

    enumtypoid = TypenameGetTypid(typname);
    Assert(OidIsValid(enumtypoid));

    qsort(labels, count, sizeof(EnumLabel), enum_label_cmp);

    for (i = 0; i < count; i++) {
        /* Initialize oid_out items to InvalidOid. */
        oid_out[i] = InvalidOid;

        /* Make sure EnumLabel indices are in range. */
        Assert(labels[i].index >= 0 && labels[i].index < count);
    }

    list = SearchSysCacheList1(ENUMTYPOIDNAME, ObjectIdGetDatum(enumtypoid));
    total = list->n_members;

    for (i = 0; i < total; i++) {
        HeapTuple tup = &list->members[i]->tuple;

        Oid oid = XProcTupleGetOid(tup);
        Form_pg_enum en = (Form_pg_enum) GETSTRUCT(tup);

        key.label = NameStr(en->enumlabel);
        found = static_cast<EnumLabel *>(
            bsearch(&key, labels, count, sizeof(EnumLabel), enum_label_cmp));
        if (found != NULL)
            oid_out[found->index] = oid;
    }

    ReleaseCatCacheList(list);
}
} // namespace
using namespace i18n::phonenumbers;

using Lock = std::lock_guard<std::mutex>;

PhoneNumberUtil::PhoneNumberFormat EnumHelper::oid2format(Oid oid)
{
    Lock l(m_mutex);
    if (m_oid2format.empty()) {
        std::call_once(m_flag, [this]()
        { load(); });
    }

    return m_oid2format[oid];
};

void EnumHelper::load()
{
    const int formatCount = 4;
    auto formatOids = static_cast<Oid *>(palloc(formatCount * sizeof(Oid)));
    EnumLabel formatEnumLabels[4] = {
        {PhoneNumberUtil::E164, "e164"},
        {PhoneNumberUtil::INTERNATIONAL, "international"},
        {PhoneNumberUtil::NATIONAL, "national"},
        {PhoneNumberUtil::RFC3966, "rfc3966"},
    };

    getEnumLabelOids("phonenumber_format", formatEnumLabels, formatOids,
                     formatCount);
    for (int i = 0; i < formatCount; i++) {
        m_oid2format[formatOids[i]] =
            static_cast<PhoneNumberUtil::PhoneNumberFormat>(i);
    };
}