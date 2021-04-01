// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/partialset.h"
#include "base/lineparser.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "genericitems/deserialisers.h"
#include "genericitems/serialisers.h"
#include "io/export/data1d.h"
#include "templates/algorithms.h"

PartialSet::PartialSet() { fingerprint_ = "NO_FINGERPRINT"; }

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
bool PartialSet::setUp(const AtomTypeList &atomTypes, double rdfRange, double binWidth)
{
    // Set up partial arrays
    if (!setUpPartials(atomTypes))
        return false;

    // Initialise histograms for g(r) calculation
    setUpHistograms(rdfRange, binWidth);

    fingerprint_ = "NO_FINGERPRINT";

    return true;
}

// Set up PartialSet without initialising histogram arrays
bool PartialSet::setUpPartials(const AtomTypeList &atomTypes)
{
    // Copy type array
    atomTypes_ = atomTypes;
    auto nTypes = atomTypes_.nItems();

    partials_.initialise(nTypes, nTypes, true);
    boundPartials_.initialise(nTypes, nTypes, true);
    unboundPartials_.initialise(nTypes, nTypes, true);
    emptyBoundPartials_.initialise(nTypes, nTypes, true);
    emptyBoundPartials_ = false;

    // Set up array matrices for partials
    for_each_pair(atomTypes_.begin(), atomTypes_.end(), [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2) {
        partials_[{n, m}].setTag(fmt::format("{}-{}//Full", at1.atomTypeName(), at2.atomTypeName()));
        boundPartials_[{n, m}].setTag(fmt::format("{}-{}//Bound", at1.atomTypeName(), at2.atomTypeName()));
        unboundPartials_[{n, m}].setTag(fmt::format("{}-{}//Unbound", at1.atomTypeName(), at2.atomTypeName()));
    });

    // Set up array for total
    total_.setTag("Total");
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

    for_each_pair(0, nTypes, [&](int i, int j) {
        fullHistograms_[{i, j}].initialise(0.0, rdfRange, binWidth);
        boundHistograms_[{i, j}].initialise(0.0, rdfRange, binWidth);
        unboundHistograms_[{i, j}].initialise(0.0, rdfRange, binWidth);
    });
}

// Reset partial arrays
void PartialSet::reset()
{
    // Zero histogram bins if present
    for (auto n = 0; n < fullHistograms_.nRows(); ++n)
        for (auto m = n; m < fullHistograms_.nColumns(); ++m)
        {
            fullHistograms_[{n, m}].zeroBins();
            boundHistograms_[{n, m}].zeroBins();
            unboundHistograms_[{n, m}].zeroBins();
        }

    // Zero partials
    for_each_pair(0, atomTypes_.nItems(), [&](int i, int j) {
        std::fill(partials_[{i, j}].values().begin(), partials_[{i, j}].values().end(), 0.0);
        std::fill(boundPartials_[{i, j}].values().begin(), boundPartials_[{i, j}].values().end(), 0.0);
        std::fill(unboundPartials_[{i, j}].values().begin(), unboundPartials_[{i, j}].values().end(), 0.0);
        emptyBoundPartials_[{i, j}] = true;
    });

    // Zero total
    std::fill(total_.values().begin(), total_.values().end(), 0.0);

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
Histogram1D &PartialSet::fullHistogram(int i, int j) { return fullHistograms_[{i, j}]; }

// Return bound histogram specified
Histogram1D &PartialSet::boundHistogram(int i, int j) { return boundHistograms_[{i, j}]; }

// Return unbound histogram specified
Histogram1D &PartialSet::unboundHistogram(int i, int j) { return unboundHistograms_[{i, j}]; }

// Return full atom-atom partial specified
Data1D &PartialSet::partial(int i, int j) { return partials_[{i, j}]; }
const Data1D &PartialSet::partial(int i, int j) const { return partials_[{i, j}]; }

// Return atom-atom partial for unbound pairs
Data1D &PartialSet::unboundPartial(int i, int j) { return unboundPartials_[{i, j}]; }
const Data1D &PartialSet::unboundPartial(int i, int j) const { return unboundPartials_[{i, j}]; }

// Return atom-atom partial for bound pairs
Data1D &PartialSet::boundPartial(int i, int j) { return boundPartials_[{i, j}]; }
const Data1D &PartialSet::boundPartial(int i, int j) const { return boundPartials_[{i, j}]; }

// Return whether specified bound partial is empty
bool PartialSet::isBoundPartialEmpty(int i, int j) const { return emptyBoundPartials_[{i, j}]; }

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
    total_.initialise(partials_[{0, 0}]);
    std::fill(total_.values().begin(), total_.values().end(), 0.0);

    for_each_pair(
        atomTypes_.begin(), atomTypes_.end(), [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2) {
            // Calculate weighting factor if requested
            double factor = 1.0;
            if (applyConcentrationWeights)
            {
                double ci = at1.fraction();
                double cj = at2.fraction();
                factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
            }

            // Add contribution from partials (bound + unbound)
            std::transform(total_.values().begin(), total_.values().end(), partials_[{typeI, typeJ}].values().begin(),
                           total_.values().begin(), [=](auto total, auto partial) { return total + partial * factor; });
        });
}

// Return total function
Data1D &PartialSet::total() { return total_; }
const Data1D &PartialSet::total() const { return total_; }

// Calculate and return total bound function
Data1D PartialSet::boundTotal(bool applyConcentrationWeights) const
{
    const auto nTypes = atomTypes_.nItems();
    if (nTypes == 0)
        return Data1D();

    Data1D bound;
    bound.initialise(boundPartials_[{0, 0}]);

    for_each_pair(
        atomTypes_.begin(), atomTypes_.end(), [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
            // Calculate weighting factor if requested
            double factor = 1.0;
            if (applyConcentrationWeights)
            {
                double ci = atd1.fraction();
                double cj = atd2.fraction();
                factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
            }

            // Add contribution
            std::transform(bound.values().begin(), bound.values().end(), boundPartials_[{typeI, typeJ}].values().begin(),
                           bound.values().begin(), [=](auto bound, auto partial) { return bound + partial * factor; });
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
    unbound.initialise(boundPartials_[{0, 0}]);

    for_each_pair(
        atomTypes_.begin(), atomTypes_.end(), [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
            // Calculate weighting factor if requested
            double factor = 1.0;
            if (applyConcentrationWeights)
            {
                double ci = atd1.fraction();
                double cj = atd2.fraction();
                factor *= ci * cj * (typeI == typeJ ? 1.0 : 2.0);
            }

            // Add contribution
            std::transform(unbound.values().begin(), unbound.values().end(), unboundPartials_[{typeI, typeJ}].values().begin(),
                           unbound.values().begin(), [=](auto unbound, auto partial) { return unbound + partial * factor; });
        });

    return unbound;
}

// Save all partials and total
bool PartialSet::save(std::string_view prefix, std::string_view tag, std::string_view suffix,
                      std::string_view abscissaUnits) const
{
    assert(!prefix.empty());

    LineParser parser;

    // Write partials
    for_each_pair_early(atomTypes_.begin(), atomTypes_.end(),
                        [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2) -> EarlyReturn<bool> {
                            // Open file and check that we're OK to proceed writing to it
                            std::string filename{
                                fmt::format("{}-{}-{}-{}.{}", prefix, tag, at1.atomTypeName(), at2.atomTypeName(), suffix)};
                            Messenger::printVerbose("Writing partial file '{}'...\n", filename);

                            parser.openOutput(filename, true);
                            if (!parser.isFileGoodForWriting())
                                return Messenger::error("Couldn't open file '{}' for writing.\n", filename);

                            auto &full = partials_[{typeI, typeJ}];
                            auto &bound = boundPartials_[{typeI, typeJ}];
                            auto &unbound = unboundPartials_[{typeI, typeJ}];
                            parser.writeLineF("# {:<14}  {:<16}  {:<16}  {:<16}\n", abscissaUnits, "Full", "Bound", "Unbound");
                            for (auto n = 0; n < full.nValues(); ++n)
                                parser.writeLineF("{:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", full.xAxis(n), full.value(n),
                                                  bound.value(n), unbound.value(n));
                            parser.closeFiles();

                            return EarlyReturn<bool>::Continue;
                        });

    Messenger::printVerbose("Writing total file '{}'...\n", total_.tag());
    Data1DExportFileFormat exportFormat(fmt::format("{}-{}-total.{}", prefix, tag, suffix));
    return exportFormat.exportData(total_);
}

/*
 * Manipulation
 */

// Adjust all partials, adding specified delta to each
void PartialSet::adjust(double delta)
{
    for_each_pair(atomTypes_.begin(), atomTypes_.end(), [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2) {
        partials_[{n, m}] += delta;
        boundPartials_[{n, m}] += delta;
        unboundPartials_[{n, m}] += delta;
    });

    total_ += delta;
}

// Form partials from stored Histogram data
void PartialSet::formPartials(double boxVolume)
{
    for_each_pair(atomTypes_.begin(), atomTypes_.end(), [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2) {
        // Calculate RDFs from histogram data
        calculateRDF(partials_[{n, m}], fullHistograms_[{n, m}], boxVolume, at1.population(), at2.population(),
                     &at1 == &at2 ? 2.0 : 1.0);
        calculateRDF(boundPartials_[{n, m}], boundHistograms_[{n, m}], boxVolume, at1.population(), at2.population(),
                     &at1 == &at2 ? 2.0 : 1.0);
        calculateRDF(unboundPartials_[{n, m}], unboundHistograms_[{n, m}], boxVolume, at1.population(), at2.population(),
                     &at1 == &at2 ? 2.0 : 1.0);

        // Set flags for bound partials specifying if they are empty (i.e. there are no contributions of that type)
        emptyBoundPartials_[{n, m}] = boundHistograms_[{n, m}].nBinned() == 0;
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
            Interpolator::addInterpolated(partials_[{localI, localJ}], source.partial(typeI, typeJ), weighting);
            Interpolator::addInterpolated(boundPartials_[{localI, localJ}], source.boundPartial(typeI, typeJ), weighting);
            Interpolator::addInterpolated(unboundPartials_[{localI, localJ}], source.unboundPartial(typeI, typeJ), weighting);

            // If the source data bound partial is *not* empty, ensure that our emptyBoundPartials_ flag is set
            // correctly
            if (!source.isBoundPartialEmpty(typeI, typeJ))
                emptyBoundPartials_[{typeI, typeJ}] = false;
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
    const auto &bins = histogram.bins();

    destination.clear();

    double shellVolume, factor, r = 0.5 * delta, lowerShellLimit = 0.0, numberDensity = nSurrounding / boxVolume;
    for (auto n = 0; n < nBins; ++n)
    {
        shellVolume = (4.0 / 3.0) * PI * (pow(lowerShellLimit + delta, 3.0) - pow(lowerShellLimit, 3.0));
        factor = nCentres * (shellVolume * numberDensity);

        destination.addPoint(r, bins[n] * (multiplier / factor));

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
    assert(source.nAtomTypes() != 0);

    // If we currently contain no data, just copy the source data
    if (atomTypes_.nItems() == 0)
    {
        (*this) = source;
        return;
    }

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
        Interpolator::addInterpolated(partials_[{localI, localJ}], source.partial(typeI, typeJ));
        Interpolator::addInterpolated(boundPartials_[{localI, localJ}], source.boundPartial(typeI, typeJ));
        Interpolator::addInterpolated(unboundPartials_[{localI, localJ}], source.unboundPartial(typeI, typeJ));

        // If the source data bound partial is *not* empty, ensure that our emptyBoundPartials_ flag is set correctly
        if (!source.isBoundPartialEmpty(typeI, typeJ))
            emptyBoundPartials_[{typeI, typeJ}] = false;
    });

    // Add total function
    Interpolator::addInterpolated(total_, source.total());
}

void PartialSet::operator-=(const double delta) { adjust(-delta); }

void PartialSet::operator*=(const double factor)
{
    auto nTypes = atomTypes_.nItems();

    for_each_pair(0, nTypes, [&](auto n, auto m) {
        partials_[{n, m}] *= factor;
        boundPartials_[{n, m}] *= factor;
        unboundPartials_[{n, m}] *= factor;
    });

    total_ *= factor;
}

/*
 * Searchers
 */

// Return Data1D with specified tag, if it exists
OptionalReferenceWrapper<const Data1D> PartialSet::searchData1D(std::string_view tag) const
{
    auto fullIt = std::find_if(partials_.begin(), partials_.end(), [tag](const auto &data) { return data.tag() == tag; });
    if (fullIt != partials_.end())
        return *fullIt;
    auto boundIt =
        std::find_if(boundPartials_.begin(), boundPartials_.end(), [tag](const auto &data) { return data.tag() == tag; });
    if (boundIt != boundPartials_.end())
        return *boundIt;
    auto unboundIt =
        std::find_if(unboundPartials_.begin(), unboundPartials_.end(), [tag](const auto &data) { return data.tag() == tag; });
    if (unboundIt != unboundPartials_.end())
        return *unboundIt;
    if (total_.tag() == tag)
        return total_;
    return {};
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool PartialSet::deserialise(LineParser &parser, const CoreData &coreData)
{
    if (parser.readNextLine(LineParser::Defaults, fingerprint_) != LineParser::Success)
        return false;

    // Read atom types
    atomTypes_.clear();
    if (!atomTypes_.deserialise(parser, coreData))
        return false;
    auto nTypes = atomTypes_.nItems();

    // Read partials
    partials_.initialise(nTypes, nTypes, true);
    boundPartials_.initialise(nTypes, nTypes, true);
    unboundPartials_.initialise(nTypes, nTypes, true);
    emptyBoundPartials_.initialise(nTypes, nTypes, true);
    emptyBoundPartials_ = false;

    for (auto typeI = 0; typeI < nTypes; ++typeI)
    {
        for (auto typeJ = typeI; typeJ < nTypes; ++typeJ)
        {
            if (!partials_[{typeI, typeJ}].deserialise(parser))
                return false;
            if (!boundPartials_[{typeI, typeJ}].deserialise(parser))
                return false;
            if (!unboundPartials_[{typeI, typeJ}].deserialise(parser))
                return false;
        }
    }

    // Read total
    if (!total_.deserialise(parser))
        return false;

    // Read empty bound flags
    if (!GenericItemDeserialiser::deserialise<Array2D<char>>(emptyBoundPartials_, parser))
        return false;

    return true;
}

// Write data through specified LineParser
bool PartialSet::serialise(LineParser &parser) const
{
    // TODO To reduce filesize we could write abscissa first, and then each Y datset afterwards since they all share a
    // common scale

    if (!parser.writeLineF("{}\n", fingerprint_))
        return false;

    // Write out AtomTypes first
    atomTypes_.serialise(parser);
    auto nTypes = atomTypes_.nItems();

    // Write individual Data1D
    for_each_pair_early(0, nTypes, [&](int typeI, int typeJ) -> EarlyReturn<bool> {
        if (!partials_[{typeI, typeJ}].serialise(parser))
            return false;
        if (!boundPartials_[{typeI, typeJ}].serialise(parser))
            return false;
        if (!unboundPartials_[{typeI, typeJ}].serialise(parser))
            return false;

        return EarlyReturn<bool>::Continue;
    });

    // Write total
    if (!total_.serialise(parser))
        return false;

    // Write empty bound flags
    if (!GenericItemSerialiser::serialise<Array2D<char>>(emptyBoundPartials_, parser))
        return false;

    return true;
}
