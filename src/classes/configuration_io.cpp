// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "kernels/potentials/producer.h"
#include <algorithm>

// Write through specified LineParser
bool Configuration::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("'{}'  {}  # nMolecules\n", name(), molecules_.size()))
        return false;

    // Write unit cell (box) lengths and angles
    const auto lengths = box()->axisLengths();
    const auto angles = box()->axisAngles();
    if (!parser.writeLineF("{:12e} {:12e} {:12e}  {}  {}  {}\n", lengths.x, lengths.y, lengths.z,
                           appliedSizeFactor_.value_or(defaultSizeFactor_), requestedSizeFactor_.value_or(defaultSizeFactor_),
                           DissolveSys::btoa(box()->type() == Box::BoxType::NonPeriodic)))
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
        if (!parser.writeLineF("{} {} {} {}\n", i.molecule()->arrayIndex(), i.x(), i.y(), i.z()))
            return false;
    }

    // If there are no defined external potentials we are done
    if (globalPotentials_.empty() && targetedPotentials_.empty())
        return true;

    // Write global potentials
    if (!parser.writeLineF("{}  # nGlobalPotentials\n", globalPotentials_.size()))
        return false;
    for (auto &pot : globalPotentials_)
        if (!pot->serialise(parser, ""))
            return false;

    // Write targeted potentials
    if (!parser.writeLineF("{}  # nTargetedPotentials\n", targetedPotentials_.size()))
        return false;
    for (auto &pot : targetedPotentials_)
        if (!pot->serialise(parser, ""))
            return false;

    return true;
}

// Read from specified LineParser
bool Configuration::deserialise(LineParser &parser, const CoreData &coreData, double pairPotentialRange, bool hasPotentials)
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

    createBoxAndCells(lengths, angles, nonPeriodic, pairPotentialRange);

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

        auto sp = coreData.findSpecies(parser.argsv(1));
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

        atom(n).setCoordinates(parser.arg3d(1));
    }

    // Finalise used AtomType list
    atomTypes_.finalise();

    // Scale box and cells according to the applied size factor
    auto appliedSF = appliedSizeFactor_.value_or(defaultSizeFactor_);
    scaleBox({appliedSF, appliedSF, appliedSF});

    // Update all relationships
    updateObjectRelationships();

    // If this an old-style configuration with no potentials we can end here
    if (!hasPotentials)
        return true;

    // Read in global potentials
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    globalPotentials_.resize(parser.argi(0));
    for (auto &pot : globalPotentials_)
    {
        // First line contains potential type
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        auto potentialType = ExternalPotentialTypes::isType(parser.argsv(0));
        if (!potentialType)
            return Messenger::error("Unrecognised external potential type '{}' found in Configuration '{}' in restart file.\n",
                                    parser.argsv(0), name());

        // Create new external potential
        pot = ExternalPotentialProducer::create(*potentialType);
        if (!pot->deserialise(parser, coreData))
            return false;
    }

    // Read in targeted potentials
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    targetedPotentials_.resize(parser.argi(0));
    for (auto &pot : targetedPotentials_)
    {
        // First line contains potential type
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        auto potentialType = ExternalPotentialTypes::isType(parser.argsv(0));
        if (!potentialType)
            return Messenger::error("Unrecognised external potential type '{}' found in Configuration '{}' in restart file.\n",
                                    parser.argsv(0), name());

        // Create new external potential
        pot = ExternalPotentialProducer::create(*potentialType);

        // Additional arguments after the potential type correspond to targets for the potential
        std::vector<int> atomIndices;
        std::vector<std::shared_ptr<AtomType>> atomTypes;

        for (auto n = 1; n < parser.nArgs(); ++n)
        {
            // Plain number - corresponds to a specific atom in the configuration
            if (DissolveSys::isNumber(parser.args(n)))
            {
                auto i = parser.argi(n);
                if (i < 0 || i >= atoms_.size())
                    throw(std::runtime_error(fmt::format("Atom index {} for targeted potential is out of range.\n", i)));
                pot->addTargetAtomIndex(i);
            }
            else if (coreData.findAtomType(parser.args(n)))
                pot->addTargetAtomType(coreData.findAtomType(parser.args(n)));
            else if (coreData.findSpecies(parser.args(n)))
                pot->addTargetSpecies(coreData.findSpecies(parser.args(n)));
            else
                throw(std::runtime_error(fmt::format("Unrecognised target '{}' for potential.\n", parser.args(n))));
        }

        // Read in the rest of the potential
        if (!pot->deserialise(parser, coreData))
            return false;
    }

    // Link targeted potentials to atoms
    linkTargetedPotentials();

    return true;
}
