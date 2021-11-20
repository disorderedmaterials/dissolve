// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/data1dstore.h"
#include "keywords/data2dstore.h"
#include "keywords/double.h"
#include "keywords/enumoptions.h"
#include "keywords/valuestore.h"
#include "keywords/vector_stringdouble.h"
#include "keywords/vector_stringpair.h"
#include "modules/datatest/datatest.h"

DataTestModule::DataTestModule() : Module()
{
    // Test
    keywords_.add<Data1DStoreKeyword>("Test", "Data1D", "Specify one-dimensional test reference data", test1DData_);
    keywords_.add<Data2DStoreKeyword>("Test", "Data2D", "Specify two-dimensional test reference data", test2DData_);
    keywords_.add<EnumOptionsKeyword<Error::ErrorType>>("Test", "ErrorType", "Type of error calculation to use", errorType_,
                                                        Error::errorTypes());
    keywords_.add<StringPairVectorKeyword>("Test", "InternalData1D", "Specify one-dimensional internal reference and test data",
                                           internal1DData_);
    keywords_.add<StringDoubleVectorKeyword>(
        "Test", "SampledDouble", "Specify test reference values for named SampledDouble (value) data", testSampledDoubleData_);
    keywords_.add<ValueStoreKeyword>("Test", "SampledVector", "Specify test reference values for named SampledVector data",
                                     testSampledVectorData_);
    keywords_.add<DoubleKeyword>("Test", "Threshold", "Threshold for error metric above which test fails", threshold_, 1.0e-15);
}

// Return type of module
std::string_view DataTestModule::type() const { return "DataTest"; }
