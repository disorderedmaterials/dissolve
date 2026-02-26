// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/base.h"
#include "classes/configuration.h"
#include "classes/potentialMap.h"

KernelBase::KernelBase(const Configuration *cfg, const PotentialMap &potentialMap, std::optional<double> energyCutoff)
    : potentialMap_(potentialMap), box_(cfg->box()), cellArray_(cfg->cells()), molecules_(cfg->molecules())
{
    cutoffDistanceSquared_ = energyCutoff.has_value() ? energyCutoff.value() * energyCutoff.value()
                                                      : PairPotential::range() * PairPotential::range();
}

KernelBase::KernelBase(const Box *box, const PotentialMap &potentialMap, std::optional<double> energyCutoff)
    : potentialMap_(potentialMap), box_(box)
{
    cutoffDistanceSquared_ = energyCutoff.has_value() ? energyCutoff.value() * energyCutoff.value()
                                                      : PairPotential::range() * PairPotential::range();
}
