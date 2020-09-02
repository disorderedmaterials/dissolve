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
    // Check that a valid Species was specified
    Species *sp = keywords_.retrieve<Species *>("Species");
    if (!sp)
        return Messenger::error("No target species provided.\n");

    Messenger::print("CheckSpecies: Target species is '{}'.\n", sp->name());

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
                Messenger::print("Type for atom {} is {}, which does not match that expected ({}).\n",
                                 i, at ? (*at).name() : "undefined", typeName);
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

    return (nAtomTypesFailed == 0);
}
