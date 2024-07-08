// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/coordinateSets.h"
#include "base/lineParser.h"
#include "base/randomBuffer.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/bool.h"
#include "keywords/fileAndFormat.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/species.h"
#include "modules/md/md.h"

CoordinateSetsProcedureNode::CoordinateSetsProcedureNode(const Species *sp)
    : ProcedureNode(NodeType::CoordinateSets), species_(sp)
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<SpeciesKeyword>("Species", "Target species", species_);
    keywords_.add<EnumOptionsKeyword<CoordinateSetsProcedureNode::CoordinateSetSource>>(
        "Source", "Source of coordinate sets on addition of the species", source_, coordinateSetSources());
    keywords_.add<BoolKeyword>("Force", "Force generation of coordinates, even if existing sets exist", force_);

    keywords_.setOrganisation("Options", "Source Data (if using file)");
    keywords_.add<FileAndFormatKeyword>("File", "File / format for coordinate sets to read in", fileSource_, "EndFile");

    keywords_.setOrganisation("Options", "Generation (if using MD)");
    keywords_.add<NodeValueKeyword>("NSets", "Number of coordinate sets to generate", nSets_, this);
    keywords_.add<NodeValueKeyword>("NSteps", "Number of steps to run (between storing coordinate sets)", nSteps_, this);
    keywords_.add<NodeValueKeyword>("DeltaT", "Timestep in ps", deltaT_, this);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool CoordinateSetsProcedureNode::mustBeNamed() const { return true; }

/*
 * Node Data
 */

// Return enum option info for SetCreationMethod
EnumOptions<CoordinateSetsProcedureNode::CoordinateSetSource> CoordinateSetsProcedureNode::coordinateSetSources()
{
    return EnumOptions<CoordinateSetsProcedureNode::CoordinateSetSource>(
        "SetCreationMethod", {{CoordinateSetsProcedureNode::CoordinateSetSource::File, "File"},
                              {CoordinateSetsProcedureNode::CoordinateSetSource::MD, "MD"}});
}

// Add new coordinate set
std::vector<Vec3<double>> &CoordinateSetsProcedureNode::addSet()
{
    assert(species_);
    return sets_.emplace_back(species_->nAtoms(), Vec3<double>());
}

void CoordinateSetsProcedureNode::setSets(std::vector<std::vector<Vec3<double>>> sets) { sets_ = std::move(sets); }

// Return number of available coordinates sets
int CoordinateSetsProcedureNode::nSets() const { return sets_.size(); }

// Return nth coordinate set
const std::vector<Vec3<double>> &CoordinateSetsProcedureNode::set(int n) const { return sets_[n]; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CoordinateSetsProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!species_)
        return Messenger::error("No Species set in CoordinateSets node.\n");

    // Clear existing sets?
    if (force_)
        sets_.clear();

    // If a file source is selected, load its contents now
    if (source_ == CoordinateSetSource::File && sets_.empty())
    {
        // Check import file format
        if (!fileSource_.hasFilename())
            return Messenger::error("A suitable coordinate file and format must be supplied.\n");

        // Open the specified file
        LineParser parser(&procedureContext.processPool());
        if ((!parser.openInput(fileSource_.filename())) || (!parser.isFileGoodForReading()))
            return Messenger::error("Couldn't open coordinate sets file '{}'.\n", fileSource_.filename());

        // Read in as many coordinate sets as exists in the file
        while (!parser.eofOrBlank())
        {
            auto &coordSet = addSet();
            if (!fileSource_.importData(parser, coordSet))
                return Messenger::error("Failed to read coordinate set {} from file.\n", sets_.size());
        }

        Messenger::print("{} coordinate sets read in for Species '{}'.\n", sets_.size(), name());
    }

    return true;
}

// Execute node
bool CoordinateSetsProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // Do we need to generate new sets?
    if (!sets_.empty())
        return true;

    Messenger::print("[CoordinateSets] Target species is '{}'.\n", species_->name());

    // For file-based import, this was done in prepare()
    if (source_ == CoordinateSetSource::File)
    {
        Messenger::print("[CoordinateSets] File source specified, so nothing more to do.\n");
        return true;
    }

    // Can't do MD on the Species if it has any missing core information
    if (!species_->checkSetUp())
        return Messenger::error("Can't generate coordinate sets via MD for species '{}' because it is not set up correctly.\n",
                                species_->name());

    // Also can't do MD on the Species if it has too few degrees of freedom
    if (species_->nAtoms() == 1)
    {
        Messenger::print("[CoordinateSets] Atomic species, so adding reference coordinate to a single set and exiting.\n");

        // Store a new set
        addSet() = {species_->atoms().begin()->r()};

        return true;
    }

    // Initialise the random number buffer for all processes
    RandomBuffer randomBuffer(procedureContext.processPool(),
                              ProcessPool::subDivisionStrategy(procedureContext.processPool().bestStrategy()));

    // Initialise random velocities
    std::vector<Vec3<double>> velocities(species_->nAtoms());
    std::generate(velocities.begin(), velocities.end(),
                  [&]()
                  {
                      return Vec3<double>(exp(randomBuffer.random() - 0.5), exp(randomBuffer.random() - 0.5),
                                          exp(randomBuffer.random() - 0.5)) /
                             sqrt(TWOPI);
                  });

    // Grab current Species coordinates
    std::vector<Vec3<double>> r(species_->nAtoms());
    std::transform(species_->atoms().begin(), species_->atoms().end(), r.begin(), [](const auto &i) { return i.r(); });

    Messenger::print("[CoordinateSets] Generating {} sets...\n", nSets_.asInteger());

    for (auto n = 0; n < nSets_.asInteger(); ++n)
    {
        // Evolve our coordinates
        r = MDModule::evolve(procedureContext.processPool(), procedureContext.potentialMap(), species_, temperature_.asDouble(),
                             nSteps_.asInteger(), deltaT_.asDouble(), r, velocities);

        // Store a new set
        addSet() = r;
    }

    return true;
}
