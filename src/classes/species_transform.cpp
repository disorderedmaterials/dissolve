// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/species.h"

// Calculate and return centre of geometry
Vector3 Species::centreOfGeometry(const Box *box) const
{
    if (atoms_.size() == 0)
        return Vector3();

    // Calculate center relative to first atom in molecule
    auto cog = atoms_.front().r();
    for (const auto &i : atoms_)
        cog += box->minimumImage(i.r(), cog);

    return (cog / atoms_.size());
}

// Set centre of geometry of species
void Species::setCentre(const Box *box, const Vector3 newCentre)
{
    // Calculate Molecule centre of geometry
    Vector3 newR;
    const auto cog = centreOfGeometry(box);

    // Apply transform
    for (int n = 0; n < atoms_.size(); ++n)
        for (auto &i : atoms_)
        {
            newR = box->minimumVector(i.r(), cog) + newCentre;
            i.setCoordinates(newR);
        }
}

// Centre coordinates at origin
void Species::centreAtOrigin()
{
    Vector3 centre;
    for (const auto &i : atoms_)
        centre += i.r();
    centre /= atoms_.size();
    for (auto &i : atoms_)
        i.translateCoordinates(-centre);
}
