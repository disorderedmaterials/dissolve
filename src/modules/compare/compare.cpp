// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/compare/compare.h"
#include "keywords/dataSource.h"
#include "keywords/rangeVector.h"

CompareModule::CompareModule() : Module(ModuleTypes::Compare)
{
    keywords_.setOrganisation("Options");
    keywords_.add<DataSourceKeyword<Data1D, Data1DImportFileFormat>>("Data1D", "Specify one-dimensional test reference data",
                                                                     dataSources_, "EndData1D");
    keywords_.add<EnumOptionsKeyword<Error::ErrorType>>("ErrorType", "Type of error calculation to use", errorType_,
                                                        Error::errorTypes());
    keywords_.add<RangeVectorKeyword>("RFactorRanges", "Ranges over which to calculate RFactors", ranges_);
}

const std::vector<DataSourceKeywordBase::DataPair> &CompareModule::dataSources() { return dataSources_; }
