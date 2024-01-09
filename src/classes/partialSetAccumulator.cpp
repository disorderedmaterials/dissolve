// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/partialSetAccumulator.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "templates/algorithms.h"

PartialSetAccumulator::PartialSetAccumulator() {}

void PartialSetAccumulator::operator+=(const PartialSet &source)
{
    const auto n = source.nAtomTypes();

    // If there are no data accumulated yet, initialise our data structures
    if (nAccumulated_ == 0)
    {
        // Initialise the arrays
        partials_.initialise(n, n, true);
        boundPartials_.initialise(n, n, true);
        unboundPartials_.initialise(n, n, true);
        total_.clear();

        // Copy tags (for retrieval and sanity checking purposes)
        dissolve::for_each_pair(ParallelPolicies::par, 0, n,
                                [&](auto i, auto j)
                                {
                                    partials_[{i, j}].setTag(source.partial(i, j).tag());
                                    boundPartials_[{i, j}].setTag(source.boundPartial(i, j).tag());
                                    unboundPartials_[{i, j}].setTag(source.unboundPartial(i, j).tag());
                                });
        total_.setTag(source.total().tag());
    }

    assert(n == partials_.nRows());

    // Accumulate the data, ensuring tags are identical - if not, we really don't want to be blindly accumulating
    dissolve::for_each_pair(ParallelPolicies::par, 0, n,
                            [&](auto i, auto j)
                            {
                                // Full partials
                                if (partials_[{i, j}].tag() != source.partial(i, j).tag())
                                    throw(std::runtime_error(fmt::format(
                                        "Can't accumulate PartialSet data as the data tags are mismatched ('{}' vs '{}').\n",
                                        partials_[{i, j}].tag(), source.partial(i, j).tag())));
                                partials_[{i, j}] += source.partial(i, j);

                                // Bound partials
                                if (boundPartials_[{i, j}].tag() != source.boundPartial(i, j).tag())
                                    throw(std::runtime_error(fmt::format(
                                        "Can't accumulate PartialSet data as the data tags are mismatched ('{}' vs '{}').\n",
                                        boundPartials_[{i, j}].tag(), source.boundPartial(i, j).tag())));
                                boundPartials_[{i, j}] += source.boundPartial(i, j);

                                // Unbound partials
                                if (unboundPartials_[{i, j}].tag() != source.unboundPartial(i, j).tag())
                                    throw(std::runtime_error(fmt::format(
                                        "Can't accumulate PartialSet data as the data tags are mismatched ('{}' vs '{}').\n",
                                        unboundPartials_[{i, j}].tag(), source.unboundPartial(i, j).tag())));
                                unboundPartials_[{i, j}] += source.unboundPartial(i, j);
                            });

    total_ += source.total();

    ++nAccumulated_;
}

/*
 * Partials Data
 */

// Return number of accumulated points
int PartialSetAccumulator::nAccumulated() const { return nAccumulated_; }

// Return full matrix, containing sampling of full atom-atom partial
const Array2D<SampledData1D> &PartialSetAccumulator::partials() const { return partials_; }

// Return bound matrix, containing sampling of atom-atom partial of bound pairs
const Array2D<SampledData1D> &PartialSetAccumulator::boundPartials() const { return boundPartials_; }

// Return unbound matrix, containing sampling of atom-atom partial of unbound pairs
const Array2D<SampledData1D> &PartialSetAccumulator::unboundPartials() const { return unboundPartials_; }

// Return the sampled total function
const SampledData1D &PartialSetAccumulator::total() const { return total_; }

// Save all partials and total (with errors)
bool PartialSetAccumulator::save(std::string_view prefix, std::string_view tag, std::string_view suffix,
                                 std::string_view abscissaUnits) const
{
    assert(!prefix.empty());

    LineParser parser;

    // Write partials
    for (auto &&[full, bound, unbound] : zip(partials_, boundPartials_, unboundPartials_))
    {
        // Open file and check that we're OK to proceed writing to it
        std::string filename{fmt::format("{}-{}-{}.{}", prefix, tag, DissolveSys::niceName(full.tag()), suffix)};
        Messenger::printVerbose("Writing partial file '{}'...\n", filename);

        parser.openOutput(filename, true);
        if (!parser.isFileGoodForWriting())
            return Messenger::error("Couldn't open file '{}' for writing.\n", filename);

        parser.writeLineF("# {:<14}  {:<16}  {:<16}  {:<16}  {:<16}  {:<16}  {:<16}\n", abscissaUnits, "Full", "Error", "Bound",
                          "Error", "Unbound", "Error");
        for (auto n = 0; n < full.nValues(); ++n)
            parser.writeLineF("{:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", full.xAxis(n),
                              full.value(n), full.error(n), bound.value(n), bound.error(n), unbound.value(n), unbound.error(n));
        parser.closeFiles();
    }

    Data1DExportFileFormat exportFormat(fmt::format("{}-{}-total.{}", prefix, tag, suffix));
    Messenger::printVerbose("Writing total file '{}'...\n", exportFormat.filename());
    return exportFormat.exportData(total_);
}

/*
 * Searchers
 */

// Return SampledData1D with specified tag, if it exists
OptionalReferenceWrapper<const SampledData1D> PartialSetAccumulator::searchSampledData1D(std::string_view tag) const
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

// Read array through specified LineParser
bool PartialSetAccumulator::deserialiseArray(LineParser &parser, Array2D<SampledData1D> &array,
                                             const std::vector<double> &xAxis)
{
    for (auto &partial : array)
    {
        // Read tag
        if (parser.readNextLine(LineParser::Defaults) != LineParser::Success)
            return false;
        partial.setTag(parser.line());
        // Initialise from existing abscissa vector and read in sampled values
        partial.initialise(xAxis);
        if (!partial.deserialiseValues(parser))
            return false;
    }

    return true;
}

// Write array through specified LineParser
bool PartialSetAccumulator::serialiseArray(LineParser &parser, const Array2D<SampledData1D> &array)
{
    for (auto &partial : array)
    {
        if (!parser.writeLine(partial.tag()))
            return false;
        if (!partial.serialiseValues(parser))
            return false;
    }

    return true;
}

// Read data through specified LineParser
bool PartialSetAccumulator::deserialise(LineParser &parser)
{
    // Read size and number of accumulated data
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto n = parser.argi(0);
    nAccumulated_ = parser.argi(1);
    if (nAccumulated_ == 0)
        return true;

    // Read total function
    if (!total_.deserialise(parser))
        return false;

    // Initialise the arrays
    partials_.initialise(n, n, true);
    boundPartials_.initialise(n, n, true);
    unboundPartials_.initialise(n, n, true);

    // Read in partials (copy abscissa from total_)
    if (!deserialiseArray(parser, partials_, total_.xAxis()))
        return false;
    if (!deserialiseArray(parser, boundPartials_, total_.xAxis()))
        return false;
    if (!deserialiseArray(parser, unboundPartials_, total_.xAxis()))
        return false;

    return true;
}

// Write data through specified LineParser
bool PartialSetAccumulator::serialise(LineParser &parser) const
{
    // Write array sizes and nAccumulated
    if (!parser.writeLineF("{} {}  # nRows/nColumns, nAccumulated\n", partials_.nRows(), nAccumulated_))
        return false;
    if (nAccumulated_ == 0)
        return true;

    // Write the total function first (abscissa and values)
    if (!total_.serialise(parser))
        return false;

    // Write partials (tags and values only)
    if (!serialiseArray(parser, partials_))
        return false;
    if (!serialiseArray(parser, boundPartials_))
        return false;
    if (!serialiseArray(parser, unboundPartials_))
        return false;

    return true;
}
