// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/masters.h"

SerialisedValue Masters::serialise() const
{
    SerialisedValue node;
    Serialisable::fromVectorToTable<>(bonds, "bonds", node);
    Serialisable::fromVectorToTable<>(angles, "angles", node);
    Serialisable::fromVectorToTable<>(torsions, "torsions", node);
    Serialisable::fromVectorToTable<>(impropers, "impropers", node);
    return node;
}

void Masters::deserialise(SerialisedValue &node)
{
    Serialisable::toMap(node, "bonds", [this](const std::string &name, SerialisedValue &bond) {
        bonds.emplace_back(std::make_unique<MasterBond>(name))->deserialise(bond);
    });
    Serialisable::toMap(node, "angles", [this](const std::string &name, SerialisedValue &angle) {
        angles.emplace_back(std::make_unique<MasterAngle>(name))->deserialise(angle);
    });
    Serialisable::toMap(node, "torsions", [this](const std::string &name, SerialisedValue &torsion) {
        torsions.emplace_back(std::make_unique<MasterTorsion>(name))->deserialise(torsion);
    });
    Serialisable::toMap(node, "impropers", [this](const std::string &name, SerialisedValue &improper) {
        impropers.emplace_back(std::make_unique<MasterImproper>(name))->deserialise(improper);
    });
    return;
}
