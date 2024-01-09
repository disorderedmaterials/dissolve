// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "items/deserialisers.h"
#include "base/lineParser.h"
#include "classes/atomTypeData.h"
#include "classes/braggReflection.h"
#include "classes/neutronWeights.h"
#include "classes/partialSet.h"
#include "classes/partialSetAccumulator.h"
#include "classes/xRayWeights.h"
#include "items/legacy.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "math/histogram3D.h"
#include "math/integerHistogram1D.h"

GenericItemDeserialiser::GenericItemDeserialiser()
{
    // PODs
    registerDeserialiser<bool>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            std::any_cast<bool &>(a) = parser.argb(0);
            return true;
        });
    registerDeserialiser<double>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            std::any_cast<double &>(a) = parser.argd(0);
            return true;
        });
    registerDeserialiser<int>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            std::any_cast<int &>(a) = parser.argi(0);
            return true;
        });

    // Standard Classes / Containers
    registerDeserialiser<std::streampos>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            // NOTE Can't implicit cast streampos into the arg for readArg(), so assume long long int for now.
            long long int pos;
            if (!parser.readArg(pos))
                return false;
            std::any_cast<std::streampos &>(a) = pos;
            return true;
        });
    registerDeserialiser<std::string>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            std::any_cast<std::string &>(a) = parser.line();
            return true;
        });
    registerDeserialiser<std::vector<double>>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            auto &v = std::any_cast<std::vector<double> &>(a);
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            v.clear();
            v.resize(parser.argi(0));
            for (auto &n : v)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                n = parser.argd(0);
            }
            return true;
        });
    registerDeserialiser<std::vector<Vec3<double>>>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            auto &v = std::any_cast<std::vector<Vec3<double>> &>(a);
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            v.clear();
            v.resize(parser.argi(0));
            for (auto &n : v)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                n = parser.arg3d(0);
            }
            return true;
        });

    // Custom Classes
    registerDeserialiser<Array2D<char>>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            auto &v = std::any_cast<Array2D<char> &>(a);
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            auto nRows = parser.argi(0), nColumns = parser.argi(1);
            v.initialise(nRows, nColumns, parser.argb(2));
            for (auto &n : v)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                n = (parser.argsv(0)[0] == 'T' || parser.argsv(0)[0] == 't');
            }
            return true;
        });
    registerDeserialiser<Array2D<double>>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            auto &v = std::any_cast<Array2D<double> &>(a);
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            auto nRows = parser.argi(0), nColumns = parser.argi(1);
            v.initialise(nRows, nColumns, parser.argb(2));
            for (auto &n : v)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                n = parser.argd(0);
            }
            return true;
        });
    registerDeserialiser<Array2D<std::vector<double>>>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            auto &v = std::any_cast<Array2D<std::vector<double>> &>(a);
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            auto nRows = parser.argi(0), nColumns = parser.argi(1);
            v.initialise(nRows, nColumns, parser.argb(2));
            for (auto &data : v)
                if (!GenericItemDeserialiser::deserialise<std::vector<double>>(data, parser, coreData))
                    return false;
            return true;
        });
    registerDeserialiser<Array2D<Data1D>>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            auto &v = std::any_cast<Array2D<Data1D> &>(a);
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            int nRows = parser.argi(0), nColumns = parser.argi(1);
            v.initialise(nRows, nColumns, parser.argb(2));
            for (auto &n : v)
                if (!n.deserialise(parser))
                    return false;
            return true;
        });
    registerDeserialiser<AtomTypeMix>(simpleDeserialiseCore<AtomTypeMix>);
    registerDeserialiser<Data1D>(simpleDeserialise<Data1D>);
    registerDeserialiser<Data2D>(simpleDeserialise<Data2D>);
    registerDeserialiser<Data3D>(simpleDeserialise<Data3D>);
    registerDeserialiser<Histogram1D>(simpleDeserialise<Histogram1D>);
    registerDeserialiser<Histogram2D>(simpleDeserialise<Histogram2D>);
    registerDeserialiser<Histogram3D>(simpleDeserialise<Histogram3D>);
    registerDeserialiser<IntegerHistogram1D>(simpleDeserialise<IntegerHistogram1D>);
    registerDeserialiser<NeutronWeights>(simpleDeserialiseCore<NeutronWeights>);
    registerDeserialiser<PartialSet>(simpleDeserialiseCore<PartialSet>);
    registerDeserialiser<PartialSetAccumulator>(simpleDeserialise<PartialSetAccumulator>);
    registerDeserialiser<SampledDouble>(simpleDeserialise<SampledDouble>);
    registerDeserialiser<SampledVector>(simpleDeserialise<SampledVector>);
    registerDeserialiser<Vec3<int>>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            auto &v = std::any_cast<Vec3<int> &>(a);
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            v = parser.arg3i(0);
            return true;
        });
    registerDeserialiser<Vec3<double>>(
        [](std::any &a, LineParser &parser, const CoreData &coreData)
        {
            auto &v = std::any_cast<Vec3<double> &>(a);
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            v = parser.arg3d(0);
            return true;
        });
    registerDeserialiser<XRayWeights>(simpleDeserialiseCore<XRayWeights>);

    // Containers of Custom Classes
    registerDeserialiser<std::vector<BraggReflection>>(vectorDeserialise<BraggReflection>);

    // Legacy Classes
    registerLegacyDeserialiser<LegacyAtomTypeListItem>(simpleDeserialiseCore<LegacyAtomTypeListItem>);
}

/*
 * Deserialisers
 */

// Deserialise object of specified type
bool GenericItemDeserialiser::deserialiseObject(std::any &a, LineParser &parser, const CoreData &coreData) const
{
    // Find a suitable deserialiser and call it
    auto it = deserialisers_.find(a.type());
    if (it == deserialisers_.end())
        it = legacyDeserialisers_.find(a.type());
    if (it == legacyDeserialisers_.end())
        throw(std::runtime_error(fmt::format(
            "Item of type '{}' cannot be deserialised as no suitable deserialiser has been registered.\n", a.type().name())));

    return (it->second)(a, parser, coreData);
}

/*
 * Instance
 */

// Return the deserialiser instance
const GenericItemDeserialiser &GenericItemDeserialiser::instance()
{
    static GenericItemDeserialiser instance;

    return instance;
}

// Deserialise supplied object
bool GenericItemDeserialiser::deserialise(std::any &a, LineParser &parser, const CoreData &coreData)
{
    return instance().deserialiseObject(a, parser, coreData);
}

// Return whether supplied object is a legacy object
bool GenericItemDeserialiser::isLegacyObject(std::any &object) { return instance().hasLegacyDeserialiser(object); }
