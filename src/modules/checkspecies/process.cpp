/*
    *** CheckSpecies Module - Processing
    *** src/modules/checkspecies/process.cpp
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

#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "modules/checkspecies/checkspecies.h"

// Run main processing
bool CheckSpeciesModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Retrieve necessary keyword values
    Species *sp = keywords_.retrieve<Species *>("Species");
    if (!sp)
        return Messenger::error("No target species provided.\n");
    const auto tolerance = keywords_.asDouble("Tolerance");

    Messenger::print("CheckSpecies: Target species is '{}'.\n", sp->name());
    Messenger::print("CheckSpecies: Tolerance for parameter checks is {:.5e}.", tolerance);

    // Check atom types
    auto nAtomTypesFailed = 0;
    if (atomTypes_.size() != 0)
    {
        Messenger::print("\nChecking atom types...\n");

        // Each tuple should contain a single integer (the atom index) and a single string (the type name)
        for (const auto &indexName : atomTypes_)
        {
            // Get specified atom - tuple contains 'human-readable' indices from 1 - N...
            auto i = std::get<0>(indexName).at(0);
            auto *spAtom = sp->atom(i - 1);
            if (!spAtom)
                return Messenger::error("Atom index {} is out of range ({} atoms in species).\n", i, sp->nAtoms());
            auto at = spAtom->atomType();

            // Get type name to validate against
            std::string_view typeName = std::get<1>(indexName).at(0);
            if ((!at) || ((*at).name() != typeName))
            {
                Messenger::print("Type for atom {} is {}, which does not match that expected ({}).\n", i,
                                 at ? (*at).name() : "undefined", typeName);
                ++nAtomTypesFailed;
            }
            else
                Messenger::print("Atom {} has correct type ({}).\n", i, typeName);
        }

        if (nAtomTypesFailed == 0)
            Messenger::print("\nAll atom types validated successfully.");
        else
            Messenger::print("\n{} atoms have incorrect types.", nAtomTypesFailed);
    }

    // Check bond parameters
    auto nBondsFailed = 0;
    if (bondParameters_.size() != 0)
    {
        Messenger::print("\nChecking bond parameters...\n");
        for (const auto &bondData : bondParameters_)
        {
            // Find the bond in the species
            auto &indices = std::get<0>(bondData);
            const auto optBond = sp->getBond(indices.at(0) - 1, indices.at(1) - 1);
            if (!optBond)
                return Messenger::error("No bond {}-{} exists in the species.", indices.at(0), indices.at(1));
            const SpeciesBond &bond = *optBond;

            // Check parameter values
            if (checkParameters(bond.parameters(), std::get<1>(bondData), tolerance))
                Messenger::print("Bond {}-{} has the correct parameters.", indices.at(0), indices.at(1));
            else
            {
                Messenger::print("Parameters for bond {}-{} are incorrect - see above.", indices.at(0), indices.at(1));
                ++nBondsFailed;
            }
        }
    }

    // Check angle parameters
    auto nAnglesFailed = 0;
    if (angleParameters_.size() != 0)
    {
        Messenger::print("\nChecking angle parameters...\n");
        for (const auto &angleData : angleParameters_)
        {
            // Find the angle in the species
            auto &indices = std::get<0>(angleData);
            const auto optAngle = sp->getAngle(indices.at(0) - 1, indices.at(1) - 1, indices.at(2) - 1);
            if (!optAngle)
                return Messenger::error("No angle {}-{}-{} exists in the species.", indices.at(0), indices.at(1),
                                        indices.at(2));
            const SpeciesAngle &angle = *optAngle;

            // Check parameter values
            if (checkParameters(angle.parameters(), std::get<1>(angleData), tolerance))
                Messenger::print("Angle {}-{}-{} has the correct parameters.", indices.at(0), indices.at(1), indices.at(2));
            else
            {
                Messenger::print("Parameters for angle {}-{}-{} are incorrect - see above.", indices.at(0), indices.at(1),
                                 indices.at(2));
                ++nAnglesFailed;
            }
        }
    }

    // Check torsion parameters
    auto nTorsionsFailed = 0;
    if (torsionParameters_.size() != 0)
    {
        Messenger::print("\nChecking torsion parameters...\n");
        for (const auto &torsionData : torsionParameters_)
        {
            // Find the torsion in the species
            auto &indices = std::get<0>(torsionData);
            const auto optTorsion = sp->getTorsion(indices.at(0) - 1, indices.at(1) - 1, indices.at(2) - 1, indices.at(3) - 1);
            if (!optTorsion)
                return Messenger::error("No torsion {}-{}-{}-{} exists in the species.", indices.at(0), indices.at(1),
                                        indices.at(2), indices.at(3));
            const SpeciesTorsion &torsion = *optTorsion;

            // Check parameter values
            if (checkParameters(torsion.parameters(), std::get<1>(torsionData), tolerance))
                Messenger::print("Torsion {}-{}-{}-{} has the correct parameters.", indices.at(0), indices.at(1), indices.at(2),
                                 indices.at(3));
            else
            {
                Messenger::print("Parameters for torsion {}-{}-{}-{} are incorrect - see above.", indices.at(0), indices.at(1),
                                 indices.at(2), indices.at(3));
                ++nTorsionsFailed;
            }
        }
    }

    // Check improper parameters
    auto nImpropersFailed = 0;
    if (improperParameters_.size() != 0)
    {
        Messenger::print("\nChecking improper parameters...\n");
        for (const auto &improperData : improperParameters_)
        {
            // Find the improper in the species
            auto &indices = std::get<0>(improperData);
            const auto optImproper =
                sp->getImproper(indices.at(0) - 1, indices.at(1) - 1, indices.at(2) - 1, indices.at(3) - 1);
            if (!optImproper)
                return Messenger::error("No improper {}-{}-{}-{} exists in the species.", indices.at(0), indices.at(1),
                                        indices.at(2), indices.at(3));
            const SpeciesImproper &improper = *optImproper;

            // Check parameter values
            if (checkParameters(improper.parameters(), std::get<1>(improperData), tolerance))
                Messenger::print("Improper {}-{}-{}-{} has the correct parameters.", indices.at(0), indices.at(1),
                                 indices.at(2), indices.at(3));
            else
            {
                Messenger::print("Parameters for improper {}-{}-{}-{} are incorrect - see above.", indices.at(0), indices.at(1),
                                 indices.at(2), indices.at(3));
                ++nImpropersFailed;
            }
        }
    }

    return (nAtomTypesFailed + nBondsFailed + nAnglesFailed + nTorsionsFailed + nImpropersFailed) == 0;
}
