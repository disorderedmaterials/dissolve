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
            i.setR(newR);
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
        i -= centre;
}

/*
 * Creation
 */

// Create atomic species
void Species::createAtomic(Elements::Element Z, InteractionPotential<ShortRangeFunctions> potential)
{
    clear();

    // Set up atom type
    auto atomType = addAtomType(Z, Elements::symbol(Z));
    atomType->interactionPotential().setFormAndParameters(potential.form(), potential.parameters());

    auto &i = atoms_.emplace_back(this);
    i.set(Z, {});
    i.setIndex(0);
    i.setAtomType(atomType);

    // Create isotopologues
    for (auto isotope : Sears91::isotopes(Z))
    {
        auto iso = addIsotopologue(std::format("{}{}", Elements::symbol(Z), Sears91::A(isotope)));
        iso->setAtomTypeIsotope(atomType, isotope);
    }
}

// Load from specified TOML file
void Species::load(std::string_view tomlFile)
{
    clear();

    SerialisedValue contents = toml::parse(std::string(tomlFile));
    if (contents.contains("species"))
    {
        deserialise(contents["species"]);
        auto name = contents["species"]["name"].as_string();
        setName(name.str);
    }
}
