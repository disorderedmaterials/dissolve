// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/potentials/producer.h"
#include "kernels/potentials/simple.h"

// External Potential Producer
namespace ExternalPotentialProducer
{
// Create an external potential of the specified type
std::unique_ptr<ExternalPotential> create(ExternalPotentialTypes::ExternalPotentialType type)
{
    switch (type)
    {
        case (ExternalPotentialTypes::ExternalPotentialType::Spherical):
            return std::make_unique<SimplePotential>();
        default:
            throw(std::runtime_error(fmt::format("Creation of external potential type '%s' not implemented.\n",
                                                 ExternalPotentialTypes::keyword(type))));
    }
}
}; // namespace ExternalPotentialProducer
