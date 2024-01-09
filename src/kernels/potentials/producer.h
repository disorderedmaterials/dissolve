// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "kernels/potentials/types.h"
#include <memory>

// Forward Declarations
class ExternalPotential;

// External Potential Producer
namespace ExternalPotentialProducer
{
// Create an external potential of the specified type
std::unique_ptr<ExternalPotential> create(ExternalPotentialTypes::ExternalPotentialType type);
}; // namespace ExternalPotentialProducer
