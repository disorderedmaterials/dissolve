// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/partialSet.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "io/export/data1D.h"
#include "items/deserialisers.h"
#include "items/serialisers.h"
#include "math/mathFunc.h"
#include "templates/algorithms.h"

PartialSet::PartialSet(const std::map<const Species *, double> &realSpeciesPopulations)
    : realSpeciesPopulations_(realSpeciesPopulations)
{
}

PartialSet::~PartialSet()
{
    partials_.clear();
    boundPartials_.clear();
    unboundPartials_.clear();
}

/*
 * Set of Partials
 */

// Initialise
void PartialSet::initialise(const AtomTypeMix &atomTypeMix, bool half)
{
    // Copy type array
    atomTypeMix_ = atomTypeMix;
    auto nTypes = atomTypeMix_.nItems();
    half_ = half;

    partials_.clear(half_);
    boundPartials_.clear(half_);
    unboundPartials_.clear(half_);

    // Create data for partials and set tags
    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypeMix_,
        [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2)
        {
            DoubleKeyedMapKey key(at1.atomTypeName(), at2.atomTypeName());
            partials_.get(key).setTag(std::format("{}-{}//Full", at1.atomTypeName(), at2.atomTypeName()));
            boundPartials_.get(key).setTag(std::format("{}-{}//Bound", at1.atomTypeName(), at2.atomTypeName()));
            unboundPartials_.get(key).setTag(std::format("{}-{}//Unbound", at1.atomTypeName(), at2.atomTypeName()));
        },
        half_);

    // Set up arrays for totals
    total_.setTag("Total");
    boundTotal_.setTag("BoundTotal");
    unboundTotal_.setTag("UnboundTotal");
    total_.clear();
    boundTotal_.clear();
    unboundTotal_.clear();
}

// Reset partial arrays
void PartialSet::reset()
{
    // Zero partials
    for (auto &partial : std::views::values(partials_))
        std::ranges::fill(partial.values(), 0.0);
    for (auto &partial : std::views::values(boundPartials_))
        std::ranges::fill(partial.values(), 0.0);
    for (auto &partial : std::views::values(unboundPartials_))
        std::ranges::fill(partial.values(), 0.0);

    // Zero totals
    std::fill(total_.values().begin(), total_.values().end(), 0.0);
    std::fill(boundTotal_.values().begin(), boundTotal_.values().end(), 0.0);
    std::fill(unboundTotal_.values().begin(), unboundTotal_.values().end(), 0.0);
}

// Return number of AtomTypes used to generate matrices
int PartialSet::nAtomTypes() const { return atomTypeMix_.nItems(); }

// Return atom types list
const AtomTypeMix &PartialSet::atomTypeMix() const { return atomTypeMix_; }

// Set new fingerprint
void PartialSet::setFingerprint(std::string_view fingerprint) { fingerprint_ = fingerprint; }

// Return fingerprint of partials
std::string_view PartialSet::fingerprint() const { return fingerprint_; }

// Return full atom-atom partials
DoubleKeyedMap<Data1D> &PartialSet::partials() { return partials_; }
const DoubleKeyedMap<Data1D> &PartialSet::partials() const { return partials_; }

// Return bound atom-atom partials
DoubleKeyedMap<Data1D> &PartialSet::boundPartials() { return boundPartials_; }
const DoubleKeyedMap<Data1D> &PartialSet::boundPartials() const { return boundPartials_; }

// Return unbound atom-atom partials
DoubleKeyedMap<Data1D> &PartialSet::unboundPartials() { return unboundPartials_; }
const DoubleKeyedMap<Data1D> &PartialSet::unboundPartials() const { return unboundPartials_; }

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
    boundTotal_.initialise(partials_.begin()->second);
    unboundTotal_.initialise(partials_.begin()->second);
    total_.initialise(partials_.begin()->second);
    std::fill(boundTotal_.values().begin(), boundTotal_.values().end(), 0.0);
    std::fill(unboundTotal_.values().begin(), unboundTotal_.values().end(), 0.0);
    std::fill(total_.values().begin(), total_.values().end(), 0.0);

    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypeMix_,
        [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2)
        {
            DoubleKeyedMapKey key(at1.atomTypeName(), at2.atomTypeName());

            // Set weighting factor if requested
            auto factor = applyConcentrationWeights ? at1.fraction() * at2.fraction() * (typeI == typeJ ? 1.0 : 2.0) : 1.0;

            // Sum bound term
            std::transform(boundTotal_.values().begin(), boundTotal_.values().end(), boundPartials_.get(key).values().begin(),
                           boundTotal_.values().begin(), [=](auto total, auto partial) { return total + partial * factor; });

            // Sum unbound term
            std::transform(unboundTotal_.values().begin(), unboundTotal_.values().end(),
                           unboundPartials_.get(key).values().begin(), unboundTotal_.values().begin(),
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
    boundTotal_.initialise(partials_.begin()->second);
    unboundTotal_.initialise(partials_.begin()->second);
    total_.initialise(partials_.begin()->second);
    std::fill(boundTotal_.values().begin(), boundTotal_.values().end(), 0.0);
    std::fill(unboundTotal_.values().begin(), unboundTotal_.values().end(), 0.0);
    std::fill(total_.values().begin(), total_.values().end(), 0.0);

    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypeMix_,
        [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2)
        {
            DoubleKeyedMapKey key(at1.atomTypeName(), at2.atomTypeName());

            // Set weighting factor if requested
            auto factor = at1.fraction() * weights.boundCoherentProduct(typeI, typeJ);

            // Sum bound term
            std::transform(boundTotal_.values().begin(), boundTotal_.values().end(), boundPartials_.get(key).values().begin(),
                           boundTotal_.values().begin(), [=](auto total, auto partial) { return total + partial * factor; });

            // Sum unbound term
            std::transform(unboundTotal_.values().begin(), unboundTotal_.values().end(),
                           unboundPartials_.get(key).values().begin(), unboundTotal_.values().begin(),
                           [=](auto total, auto partial) { return total + partial * factor; });
        },
        half_);

    total_ += boundTotal_;
    total_ += unboundTotal_;
}

// Return total function
Data1D &PartialSet::total() { return total_; }
const Data1D &PartialSet::total() const { return total_; }

// Return total bound function
Data1D &PartialSet::boundTotal() { return boundTotal_; }
const Data1D &PartialSet::boundTotal() const { return boundTotal_; }

// Effective density
void PartialSet::setEffectiveDensity(double rho) { rho_ = rho; }
double PartialSet::effectiveDensity() const { return rho_; }

// Return total unbound function
Data1D &PartialSet::unboundTotal() { return unboundTotal_; }
const Data1D &PartialSet::unboundTotal() const { return unboundTotal_; }

// Species populations
const std::map<const Species *, double> &PartialSet::realSpeciesPopulations() const { return realSpeciesPopulations_; }

// Save all partials and total
bool PartialSet::save(std::string_view prefix, std::string_view tag, std::string_view suffix,
                      std::string_view abscissaUnits) const
{
    assert(!prefix.empty());

    LineParser parser;

    // Write partials
    for_each_pair_early(
        atomTypeMix_,
        [&](int typeI, const AtomTypeData &at1, int typeJ, const AtomTypeData &at2) -> EarlyReturn<bool>
        {
            DoubleKeyedMapKey key(at1.atomTypeName(), at2.atomTypeName());

            // Open file and check that we're OK to proceed writing to it
            std::string filename{std::format("{}-{}-{}-{}.{}", prefix, tag, at1.atomTypeName(), at2.atomTypeName(), suffix)};
            Messenger::printVerbose("Writing partial file '{}'...\n", filename);

            parser.openOutput(filename, true);
            if (!parser.isFileGoodForWriting())
                return Messenger::error("Couldn't open file '{}' for writing.\n", filename);

            auto &full = partials_.get(key);
            auto &bound = boundPartials_.get(key);
            auto &unbound = unboundPartials_.get(key);

            parser.writeLineF("# {:<14}  {:<16}  {:<16}  {:<16}\n", abscissaUnits, "Full", "Bound", "Unbound");
            for (auto n = 0; n < full.nValues(); ++n)
                parser.writeLineF("{:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", full.xAxis(n), full.value(n), bound.value(n),
                                  unbound.value(n));
            parser.closeFiles();

            return EarlyReturn<bool>::Continue;
        },
        half_);

    Messenger::printVerbose("Writing total file '{}'...\n", total_.tag());
    if (!Data1DExportFileFormat(std::format("{}-{}-total.{}", prefix, tag, suffix)).exportData(total_))
        return false;

    Messenger::printVerbose("Writing bound total file '{}'...\n", boundTotal_.tag());
    if (!Data1DExportFileFormat(std::format("{}-{}-bound.{}", prefix, tag, suffix)).exportData(boundTotal_))
        return false;

    Messenger::printVerbose("Writing unbound total file '{}'...\n", unboundTotal_.tag());
    if (!Data1DExportFileFormat(std::format("{}-{}-unbound.{}", prefix, tag, suffix)).exportData(unboundTotal_))
        return false;

    return true;
}

/*
 * Manipulation
 */

// Adjust all partials, adding specified delta to each
void PartialSet::adjust(double delta)
{
    // Full partials
    dissolve::for_each(ParallelPolicies::par, partials_.begin(), partials_.end(),
                       [delta](auto &pair) { pair.second += delta; });

    // Bound partials
    dissolve::for_each(ParallelPolicies::par, boundPartials_.begin(), boundPartials_.end(),
                       [delta](auto &pair) { pair.second += delta; });

    // Unbound partials
    dissolve::for_each(ParallelPolicies::par, unboundPartials_.begin(), unboundPartials_.end(),
                       [delta](auto &pair) { pair.second += delta; });

    // Totals
    total_ += delta;
    boundTotal_ += delta;
    unboundTotal_ += delta;
}

// Add in partials from source PartialSet to our own
void PartialSet::addPartials(PartialSet &source, double weighting)
{
    // Full partials
    for (auto &[key, partial] : source.partials_.map())
        Interpolator::addInterpolated(partial, partials_.map()[key], weighting);

    // Bound partials
    for (auto &[key, partial] : source.boundPartials_.map())
        Interpolator::addInterpolated(partial, boundPartials_.map()[key], weighting);

    // Unbound partials
    for (auto &[key, partial] : source.unboundPartials_.map())
        Interpolator::addInterpolated(partial, unboundPartials_.map()[key], weighting);

    // Add total function
    Interpolator::addInterpolated(source.total_, total_, weighting);
    Interpolator::addInterpolated(source.boundTotal_, boundTotal_, weighting);
    Interpolator::addInterpolated(source.unboundTotal_, unboundTotal_, weighting);
}

/*
 * Operators
 */

void PartialSet::operator+=(const double delta) { adjust(delta); }

void PartialSet::operator+=(const PartialSet &source)
{
    // Full partials
    for (auto &[key, partial] : source.partials_.map())
        partials_.map()[key] += partial;

    // Bound partials
    for (auto &[key, partial] : source.boundPartials_.map())
        boundPartials_.map()[key] += partial;

    // Unbound partials
    for (auto &[key, partial] : source.unboundPartials_.map())
        unboundPartials_.map()[key] += partial;

    // Add total function
    Interpolator::addInterpolated(source.total(), total_);
}

void PartialSet::operator-=(const double delta) { adjust(-delta); }

void PartialSet::operator*=(const double factor)
{
    // Full partials
    dissolve::for_each(ParallelPolicies::par, partials_.begin(), partials_.end(),
                       [factor](auto &pair) { pair.second *= factor; });

    // Bound partials
    dissolve::for_each(ParallelPolicies::par, boundPartials_.begin(), boundPartials_.end(),
                       [factor](auto &pair) { pair.second *= factor; });

    // Unbound partials
    dissolve::for_each(ParallelPolicies::par, unboundPartials_.begin(), unboundPartials_.end(),
                       [factor](auto &pair) { pair.second *= factor; });

    total_ *= factor;
    boundTotal_ *= factor;
    unboundTotal_ *= factor;
}

PartialSet PartialSet::operator*(const double factor) const
{
    auto result = (*this);
    result *= factor;
    return result;
}

/*
 * Searchers
 */

// Return Data1D with specified tag, if it exists
OptionalReferenceWrapper<const Data1D> PartialSet::searchData1D(std::string_view tag) const
{
    auto fullIt =
        std::find_if(partials_.begin(), partials_.end(), [tag](const auto &data) { return data.second.tag() == tag; });
    if (fullIt != partials_.end())
        return fullIt->second;
    auto boundIt = std::find_if(boundPartials_.begin(), boundPartials_.end(),
                                [tag](const auto &data) { return data.second.tag() == tag; });
    if (boundIt != boundPartials_.end())
        return boundIt->second;
    auto unboundIt = std::find_if(unboundPartials_.begin(), unboundPartials_.end(),
                                  [tag](const auto &data) { return data.second.tag() == tag; });
    if (unboundIt != unboundPartials_.end())
        return unboundIt->second;
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

    // Clear partials
    partials_.clear(half_);
    boundPartials_.clear(half_);
    unboundPartials_.clear(half_);

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nPartials = parser.argi(0);

    // Read in individual partials
    for (auto n = 0; n < nPartials; ++n)
    {
        // Read key
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        auto key = parser.args(0);

        if (!partials_.map()[key].deserialise(parser))
            return false;
        if (!boundPartials_.map()[key].deserialise(parser))
            return false;
        if (!unboundPartials_.map()[key].deserialise(parser))
            return false;
    }

    // Read totals
    if (!total_.deserialise(parser))
        return false;
    if (!boundTotal_.deserialise(parser))
        return false;
    if (!unboundTotal_.deserialise(parser))
        return false;

    return true;
}

std::string writeDataPoint(int i, Data1D data)
{
    if (data.valuesHaveErrors())
        return std::format("{} {}", data.value(i), data.error(i));
    else
        return std::format("{}", data.value(i));
}

// Write data through specified LineParser
bool PartialSet::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("'{}'  {}\n", fingerprint_, half_))
        return false;

    // Write out AtomTypes first
    atomTypeMix_.serialise(parser);

    // Write number of keys to expect
    if (!parser.writeLineF("{}\n", partials_.size()))
        return false;

    // Write partials using the full partials as the master key set
    for (const auto &[key, partial] : partials_)
    {
        auto &bound = boundPartials_.map().at(key);
        auto &unbound = unboundPartials_.map().at(key);

        // Write key
        if (!parser.writeLineF("{}\n", key))
            return false;

        if (!partial.serialise(parser))
            return false;
        if (!bound.serialise(parser))
            return false;
        if (!unbound.serialise(parser))
            return false;
    }

    // Write totals
    if (!total_.serialise(parser))
        return false;
    if (!boundTotal_.serialise(parser))
        return false;
    if (!unboundTotal_.serialise(parser))
        return false;

    return true;
}
