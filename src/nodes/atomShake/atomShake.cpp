// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "atomShake.h"
#include "base/randomBuffer.h"
#include "base/timer.h"
#include "classes/box.h"
#include "classes/changeStore.h"
#include "classes/configuration.h"
#include "classes/regionalDistributor.h"
#include "kernels/producer.h"
#include "main/dissolve.h"

AtomShakeNode::AtomShakeNode()
{
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::Invalidates});

    addOption<Number>("ShakesPerAtom", "Number of shakes to attempt per atom", nShakesPerAtom_);
    addOption<Number>("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_);
    addOption<Number>("StepSizeMax", "Maximum allowed value for step size, in Angstroms", stepSizeMax_);
    addOption<Number>("StepSizeMin", "Minimum allowed value for step size, in Angstroms", stepSizeMin_);
}

std::string_view AtomShakeNode::type() const { return "AtomShake"; }

std::string_view AtomShakeNode::summary() const
{
    return "Perform an atomic Monte Carlo \"shake\" of every atom in each target configuration.";
}

