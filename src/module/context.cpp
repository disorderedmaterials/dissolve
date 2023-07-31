// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "module/context.h"

ModuleContext::ModuleContext(const ProcessPool &procPool, const CoreData &coreData, const double &pairPotentialRange, const double &pairPotentialDelta, const PotentialMap &potentialMap) :
processPool_(procPool), coreData_(coreData), pairPotentialRange_(pairPotentialRange), pairPotentialDelta_(pairPotentialDelta), potentialMap_(potentialMap)
{}

const ProcessPool &ModuleContext::processPool() const { return processPool_; }

const CoreData &ModuleContext::coreData() const
{
    return coreData_;
}

const double &ModuleContext::pairPotentialRange() const
{
    return pairPotentialRange_;
}

const double &ModuleContext::pairPotentialDelta() const
{
    return pairPotentialDelta_;
}

const PotentialMap &ModuleContext::potentialMap() const
{
    return potentialMap_;
}