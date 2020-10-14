// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/species.h"

// Calculate and return centre of geometry
Vec3<double> Species::centreOfGeometry(const Box *box) const
{
    if (atoms_.nItems() == 0)
        return Vec3<double>();

    // Calculate center relative to first atom in molecule
    auto cog = atoms_.first()->r();
    for (auto *i = atoms_.first()->next(); i != nullptr; i = i->next())
        cog += box->minimumImage(i->r(), cog);

    return (cog / atoms_.nItems());
}

// Set centre of geometry of species
void Species::setCentre(const Box *box, const Vec3<double> newCentre)
{
    // Calculate Molecule centre of geometry
    Vec3<double> newR;
    const auto cog = centreOfGeometry(box);

    // Apply transform
    for (int n = 0; n < atoms_.nItems(); ++n)
        for (auto *i = atoms_.first(); i != nullptr; i = i->next())
        {
            newR = box->minimumVector(i->r(), cog) + newCentre;
            i->setCoordinates(newR);
        }

    ++version_;
}

// Centre coordinates at origin
void Species::centreAtOrigin()
{
    Vec3<double> centre;
    for (auto *i = atoms_.first(); i != nullptr; i = i->next())
        centre += i->r();
    centre /= atoms_.nItems();
    for (auto *i = atoms_.first(); i != nullptr; i = i->next())
        i->translateCoordinates(-centre);

    ++version_;
}
