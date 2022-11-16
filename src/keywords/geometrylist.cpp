// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/geometrylist.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "templates/algorithms.h"

GeometryListKeyword::GeometryListKeyword::GeometryListKeyword(std::vector<Geometry> &data, Geometry::GeometryType geometryType)
    : KeywordBase(typeid(this)), data_(data), geometryType_(geometryType)
{
}

/*
 * Data
 */

// Return reference to vector of data
std::vector<Geometry> &GeometryListKeyword::data() { return data_; };
const std::vector<Geometry> &GeometryListKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int GeometryListKeyword::minArguments() const
{
    if (geometryType_ == Geometry::DistanceType)
        return 3;
    else if (geometryType_ == Geometry::AngleType)
        return 4;
    else
        return 5;
}

// Return maximum number of arguments accepted
std::optional<int> GeometryListKeyword::maxArguments() const { return minArguments(); }

// Deserialise from supplied LineParser, starting at given argument offset
bool GeometryListKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    Geometry &g = data_.emplace_back();
    for (auto i = startArg; i <= (startArg + minArguments() - 1); i++)
    {
        if (parser.argi(i) < 1)
            return Messenger::error("Index value, {}, not appropriate", parser.argi(i));
    }

    if (minArguments() == 3)
        g.set(parser.argd(2 + startArg), parser.argi(startArg) - 1, parser.argi(1 + startArg) - 1);
    else if (maxArguments() == 4)
        g.set(parser.argd(3 + startArg), parser.argi(startArg) - 1, parser.argi(1 + startArg) - 1,
              parser.argi(2 + startArg) - 1);
    else
        g.set(parser.argd(4 + startArg), parser.argi(startArg) - 1, parser.argi(1 + startArg) - 1,
              parser.argi(2 + startArg) - 1, parser.argi(3 + startArg) - 1);

    return true;
}

// Serialise data to specified LineParser
bool GeometryListKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_.empty())
        return true;

    std::string index;

    for (auto &ref : data_)
    {
        index.clear();
        for (auto n = 0; n < minArguments() - 1; ++n)
            index += fmt::format("  {}", ref.indices(n) + 1);
        if (!parser.writeLineF("{}{}{}  {:12.4e}\n", prefix, keywordName, index, ref.value()))
            return false;
    }

    return true;
}

// Express as a serialisable value
SerialisedValue GeometryListKeyword::serialise() const
{
    SerialisedValue result;
    switch (geometryType_)
    {
        case Geometry::GeometryType::AngleType:
            result["type"] = "angle";
            break;
        case Geometry::GeometryType::DistanceType:
            result["type"] = "distance";
            break;
        case Geometry::GeometryType::TorsionType:
        default:
            result["type"] = "torsion";
            break;
    }
    std::vector<SerialisedValue> geometries;
    for (auto &ref : data_)
    {
        SerialisedValue item;
        item["value"] = ref.value();
        std::vector<SerialisedValue> indices;
        for (auto n = 0; n < minArguments() - 1; ++n)
            indices.emplace_back(ref.indices(n));
        item["indices"] = indices;
        geometries.push_back(item);
    }
    result["geometries"] = geometries;
    return result;
}

// Read values from a serialisable value
void GeometryListKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    auto typeString = toml::find<std::string>(node, "type");
    if (typeString == "angle")
        geometryType_ = Geometry::GeometryType::AngleType;
    else if (typeString == "distance")
        geometryType_ = Geometry::GeometryType::DistanceType;
    else
        geometryType_ = Geometry::GeometryType::TorsionType;

    for (auto item : node.at("geometries").as_array())
    {
        auto value = toml::find<double>(item, "value");
        auto indices = toml::find<std::vector<int>>(item, "indices");
        Geometry geo;
        switch (geometryType_)
        {
            case Geometry::GeometryType::AngleType:
                geo.set(value, indices[0], indices[1], indices[2]);
                break;
            case Geometry::GeometryType::DistanceType:
                geo.set(value, indices[0], indices[1]);
                break;
            case Geometry::GeometryType::TorsionType:
                geo.set(value, indices[0], indices[1], indices[2], indices[3]);
                break;
            default:
                throw toml::syntax_error(fmt::format("Cannot handle GeometryType {}", geometryType_), node.location());
                break;
        }
        data_.push_back(geo);
    }
}

// Has not changed from initial value
bool GeometryListKeyword::isDefault() const { return data_.empty(); }
