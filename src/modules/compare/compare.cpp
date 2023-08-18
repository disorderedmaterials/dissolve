// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/compare/compare.h"
#include "keywords/dataSource.h"
#include "math/error.h"

// TODO: CHANGE ADD TO MODULETYPES

CompareModule::CompareModule() : Module(ModuleTypes::DataTest)
{
    keywords_.setOrganisation("Options");
    keywords_.add<DataSourceKeyword<Data1D, std::function<bool(Data1D)>>>(
        "Data1D", "Specify one-dimensional test reference data", &CompareModule::addData<Data1D>, this, std::placeholders::_1);
    keywords_.add<DataSourceKeyword<Data2D, std::function<bool(Data2D)>>>(
        "Data2D", "Specify two-dimensional test reference data", &CompareModule::addData<Data2D>, this, std::placeholders::_1);
    keywords_.add<DataSourceKeyword<Data3D, std::function<bool(Data3D)>>>(
        "Data3D", "Specify three-dimensional test reference data", &CompareModule::addData<Data3D>, this,
        std::placeholders::_1);
    keywords_.add<EnumOptionsKeyword<Error::ErrorType>>("ErrorType", "Type of error calculation to use", errorType_,
                                                        Error::errorTypes());
}
