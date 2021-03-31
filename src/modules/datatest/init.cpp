// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "math/error.h"
#include "modules/datatest/datatest.h"

// Perform any necessary initialisation for the Module
void DataTestModule::initialise()
{
    // Test
    keywords_.add("Test", new Data1DStoreKeyword(test1DData_), "Data1D", "Specify one-dimensional test reference data",
                  "<target> <fileformat> <filename> [options...]");
    keywords_.add("Test", new Data2DStoreKeyword(test2DData_), "Data2D", "Specify two-dimensional test reference data",
                  "<target> <fileformat> <filename> [options...]");
    keywords_.add("Test", new EnumOptionsKeyword<Error::ErrorType>(Error::errorTypes() = Error::EuclideanError), "ErrorType",
                  "Type of error calculation to use");
    keywords_.add("Test", new StringPairVectorKeyword(internal1DData_), "InternalData1D",
                  "Specify one-dimensional internal reference and test data", "<target1> <target2>");
    keywords_.add("Test", new DoubleKeyword(5.0e-3, 1.0e-5), "Threshold", "Threshold for error metric above which test fails");
}
