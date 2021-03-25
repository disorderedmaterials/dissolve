// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "genericitems/serialisers.h"
#include "base/lineparser.h"
#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/xrayweights.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "math/histogram1d.h"
#include "math/histogram2d.h"
#include "math/histogram3d.h"

GenericItemSerialiser::GenericItemSerialiser()
{
    // PODs
    registerSerialiser<bool>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return parser.writeLineF("{}\n", DissolveSys::btoa(std::any_cast<bool>(a)));
    });
    registerSerialiser<double>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return parser.writeLineF("{}\n", std::any_cast<double>(a));
    });
    registerSerialiser<int>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return parser.writeLineF("{}\n", std::any_cast<int>(a));
    });

    // stdlib
    registerSerialiser<std::string>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return parser.writeLineF("{}\n", std::any_cast<std::string>(a));
    });
    registerSerialiser<std::streampos>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return parser.writeLineF("{}\n", std::any_cast<std::streampos>(a));
    });
    registerSerialiser<std::vector<double>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const std::vector<double> &>(a);
        if (!parser.writeLineF("{}\n", v.size()))
            return false;
        for (auto &n : v)
            if (!parser.writeLineF("{:16.9e}\n", n))
                return false;
        return true;
    });

    // Custom Classes
    registerSerialiser<Array<double>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const Array<double> &>(a);
        if (!parser.writeLineF("{}\n", v.nItems()))
            return false;
        for (auto n = 0; n < v.nItems(); ++n)
            if (!parser.writeLineF("{:16.9e}\n", v.at(n)))
                return false;
        return true;
    });
    registerSerialiser<Array<SampledDouble>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const Array<SampledDouble> &>(a);
        if (!parser.writeLineF("{}\n", v.nItems()))
            return false;
        for (auto n = 0; n < v.nItems(); ++n)
            if (!v.at(n).serialise(parser))
                return false;
        return true;
    });
    registerSerialiser<Array<Vec3<double>>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const Array<Vec3<double>> &>(a);
        if (!parser.writeLineF("{}\n", v.nItems()))
            return false;
        for (auto n = 0; n < v.nItems(); ++n)
            if (!parser.writeLineF("{}\n", v.at(n).x, v.at(n).y, v.at(n).z))
                return false;
        return true;
    });
    registerSerialiser<Array2D<char>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const Array2D<char> &>(a);
        if (!parser.writeLineF("{}  {}  {}\n", v.nRows(), v.nColumns(), DissolveSys::btoa(v.halved())))
            return false;
        for (auto &n : v)
        {
            if (!parser.writeLineF("{}\n", n ? 'T' : 'F'))
                return false;
        }
        return true;
    });
    registerSerialiser<Array2D<double>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const Array2D<double> &>(a);
        if (!parser.writeLineF("{}  {}  {}\n", v.nRows(), v.nColumns(), DissolveSys::btoa(v.halved())))
            return false;
        for (auto &n : v)
        {
            if (!parser.writeLineF("{}\n", n))
                return false;
        }
        return true;
    });
    registerSerialiser<Array2D<std::vector<double>>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const Array2D<std::vector<double>> &>(a);
        if (!parser.writeLineF("{}  {}  {}\n", v.nRows(), v.nColumns(), DissolveSys::btoa(v.halved())))
            return false;
        for (auto &data : v)
        {
            if (!serialiseObject(data, parser, coreData))
                return false;
        }
        return true;
    });
    registerSerialiser<Array2D<Data1D>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const Array2D<Data1D> &>(a);
        if (!parser.writeLineF("{}  {}  {}\n", v.nRows(), v.nColumns(), DissolveSys::btoa(v.halved())))
            return false;
        for (auto &n : v)
            if (!n.serialise(parser))
                return false;
        return true;
    });
    registerSerialiser<AtomTypeList>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const AtomTypeList &>(a).serialise(parser);
    });
    registerSerialiser<Data1D>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const Data1D &>(a).serialise(parser);
    });
    registerSerialiser<Data2D>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const Data2D &>(a).serialise(parser);
    });
    registerSerialiser<Data3D>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const Data3D &>(a).serialise(parser);
    });
    registerSerialiser<Histogram1D>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const Histogram1D &>(a).serialise(parser);
    });
    registerSerialiser<Histogram2D>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const Histogram2D &>(a).serialise(parser);
    });
    registerSerialiser<Histogram3D>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const Histogram3D &>(a).serialise(parser);
    });
    registerSerialiser<NeutronWeights>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const NeutronWeights &>(a).serialise(parser);
    });
    registerSerialiser<PartialSet>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const PartialSet &>(a).serialise(parser);
    });
    registerSerialiser<SampledDouble>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const SampledDouble &>(a).serialise(parser);
    });
    registerSerialiser<Vec3<int>>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        const auto &v = std::any_cast<const Vec3<int> &>(a);
        return parser.writeLineF("{}  {}  {}\n", v.x, v.y, v.z);
    });
    registerSerialiser<XRayWeights>([](const std::any &a, LineParser &parser, const CoreData &coreData) {
        return std::any_cast<const XRayWeights &>(a).serialise(parser);
    });
}

/*
 * Instance
 */

// Return the serialiser instance
const GenericItemSerialiser &GenericItemSerialiser::instance()
{
    static GenericItemSerialiser _instance;

    return _instance;
}

/*
 * Serialisers
 */

// Serialise object of specified type
bool GenericItemSerialiser::serialiseObject(const std::any &a, LineParser &parser, const CoreData &coreData) const
{
    // Find a suitable serialiser and call it
    auto it = serialisers_.find(a.type());
    if (it == serialisers_.end())
        throw(std::runtime_error(fmt::format(
            "Item of type '{}' cannot be serialised as no suitable serialiser has been registered.\n", a.type().name())));

    return (it->second)(a, parser, coreData);
}

/*
 * Static Functions
 */

// Serialise supplied object
bool GenericItemSerialiser::serialise(const std::any &a, LineParser &parser)
{
    static CoreData dummyCoreData;
    return serialise(a, parser, dummyCoreData);
}
bool GenericItemSerialiser::serialise(const std::any &a, LineParser &parser, const CoreData &coreData)
{
    return instance().serialiseObject(a, parser, coreData);
}
