// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/compare/compare.h"
#include "keywords/dataSource.h"
#include "keywords/rangeVector.h"

CompareModule::CompareModule() : Module(ModuleTypes::Compare)
{

    keywords_.setOrganisation("Options");
    keywords_.add<DataSourceKeyword<Data1D, std::function<bool(std::vector<Data1D>)>>>(
        "Data1D", "Specify one-dimensional test reference data", [&](std::vector<Data1D> d) { return addData<Data1D>(d); },
        "EndData1D");
    keywords_.add<DataSourceKeyword<Data2D, std::function<bool(std::vector<Data2D>)>>>(
        "Data2D", "Specify two-dimensional test reference data", [&](std::vector<Data2D> d) { return addData<Data2D>(d); },
        "EndData2D");
    keywords_.add<DataSourceKeyword<Data3D, std::function<bool(std::vector<Data3D>)>>>(
        "Data3D", "Specify three-dimensional test reference data", [&](std::vector<Data3D> d) { return addData<Data3D>(d); },
        "EndData3D");
    keywords_.add<EnumOptionsKeyword<Error::ErrorType>>("ErrorType", "Type of error calculation to use", errorType_,
                                                        Error::errorTypes());
    keywords_.add<RangeVectorKeyword>("RFactorRanges", "Ranges over which to calculate RFactors", ranges_);
}
