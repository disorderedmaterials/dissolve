// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/library.h"
#include "base/sysfunc.h"
#include "data/ff/kulmala2010/kulmala2010.h"
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
#include "data/ff/pcl2019/anions.h"
#include "data/ff/pcl2019/cations.h"
#include "data/ff/spcfw/spcfw.h"
#include "data/ff/strader2002/dmso.h"
#include "data/ff/uff/uff.h"
#include "data/ff/xml/base.h"

// Static Members
std::vector<std::shared_ptr<Forcefield>> ForcefieldLibrary::forcefields_;

/*
 * Private Functions
 */

// Set up supplied forcefield for use, and add to internal list
bool ForcefieldLibrary::registerForcefield(std::shared_ptr<Forcefield> ff)
{
    if (forcefields_.empty())
        registerForcefields();
    return registerForcefield_(ff);
}

// Set up supplied forcefield for use, and add to internal list
bool ForcefieldLibrary::registerForcefield_(std::shared_ptr<Forcefield> ff)
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
    registerForcefield_(std::make_shared<Forcefield_Kulmala2010>());
    registerForcefield_(std::make_shared<Forcefield_Ludwig_NTf2>());
    registerForcefield_(std::make_shared<Forcefield_Ludwig_Py5>());
    registerForcefield_(std::make_shared<Forcefield_Ludwig_Py4OH>());
    registerForcefield_(std::make_shared<Forcefield_OPLSAA2005_Alcohols>());
    registerForcefield_(std::make_shared<Forcefield_OPLSAA2005_Alkanes>());
    registerForcefield_(std::make_shared<Forcefield_OPLSAA2005_Alkenes>());
    registerForcefield_(std::make_shared<Forcefield_OPLSAA2005_Aromatics>());
    registerForcefield_(std::make_shared<Forcefield_OPLSAA2005_Diols>());
    registerForcefield_(std::make_shared<Forcefield_OPLSAA2005_NobleGases>());
    registerForcefield_(std::make_shared<Forcefield_OPLSAA2005_Triols>());
    registerForcefield_(std::make_shared<Forcefield_PCL2019_Anions>());
    registerForcefield_(std::make_shared<Forcefield_PCL2019_Cations>());
    registerForcefield_(std::make_shared<Forcefield_SPCFw>());
    registerForcefield_(std::make_shared<Forcefield_Strader2002>());
    registerForcefield_(std::make_shared<Forcefield_UFF>());
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
