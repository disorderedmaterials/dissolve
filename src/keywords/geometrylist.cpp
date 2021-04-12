// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/geometrylist.h"
#include "base/lineparser.h"
#include "classes/coredata.h"

GeometryListKeyword::GeometryListKeyword::GeometryListKeyword(List<Geometry> &data, Geometry::GeometryType type)
    : KeywordData<List<Geometry> &>(KeywordBase::GeometryListData, data), type_(type)
{
}

GeometryListKeyword::~GeometryListKeyword() {}

// Return minimum number of arguments accepted
int GeometryListKeyword::minArguments() const
{
    if (type_ == Geometry::DistanceType)
        return 3;
    else if (type_ == Geometry::AngleType)
        return 4;
    else
        return 5;
}

// Return maximum number of arguments accepted
int GeometryListKeyword::maxArguments() const
{
    if (type_ == Geometry::DistanceType)
        return 3;
    else if (type_ == Geometry::AngleType)
        return 4;
    else
        return 5;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool GeometryListKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    Geometry *g = data_.add();
    for (auto i = startArg; i <= (startArg + maxArguments() - 1); i++)
    {
        if (parser.argi(i) < 1)
            return Messenger::error("Index value, {}, not appropriate", parser.argi(i));
    }

    if (maxArguments() == 3)
        g->set(parser.argd(2 + startArg), parser.argi(startArg) - 1, parser.argi(1 + startArg) - 1);
    else if (maxArguments() == 4)
        g->set(parser.argd(3 + startArg), parser.argi(startArg) - 1, parser.argi(1 + startArg) - 1,
               parser.argi(2 + startArg) - 1);
    else
        g->set(parser.argd(4 + startArg), parser.argi(startArg) - 1, parser.argi(1 + startArg) - 1,
               parser.argi(2 + startArg) - 1, parser.argi(3 + startArg) - 1);

    hasBeenSet();

    return true;
}

// Write keyword data to specified LineParser
bool GeometryListKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    std::string index;

    ListIterator<Geometry> GeoIterator(data_);
    while (Geometry *ref = GeoIterator.iterate())
    {
        index.clear();
        for (auto n = 0; n < maxArguments() - 1; ++n)
            index += fmt::format("  {}", ref->indices(n) + 1);
        if (!parser.writeLineF("{}{}{}  {:12.4e}\n", prefix, keywordName, index, ref->value()))
            return false;
    }

    return true;
}
