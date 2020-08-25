/*
    *** DataTest Module - Initialisation
    *** src/modules/datatest/init.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    keywords_.add("Test", new EnumOptionsKeyword<Error::ErrorType>(Error::errorTypes() = Error::PercentError), "ErrorType",
                  "Type of error calculation to use");
    keywords_.add("Test", new ModuleRefListKeyword(targetModule_, 1), "Target", "Module containing target data", "<Module>");
    keywords_.add("Test", new DoubleKeyword(0.1, 1.0e-5), "Threshold", "Test threshold (%error) above which test fails",
                  "<threshold[0.1]>");
}
