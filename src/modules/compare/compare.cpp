// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/compare/compare.h"
#include "classes/dataSource.h"
#include "keywords/dataSource.h"
#include "keywords/rangeVector.h"

CompareModule::CompareModule() : Module(ModuleTypes::Compare)
{
    keywords_.setOrganisation("Options");
    keywords_.add<EnumOptionsKeyword<DataSource<Data1D>::DataSourceType>>(
        "DataType", "Specify whether the data is in Dissolve (Internal) or in the file system (External)", dataType_,
        DataSource<Data1D>::dataSourceTypes());
    keywords_.add<DataSourceKeyword<Data1D>>("Data1D", "Specify one-dimensional test reference data", data1dSources_,
                                             "EndData1D");
    keywords_.add<EnumOptionsKeyword<Error::ErrorType>>("ErrorType", "Type of error calculation to use", errorType_,
                                                        Error::errorTypes());
    keywords_.add<RangeVectorKeyword>("ErrorRange", "Ranges over which to calculate RFactors", ranges_);
}

const std::vector<DataSourceKeyword<Data1D>::DataPair> &CompareModule::data1dSources() const { return data1dSources_; }
const std::map<const DataSourceKeyword<Data1D>::DataPair *, RangeErrorPair> &CompareModule::data1dSourcesErrors() const
{
    return data1dSourcesErrors_;
}
