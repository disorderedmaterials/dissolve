/*
    *** Configuration I/O
    *** src/classes/configuration_io.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include <algorithm>

// Write through specified LineParser
bool Configuration::write(LineParser &parser) const
{
    int molId;

    if (!parser.writeLineF("'{}'  {}  # nMolecules\n", name(), molecules_.size()))
        return false;

    // Write unit cell (box) lengths and angles
    const auto lengths = box()->axisLengths();
    const auto angles = box()->axisAngles();
    if (!parser.writeLineF("{:12e} {:12e} {:12e}  {}  {}\n", lengths.x, lengths.y, lengths.z, requestedSizeFactor_,
                           DissolveSys::btoa(box()->type() == Box::NonPeriodicBoxType)))
        return false;
    if (!parser.writeLineF("{:12e} {:12e} {:12e}\n", angles.x, angles.y, angles.z))
        return false;

    // Write total number of Molecules
    if (!parser.writeLineF("{}\n", molecules_.size()))
        return false;

    // Write Molecule types - write sequential Molecules with same type as single line
    auto moleculeCount = 0;
    const Species *lastType = nullptr;
    for (int n = 0; n < molecules_.size(); ++n)
    {
        // If the last Molecule's Species is the same as this one, increment counter and move on
        if (lastType == molecules_[n]->species())
        {
            ++moleculeCount;
            continue;
        }

        // Species is different between this molecule and the last - write this info, and reset the counter
        if (lastType && (!parser.writeLineF("{}  '{}'\n", moleculeCount, lastType->name())))
            return false;
        moleculeCount = 1;
        lastType = molecules_[n]->species();
    }
    // Write final molecule count / type
    if ((moleculeCount > 0) && (!parser.writeLineF("{}  '{}'\n", moleculeCount, lastType->name())))
        return false;

    // Write all Atoms - for each write index and coordinates
    if (!parser.writeLineF("{}  # nAtoms\n", atoms_.nItems()))
        return false;
    for (int n = 0; n < atoms_.nItems(); ++n)
    {
        const Atom *i = atoms_.constValue(n);
        if (!parser.writeLineF("{} {:e} {:e} {:e}\n", i->molecule()->arrayIndex(), i->x(), i->y(), i->z()))
            return false;
    }

    return true;
}

// Read through specified LineParser
bool Configuration::read(LineParser &parser, const List<Species> &availableSpecies, double pairPotentialRange)
{
    // Clear current contents of Configuration
    empty();

    // Read configuration name and nMolecules
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    setName(parser.argsv(0));

    /*
     * Read box definition
     * Lengths, along with atomic coordinates, reflect the specified size factor (if present).
     * Create box with unscaled lengths - they will be scaled according to the size factor at the end of the routine.
     */
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto scaledLengths = parser.arg3d(0);
    requestedSizeFactor_ = (parser.hasArg(3) ? parser.argd(3) : 1.0);
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;

    appliedSizeFactor_ = requestedSizeFactor_;
    const auto lengths = scaledLengths / appliedSizeFactor_;
    const auto angles = parser.arg3d(0);
    if (!createBox(lengths, angles))
        return false;

    // Read total number of Molecules to expect
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    const auto expectedNMols = parser.argi(0);

    // Read Species types for Molecules
    auto nMolsRead = 0;
    Species *sp = nullptr;
    while (nMolsRead < expectedNMols)
    {
        // Read line containing number of molecules and Species name
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        for (sp = availableSpecies.first(); sp != nullptr; sp = sp->next())
            if (DissolveSys::sameString(sp->name(), parser.argsv(1)))
                break;
        if (!sp)
            return Messenger::error("Unrecognised Species '{}' found in Configuration '{}' in restart file.\n", parser.argsv(1),
                                    name());

        // Set Species pointers for this range of Molecules
        auto nMols = parser.argi(0);
        for (int n = 0; n < nMols; ++n)
            addMolecule(sp);

        // Increase our counter
        nMolsRead += parser.argi(0);
    }

    // Read in Atoms
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nAtoms = parser.argi(0);
    for (int n = 0; n < nAtoms; ++n)
    {
        // Each line contains molecule ID and coordinates only
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;

        atom(n)->setCoordinates(parser.arg3d(1));
    }

    // Finalise used AtomType list
    usedAtomTypes_.finalise();

    // Set-up Cells for the Box
    cells_.generate(box_, requestedCellDivisionLength_, pairPotentialRange);

    // Scale box and cells according to the applied size factor
    scaleBox(appliedSizeFactor_);

    // Update Cell locations for Atoms
    updateCellContents();

    return true;
}
