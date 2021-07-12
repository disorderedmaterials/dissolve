// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/speciessite.h"
#include "base/lineparser.h"
#include "classes/site.h"
#include "classes/species.h"
#include "data/atomicmasses.h"
#include "templates/algorithms.h"
#include <numeric>

SpeciesSite::SpeciesSite(const Species *parent) : parent_(parent), originMassWeighted_(false) {}

/*
 * Basic Information
 */

// Set name of site
void SpeciesSite::setName(std::string_view newName) { name_ = newName; }

// Return name of site
std::string_view SpeciesSite::name() const { return name_; }

// Return species parent
const Species *SpeciesSite::parent() const { return parent_; }

// Return version
int SpeciesSite::version() const { return version_; }

/*
 * Definition
 */

// Add origin atom
bool SpeciesSite::addOriginAtom(const SpeciesAtom *originAtom)
{
    assert(originAtom);

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(originAtoms_.begin(), originAtoms_.end(), originAtom) != originAtoms_.end())
        return Messenger::error("Origin atom index {} specified twice for site '{}'.\n", originAtom->index(), name_);

    originAtoms_.push_back(originAtom);

    ++version_;

    return true;
}

// Add origin atom from index
bool SpeciesSite::addOriginAtom(int atomIndex)
{
    assert(parent_);
    return addOriginAtom(&parent_->atom(atomIndex));
}

// Set origin atoms
bool SpeciesSite::setOriginAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    originAtoms_.clear();

    ++version_;

    for (auto *i : atoms)
        if (!addOriginAtom(i))
        {
            originAtoms_.clear();
            return false;
        }

    return true;
}

// Return integer array of indices from which the origin should be formed
std::vector<int> SpeciesSite::originAtomIndices() const
{
    std::vector<int> indices;
    std::transform(originAtoms_.begin(), originAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

// Set whether the origin should be calculated with mass-weighted positions
void SpeciesSite::setOriginMassWeighted(bool b)
{
    originMassWeighted_ = b;

    ++version_;
}

// Return whether the origin should be calculated with mass-weighted positions
bool SpeciesSite::originMassWeighted() const { return originMassWeighted_; }

// Add x-axis atom
bool SpeciesSite::addXAxisAtom(const SpeciesAtom *xAxisAtom)
{
    assert(xAxisAtom);

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(xAxisAtoms_.begin(), xAxisAtoms_.end(), xAxisAtom) != xAxisAtoms_.end())
        return Messenger::error("X-axis atom index {} specified twice for site '{}'.\n", xAxisAtom->index(), name_);

    xAxisAtoms_.push_back(xAxisAtom);

    ++version_;

    return true;
}

// Add x-axis atom from index
bool SpeciesSite::addXAxisAtom(int atomIndex)
{
    assert(parent_);
    return addXAxisAtom(&parent_->atom(atomIndex));
}

// Set x-axis atoms
bool SpeciesSite::setXAxisAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    xAxisAtoms_.clear();

    ++version_;

    if (!std::all_of(atoms.begin(), atoms.end(), [&](const auto *i) { return addXAxisAtom(i); }))
    {
        xAxisAtoms_.clear();
        return false;
    }

    return true;
}

// Return integer array of indices from which x-axis should be formed
std::vector<int> SpeciesSite::xAxisAtomIndices() const
{
    std::vector<int> indices;
    std::transform(xAxisAtoms_.begin(), xAxisAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

// Add y-axis atom
bool SpeciesSite::addYAxisAtom(const SpeciesAtom *yAxisAtom)
{
    assert(yAxisAtom);

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(yAxisAtoms_.begin(), yAxisAtoms_.end(), yAxisAtom) != yAxisAtoms_.end())
        return Messenger::error("Y-axis atom index {} specified twice for site '{}'.\n", yAxisAtom->index(), name_);

    yAxisAtoms_.push_back(yAxisAtom);

    ++version_;

    return true;
}

// Add y-axis atom from index
bool SpeciesSite::addYAxisAtom(int atomIndex)
{
    assert(parent_);
    return addYAxisAtom(&parent_->atom(atomIndex));
}

// Set y-axis atoms
bool SpeciesSite::setYAxisAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    yAxisAtoms_.clear();

    ++version_;

    if (!std::all_of(atoms.begin(), atoms.end(), [&](const auto *i) { return addYAxisAtom(i); }))
    {
        yAxisAtoms_.clear();
        return false;
    }

    return true;
}

// Return integer array of indices from which y-axis should be formed
std::vector<int> SpeciesSite::yAxisAtomIndices() const
{
    std::vector<int> indices;
    std::transform(yAxisAtoms_.begin(), yAxisAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

// Return whether the site has defined axes sites
bool SpeciesSite::hasAxes() const { return !(xAxisAtoms_.empty() || yAxisAtoms_.empty()); }

/*
 * Generation from Parent
 */

// Create and return Site description from parent Species
Site *SpeciesSite::createFromParent() const
{
    // Get origin atom indices from site
    std::vector<int> originIndices = originAtomIndices();
    if (originIndices.empty())
        return nullptr;

    Site *site = nullptr;

    // Calculate origin
    Vec3<double> origin;
    double mass;
    if (originMassWeighted_)
    {
        double massNorm = 0.0;
        for (const auto &index : originIndices)
        {
            mass = AtomicMass::mass(parent_->atom(index).Z());
            origin += parent_->atom(index).r() * mass;
            massNorm += mass;
        }
        origin /= massNorm;
    }
    else
    {
        for (const auto &index : originIndices)
            origin += parent_->atom(index).r();
        origin /= originIndices.size();
    }

    // Calculate axes and store data if required
    if (hasAxes())
    {
        // If the site has axes, grab the atom indices involved
        std::vector<int> xAxisIndices, yAxisIndices;
        if (hasAxes())
        {
            xAxisIndices = xAxisAtomIndices();
            yAxisIndices = yAxisAtomIndices();
        }

        Vec3<double> v;

        // Get average position of supplied x-axis atoms
        for (const auto &index : xAxisIndices)
            v += parent_->atom(index).r();
        v /= xAxisIndices.size();

        // Get vector from site origin and normalise it
        auto x = v - origin;
        x.normalise();

        // Get average position of supplied y-axis atoms
        v.zero();
        for (const auto &index : yAxisIndices)
            v += parent_->atom(index).r();
        v /= yAxisIndices.size();

        // Get vector from site origin, normalise it, and orthogonalise
        auto y = v - origin;
        y.orthogonalise(x);
        y.normalise();

        // Calculate z vector from cross product of x and y
        Vec3<double> z = x * y;

        // Store data
        site = new OrientedSite(nullptr, origin, x, y, z);
    }
    else
        site = new Site(nullptr, origin);

    return site;
}

/*
 * Read / Write
 */

// Return enum option info for SiteKeyword
EnumOptions<SpeciesSite::SiteKeyword> SpeciesSite::keywords()
{
    return EnumOptions<SpeciesSite::SiteKeyword>("SiteKeyword",
                                                 {{SpeciesSite::EndSiteKeyword, "EndSite"},
                                                  {SpeciesSite::OriginKeyword, "Origin", OptionArguments::OneOrMore},
                                                  {SpeciesSite::OriginMassWeightedKeyword, "OriginMassWeighted", 1},
                                                  {SpeciesSite::XAxisKeyword, "XAxis", OptionArguments::OneOrMore},
                                                  {SpeciesSite::YAxisKeyword, "YAxis", OptionArguments::OneOrMore}});
}

// Read site definition from specified LineParser
bool SpeciesSite::read(LineParser &parser)
{
    Messenger::printVerbose("\nReading information for Site '{}'...\n", name());

    auto blockDone = false, error = false;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
            return keywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            case (SpeciesSite::EndSiteKeyword):
                Messenger::print("Found end of Site '{}'.\n", name());
                blockDone = true;
                break;
            case (SpeciesSite::OriginKeyword):
                for (auto n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addOriginAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add origin atom for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            case (SpeciesSite::OriginMassWeightedKeyword):
                setOriginMassWeighted(parser.argb(1));
                break;
            case (SpeciesSite::XAxisKeyword):
                for (int n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addXAxisAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add x-axis atom for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            case (SpeciesSite::YAxisKeyword):
                for (int n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addYAxisAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add y-axis atom for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            default:
                Messenger::error("Site block keyword '{}' not accounted for.\n", keywords().keyword(kwd));
                error = true;
                break;
        }

        // Error encountered?
        if (error)
            break;

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no error and the blockDone flag isn't set, return an error
    if (!error && !blockDone)
    {
        Messenger::error("Unterminated Site block found.\n");
        error = true;
    }

    return (!error);
}

// Write site definition to specified LineParser
bool SpeciesSite::write(LineParser &parser, std::string_view prefix)
{
    // Write start of site definition
    if (!parser.writeLineF("{}Site  '{}'\n", prefix, name()))
        return false;

    // Origin atom indices
    if (!originAtoms_.empty())
    {
        if (!parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(OriginKeyword),
                               joinStrings(originAtomIndices(), "  ", [](const auto i) { return i + 1; })))
            return false;
    }

    // Origin mass weighted?
    if (originMassWeighted_ && (!parser.writeLineF("{}  {}  True\n", prefix, keywords().keyword(OriginMassWeightedKeyword))))
        return false;

    // X-Axis atom indices
    if (!xAxisAtoms_.empty())
    {
        if (!parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(XAxisKeyword),
                               joinStrings(xAxisAtomIndices(), "  ", [](const auto i) { return i + 1; })))
            return false;
    }

    // Y-Axis atom indices
    if (!yAxisAtoms_.empty())
    {
        if (!parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(YAxisKeyword),
                               joinStrings(yAxisAtomIndices(), "  ", [](const auto i) { return i + 1; })))
            return false;
    }

    // Write start of site definition
    if (!parser.writeLineF("{}{}\n", prefix, keywords().keyword(EndSiteKeyword)))
        return false;

    return true;
}
