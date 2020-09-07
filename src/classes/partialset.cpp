/*
    *** Set of Partials
    *** src/classes/partialset.cpp
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

#include "classes/partialset.h"
#include "base/lineparser.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "genericitems/array2dbool.h"
#include "io/export/data1d.h"
#include "templates/algorithms.h"

PartialSet::PartialSet() : ListItem<PartialSet>() { fingerprint_ = "NO_FINGERPRINT"; }

PartialSet::~PartialSet()
{
    fullHistograms_.clear();
    boundHistograms_.clear();
    unboundHistograms_.clear();

    partials_.clear();
    boundPartials_.clear();
    emptyBoundPartials_.clear();
    unboundPartials_.clear();
}

/*
 * Set of Partials
 */

// Set up PartialSet
bool PartialSet::setUp(const AtomTypeList &atomTypes, double rdfRange, double binWidth, std::string_view prefix,
                       std::string_view tag, std::string_view suffix, std::string_view abscissaUnits)
{
    // Set up partial arrays
    if (!setUpPartials(atomTypes, prefix, tag, suffix, abscissaUnits))
        return false;

    // Initialise histograms for g(r) calcultion
    setUpHistograms(rdfRange, binWidth);

    fingerprint_ = "NO_FINGERPRINT";

    return true;
}

// Set up PartialSet without initialising histogram arrays
bool PartialSet::setUpPartials(const AtomTypeList &atomTypes, std::string_view prefix, std::string_view tag,
                               std::string_view suffix, std::string_view abscissaUnits)
{
    abscissaUnits_ = abscissaUnits;

    // Copy type array
    atomTypes_ = atomTypes;
    auto nTypes = atomTypes_.nItems();

    partials_.initialise(nTypes, nTypes, true);
    boundPartials_.initialise(nTypes, nTypes, true);
    unboundPartials_.initialise(nTypes, nTypes, true);
    emptyBoundPartials_.initialise(nTypes, nTypes, true);
    emptyBoundPartials_ = false;

    // Set up array matrices for partials
    std::string title;
    for_each_pair(atomTypes_.begin(), atomTypes_.end(), [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2) {
        title = fmt::format("{}-{}-{}-{}.{}", prefix, tag, at1.atomTypeName(), at2.atomTypeName(), suffix);
        partials_.at(n, m).setName(title);
        boundPartials_.at(n, m).setName(title);
        unboundPartials_.at(n, m).setName(title);
    });

    // Set up array for total
    total_.setName(fmt::format("{}-{}-total.{}", prefix, tag, suffix));
    total_.clear();

    fingerprint_ = "NO_FINGERPRINT";

    return true;
}

// Set up histogram arrays for g(r) calculation
void PartialSet::setUpHistograms(double rdfRange, double binWidth)
{
    rdfRange_ = rdfRange;
    rdfBinWidth_ = binWidth;

    auto nTypes = atomTypes_.nItems();

    fullHistograms_.initialise(nTypes, nTypes, true);
    boundHistograms_.initialise(nTypes, nTypes, true);
    unboundHistograms_.initialise(nTypes, nTypes, true);

    for (int n = 0; n < nTypes; ++n)
    {
        for (int m = n; m < nTypes; ++m)
        {
            // Histogram arrays for g(r)
            fullHistograms_.at(n, m).initialise(0.0, rdfRange, binWidth);
            boundHistograms_.at(n, m).initialise(0.0, rdfRange, binWidth);
            unboundHistograms_.at(n, m).initialise(0.0, rdfRange, binWidth);
        }
    }
}

// Reset partial arrays
void PartialSet::reset()
{
    auto nTypes = atomTypes_.nItems();
    for (int n = 0; n < nTypes; ++n)
    {
        for (int m = n; m < nTypes; ++m)
        {
            fullHistograms_.at(n, m).zeroBins();
            boundHistograms_.at(n, m).zeroBins();
            unboundHistograms_.at(n, m).zeroBins();

            partials_.at(n, m).values() = 0.0;
            boundPartials_.at(n, m).values() = 0.0;
            unboundPartials_.at(n, m).values() = 0.0;
            emptyBoundPartials_.at(n, m) = true;
        }
    }
    total_.values() = 0.0;

    fingerprint_ = "NO_FINGERPRINT";
}

// Return number of AtomTypes used to generate matrices
int PartialSet::nAtomTypes() const { return atomTypes_.nItems(); }

// Return atom types list
const AtomTypeList &PartialSet::atomTypes() const { return atomTypes_; }

// Return RDF range used to initialise arrays
double PartialSet::rdfRange() const { return rdfRange_; }

// Return RDF bin width used to initialise arrays
double PartialSet::rdfBinWidth() const { return rdfBinWidth_; }

// Set new fingerprint
void PartialSet::setFingerprint(std::string_view fingerprint) { fingerprint_ = fingerprint; }

// Return fingerprint of partials
std::string_view PartialSet::fingerprint() const { return fingerprint_; }

// Return full histogram specified
Histogram1D &PartialSet::fullHistogram(int i, int j) { return fullHistograms_.at(i, j); }

// Return bound histogram specified
Histogram1D &PartialSet::boundHistogram(int i, int j) { return boundHistograms_.at(i, j); }

// Return unbound histogram specified
Histogram1D &PartialSet::unboundHistogram(int i, int j) { return unboundHistograms_.at(i, j); }

// Return full atom-atom partial specified
Data1D &PartialSet::partial(int i, int j) { return partials_.at(i, j); }

// Return full atom-atom partial specified (const)
Data1D &PartialSet::constPartial(int i, int j) const { return partials_.constAt(i, j); }

// Return atom-atom partial for unbound pairs
Data1D &PartialSet::unboundPartial(int i, int j) { return unboundPartials_.at(i, j); }

// Return atom-atom partial for unbound pairs (const)
Data1D &PartialSet::constUnboundPartial(int i, int j) const { return unboundPartials_.constAt(i, j); }

// Return atom-atom partial for bound pairs
Data1D &PartialSet::boundPartial(int i, int j) { return boundPartials_.at(i, j); }

// Return atom-atom partial for bound pairs (const)
Data1D &PartialSet::constBoundPartial(int i, int j) const { return boundPartials_.constAt(i, j); }

// Return whether specified bound partial is empty
bool PartialSet::isBoundPartialEmpty(int i, int j) const { return emptyBoundPartials_.constAt(i, j); }

// Sum partials into total
void PartialSet::formTotal(bool applyConcentrationWeights)
{
    auto nTypes = atomTypes_.nItems();
    if (nTypes == 0)
    {
        total_.clear();
        return;
    }

    // Copy x and y arrays from one of the partials, and zero the latter
    total_.initialise(partials_.at(0, 0));
    total_.values() = 0.0;

    int typeI, typeJ;
    for_each_pair(atomTypes_.begin(), atomTypes_.end(),
                  [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2) {
                      // Calculate weighting factor if requested
                      double factor = 1.0;
                      if (applyConcentrationWeights)
                      {
                          double ci = at1.fraction();
                          double cj = at2.fraction();
                          factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
                      }

                      // Add contribution from partials (bound + unbound)
                      total_.values() += partials_.at(typeI, typeJ).values() * factor;
                  });
}

// Return total function
Data1D &PartialSet::total() { return total_; }

// Return copy of total function
Data1D PartialSet::constTotal() const { return total_; }

// Calculate and return total bound function
Data1D PartialSet::boundTotal(bool applyConcentrationWeights) const
{
    const auto nTypes = atomTypes_.nItems();
    if (nTypes == 0)
        return Data1D();

    Data1D bound;
    bound.initialise(boundPartials_.constAt(0, 0));

    for_each_pair(atomTypes_.begin(), atomTypes_.end(),
                  [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
                      // Calculate weighting factor if requested
                      double factor = 1.0;
                      if (applyConcentrationWeights)
                      {
                          double ci = atd1.fraction();
                          double cj = atd2.fraction();
                          factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
                      }

                      // Add contribution
                      bound.values() += boundPartials_.constAt(typeI, typeJ).values() * factor;
                  });

    return bound;
}

// Calculate and return total unbound function
Data1D PartialSet::unboundTotal(bool applyConcentrationWeights) const
{
    const auto nTypes = atomTypes_.nItems();
    if (nTypes == 0)
        return Data1D();

    Data1D unbound;
    unbound.initialise(boundPartials_.constAt(0, 0));

    for_each_pair(atomTypes_.begin(), atomTypes_.end(),
                  [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
                      // Calculate weighting factor if requested
                      double factor = 1.0;
                      if (applyConcentrationWeights)
                      {
                          double ci = atd1.fraction();
                          double cj = atd2.fraction();
                          factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
                      }

                      // Add contribution
                      unbound.values() += unboundPartials_.constAt(typeI, typeJ).values() * factor;
                  });

    return unbound;
}

// Save all partials and total
bool PartialSet::save()
{
    LineParser parser;
    int typeI, typeJ, n;

    auto nTypes = atomTypes_.nItems();
    for (typeI = 0; typeI < nTypes; ++typeI)
    {
        for (typeJ = typeI; typeJ < nTypes; ++typeJ)
        {
            // Open file and check that we're OK to proceed writing to it
            std::string filename{partials_.at(typeI, typeJ).name()};
            Messenger::printVerbose("Writing partial file '{}'...\n", filename);

            parser.openOutput(filename, true);
            if (!parser.isFileGoodForWriting())
            {
                Messenger::error("Couldn't open file '{}' for writing.\n", filename);
                return false;
            }

            auto &full = partials_.at(typeI, typeJ);
            auto &bound = boundPartials_.at(typeI, typeJ);
            auto &unbound = unboundPartials_.at(typeI, typeJ);
            parser.writeLineF("# {:<14}  {:<16}  {:<16}  {:<16}\n", abscissaUnits_, "Full", "Bound", "Unbound");
            for (n = 0; n < full.nValues(); ++n)
                parser.writeLineF("{:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", full.constXAxis(n), full.constValue(n),
                                  bound.constValue(n), unbound.constValue(n));
            parser.closeFiles();
        }
    }

    Messenger::printVerbose("Writing total file '{}'...\n", total_.name());
    Data1DExportFileFormat exportFormat(total_.name());
    return exportFormat.exportData(total_);
}

// Name all object based on the supplied prefix
void PartialSet::setObjectTags(std::string_view prefix, std::string_view suffix)
{
    // Set up suffix (if any)
    std::string actualSuffix;
    if (!suffix.empty())
        actualSuffix = fmt::format("_{}", suffix);

    objectNamePrefix_ = prefix;

    for_each_pair(
        atomTypes_.begin(), atomTypes_.end(), [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2) {
            partials_.at(typeI, typeJ)
                .setObjectTag(fmt::format("{}//{}-{}//Full{}", prefix, at1.atomTypeName(), at2.atomTypeName(), actualSuffix));
            boundPartials_.at(typeI, typeJ)
                .setObjectTag(fmt::format("{}//{}-{}//Bound{}", prefix, at1.atomTypeName(), at2.atomTypeName(), actualSuffix));
            unboundPartials_.at(typeI, typeJ)
                .setObjectTag(
                    fmt::format("{}//{}-{}//Unbound{}", prefix, at1.atomTypeName(), at2.atomTypeName(), actualSuffix));
        });

    total_.setObjectTag(fmt::format("{}//Total{}", prefix, actualSuffix));
}

// Return prefix applied to object names
std::string_view PartialSet::objectNamePrefix() const { return objectNamePrefix_; }

// Set underlying Data1D file names
void PartialSet::setFileNames(std::string_view prefix, std::string_view tag, std::string_view suffix)
{
    auto nTypes = atomTypes_.nItems();

    // Set titles for partials
    std::string title;
    int n = 0, m = 0;
    for_each_pair(atomTypes_.begin(), atomTypes_.end(), [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2) {
        title = fmt::format("{}-{}-{}-{}.{}", prefix, tag, at1.atomTypeName(), at2.atomTypeName(), suffix);
        partials_.at(n, m).setName(title);
        boundPartials_.at(n, m).setName(title);
        unboundPartials_.at(n, m).setName(title);
    });

    // Set up array for total
    total_.setName(fmt::format("{}-{}-total.{}", prefix, tag, suffix));
}

/*
 * Manipulation
 */

// Adjust all partials, adding specified delta to each
void PartialSet::adjust(double delta)
{
    auto nTypes = atomTypes_.nItems();

    for_each_pair(atomTypes_.begin(), atomTypes_.end(), [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2) {
        partials_.at(n, m).values() += delta;
        boundPartials_.at(n, m).values() += delta;
        unboundPartials_.at(n, m).values() += delta;
    });

    total_.values() += delta;
}

// Form partials from stored Histogram data
void PartialSet::formPartials(double boxVolume)
{
    auto nTypes = atomTypes_.nItems();

    for_each_pair(atomTypes_.begin(), atomTypes_.end(), [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2) {
        // Calculate RDFs from histogram data
        calculateRDF(partials_.at(n, m), fullHistograms_.at(n, m), boxVolume, at1.population(), at2.population(),
                     &at1 == &at2 ? 2.0 : 1.0);
        calculateRDF(boundPartials_.at(n, m), boundHistograms_.at(n, m), boxVolume, at1.population(), at2.population(),
                     &at1 == &at2 ? 2.0 : 1.0);
        calculateRDF(unboundPartials_.at(n, m), unboundHistograms_.at(n, m), boxVolume, at1.population(), at2.population(),
                     &at1 == &at2 ? 2.0 : 1.0);

        // Set flags for bound partials specifying if they are empty (i.e. there are no contributions of that type)
        emptyBoundPartials_.at(n, m) = boundHistograms_.at(n, m).nBinned() == 0;
    });
}

// Add in partials from source PartialSet to our own
bool PartialSet::addPartials(PartialSet &source, double weighting)
{
    // Loop over partials in source set
    int typeI, typeJ, localI, localJ;

    auto sourceNTypes = source.atomTypes_.nItems();
    for (typeI = 0; typeI < sourceNTypes; ++typeI)
    {
        const auto atI = source.atomTypes_.atomType(typeI);
        localI = atomTypes_.indexOf(atI);
        if (localI == -1)
        {
            Messenger::error("AtomType '{}' not present in this PartialSet, so can't add in the associated data.\n",
                             atI->name());
            return false;
        }

        for (typeJ = typeI; typeJ < sourceNTypes; ++typeJ)
        {
            const auto atJ = source.atomTypes_.atomType(typeJ);
            localJ = atomTypes_.indexOf(atJ);
            if (localJ == -1)
            {
                Messenger::error("AtomType '{}' not present in this PartialSet, so can't add in the associated data.\n",
                                 atJ->name());
                return false;
            }

            // Add interpolated source partials to our set
            Interpolator::addInterpolated(partials_.at(localI, localJ), source.partial(typeI, typeJ), weighting);
            Interpolator::addInterpolated(boundPartials_.at(localI, localJ), source.boundPartial(typeI, typeJ), weighting);
            Interpolator::addInterpolated(unboundPartials_.at(localI, localJ), source.unboundPartial(typeI, typeJ), weighting);

            // If the source data bound partial is *not* empty, ensure that our emptyBoundPartials_ flag is set
            // correctly
            if (!source.isBoundPartialEmpty(typeI, typeJ))
                emptyBoundPartials_.at(typeI, typeJ) = false;
        }
    }

    // Add total function
    Interpolator::addInterpolated(total_, source.total(), weighting);

    return true;
}

// Calculate and return RDF from supplied Histogram and normalisation data
void PartialSet::calculateRDF(Data1D &destination, Histogram1D &histogram, double boxVolume, int nCentres, int nSurrounding,
                              double multiplier)
{
    auto nBins = histogram.nBins();
    double delta = histogram.binWidth();
    const Array<long int> &bins = histogram.bins();

    destination.clear();

    double shellVolume, factor, r = 0.5 * delta, lowerShellLimit = 0.0, numberDensity = nSurrounding / boxVolume;
    for (int n = 0; n < nBins; ++n)
    {
        shellVolume = (4.0 / 3.0) * PI * (pow(lowerShellLimit + delta, 3.0) - pow(lowerShellLimit, 3.0));
        factor = nCentres * (shellVolume * numberDensity);

        destination.addPoint(r, bins.constAt(n) * (multiplier / factor));

        r += delta;
        lowerShellLimit += delta;
    }
}

/*
 * Operators
 */

void PartialSet::operator+=(const double delta) { adjust(delta); }

void PartialSet::operator+=(const PartialSet &source)
{
    // If we currently contain no data, just copy the source data
    if (atomTypes_.nItems() == 0)
    {
        (*this) = source;
        return;
    }

    auto sourceNTypes = source.atomTypes_.nItems();

    // Loop over partials in source set
    const auto &types = source.atomTypes();
    for_each_pair(types.begin(), types.end(), [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
        const auto atI = atd1.atomType();
        const auto atJ = atd2.atomType();
        int localI = atomTypes_.indexOf(atI);
        int localJ = atomTypes_.indexOf(atJ);
        if (localI == -1)
        {
            Messenger::error("AtomType '{}' not present in this PartialSet, so can't add in the associated data.\n",
                             atI->name());
            return;
        }
        if (localJ == -1)
        {
            Messenger::error("AtomType '{}' not present in this PartialSet, so can't add in the associated data.\n",
                             atJ->name());
            return;
        }

        // Add interpolated source partials to our set
        Interpolator::addInterpolated(partials_.at(localI, localJ), source.constPartial(typeI, typeJ));
        Interpolator::addInterpolated(boundPartials_.at(localI, localJ), source.constBoundPartial(typeI, typeJ));
        Interpolator::addInterpolated(unboundPartials_.at(localI, localJ), source.constUnboundPartial(typeI, typeJ));

        // If the source data bound partial is *not* empty, ensure that our emptyBoundPartials_ flag is set correctly
        if (!source.isBoundPartialEmpty(typeI, typeJ))
            emptyBoundPartials_.at(typeI, typeJ) = false;
    });

    // Add total function
    Interpolator::addInterpolated(total_, source.constTotal());
}

void PartialSet::operator-=(const double delta) { adjust(-delta); }

void PartialSet::operator*=(const double factor)
{
    auto nTypes = atomTypes_.nItems();

    for (int n = 0; n < nTypes; ++n)
    {
        for (int m = n; m < nTypes; ++m)
        {
            partials_.at(n, m).values() *= factor;
            boundPartials_.at(n, m).values() *= factor;
            unboundPartials_.at(n, m).values() *= factor;
        }
    }

    total_.values() *= factor;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view PartialSet::itemClassName() { return "PartialSet"; }

// Read data through specified LineParser
bool PartialSet::read(LineParser &parser, CoreData &coreData)
{
    if (parser.readNextLine(LineParser::Defaults, objectNamePrefix_) != LineParser::Success)
        return false;
    if (parser.readNextLine(LineParser::Defaults, abscissaUnits_) != LineParser::Success)
        return false;
    if (parser.readNextLine(LineParser::Defaults, fingerprint_) != LineParser::Success)
        return false;

    // Read atom types
    atomTypes_.clear();
    if (!atomTypes_.read(parser, coreData))
        return false;
    auto nTypes = atomTypes_.nItems();

    // Read partials
    partials_.initialise(nTypes, nTypes, true);
    boundPartials_.initialise(nTypes, nTypes, true);
    unboundPartials_.initialise(nTypes, nTypes, true);
    emptyBoundPartials_.initialise(nTypes, nTypes, true);
    emptyBoundPartials_ = false;

    for (int typeI = 0; typeI < nTypes; ++typeI)
    {
        for (int typeJ = typeI; typeJ < nTypes; ++typeJ)
        {
            if (!partials_.at(typeI, typeJ).read(parser, coreData))
                return false;
            if (!boundPartials_.at(typeI, typeJ).read(parser, coreData))
                return false;
            if (!unboundPartials_.at(typeI, typeJ).read(parser, coreData))
                return false;
        }
    }

    // Read total
    if (!total_.read(parser, coreData))
        return false;

    // Read empty bound flags
    if (!GenericItemContainer<Array2D<bool>>::read(emptyBoundPartials_, parser))
        return false;

    return true;
}

// Write data through specified LineParser
bool PartialSet::write(LineParser &parser)
{
    // TODO To reduce filesize we could write abscissa first, and then each Y datset afterwards since they all share a
    // common scale

    if (!parser.writeLineF("{}\n", objectNamePrefix_))
        return false;
    if (!parser.writeLineF("{}\n", abscissaUnits_))
        return false;
    if (!parser.writeLineF("{}\n", fingerprint_))
        return false;

    // Write out AtomTypes first
    atomTypes_.write(parser);
    auto nTypes = atomTypes_.nItems();

    // Write individual Data1D
    for (int typeI = 0; typeI < nTypes; ++typeI)
    {
        for (int typeJ = typeI; typeJ < nTypes; ++typeJ)
        {
            if (!partials_.at(typeI, typeJ).write(parser))
                return false;
            if (!boundPartials_.at(typeI, typeJ).write(parser))
                return false;
            if (!unboundPartials_.at(typeI, typeJ).write(parser))
                return false;
        }
    }

    // Write total
    if (!total_.write(parser))
        return false;

    // Write empty bound flags
    if (!GenericItemContainer<Array2D<bool>>::write(emptyBoundPartials_, parser))
        return false;

    return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from root to all other processes
bool PartialSet::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    // The structure should have already been setup(), so arrays should be ready to copy
    auto nTypes = atomTypes_.nItems();
    for (int typeI = 0; typeI < nTypes; ++typeI)
    {
        for (int typeJ = typeI; typeJ < nTypes; ++typeJ)
        {
            if (!partials_.at(typeI, typeJ).broadcast(procPool, root, coreData))
                return Messenger::error("Failed to broadcast partials_ array.\n");
            if (!boundPartials_.at(typeI, typeJ).broadcast(procPool, root, coreData))
                return Messenger::error("Failed to broadcast boundPartials_ array.\n");
            if (!unboundPartials_.at(typeI, typeJ).broadcast(procPool, root, coreData))
                return Messenger::error("Failed to broadcast unboundPartials_ array.\n");
        }
    }

    if (!total_.broadcast(procPool, root, coreData))
        return Messenger::error("Failed to broadcast total_.\n");

    if (!procPool.broadcast(emptyBoundPartials_))
        return Messenger::error("Failed to broadcast emptyBoundPartials_ array.\n");
    if (!procPool.broadcast(objectNamePrefix_))
        return false;
#endif
    return true;
}

// Check item equality
bool PartialSet::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    auto nTypes = atomTypes_.nItems();
    for (int typeI = 0; typeI < nTypes; ++typeI)
    {
        for (int typeJ = typeI; typeJ < nTypes; ++typeJ)
        {
            if (!partials_.at(typeI, typeJ).equality(procPool))
                return Messenger::error("PartialSet full partial {}-{} is not equivalent.\n", typeI, typeJ);
            if (!boundPartials_.at(typeI, typeJ).equality(procPool))
                return Messenger::error("PartialSet bound partial {}-{} is not equivalent.\n", typeI, typeJ);
            if (!unboundPartials_.at(typeI, typeJ).equality(procPool))
                return Messenger::error("PartialSet unbound partial {}-{} is not equivalent.\n", typeI, typeJ);
        }
    }
    if (!total_.equality(procPool))
        return Messenger::error("PartialSet total sum is not equivalent.\n");
    if (!procPool.equality(emptyBoundPartials_))
        return Messenger::error("PartialSet emptyBoundPartials array is not equivalent.\n");
#endif
    return true;
}
