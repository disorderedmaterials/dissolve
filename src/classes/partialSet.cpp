// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/partialSet.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "io/export/data1D.h"
#include "items/deserialisers.h"
#include "items/serialisers.h"
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
bool PartialSet::setUp(const AtomTypeMix &atomTypeMix, double rdfRange, double binWidth)
{
    // Set up partial arrays
    if (!setUpPartials(atomTypeMix, half_))
        return false;

    // Initialise histograms for g(r) calculation
    setUpHistograms(rdfRange, binWidth);

    fingerprint_ = "NO_FINGERPRINT";

    return true;
}

// Set up PartialSet without initialising histogram arrays
bool PartialSet::setUpPartials(const AtomTypeMix &atomTypeMix, bool half)
{
    // Copy type array
    atomTypeMix_ = atomTypeMix;
    auto nTypes = atomTypeMix_.nItems();
    half_ = half;

    partials_.initialise(nTypes, nTypes, half_);
    boundPartials_.initialise(nTypes, nTypes, half_);
    unboundPartials_.initialise(nTypes, nTypes, half_);
    emptyBoundPartials_.initialise(nTypes, nTypes, half_);
    emptyBoundPartials_ = false;

    // Set up array matrices for partials
    dissolve::for_each_pair(
        ParallelPolicies::par, atomTypeMix_.begin(), atomTypeMix_.end(),
        [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2)
        {
            partials_[{n, m}].setTag(fmt::format("{}-{}//Full", at1.atomTypeName(), at2.atomTypeName()));
            boundPartials_[{n, m}].setTag(fmt::format("{}-{}//Bound", at1.atomTypeName(), at2.atomTypeName()));
            unboundPartials_[{n, m}].setTag(fmt::format("{}-{}//Unbound", at1.atomTypeName(), at2.atomTypeName()));
        },
        half_);

    // Set up arrays for totals
    total_.setTag("Total");
    boundTotal_.setTag("BoundTotal");
    unboundTotal_.setTag("UnboundTotal");
    total_.clear();
    boundTotal_.clear();
    unboundTotal_.clear();

    fingerprint_ = "NO_FINGERPRINT";

    return true;
}

// Set up histogram arrays for g(r) calculation
void PartialSet::setUpHistograms(double rdfRange, double binWidth)
{
    auto nTypes = atomTypeMix_.nItems();

    fullHistograms_.initialise(nTypes, nTypes, half_);
    boundHistograms_.initialise(nTypes, nTypes, half_);
    unboundHistograms_.initialise(nTypes, nTypes, half_);

    dissolve::for_each_pair(
        ParallelPolicies::par, 0, nTypes,
        [&](int i, int j)
        {
            fullHistograms_[{i, j}].initialise(0.0, rdfRange, binWidth);
            boundHistograms_[{i, j}].initialise(0.0, rdfRange, binWidth);
            unboundHistograms_[{i, j}].initialise(0.0, rdfRange, binWidth);
        },
        half_);
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
    dissolve::for_each_pair(
        ParallelPolicies::par, 0, atomTypeMix_.nItems(),
        [&](int i, int j)
        {
            std::fill(partials_[{i, j}].values().begin(), partials_[{i, j}].values().end(), 0.0);
            std::fill(boundPartials_[{i, j}].values().begin(), boundPartials_[{i, j}].values().end(), 0.0);
            std::fill(unboundPartials_[{i, j}].values().begin(), unboundPartials_[{i, j}].values().end(), 0.0);
            emptyBoundPartials_[{i, j}] = true;
        },
        half_);

    // Zero totals
    std::fill(total_.values().begin(), total_.values().end(), 0.0);
    std::fill(boundTotal_.values().begin(), boundTotal_.values().end(), 0.0);
    std::fill(unboundTotal_.values().begin(), unboundTotal_.values().end(), 0.0);

    fingerprint_ = "NO_FINGERPRINT";
}

// Return number of AtomTypes used to generate matrices
int PartialSet::nAtomTypes() const { return atomTypeMix_.nItems(); }

// Return atom types list
const AtomTypeMix &PartialSet::atomTypeMix() const { return atomTypeMix_; }

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
void PartialSet::formTotals(bool applyConcentrationWeights)
{
    auto nTypes = atomTypeMix_.nItems();
    if (nTypes == 0)
    {
        total_.clear();
        boundTotal_.clear();
        unboundTotal_.clear();
        return;
    }

    // Copy x and y arrays from one of the partials, and zero the latter
    boundTotal_.initialise(partials_[{0, 0}]);
    unboundTotal_.initialise(partials_[{0, 0}]);
    total_.initialise(partials_[{0, 0}]);
    std::fill(boundTotal_.values().begin(), boundTotal_.values().end(), 0.0);
    std::fill(unboundTotal_.values().begin(), unboundTotal_.values().end(), 0.0);
    std::fill(total_.values().begin(), total_.values().end(), 0.0);

    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypeMix_.begin(), atomTypeMix_.end(),
        [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2)
        {
            // Set weighting factor if requested
            auto factor = applyConcentrationWeights ? at1.fraction() * at2.fraction() * (typeI == typeJ ? 1.0 : 2.0) : 1.0;

            // Sum bound term
            std::transform(boundTotal_.values().begin(), boundTotal_.values().end(),
                           boundPartials_[{typeI, typeJ}].values().begin(), boundTotal_.values().begin(),
                           [=](auto total, auto partial) { return total + partial * factor; });

            // Sum unbound term
            std::transform(unboundTotal_.values().begin(), unboundTotal_.values().end(),
                           unboundPartials_[{typeI, typeJ}].values().begin(), unboundTotal_.values().begin(),
                           [=](auto total, auto partial) { return total + partial * factor; });
        },
        half_);

    total_ += boundTotal_;
    total_ += unboundTotal_;
}

// Sum partials into total for TR
void PartialSet::formTRTotals(NeutronWeights weights)
{
    auto nTypes = atomTypeMix_.nItems();
    if (nTypes == 0)
    {
        total_.clear();
        boundTotal_.clear();
        unboundTotal_.clear();
        return;
    }

    // Copy x and y arrays from one of the partials, and zero the latter
    boundTotal_.initialise(partials_[{0, 0}]);
    unboundTotal_.initialise(partials_[{0, 0}]);
    total_.initialise(partials_[{0, 0}]);
    std::fill(boundTotal_.values().begin(), boundTotal_.values().end(), 0.0);
    std::fill(unboundTotal_.values().begin(), unboundTotal_.values().end(), 0.0);
    std::fill(total_.values().begin(), total_.values().end(), 0.0);

    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypeMix_.begin(), atomTypeMix_.end(),
        [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2)
        {
            // Set weighting factor if requested
            auto factor = at1.fraction() * weights.boundCoherentProduct(typeI, typeJ) * (typeI == typeJ ? 1.0 : 2.0);

            // Sum bound term
            std::transform(boundTotal_.values().begin(), boundTotal_.values().end(),
                           boundPartials_[{typeI, typeJ}].values().begin(), boundTotal_.values().begin(),
                           [=](auto total, auto partial) { return total + partial * factor; });

            // Sum unbound term
            std::transform(unboundTotal_.values().begin(), unboundTotal_.values().end(),
                           unboundPartials_[{typeI, typeJ}].values().begin(), unboundTotal_.values().begin(),
                           [=](auto total, auto partial) { return total + partial * factor; });
        });

    total_ += boundTotal_;
    total_ += unboundTotal_;
}

// Return total function
Data1D &PartialSet::total() { return total_; }
const Data1D &PartialSet::total() const { return total_; }

// Return total bound function
Data1D &PartialSet::boundTotal() { return boundTotal_; }
const Data1D &PartialSet::boundTotal() const { return boundTotal_; }

// Return total unbound function
Data1D &PartialSet::unboundTotal() { return unboundTotal_; }
const Data1D &PartialSet::unboundTotal() const { return unboundTotal_; }

// Save all partials and total
bool PartialSet::save(std::string_view prefix, std::string_view tag, std::string_view suffix,
                      std::string_view abscissaUnits) const
{
    assert(!prefix.empty());

    LineParser parser;

    // Write partials
    for_each_pair_early(
        atomTypeMix_.begin(), atomTypeMix_.end(),
        [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2) -> EarlyReturn<bool>
        {
            // Open file and check that we're OK to proceed writing to it
            std::string filename{fmt::format("{}-{}-{}-{}.{}", prefix, tag, at1.atomTypeName(), at2.atomTypeName(), suffix)};
            Messenger::printVerbose("Writing partial file '{}'...\n", filename);

            parser.openOutput(filename, true);
            if (!parser.isFileGoodForWriting())
                return Messenger::error("Couldn't open file '{}' for writing.\n", filename);

            auto &full = partials_[{typeI, typeJ}];
            auto &bound = boundPartials_[{typeI, typeJ}];
            auto &unbound = unboundPartials_[{typeI, typeJ}];
            parser.writeLineF("# {:<14}  {:<16}  {:<16}  {:<16}\n", abscissaUnits, "Full", "Bound", "Unbound");
            for (auto n = 0; n < full.nValues(); ++n)
                parser.writeLineF("{:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", full.xAxis(n), full.value(n), bound.value(n),
                                  unbound.value(n));
            parser.closeFiles();

            return EarlyReturn<bool>::Continue;
        },
        half_);

    Messenger::printVerbose("Writing total file '{}'...\n", total_.tag());
    if (!Data1DExportFileFormat(fmt::format("{}-{}-total.{}", prefix, tag, suffix)).exportData(total_))
        return false;

    Messenger::printVerbose("Writing bound total file '{}'...\n", boundTotal_.tag());
    if (!Data1DExportFileFormat(fmt::format("{}-{}-bound.{}", prefix, tag, suffix)).exportData(boundTotal_))
        return false;

    Messenger::printVerbose("Writing unbound total file '{}'...\n", unboundTotal_.tag());
    if (!Data1DExportFileFormat(fmt::format("{}-{}-unbound.{}", prefix, tag, suffix)).exportData(unboundTotal_))
        return false;

    return true;
}

/*
 * Manipulation
 */

// Adjust all partials, adding specified delta to each
void PartialSet::adjust(double delta)
{
    dissolve::for_each_pair(
        ParallelPolicies::par, atomTypeMix_.begin(), atomTypeMix_.end(),
        [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2)
        {
            partials_[{n, m}] += delta;
            boundPartials_[{n, m}] += delta;
            unboundPartials_[{n, m}] += delta;
        },
        half_);

    total_ += delta;
    boundTotal_ += delta;
    unboundTotal_ += delta;
}

// Form partials from stored Histogram data
void PartialSet::formPartials(double boxVolume)
{
    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypeMix_.begin(), atomTypeMix_.end(),
        [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2)
        {
            // Calculate RDFs from histogram data
            calculateRDF(partials_[{n, m}], fullHistograms_[{n, m}], boxVolume, at1.population(), at2.population(),
                         &at1 == &at2 ? 2.0 : 1.0);
            calculateRDF(boundPartials_[{n, m}], boundHistograms_[{n, m}], boxVolume, at1.population(), at2.population(),
                         &at1 == &at2 ? 2.0 : 1.0);
            calculateRDF(unboundPartials_[{n, m}], unboundHistograms_[{n, m}], boxVolume, at1.population(), at2.population(),
                         &at1 == &at2 ? 2.0 : 1.0);

            // Set flags for bound partials specifying if they are empty (i.e. there are no
            // contributions of that type)
            emptyBoundPartials_[{n, m}] = boundHistograms_[{n, m}].nBinned() == 0;
        },
        half_);
}

// Add in partials from source PartialSet to our own
bool PartialSet::addPartials(PartialSet &source, double weighting)
{
    auto sourceNTypes = source.atomTypeMix_.nItems();
    for (auto typeI = 0; typeI < sourceNTypes; ++typeI)
    {
        const auto atI = source.atomTypeMix_.atomType(typeI);
        auto localI = atomTypeMix_.indexOf(atI);
        if (!localI)
        {
            return Messenger::error("AtomType '{}' not present in this PartialSet, so can't add in the associated data.\n",
                                    atI->name());
        }

        for (auto typeJ = typeI; typeJ < sourceNTypes; ++typeJ)
        {
            const auto atJ = source.atomTypeMix_.atomType(typeJ);
            auto localJ = atomTypeMix_.indexOf(atJ);
            if (!localJ)
            {
                return Messenger::error("AtomType '{}' not present in this PartialSet, so can't add in the associated data.\n",
                                        atJ->name());
            }

            // Add interpolated source partials to our set
            Interpolator::addInterpolated(source.partial(typeI, typeJ), partials_[{*localI, *localJ}], weighting);
            Interpolator::addInterpolated(source.boundPartial(typeI, typeJ), boundPartials_[{*localI, *localJ}], weighting);
            Interpolator::addInterpolated(source.unboundPartial(typeI, typeJ), unboundPartials_[{*localI, *localJ}], weighting);

            // If the source data bound partial is *not* empty, ensure that our emptyBoundPartials_ flag is set
            // correctly
            if (!source.isBoundPartialEmpty(typeI, typeJ))
                emptyBoundPartials_[{typeI, typeJ}] = false;
        }
    }

    // Add total function
    Interpolator::addInterpolated(source.total_, total_, weighting);
    Interpolator::addInterpolated(source.boundTotal_, boundTotal_, weighting);
    Interpolator::addInterpolated(source.unboundTotal_, unboundTotal_, weighting);

    return true;
}

// Calculate RDF from supplied Histogram and normalisation data
void PartialSet::calculateRDF(Data1D &destination, const Histogram1D &histogram, double boxVolume, int nCentres,
                              int nSurrounding, double multiplier)
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
    if (atomTypeMix_.nItems() == 0)
    {
        (*this) = source;
        return;
    }

    // Loop over partials in source set
    const auto &types = source.atomTypeMix();
    dissolve::for_each_pair(
        ParallelPolicies::seq, types.begin(), types.end(),
        [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2)
        {
            auto optPairIndex = atomTypeMix_.indexOf(atd1.atomType(), atd2.atomType());
            if (!optPairIndex)
            {
                Messenger::error("AtomType pair '{}-{}' not present in this PartialSet, so can't add in the associated data.\n",
                                 atd1.atomTypeName(), atd2.atomTypeName());
                return;
            }
            auto &[localI, localJ] = *optPairIndex;

            // Add interpolated source partials to our set
            Interpolator::addInterpolated(source.partial(typeI, typeJ), partials_[{localI, localJ}]);
            Interpolator::addInterpolated(source.boundPartial(typeI, typeJ), boundPartials_[{localI, localJ}]);
            Interpolator::addInterpolated(source.unboundPartial(typeI, typeJ), unboundPartials_[{localI, localJ}]);

            // If the source data bound partial is *not* empty, ensure that our emptyBoundPartials_ flag is set correctly
            if (!source.isBoundPartialEmpty(typeI, typeJ))
                emptyBoundPartials_[{typeI, typeJ}] = false;
        },
        half_);

    // Add total function
    Interpolator::addInterpolated(source.total(), total_);
}

void PartialSet::operator-=(const double delta) { adjust(-delta); }

void PartialSet::operator*=(const double factor)
{
    dissolve::for_each_pair(
        ParallelPolicies::par, 0, atomTypeMix_.nItems(),
        [&](auto n, auto m)
        {
            partials_[{n, m}] *= factor;
            boundPartials_[{n, m}] *= factor;
            unboundPartials_[{n, m}] *= factor;
        },
        half_);

    total_ *= factor;
    boundTotal_ *= factor;
    unboundTotal_ *= factor;
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
    if (boundTotal_.tag() == tag)
        return boundTotal_;
    if (unboundTotal_.tag() == tag)
        return unboundTotal_;
    return {};
}

/*
 * Serialisation
 */

int readDataPoint(int argIndex, LineParser &parser, Data1D &data)
{
    data.values().push_back(parser.argd(argIndex++));
    if (data.valuesHaveErrors())
        data.errors().push_back(parser.argd(argIndex++));
    return argIndex;
}

// Read data through specified LineParser
bool PartialSet::deserialise(LineParser &parser, const CoreData &coreData)
{
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    fingerprint_ = parser.argsv(0);
    half_ = parser.hasArg(1) ? parser.argb(1) : true;

    // Read atom types
    atomTypeMix_.clear();
    if (!atomTypeMix_.deserialise(parser, coreData))
        return false;
    auto nTypes = atomTypeMix_.nItems();

    // Read partials
    partials_.initialise(nTypes, nTypes, half_);
    boundPartials_.initialise(nTypes, nTypes, half_);
    unboundPartials_.initialise(nTypes, nTypes, half_);
    emptyBoundPartials_.initialise(nTypes, nTypes, half_);
    emptyBoundPartials_ = false;

    for (auto typeI = 0; typeI < nTypes; ++typeI)
    {
        for (auto typeJ = 0; typeJ < nTypes; ++typeJ)
        {
            if (half_ && typeJ < typeI)
                continue;
            auto &part = partials_[{typeI, typeJ}];
            auto &bound = boundPartials_[{typeI, typeJ}];
            auto &unbound = unboundPartials_[{typeI, typeJ}];

            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            part.setTag(parser.argsv(0));
            bound.setTag(parser.argsv(1));
            unbound.setTag(parser.argsv(2));

            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            auto nPoints = parser.argi(0);
            auto partError = parser.argb(1);
            if (partError)
                part.addErrors();
            auto boundError = parser.argb(2);
            if (boundError)
                bound.addErrors();
            auto unboundError = parser.argb(3);
            if (unboundError)
                unbound.addErrors();

            part.xAxis().reserve(nPoints);
            part.values().reserve(nPoints);
            if (part.valuesHaveErrors())
                part.errors().reserve(nPoints);
            bound.xAxis().reserve(nPoints);
            bound.values().reserve(nPoints);
            if (bound.valuesHaveErrors())
                bound.errors().reserve(nPoints);
            unbound.xAxis().reserve(nPoints);
            unbound.values().reserve(nPoints);
            if (unbound.valuesHaveErrors())
                unbound.errors().reserve(nPoints);

            for (auto n = 0; n < nPoints; ++n)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                int argIndex = 0;
                auto x = parser.argd(argIndex++);
                part.xAxis().push_back(x);
                bound.xAxis().push_back(x);
                unbound.xAxis().push_back(x);
                argIndex = readDataPoint(argIndex, parser, part);
                argIndex = readDataPoint(argIndex, parser, bound);
                argIndex = readDataPoint(argIndex, parser, unbound);
            }
        }
    }

    // Read totals
    if (!total_.deserialise(parser))
        return false;
    if (GenericList::baseDataVersion() == GenericList::DeserialisableDataVersion::Version08X)
    {
        // Writing of bound & unbound totals introduced in 0.9.0
        boundTotal_.initialise(total_);
        unboundTotal_.initialise(total_);
    }
    else
    {
        if (!boundTotal_.deserialise(parser))
            return false;
        if (!unboundTotal_.deserialise(parser))
            return false;
    }

    // Read empty bound flags
    if (!GenericItemDeserialiser::deserialise<Array2D<char>>(emptyBoundPartials_, parser))
        return false;

    return true;
}

std::string writeDataPoint(int i, Data1D data)
{
    if (data.valuesHaveErrors())
        return fmt::format("{} {}", data.value(i), data.error(i));
    else
        return fmt::format("{}", data.value(i));
}

// Write data through specified LineParser
bool PartialSet::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("'{}'  {}\n", fingerprint_, half_))
        return false;

    // Write out AtomTypes first
    atomTypeMix_.serialise(parser);
    auto nTypes = atomTypeMix_.nItems();

    // Write individual Data1D
    auto success = for_each_pair_early(
        0, nTypes,
        [&](int typeI, int typeJ) -> EarlyReturn<bool>
        {
            const auto &part = partials_[{typeI, typeJ}];
            const auto &bound = boundPartials_[{typeI, typeJ}];
            const auto &unbound = unboundPartials_[{typeI, typeJ}];

            // Write tag
            if (!parser.writeLineF("'{}' '{}' '{}'\n", part.tag(), bound.tag(), unbound.tag()))
                return false;

            // Write axis size and errors flag
            if (!parser.writeLineF("{} {} {} {}\n", part.xAxis().size(), DissolveSys::btoa(part.valuesHaveErrors()),
                                   DissolveSys::btoa(bound.valuesHaveErrors()), DissolveSys::btoa(unbound.valuesHaveErrors())))
                return false;

            for (auto i = 0; i < part.xAxis().size(); ++i)
            {
                if (!parser.writeLineF("{} {} {} {}\n", part.xAxis(i), writeDataPoint(i, part), writeDataPoint(i, bound),
                                       writeDataPoint(i, unbound)))
                    return false;
            }

            return EarlyReturn<bool>::Continue;
        });

    if (!success.value_or(true))
        return false;

    // Write totals
    if (!total_.serialise(parser))
        return false;
    if (!boundTotal_.serialise(parser))
        return false;
    if (!unboundTotal_.serialise(parser))
        return false;

    // Write empty bound flags
    if (!GenericItemSerialiser::serialise<Array2D<char>>(emptyBoundPartials_, parser))
        return false;

    return true;
}
