// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/producer.h"
#include "kernels/potentials/cylindrical.h"
#include "kernels/potentials/regional.h"
#include "kernels/potentials/spherical.h"

// External Potential Producer
namespace ExternalPotentialProducer
{
// Create an external potential of the specified type
std::unique_ptr<ExternalPotential> create(ExternalPotentialTypes::ExternalPotentialType type)
{
    switch (type)
    {
        case (ExternalPotentialTypes::ExternalPotentialType::Spherical):
            return std::make_unique<SphericalPotential>();
        case (ExternalPotentialTypes::ExternalPotentialType::Cylindrical):
            return std::make_unique<CylindricalPotential>();
        case (ExternalPotentialTypes::ExternalPotentialType::Regional):
            return std::make_unique<RegionalPotential>();
        default:
            throw(std::runtime_error(fmt::format("Creation of external potential type '{}' not implemented.\n",
                                                 ExternalPotentialTypes::keyword(type))));
    }
}
}; // namespace ExternalPotentialProducer
