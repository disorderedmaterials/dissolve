/*
    *** Forcefield Library
    *** src/data/fflibrary.cpp
    Copyright T. Youngs 2019-2020

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

#include "data/fflibrary.h"
#include "base/sysfunc.h"
#include "data/ff/kulmala2010.h"
#include "data/ff/ludwig/ntf2.h"
#include "data/ff/ludwig/py4oh.h"
#include "data/ff/ludwig/py5.h"
#include "data/ff/oplsaa2005/alcohols.h"
#include "data/ff/oplsaa2005/alkanes.h"
#include "data/ff/oplsaa2005/alkenes.h"
#include "data/ff/oplsaa2005/aromatics.h"
#include "data/ff/oplsaa2005/diols.h"
#include "data/ff/oplsaa2005/noblegases.h"
#include "data/ff/oplsaa2005/triols.h"
#include "data/ff/spcfw.h"
#include "data/ff/uff.h"

// Static Members
std::vector<std::shared_ptr<Forcefield>> ForcefieldLibrary::forcefields_;

/*
 * Private Functions
 */

// Set up supplied forcefield for use, and add to internal list
bool ForcefieldLibrary::registerForcefield(std::shared_ptr<Forcefield> ff)
{
    // Set up the forcefield, returning if not successful
    if (!ff->prepare())
        return Messenger::error("Failed to prepare and set up forcefield '{}' - it will not be registered.\n", ff->name());

    // Generate NETA definitions for all atom types in the forcefield
    if (!ff->createNETADefinitions())
        return Messenger::error("Failed to generate NETA definitions for forcefield '{}' - it will not be registered.\n",
                                ff->name());

    forcefields_.push_back(ff);

    return true;
}

// Register Forcefields for use
void ForcefieldLibrary::registerForcefields()
{
    registerForcefield(std::make_shared<Forcefield_OPLSAA2005_Alcohols>());
    registerForcefield(std::make_shared<Forcefield_OPLSAA2005_Alkanes>());
    registerForcefield(std::make_shared<Forcefield_OPLSAA2005_Alkenes>());
    registerForcefield(std::make_shared<Forcefield_OPLSAA2005_Aromatics>());
    registerForcefield(std::make_shared<Forcefield_OPLSAA2005_Diols>());
    registerForcefield(std::make_shared<Forcefield_OPLSAA2005_NobleGases>());
    registerForcefield(std::make_shared<Forcefield_OPLSAA2005_Triols>());
    registerForcefield(std::make_shared<Forcefield_Kulmala2010>());
    registerForcefield(std::make_shared<Forcefield_SPCFw>());
    registerForcefield(std::make_shared<Forcefield_UFF>());
    registerForcefield(std::make_shared<Forcefield_Ludwig_NTf2>());
    registerForcefield(std::make_shared<Forcefield_Ludwig_Py5>());
    registerForcefield(std::make_shared<Forcefield_Ludwig_Py4OH>());
}

/*
 * Public Functions
 */

// Return list of available Forcefields
std::vector<std::shared_ptr<Forcefield>> &ForcefieldLibrary::forcefields()
{
    // If the list is empty, construct the forcefield objects
    if (forcefields_.empty())
        registerForcefields();

    return forcefields_;
}

// Return named Forcefield, if it exists
std::shared_ptr<Forcefield> ForcefieldLibrary::forcefield(std::string_view name)
{
    auto it = std::find_if(forcefields().begin(), forcefields().end(),
                           [&name](const std::shared_ptr<Forcefield> ff) { return DissolveSys::sameString(ff->name(), name); });
    if (it == forcefields().end())
        return nullptr;

    return *it;
}
