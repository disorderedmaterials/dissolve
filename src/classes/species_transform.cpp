// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/species.h"

// Calculate and return centre of geometry
Vec3<double> Species::centreOfGeometry(const Box *box) const
{
    if (atoms_.size() == 0)
        return Vec3<double>();

    // Calculate center relative to first atom in molecule
    auto cog = atoms_.front().r();
    for (const auto &i : atoms_)
        cog += box->minimumImage(i.r(), cog);

    return (cog / atoms_.size());
}

// Set centre of geometry of species
void Species::setCentre(const Box *box, const Vec3<double> newCentre)
{
    // Calculate Molecule centre of geometry
    Vec3<double> newR;
    const auto cog = centreOfGeometry(box);

    // Apply transform
    for (int n = 0; n < atoms_.size(); ++n)
        for (auto &i : atoms_)
        {
            newR = box->minimumVector(i.r(), cog) + newCentre;
            i.setCoordinates(newR);
        }

    ++version_;
}

// Centre coordinates at origin
void Species::centreAtOrigin()
{
    Vec3<double> centre;
    for (const auto &i : atoms_)
        centre += i.r();
    centre /= atoms_.size();
    for (auto &i : atoms_)
        i.translateCoordinates(-centre);

    ++version_;
}
