// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/xrayweights.h"
#include "genericitems/list.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "math/histogram1d.h"
#include "math/histogram2d.h"
#include "math/histogram3d.h"
#include <ios>

// Register all producers
void GenericList::registerProducers()
{
    // PODs
    registerProducer<bool>("bool");
    registerProducer<double>("double");
    registerProducer<int>("int");

    // stdlib
    registerProducer<std::string>("std::string");
    registerProducer<std::streampos>("streampos");
    registerProducer<std::vector<double>>("std::vector<double>");

    // Custom Classes
    registerProducer<Array<double>>("Array<double>");
    registerProducer<Array<SampledDouble>>("Array<SampledDouble>");
    registerProducer<Array<Vec3<double>>>("Array<Vec3<double>>");
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
    registerProducer<Vec3<int>>("Vec3<int>");
    registerProducer<Vec3<double>>("Vec3<double>");
    registerProducer<XRayWeights>("XRayWeights");
}
