// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/partialSetAccumulator.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include <format>

void PartialSetAccumulator::operator+=(const PartialSet &source)
{
    // If this is the first accumulation, initialise our maps with the "mirrored" state of the source
    if (nAccumulated_ == 0)
    {
        partials_.clear(source.partials().mirroredAreEquivalent());
        boundPartials_.clear(source.boundPartials().mirroredAreEquivalent());
        unboundPartials_.clear(source.unboundPartials().mirroredAreEquivalent());
    }

    // Full partials
    for (auto &[key, data] : source.partials())
    {
        partials_.map()[key] += data;
        partials_.map()[key].setTag(data.tag());
    }

    // Bound partials
    for (auto &[key, data] : source.boundPartials())
    {
        boundPartials_.map()[key] += data;
        boundPartials_.map()[key].setTag(data.tag());
    }

    // Unbound partials
    for (auto &[key, data] : source.unboundPartials())
    {
        unboundPartials_.map()[key] += data;
        unboundPartials_.map()[key].setTag(data.tag());
    }

    // Total
    total_.setTag(source.total().tag());
    total_ += source.total();

    ++nAccumulated_;
}

/*
 * Partials Data
 */

// Return number of accumulated points
int PartialSetAccumulator::nAccumulated() const { return nAccumulated_; }

// Return full matrix, containing sampling of full atom-atom partial
const DoubleKeyedMap<SampledData1D> &PartialSetAccumulator::partials() const { return partials_; }

// Return bound matrix, containing sampling of atom-atom partial of bound pairs
const DoubleKeyedMap<SampledData1D> &PartialSetAccumulator::boundPartials() const { return boundPartials_; }

// Return unbound matrix, containing sampling of atom-atom partial of unbound pairs
const DoubleKeyedMap<SampledData1D> &PartialSetAccumulator::unboundPartials() const { return unboundPartials_; }

// Return the sampled total function
const SampledData1D &PartialSetAccumulator::total() const { return total_; }

// Save all partials and total (with errors)
bool PartialSetAccumulator::save(std::string_view prefix, std::string_view tag, std::string_view suffix,
                                 std::string_view abscissaUnits) const
{
    assert(!prefix.empty());

    LineParser parser;

    // Write partials
    for (auto &[key, fullPartial] : partials_)
    {
        // Locate the corresponding bound and unbound partials
        if (!boundPartials_.map().contains(key))
            return Messenger::error("No bound partial {} exists in the accumulated partials.\n", key);
        if (!unboundPartials_.map().contains(key))
            return Messenger::error("No unbound partial {} exists in the accumulated partials.\n", key);
        auto &boundPartial = boundPartials_.map().at(key);
        auto &unboundPartial = unboundPartials_.map().at(key);

        // Open file and check that we're OK to proceed writing to it
        std::string filename{std::format("{}-{}-{}.{}", prefix, tag, DissolveSys::niceName(fullPartial.tag()), suffix)};
        Messenger::printVerbose("Writing partial file '{}'...\n", filename);

        parser.openOutput(filename, true);
        if (!parser.isFileGoodForWriting())
            return Messenger::error("Couldn't open file '{}' for writing.\n", filename);

        parser.writeLineF("# {:<14}  {:<16}  {:<16}  {:<16}  {:<16}  {:<16}  {:<16}\n", abscissaUnits, "Full", "Error", "Bound",
                          "Error", "Unbound", "Error");
        for (auto n = 0; n < fullPartial.nValues(); ++n)
            parser.writeLineF("{:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", fullPartial.xAxis(n),
                              fullPartial.value(n), fullPartial.error(n), boundPartial.value(n), boundPartial.error(n),
                              unboundPartial.value(n), unboundPartial.error(n));
        parser.closeFiles();
    }

    Data1DExportFileFormat exportFormat(std::format("{}-{}-total.{}", prefix, tag, suffix));
    Messenger::printVerbose("Writing total file '{}'...\n", exportFormat.filename());
    return exportFormat.exportData(total_);
}

/*
 * Searchers
 */

// Return SampledData1D with specified tag, if it exists
OptionalReferenceWrapper<const SampledData1D> PartialSetAccumulator::searchSampledData1D(std::string_view tag) const
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
    return {};
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool PartialSetAccumulator::deserialise(LineParser &parser)
{
    // Read size and number of accumulated data
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    const auto nPartials = parser.argi(0);
    nAccumulated_ = parser.argi(1);
    auto mirroredEquivalent = parser.argb(2);

    // Clear data
    partials_.clear(mirroredEquivalent);
    boundPartials_.clear(mirroredEquivalent);
    unboundPartials_.clear(mirroredEquivalent);

    if (nAccumulated_ == 0)
        return true;

    // Read total function
    if (!total_.deserialise(parser))
        return false;

    // Read in individual partials
    for (auto n = 0; n < nPartials; ++n)
    {
        // Read key
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        auto key = parser.args(0);

        // Read full partial
        auto &partial = partials_.map()[key];
        partial.initialise(total_.xAxis());
        if (!partial.serialiseValues(parser))
            return false;

        // Read bound partial
        auto &boundPartial = boundPartials_.map()[key];
        boundPartial.initialise(total_.xAxis());
        if (!boundPartial.serialiseValues(parser))
            return false;

        // Read unbound partial
        auto &unboundPartial = unboundPartials_.map()[key];
        unboundPartial.initialise(total_.xAxis());
        if (!unboundPartial.serialiseValues(parser))
            return false;
    }

    return true;
}

// Write data through specified LineParser
bool PartialSetAccumulator::serialise(LineParser &parser) const
{
    // Write size information
    if (!parser.writeLineF("{} {} {} # nPartials, nAccumulated, mirroredEquivalent\n", partials_.size(), nAccumulated_,
                           partials_.mirroredAreEquivalent()))
        return false;
    if (nAccumulated_ == 0)
        return true;

    // Write the total function first (abscissa and values)
    if (!total_.serialise(parser))
        return false;

    // Write partials
    for (auto &[key, fullPartial] : partials_)
    {
        // Locate the corresponding bound and unbound partials
        if (!boundPartials_.map().contains(key))
            return Messenger::error("No bound partial exists in the accumulated partials.\n", key);
        if (!unboundPartials_.map().contains(key))
            return Messenger::error("No unbound partial {} exists in the accumulated partials.\n", key);

        auto &partial = partials_.map().at(key);
        auto &boundPartial = boundPartials_.map().at(key);
        auto &unboundPartial = unboundPartials_.map().at(key);

        if (!parser.writeLineF("{}\n", key))
            return false;
        if (!partial.serialiseValues(parser))
            return false;
        if (!boundPartial.serialiseValues(parser))
            return false;
        if (!unboundPartial.serialiseValues(parser))
            return false;
    }

    return true;
}
