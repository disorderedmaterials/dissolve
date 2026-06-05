// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/fragment.h"
#include "classes/species.h"
#include "classes/structure.h"

// Finalise all relationships between intramolecular data
void Species::finaliseIntramolecularData(bool recalculateAnglesAndTorsions)
{
    // Set bond pointers in individual atoms
    for (const auto &bond : bonds_)
    {
        bond.i()->addBond(&bond);
        bond.j()->addBond(&bond);
    }

    // Determine angles and torsions from bond connectivity
    if (recalculateAnglesAndTorsions)
        for (auto &jk : bonds_)
        {
            // Get atoms 'j' and 'k'
            auto *j = jk.i();
            auto *k = jk.j();

            // Swap j and k over if j is terminal and has only a single bond (i.e. jk)
            if (j->bonds().size() == 1)
                std::swap(j, k);

            // Loop over bonds 'ij'
            for (auto *ij : j->bonds())
            {
                // Avoid 'ij' == 'jk'
                if (ij == &jk)
                    continue;

                // Get atom 'i'
                auto *i = ij->partner(j);

                // Add angle i-j-k
                if (!getAngle(i, j, k))
                    angles_.emplace_back(this, i, j, k);

                // Loop over bonds 'kl'
                for (auto kl : k->bonds())
                {
                    // Avoid 'kl' == 'jk'
                    if (kl == &jk)
                        continue;

                    // Get atom 'l'
                    auto *l = kl->partner(k);

                    // Add angle j-k-l
                    if (!getAngle(j, k, l))
                        angles_.emplace_back(this, j, k, l);

                    // Add torsion i-j-k-l
                    if (!getTorsion(i, j, k, l))
                        torsions_.emplace_back(this, i, j, k, l);
                }
            }
        }

    // Set angle, torsion, and improper data in atoms
    for (const auto &angle : angles_)
    {
        angle.i()->addAngle(&angle);
        angle.j()->addAngle(&angle);
        angle.k()->addAngle(&angle);
    }
    for (const auto &torsion : torsions_)
    {
        torsion.i()->addTorsion(&torsion);
        torsion.j()->addTorsion(&torsion);
        torsion.k()->addTorsion(&torsion);
        torsion.l()->addTorsion(&torsion);
    }
    for (const auto &improper : impropers_)
    {
        improper.i()->addImproper(&improper);
        improper.j()->addImproper(&improper);
        improper.k()->addImproper(&improper);
        improper.l()->addImproper(&improper);
    }

    // Set-up excluded / scaled interactions on atoms arising from bonds, angles, and torsions
    auto index = 0;
    for (auto &i : atoms_)
    {
        i.setScaledInteractions();
        i.setIndex(index++);
    }

    // If this is a periodic species, we're done
    if (box_->type() != Box::BoxType::NonPeriodic)
    {
        attachedAtomListsGenerated_ = false;
        return;
    }

    // Generate attached atom lists for all intramolecular terms
    // Bonds
    for (auto &bond : bonds_)
    {
        // Select all Atoms attached to Atom 'i', excluding the Bond as a path
        auto selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, bond.i()->index(), {&bond});

        // If the list now contains Atom j, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i' itself In that case we can also finish the list for Atom 'j', and continue the loop.
        if (std::find(selection.begin(), selection.end(), bond.j()->index()) != selection.end())
        {
            Messenger::printVerbose("Bond between Atoms {}-{} is present in a cycle, so a minimal set of attached "
                                    "atoms will be used.\n",
                                    bond.i()->index(), bond.j()->index());
            bond.setAttachedAtoms(0, bond.i()->index());
            bond.setAttachedAtoms(1, bond.j()->index());
            bond.setInCycle(true);
            continue;
        }
        else
            bond.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'i', excluding the Bond as a path
        selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, bond.j()->index(), {&bond});
        bond.setAttachedAtoms(1, selection);
    }

    // Angles - termini are 'i' and 'k'
    for (auto &angle : angles_)
    {
        // Grab relevant bonds
        auto ji = angle.j()->getBondWith(angle.i());
        auto jk = angle.j()->getBondWith(angle.k());

        // Select all Atoms attached to Atom 'i', excluding the Bond ji as a path
        auto selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, angle.i()->index(), {ji, jk});

        // Remove Atom 'j' from the list if it's there
        auto jit = std::find(selection.begin(), selection.end(), angle.j()->index());
        if (jit != selection.end())
            selection.erase(jit);

        // If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i' itself In that case we can also finish the list for Atom 'k', and continue the loop.
        if (std::find(selection.begin(), selection.end(), angle.k()->index()) != selection.end())
        {
            Messenger::printVerbose("Angle between Atoms {}-{}-{} is present in a cycle, so a minimal set of "
                                    "attached atoms will be used.\n",
                                    angle.i()->index(), angle.j()->index(), angle.k()->index());
            angle.setAttachedAtoms(0, angle.i()->index());
            angle.setAttachedAtoms(1, angle.k()->index());
            angle.setInCycle(true);
            continue;
        }
        else
            angle.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
        selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, angle.k()->index(), {ji, jk});

        // Remove Atom 'j' from the list if it's there
        jit = std::find(selection.begin(), selection.end(), angle.j()->index());
        if (jit != selection.end())
            selection.erase(jit);

        angle.setAttachedAtoms(1, selection);
    }

    // Torsions - termini are 'j' and 'k'
    for (auto &torsion : torsions_)
    {
        // Grab relevant Bond (if it exists)
        auto jk = torsion.j()->getBondWith(torsion.k());

        // Select all Atoms attached to Atom 'j', excluding the Bond ji as a path
        auto selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, torsion.j()->index(), {jk});

        // Remove Atom 'j' from the list
        selection.erase(std::remove(selection.begin(), selection.end(), torsion.j()->index()));

        // If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the
        // Atom 'i'
        if (std::find(selection.begin(), selection.end(), torsion.k()->index()) != selection.end())
        {
            Messenger::printVerbose("Torsion between Atoms {}-{}-{}-{} is present in a cycle, so a minimal set of "
                                    "attached atoms will be used.\n",
                                    torsion.i()->index(), torsion.j()->index(), torsion.k()->index(), torsion.l()->index());
            torsion.setAttachedAtoms(0, torsion.i()->index());
            torsion.setAttachedAtoms(1, torsion.l()->index());
            torsion.setInCycle(true);
            continue;
        }
        else
            torsion.setAttachedAtoms(0, selection);

        // Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
        selection = Fragment<SpeciesAtom, SpeciesBond>::get(atoms_, torsion.k()->index(), {jk});

        // Remove Atom 'k' from the list
        selection.erase(std::remove(selection.begin(), selection.end(), torsion.k()->index()));

        torsion.setAttachedAtoms(1, selection);
    }

    attachedAtomListsGenerated_ = true;
}

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

// Apply random noise to atoms
void Species::randomiseCoordinates(double maxDisplacement)
{
    for (auto &i : atoms_)
        i += Vector3::randomUnit() * maxDisplacement;
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

    // Add a central site
    auto *site = addSite("Origin");
    site->addStaticOriginAtom(0);
}

// Load from specified TOML file
void Species::load(std::string_view tomlFile)
{
    clear();

    SerialisedValue contents = toml::parse(std::string(tomlFile));
    if (contents.contains("species"))
        deserialise(contents["species"]);
}

// Create from structure and forcefield
void Species::create(const Structure &structure)
{
    clear();

    // Copy atoms
    atoms_.reserve(structure.atoms().size());
    for (auto &atom : structure.atoms())
    {
        auto &i = atoms_.emplace_back(this);
        i.set(atom->Z(), atom->r(), atom->q());
        i.setIndex(atoms_.size() - 1);
    }

    // Copy bonds
    bonds_.reserve(structure.bonds().size());
    for (auto &bond : structure.bonds())
        bonds_.emplace_back(this, &atoms_[bond->i()->index()], &atoms_[bond->j()->index()]);

    // Perform rest of setup
    finaliseIntramolecularData();
}

// Return whether the attached atoms lists have been created
bool Species::attachedAtomListsGenerated() const { return attachedAtomListsGenerated_; }
