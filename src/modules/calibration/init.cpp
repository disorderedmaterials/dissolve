/*
    *** Calibration Module - Initialisation
    *** src/modules/calibration/init.cpp
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
#include "modules/calibration/calibration.h"

// Perform any necessary initialisation for the Module
void CalibrationModule::initialise()
{
    // Calculation
    keywords_.add("Calculation", new BoolKeyword(true), "OnlyWhenEnergyStable",
                  "Only perform calibrations when all related Configuration energies are stable");

    // RDF Calibration
    keywords_.add("RDF Calibration", new ModuleRefListKeyword(intraBroadeningModules_, std::vector<std::string>{"RDF"}),
                  "AdjustIntraBroadening", "Add specified RDF module as a target for IntraBroadening adjustment",
                  "<RDFModule>");

    // NeutronSQ Calibration
    keywords_.add("NeutronSQ Calibration",
                  new ModuleRefListKeyword(intraBroadeningNeutronGRReferences_, std::vector<std::string>{"NeutronSQ"}),
                  "IntraBroadeningNeutronGRReference",
                  "Add G(r) data in the specified NeutronSQ module as a reference for IntraBroadening adjustment",
                  "<NeutronSQModule>");
    keywords_.add("NeutronSQ Calibration",
                  new ModuleRefListKeyword(intraBroadeningNeutronSQReferences_, std::vector<std::string>{"NeutronSQ"}),
                  "IntraBroadeningNeutronSQReference",
                  "Add S(Q) data in the specified NeutronSQ module as a reference for IntraBroadening adjustment",
                  "<NeutronSQModule>");
}
