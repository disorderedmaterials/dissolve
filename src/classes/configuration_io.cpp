// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/pairPotential.h"
#include "classes/species.h"
#include <algorithm>

// Write through specified LineParser
bool Configuration::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("'{}'  {}  # nMolecules\n", name(), molecules_.size()))
        return false;

    // Write unit cell (box) lengths and angles
    const auto lengths = box().axisLengths();
    const auto angles = box().axisAngles();
    if (!parser.writeLineF("{:12e} {:12e} {:12e}  {}  {}  {}\n", lengths.x, lengths.y, lengths.z,
                           appliedSizeFactor_.value_or(defaultSizeFactor_), requestedSizeFactor_.value_or(defaultSizeFactor_),
                           DissolveSys::btoa(box().type() == Box::BoxType::None)))
        return false;
    if (!parser.writeLineF("{:12e} {:12e} {:12e}\n", angles.x, angles.y, angles.z))
        return false;

    // Write total number of Molecules
    if (!parser.writeLineF("{}\n", molecules_.size()))
        return false;

    // Write Molecule types - write sequential Molecules with same type as single line
    auto moleculeCount = 0;
    const Species *lastType = nullptr;
    for (const auto &molecule : molecules_)
    {
        // If the last Molecule's Species is the same as this one, increment counter and move on
        if (lastType == molecule->species())
        {
            ++moleculeCount;
            continue;
        }

        // Species is different between this molecule and the last - write this info, and reset the counter
        if (lastType && (!parser.writeLineF("{}  '{}'\n", moleculeCount, lastType->name())))
            return false;
        moleculeCount = 1;
        lastType = molecule->species();
    }
    // Write final molecule count / type
    if ((moleculeCount > 0) && (!parser.writeLineF("{}  '{}'\n", moleculeCount, lastType->name())))
        return false;

    // Write all Atoms - for each write index and coordinates
    if (!parser.writeLineF("{}  # nAtoms\n", atoms_.size()))
        return false;
    for (const auto &i : atoms_)
    {
        if (!parser.writeLineF("{} {} {} {}\n", i.molecule()->arrayIndex(), i.r().x, i.r().y, i.r().z))
            return false;
    }

    return true;
}

// Read from specified LineParser
bool Configuration::deserialise(LineParser &parser, const CoreData &coreData, bool hasPotentials)
{
    // Clear current contents of Configuration
    empty();

    // Read configuration name and nMolecules
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    setName(parser.argsv(0));

    /*
     * Read box definition
     * Lengths, along with atomic coordinates, reflect the applied size factor.
     * Create box with unscaled lengths - they will be scaled according to the size factor at the end of the routine.
     */
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    if (parser.argd(3) > 1.0)
        appliedSizeFactor_ = parser.argd(3);
    else
        appliedSizeFactor_ = std::nullopt;
    requestedSizeFactor_ = parser.argd(4);
    auto nonPeriodic = parser.argb(5);
    const auto lengths = parser.arg3d(0) / appliedSizeFactor_.value_or(defaultSizeFactor_);

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    const auto angles = parser.arg3d(0);

    createBoxAndCells(lengths, angles, nonPeriodic);

    // Read total number of Molecules to expect
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    const auto expectedNMols = parser.argi(0);

    // Read Species types for Molecules
    auto nMolsRead = 0;
    while (nMolsRead < expectedNMols)
    {
        // Read line containing number of molecules and Species name
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;

        auto sp = coreData.findSpecies(DissolveSys::niceName(parser.argsv(1)));
        if (!sp)
            return Messenger::error("Unrecognised Species '{}' found in Configuration '{}' in restart file.\n", parser.argsv(1),
                                    name());

        // Set Species pointers for this range of Molecules
        auto nMols = parser.argi(0);
        for (auto n = 0; n < nMols; ++n)
            addMolecule(sp);

        // Increase our counter
        nMolsRead += parser.argi(0);
    }

    // Read in Atoms
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nAtoms = parser.argi(0);
    // Reserve space for the atoms, plus some buffer space
    atoms_.reserve(nAtoms + std::min(1000, (nAtoms) >> 2));
    for (auto n = 0; n < nAtoms; ++n)
    {
        // Each line contains molecule ID and coordinates only
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;

        atom(n).setR(parser.arg3d(1));
    }

    // Scale box and cells according to the applied size factor
    auto appliedSF = appliedSizeFactor_.value_or(defaultSizeFactor_);
    scaleBox({appliedSF, appliedSF, appliedSF});

    // Update all relationships
    updateObjectRelationships();

    // If this an old-style configuration with no potentials we can end here
    if (!hasPotentials)
        return true;

    return true;
}
