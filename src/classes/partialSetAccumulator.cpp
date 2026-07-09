// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/partialSetAccumulator.h"
#include "base/sysFunc.h"
#include "nodes/exportData.h"
#include <format>

void PartialSetAccumulator::operator+=(const PartialSet &source)
{
    // If this is the first accumulation, initialise our maps with the "mirrored" state of the source
    if (nAccumulated_ == 0)
    {
        partials_ = DoubleKeyedMap<SampledData1D>(source.partials().mirroredAreEquivalent());
        boundPartials_ = DoubleKeyedMap<SampledData1D>(source.boundPartials().mirroredAreEquivalent());
        unboundPartials_ = DoubleKeyedMap<SampledData1D>(source.unboundPartials().mirroredAreEquivalent());
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

        std::ofstream outfile(filename, std::ios::out);
        std::ostreambuf_iterator<char> out(outfile);

        if (!outfile)
            return Messenger::error("Couldn't open file '{}' for writing.\n", filename);

        std::format_to(out, "# {:<14}  {:<16}  {:<16}  {:<16}  {:<16}  {:<16}  {:<16}\n", abscissaUnits, "Full", "Error",
                       "Bound", "Error", "Unbound", "Error");
        for (auto n = 0; n < fullPartial.nValues(); ++n)
            std::format_to(out, "{:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", fullPartial.xAxis(n),
                           fullPartial.value(n), fullPartial.error(n), boundPartial.value(n), boundPartial.error(n),
                           unboundPartial.value(n), unboundPartial.error(n));
        outfile.close();
    }

    return ExportDataNode::write(total_, std::format("{}-{}-total.{}", prefix, tag, suffix));
}
