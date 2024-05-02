// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "items/serialisers.h"
#include "base/lineParser.h"
#include "classes/braggReflection.h"
#include "classes/kVector.h"
#include "classes/neutronWeights.h"
#include "classes/partialSet.h"
#include "classes/partialSetAccumulator.h"
#include "classes/xRayWeights.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "math/histogram3D.h"
#include "math/integerHistogram1D.h"

GenericItemSerialiser::GenericItemSerialiser()
{
    // PODs
    registerSerialiser<bool>([](const std::any &a, LineParser &parser)
                             { return parser.writeLineF("{}\n", DissolveSys::btoa(std::any_cast<bool>(a))); });
    registerSerialiser<double>([](const std::any &a, LineParser &parser)
                               { return parser.writeLineF("{}\n", std::any_cast<double>(a)); });
    registerSerialiser<int>([](const std::any &a, LineParser &parser)
                            { return parser.writeLineF("{}\n", std::any_cast<int>(a)); });

    // Standard Classes / Containers
    registerSerialiser<std::string>([](const std::any &a, LineParser &parser)
                                    { return parser.writeLineF("{}\n", std::any_cast<std::string>(a)); });
    registerSerialiser<std::streampos>([](const std::any &a, LineParser &parser)
                                       { return parser.writeLineF("{}\n", std::any_cast<std::streampos>(a)); });
    registerSerialiser<std::vector<double>>(
        [](const std::any &a, LineParser &parser)
        {
            const auto &v = std::any_cast<const std::vector<double> &>(a);
            if (!parser.writeLineF("{}\n", v.size()))
                return false;
            for (auto &n : v)
                if (!parser.writeLineF("{}\n", n))
                    return false;
            return true;
        });
    registerSerialiser<std::vector<Vec3<double>>>(
        [](const std::any &a, LineParser &parser)
        {
            const auto &v = std::any_cast<const std::vector<Vec3<double>> &>(a);
            if (!parser.writeLineF("{}\n", v.size()))
                return false;
            for (auto &n : v)
                if (!parser.writeLineF("{} {} {}\n", n.x, n.y, n.z))
                    return false;
            return true;
        });

    // Custom Classes / Containers
    registerSerialiser<Array2D<char>>(
        [](const std::any &a, LineParser &parser)
        {
            const auto &v = std::any_cast<const Array2D<char> &>(a);
            if (!parser.writeLineF("{}  {}  {}\n", v.nRows(), v.nColumns(), DissolveSys::btoa(v.halved())))
                return false;
            for (auto &n : v)
                if (!parser.writeLineF("{}\n", n ? 'T' : 'F'))
                    return false;
            return true;
        });
    registerSerialiser<Array2D<double>>(
        [](const std::any &a, LineParser &parser)
        {
            const auto &v = std::any_cast<const Array2D<double> &>(a);
            if (!parser.writeLineF("{}  {}  {}\n", v.nRows(), v.nColumns(), DissolveSys::btoa(v.halved())))
                return false;
            for (auto &n : v)
                if (!parser.writeLineF("{}\n", n))
                    return false;
            return true;
        });
    registerSerialiser<Array2D<std::vector<double>>>(
        [](const std::any &a, LineParser &parser)
        {
            const auto &v = std::any_cast<const Array2D<std::vector<double>> &>(a);
            if (!parser.writeLineF("{}  {}  {}\n", v.nRows(), v.nColumns(), DissolveSys::btoa(v.halved())))
                return false;
            for (auto &data : v)
                if (!GenericItemSerialiser::serialise<std::vector<double>>(data, parser))
                    return false;
            return true;
        });
    registerSerialiser<Array2D<Data1D>>(
        [](const std::any &a, LineParser &parser)
        {
            const auto &v = std::any_cast<const Array2D<Data1D> &>(a);
            if (!parser.writeLineF("{}  {}  {}\n", v.nRows(), v.nColumns(), DissolveSys::btoa(v.halved())))
                return false;
            for (auto &n : v)
                if (!n.serialise(parser))
                    return false;
            return true;
        });
    registerSerialiser<AtomTypeMix>(simpleSerialise<AtomTypeMix>);
    registerSerialiser<Data1D>(simpleSerialise<Data1D>);
    registerSerialiser<Data2D>(simpleSerialise<Data2D>);
    registerSerialiser<Data3D>(simpleSerialise<Data3D>);
    registerSerialiser<Histogram1D>(simpleSerialise<Histogram1D>);
    registerSerialiser<Histogram2D>(simpleSerialise<Histogram2D>);
    registerSerialiser<Histogram3D>(simpleSerialise<Histogram3D>);
    registerSerialiser<IntegerHistogram1D>(simpleSerialise<IntegerHistogram1D>);
    registerSerialiser<NeutronWeights>(simpleSerialise<NeutronWeights>);
    registerSerialiser<PartialSet>(simpleSerialise<PartialSet>);
    registerSerialiser<PartialSetAccumulator>(simpleSerialise<PartialSetAccumulator>);
    registerSerialiser<SampledData1D>(simpleSerialise<SampledData1D>);
    registerSerialiser<SampledDouble>(simpleSerialise<SampledDouble>);
    registerSerialiser<SampledVector>(simpleSerialise<SampledVector>);
    registerSerialiser<Vec3<int>>(
        [](const std::any &a, LineParser &parser)
        {
            const auto &v = std::any_cast<const Vec3<int> &>(a);
            return parser.writeLineF("{}  {}  {}\n", v.x, v.y, v.z);
        });
    registerSerialiser<XRayWeights>(simpleSerialise<XRayWeights>);

    // Containers of Custom Classes
    registerSerialiser<std::vector<BraggReflection>>(vectorSerialise<BraggReflection>);
}

/*
 * Instance
 */

// Return the serialiser instance
const GenericItemSerialiser &GenericItemSerialiser::instance()
{
    static GenericItemSerialiser instance;

    return instance;
}

/*
 * Serialisers
 */

// Serialise object of specified type
bool GenericItemSerialiser::serialiseObject(const std::any &a, LineParser &parser) const
{
    // Find a suitable serialiser and call it
    auto it = serialisers_.find(a.type());
    if (it == serialisers_.end())
        throw(std::runtime_error(fmt::format(
            "Item of type '{}' cannot be serialised as no suitable serialiser has been registered.\n", a.type().name())));

    return (it->second)(a, parser);
}

/*
 * Static Functions
 */

// Serialise supplied object
bool GenericItemSerialiser::serialise(const std::any &a, LineParser &parser) { return instance().serialiseObject(a, parser); }
