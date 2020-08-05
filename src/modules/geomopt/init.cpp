/*
    *** Geometry Optimisation Module - Initialisation
    *** src/modules/geomopt/init.cpp
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
#include "modules/geomopt/geomopt.h"

// Perform any necessary initialisation for the Module
void GeometryOptimisationModule::initialise()
{
    // Method Control
    keywords_.add("Calculation", new IntegerKeyword(1000, 1), "NCycles", "Maximum number of minimisation cycles to perform");
    keywords_.add("Calculation", new DoubleKeyword(1.0e-5, 1.0e-10), "StepSize", "Initial step size to employ");
    keywords_.add("Calculation", new DoubleKeyword(1.0e-4, 1.0e-10), "Tolerance",
                  "Tolerance controlling convergence of algorithm)");
}
