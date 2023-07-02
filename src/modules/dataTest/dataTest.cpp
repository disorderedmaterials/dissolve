// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/dataTest/dataTest.h"
#include "keywords/data1DStore.h"
#include "keywords/data2DStore.h"
#include "keywords/data3DStore.h"
#include "keywords/double.h"
#include "keywords/enumOptions.h"
#include "keywords/valueStore.h"
#include "keywords/vectorStringDouble.h"
#include "keywords/vectorStringPair.h"

DataTestModule::DataTestModule() : Module(ModuleTypes::DataTest)
{
    keywords_.setOrganisation("Test");
    keywords_.add<Data1DStoreKeyword>("Data1D", "Specify one-dimensional test reference data", test1DData_);
    keywords_.add<Data2DStoreKeyword>("Data2D", "Specify two-dimensional test reference data", test2DData_);
    keywords_.add<Data3DStoreKeyword>("Data3D", "Specify three-dimensional test reference data", test3DData_);
    keywords_.add<EnumOptionsKeyword<Error::ErrorType>>("ErrorType", "Type of error calculation to use", errorType_,
                                                        Error::errorTypes());
    keywords_.add<StringPairVectorKeyword>("InternalData1D", "Specify one-dimensional internal reference and test data",
                                           internal1DData_);
    keywords_.add<StringDoubleVectorKeyword>(
        "SampledDouble", "Specify test reference values for named SampledDouble (value) data", testSampledDoubleData_);
    keywords_.add<ValueStoreKeyword>("SampledVector", "Specify test reference values for named SampledVector data",
                                     testSampledVectorData_);
    keywords_.add<DoubleKeyword>("Threshold", "Threshold for error metric above which test fails", threshold_, 1.0e-15);
}
