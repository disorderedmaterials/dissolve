// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "genericitems/producers.h"
#include "classes/braggreflection.h"
#include "classes/kvector.h"
#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/partialsetaccumulator.h"
#include "classes/xrayweights.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "math/histogram1d.h"
#include "math/histogram2d.h"
#include "math/histogram3d.h"
#include <ios>

GenericItemProducer::GenericItemProducer()
{
    // PODs
    registerProducer<bool>("bool");
    registerProducer<double>("double");
    registerProducer<int>("int");

    // Standard Classes / Containers
    registerProducer<std::string>("std::string");
    registerProducer<std::streampos>("streampos");
    registerProducer<std::vector<double>>("std::vector<double>");
    registerProducer<std::vector<Vec3<double>>>("std::vector<Vec3<double>>");

    // Custom Classes / Containers
    registerProducer<Array<Vec3<double>>>("Array<Vec3<double>>");
    registerProducer<Array<double>>("Array<double>");
    registerProducer<Array<SampledDouble>>("Array<SampledDouble>");
    registerProducer<Array<Vec3<double>>>("Array<Vec3<double>>");
    registerProducer<Array2D<double>>("Array2D<double>");
    registerProducer<Array2D<std::vector<double>>>("Array2D<std::vector<double>>");
    registerProducer<Array2D<Data1D>>("Array2D<Data1D>");
    registerProducer<AtomTypeList>("AtomTypeList");
    registerProducer<Data1D>("Data1D");
    registerProducer<Data2D>("Data2D");
    registerProducer<Data3D>("Data3D");
    registerProducer<Histogram1D>("Histogram1D");
    registerProducer<Histogram2D>("Histogram2D");
    registerProducer<Histogram3D>("Histogram3D");
    registerProducer<NeutronWeights>("NeutronWeights");
    registerProducer<PartialSet>("PartialSet");
    registerProducer<PartialSetAccumulator>("PartialSetAccumulator");
    registerProducer<SampledDouble>("SampledDouble");
    registerProducer<Vec3<int>>("Vec3<int>");
    registerProducer<Vec3<double>>("Vec3<double>");
    registerProducer<XRayWeights>("XRayWeights");

    // Containers of Custom Classes
    registerProducer<std::vector<BraggReflection>>("std::vector<BraggReflection>");
    registerProducer<std::vector<KVector>>("std::vector<KVector>");
}

/*
 * Producers
 */

// Produce object of specified type
std::any GenericItemProducer::produce(const std::type_info &objectType) const
{
    auto it = producers_.find(objectType);
    if (it == producers_.end())
        throw(std::runtime_error(
            fmt::format("A producer has not been registered for type '{}', so a new object of this type cannot be created.\n",
                        objectType.name())));

    return (it->second)();
}

// Produce object od named class
std::any GenericItemProducer::produce(const std::string_view className) const
{
    auto it =
        std::find_if(classNames_.begin(), classNames_.end(), [className](auto &item) { return item.second == className; });
    if (it == classNames_.end())
        throw(std::runtime_error(fmt::format(
            "A producer has not been registered for class name '{}', so a new object of this type cannot be created.\n",
            className)));

    auto producer = producers_.find(it->first);
    return (producer->second)();
}

// Return class name for specified type
std::string GenericItemProducer::className(const std::type_info &objectType) const
{
    auto it = classNames_.find(objectType);
    if (it == classNames_.end())
        throw(std::runtime_error(fmt::format("Class name has not been registered for type '{}'.\n", objectType.name())));

    return it->second;
}

/*
 * Instance
 */

// Return the producer instance
const GenericItemProducer &GenericItemProducer::instance()
{
    static GenericItemProducer instance;

    return instance;
}

/*
 * Static Functions
 */

// Create new item of the named class type
std::any GenericItemProducer::create(std::string_view className) { return instance().produce(className); }
