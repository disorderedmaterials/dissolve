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

// Register Forcefields for use
void ForcefieldLibrary::registerForcefields()
{
    forcefields_.push_back(std::make_shared<Forcefield_OPLSAA2005_Alcohols>());
    forcefields_.push_back(std::make_shared<Forcefield_OPLSAA2005_Alkanes>());
    forcefields_.push_back(std::make_shared<Forcefield_OPLSAA2005_Alkenes>());
    forcefields_.push_back(std::make_shared<Forcefield_OPLSAA2005_Aromatics>());
    forcefields_.push_back(std::make_shared<Forcefield_OPLSAA2005_Diols>());
    forcefields_.push_back(std::make_shared<Forcefield_OPLSAA2005_NobleGases>());
    forcefields_.push_back(std::make_shared<Forcefield_OPLSAA2005_Triols>());
    forcefields_.push_back(std::make_shared<Forcefield_SPCFw>());
    forcefields_.push_back(std::make_shared<Forcefield_UFF>());
    forcefields_.push_back(std::make_shared<Forcefield_NTf2_Ludwig>());
    forcefields_.push_back(std::make_shared<Forcefield_Py5_Ludwig>());
    forcefields_.push_back(std::make_shared<Forcefield_Py4OH_Ludwig>());
}

/*
 * Public Functions
 */

// Return list of available Forcefields
std::vector<std::shared_ptr<Forcefield>> &ForcefieldLibrary::forcefields()
{
    // If the list is empty, we haven't yet constructed the list...
    if (forcefields_.empty())
        registerForcefields();

    return forcefields_;
}

// Return named Forcefield, if it exists
std::shared_ptr<Forcefield> ForcefieldLibrary::forcefield(const string name)
{
    for (auto &ff : forcefields())
    {
        if (DissolveSys::sameString(ff->name(), name.c_str()))
            return ff;
    }
    auto it = std::find_if(forcefields().begin(), forcefields().end(), [&name](const std::shared_ptr<Forcefield> ff) {
        return DissolveSys::sameString(ff->name(), name.c_str());
    });
    if (it == forcefields().end())
        return nullptr;

    return *it;
}
