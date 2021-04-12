// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/masterintra.h"
#include "classes/species.h"
#include "modules/energy/energy.h"

Configuration::Configuration() : ListItem<Configuration>(), generator_(ProcedureNode::GenerationContext, "EndGenerator")
{
    box_ = nullptr;

    clear();
}

Configuration::~Configuration() { clear(); }

void Configuration::operator=(Configuration &source)
{
    Messenger::error("XXX CONFIGURATION COPY (via ASSIGNMENT OPERATOR) IS NOT YET IMPLEMENTED.\n");
}

// Clear all data
void Configuration::clear()
{
    // Clear generated content
    empty();

    // Reset composition
    requestedSizeFactor_ = 1.0;
    appliedSizeFactor_ = 1.0;

    // Reset box / Cells
    requestedCellDivisionLength_ = 7.0;
    contentsVersion_.zero();

    // Reset definition
    temperature_ = 300.0;
    generator_.clear();
}

/*
 * Definition
 */

// Set name of the Configuration
void Configuration::setName(std::string_view name)
{
    name_ = name;

    // Generate a nice name (i.e. no spaces, slashes etc.)
    niceName_ = DissolveSys::niceName(name_);
}

// Return name of the Configuration
std::string_view Configuration::name() const { return name_; }

// Return nice name of the Configuration
std::string_view Configuration::niceName() const { return niceName_; }

// Return the current generator
Procedure &Configuration::generator() { return generator_; }

// Create the Configuration according to its generator Procedure
bool Configuration::generate(ProcessPool &procPool, double pairPotentialRange)
{
    // Empty the current contents
    empty();

    // Generate the contents
    Messenger::print("\nExecuting generator procedure for Configuration '{}'...\n\n", niceName());
    GenericList dummyList;
    auto result = generator_.execute(procPool, this, "Generator", dummyList);
    if (!result)
        return Messenger::error("Failed to generate Configuration '{}'.\n", niceName());
    Messenger::print("\n");

    // Set-up Cells for the Box
    if (cells_.nCells() == 0)
        cells_.generate(box_, requestedCellDivisionLength_, pairPotentialRange);

    // Make sure Cell contents / Atom locations are up-to-date
    updateCellContents();

    // Finalise used AtomType list
    usedAtomTypes_.finalise();

    ++contentsVersion_;

    // Sanity check the contents - if we have zero atoms then there's a problem!
    if (nAtoms() == 0)
        return Messenger::error("Generated contents for Configuration '{}' contains no atoms!\n", name());

    return true;
}

// Return import coordinates file / format
CoordinateImportFileFormat &Configuration::inputCoordinates() { return inputCoordinates_; }

// Load coordinates from file
bool Configuration::loadCoordinates(LineParser &parser, CoordinateImportFileFormat::CoordinateImportFormat format)
{
    // Load coordinates into temporary array
    Array<Vec3<double>> r;
    CoordinateImportFileFormat coordinateFormat(format);
    if (!coordinateFormat.importData(parser, r))
        return false;

    // Temporary array now contains some number of atoms - does it match the number in the configuration's molecules?
    if (atoms_.size() != r.nItems())
        return Messenger::error(
            "Number of atoms read from initial coordinates file ({}) does not match that in Configuration ({}).\n", r.nItems(),
            atoms_.size());

    // All good, so copy atom coordinates over into our array
    for (auto n = 0; n < atoms_.size(); ++n)
        atoms_[n]->setCoordinates(r[n]);

    return true;
}

// Initialise (generate or load) the basic contents of the Configuration
bool Configuration::initialiseContent(ProcessPool &procPool, double pairPotentialRange, bool emptyCurrentContent)
{
    // Clear existing content?
    if (emptyCurrentContent)
        empty();

    appliedSizeFactor_ = 1.0;
    requestedSizeFactor_ = 1.0;

    // If the Configuration is currently empty, run the generator Procedure and potentially load coordinates from file
    if (nAtoms() == 0)
    {
        // Run the generator procedure (we will need species / atom info to load any coordinates in)
        if (!generate(procPool, pairPotentialRange))
            return false;

        // If there are still no atoms, complain.
        if (nAtoms() == 0)
            return false;

        // If an input file was specified, try to load it
        if (inputCoordinates_.hasValidFileAndFormat())
        {
            if (DissolveSys::fileExists(inputCoordinates_))
            {
                Messenger::print("Loading initial coordinates from file '{}'...\n", inputCoordinates_.filename());
                LineParser inputFileParser(&procPool);
                if (!inputFileParser.openInput(inputCoordinates_))
                    return false;
                if (!loadCoordinates(inputFileParser, inputCoordinates_.coordinateFormat()))
                    return false;
                inputFileParser.closeFiles();

                // Need to update cell locations now, as we have new atom positions
                updateCellContents();
            }
            else
                return Messenger::error("Input coordinates file '{}' specified for Configuration '{}', but the "
                                        "file doesn't exist.\n",
                                        name(), inputCoordinates_.filename());
        }
    }

    return true;
}

// Set configuration temperature
void Configuration::setTemperature(double t) { temperature_ = t; }

// Return configuration temperature
double Configuration::temperature() const { return temperature_; }

/*
 * Parallel Comms
 */

// Set up process pool for this Configuration
bool Configuration::setUpProcessPool(Array<int> worldRanks)
{
    // Create pool
    processPool_.setUp(name_, worldRanks);

    // Assign processes, and
    if (!processPool_.assignProcessesToGroups())
        return false;
    processPool_.setGroupsFixed();

    return true;
}

// Return process pool for this Configuration
ProcessPool &Configuration::processPool() { return processPool_; }
