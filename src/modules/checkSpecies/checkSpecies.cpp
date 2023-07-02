// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/checkSpecies/checkSpecies.h"
#include "keywords/double.h"
#include "keywords/optionalDouble.h"
#include "keywords/species.h"
#include "keywords/vectorIntDouble.h"
#include "keywords/vectorIntString.h"

CheckSpeciesModule::CheckSpeciesModule() : Module(ModuleTypes::CheckSpecies)
{
    // Target Species
    keywords_.addTarget<SpeciesKeyword>("Species", "Target species to check", targetSpecies_);

    keywords_.setOrganisation("Options", "Atom Checks");
    keywords_.add<IntegerStringVectorKeyword>("AtomType", "Check that atom index <id> has the atom type <type>", atomTypes_, 1,
                                              1);
    keywords_.add<DoubleKeyword>("ChargeTolerance", "Tolerance beyond which charges are said to differ", chargeTolerance_,
                                 1.0e-5);
    keywords_.add<OptionalDoubleKeyword>("TotalCharge", "Check the total charge of the species against the specified value",
                                         totalCharge_, -5.0, std::nullopt, 1.0, "Off");

    keywords_.setOrganisation("Options", "Intramolecular Checks");
    keywords_.add<IntegerDoubleVectorKeyword>("Bond", "Check that the bond <i>-<j> has the correct parameters", bondParameters_,
                                              2);
    keywords_.add<IntegerDoubleVectorKeyword>("Angle", "Check that the angle <i>-<j>-<k> has the correct parameters",
                                              angleParameters_, 3);
    keywords_.add<IntegerDoubleVectorKeyword>("Torsion", "Check that the torsion <i>-<j>-<k>-<l> has the correct parameters",
                                              torsionParameters_, 4);
    keywords_.add<IntegerDoubleVectorKeyword>("Improper", "Check that the improper <i>-<j>-<k>-<l> has the correct parameters",
                                              improperParameters_, 4);
    keywords_.add<DoubleKeyword>("Tolerance", "Tolerance beyond which parameters are said to differ", tolerance_, 1.0e-5);
}
