/*
    *** Forces Module - Initialisation
    *** src/modules/forces/init.cpp
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
#include "modules/forces/forces.h"

// Perform any necessary initialisation for the Module
void ForcesModule::initialise()
{
    // Test
    keywords_.add("Test", new BoolKeyword(false), "Test", "Test parallel force routines against simplified, serial ones");
    keywords_.add("Test", new BoolKeyword(false), "TestAnalytic",
                  "Compare parallel force routines against exact (analytic) force rather than tabulated values");
    keywords_.add("Test", new BoolKeyword(true), "TestInter", "Include interatomic forces in test");
    keywords_.add("Test", new BoolKeyword(true), "TestIntra", "Include intramolecular forces in test");
    keywords_.add("Test", new FileAndFormatKeyword(referenceForces_, "EndTestReference"), "TestReference",
                  "Reference forces for test");
    keywords_.add("Test", new DoubleKeyword(0.1), "TestThreshold", "Threshold of force (%) at which test comparison will fail");

    // Export
    keywords_.add("Export", new FileAndFormatKeyword(exportedForces_, "EndSaveForces"), "SaveForces",
                  "File to save calculated forces to");
}
