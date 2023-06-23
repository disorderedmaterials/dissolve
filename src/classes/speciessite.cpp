// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/speciessite.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "classes/site.h"
#include "classes/species.h"
#include "data/atomicmasses.h"
#include "neta/matchedgroup.h"
#include "neta/neta.h"
#include "neta/neta.h"
#include "templates/algorithms.h"
#include <numeric>
#include <string_view>

SpeciesSite::SpeciesSite(const Species *parent, SiteType type) : parent_(parent), type_(type), originMassWeighted_(false) {}
SpeciesSite::SpeciesSite(const Species *parent, std::string name, SiteType type)
    : name_(name), parent_(parent), type_(type), originMassWeighted_(false)
{
}

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
 * Basic Definition
 */

// Clear definition data from site
void SpeciesSite::clearDefinition()
{
    staticOriginAtoms_.clear();
    staticXAxisAtoms_.clear();
    staticYAxisAtoms_.clear();
    dynamicElements_.clear();
    dynamicAtomTypes_.clear();
    sitesOriginAtomsIndices_.clear();
    sitesXAxisAtomsIndices_.clear();
    sitesYAxisAtomsIndices_.clear();
    fragment_ = NETADefinition{};

    ++version_;
}

// Set type of site
void SpeciesSite::setType(SpeciesSite::SiteType type)
{
    type_ = type;

    clearDefinition();
}

// Return type of site
SpeciesSite::SiteType SpeciesSite::type() const { return type_; }

// Return whether the site has defined axes sites
bool SpeciesSite::hasAxes() const
{
    return (type_ == SiteType::Static && !(staticXAxisAtoms_.empty() && staticYAxisAtoms_.empty())) ||
           (type_ == SiteType::Fragment && fragment_.hasAxes());
}

/*
 * Static Site Definition
 */

// Add origin atom
bool SpeciesSite::addStaticOriginAtom(const SpeciesAtom *originAtom)
{
    assert(originAtom);

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting origin atoms for a non-static site is not permitted.\n");

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(staticOriginAtoms_.begin(), staticOriginAtoms_.end(), originAtom) != staticOriginAtoms_.end())
        return Messenger::error("Origin atom index {} specified twice for site '{}'.\n", originAtom->index(), name_);

    staticOriginAtoms_.push_back(originAtom);

    ++version_;

    return generateUniqueSites();
}

// Add origin atom from index
bool SpeciesSite::addStaticOriginAtom(int atomIndex)
{
    assert(parent_);

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting origin atoms for a non-static site is not permitted.\n");

    return addStaticOriginAtom(&parent_->atom(atomIndex));
}

// Set origin atoms
bool SpeciesSite::setStaticOriginAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting origin atoms for a non-static site is not permitted.\n");

    staticOriginAtoms_.clear();

    ++version_;

    for (auto *i : atoms)
        if (!addStaticOriginAtom(i))
        {
            staticOriginAtoms_.clear();
            return false;
        }

    return generateUniqueSites();
}

// Return origin atom vector
const std::vector<const SpeciesAtom *> &SpeciesSite::staticOriginAtoms() const { return staticOriginAtoms_; }

// Return integer array of indices from which the origin should be formed
std::vector<int> SpeciesSite::staticOriginAtomIndices() const
{
    std::vector<int> indices;
    std::transform(staticOriginAtoms_.begin(), staticOriginAtoms_.end(), std::back_inserter(indices),
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
bool SpeciesSite::addStaticXAxisAtom(const SpeciesAtom *xAxisAtom)
{
    assert(xAxisAtom);

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static site is not permitted.\n");

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(staticXAxisAtoms_.begin(), staticXAxisAtoms_.end(), xAxisAtom) != staticXAxisAtoms_.end())
        return Messenger::error("X-axis atom index {} specified twice for site '{}'.\n", xAxisAtom->index(), name_);

    staticXAxisAtoms_.push_back(xAxisAtom);

    ++version_;

    return generateUniqueSites();
}

// Add x-axis atom from index
bool SpeciesSite::addStaticXAxisAtom(int atomIndex)
{
    assert(parent_);

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static site is not permitted.\n");

    return addStaticXAxisAtom(&parent_->atom(atomIndex));
}

// Set x-axis atoms
bool SpeciesSite::setStaticXAxisAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static site is not permitted.\n");

    staticXAxisAtoms_.clear();

    ++version_;

    if (!std::all_of(atoms.begin(), atoms.end(), [&](const auto *i) { return addStaticXAxisAtom(i); }))
    {
        staticXAxisAtoms_.clear();
        return false;
    }

    return generateUniqueSites();
}

// Return x-axis atom vector
const std::vector<const SpeciesAtom *> &SpeciesSite::staticXAxisAtoms() const { return staticXAxisAtoms_; }

// Return integer array of indices from which x-axis should be formed
std::vector<int> SpeciesSite::staticXAxisAtomIndices() const
{
    std::vector<int> indices;
    std::transform(staticXAxisAtoms_.begin(), staticXAxisAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

// Add y-axis atom
bool SpeciesSite::addStaticYAxisAtom(const SpeciesAtom *yAxisAtom)
{
    assert(yAxisAtom);

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static site is not permitted.\n");

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(staticYAxisAtoms_.begin(), staticYAxisAtoms_.end(), yAxisAtom) != staticYAxisAtoms_.end())
        return Messenger::error("Y-axis atom index {} specified twice for site '{}'.\n", yAxisAtom->index(), name_);

    staticYAxisAtoms_.push_back(yAxisAtom);

    ++version_;

    return generateUniqueSites();
}

// Add y-axis atom from index
bool SpeciesSite::addStaticYAxisAtom(int atomIndex)
{
    assert(parent_);

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static site is not permitted.\n");

    return addStaticYAxisAtom(&parent_->atom(atomIndex));
}

// Set y-axis atoms
bool SpeciesSite::setStaticYAxisAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static site is not permitted.\n");

    staticYAxisAtoms_.clear();

    ++version_;

    if (!std::all_of(atoms.begin(), atoms.end(), [&](const auto *i) { return addStaticYAxisAtom(i); }))
    {
        staticYAxisAtoms_.clear();
        return false;
    }

    return generateUniqueSites();
}

// Return y-axis atom vector
const std::vector<const SpeciesAtom *> &SpeciesSite::staticYAxisAtoms() const { return staticYAxisAtoms_; }

// Return integer array of indices from which y-axis should be formed
std::vector<int> SpeciesSite::staticYAxisAtomIndices() const
{
    std::vector<int> indices;
    std::transform(staticYAxisAtoms_.begin(), staticYAxisAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

/*
 * Dynamic Site Definition
 */

// Add target Elements for selection as sites
bool SpeciesSite::addDynamicElement(Elements::Element el)
{
    if (type_ != SiteType::Dynamic)
        return Messenger::error("Setting element targets for a non-dynamic site is not permitted.\n");

    if (std::find(dynamicElements_.begin(), dynamicElements_.end(), el) != dynamicElements_.end())
        return Messenger::error("Element '{}' is already defined as a target for site '{}'.\n", Elements::symbol(el), name_);

    dynamicElements_.push_back(el);

    return generateUniqueSites();
}

// Set target Elements for selection as sites
bool SpeciesSite::setDynamicElements(const std::vector<Elements::Element> &els)
{
    dynamicElements_.clear();

    if (std::all_of(els.begin(), els.end(), [&](const auto el) { return addDynamicElement(el); }))
        return generateUniqueSites();
    else
        return false;
}

// Return elements for selection as sites
const std::vector<Elements::Element> SpeciesSite::dynamicElements() const { return dynamicElements_; }

// Add target atom type for selection as sites
bool SpeciesSite::addDynamicAtomType(const std::shared_ptr<AtomType> &at)
{
    if (type_ != SiteType::Dynamic)
        return Messenger::error("Setting atom type targets for a non-dynamic site is not permitted.\n");

    if (std::find(dynamicAtomTypes_.begin(), dynamicAtomTypes_.end(), at) != dynamicAtomTypes_.end())
        return Messenger::error("Atom type '{}' is already defined as a target for site '{}'.\n", at->name(), name_);

    dynamicAtomTypes_.push_back(at);

    return generateUniqueSites();
}

// Set target atom types for selection as sites
bool SpeciesSite::setDynamicAtomTypes(const std::vector<std::shared_ptr<AtomType>> &types)
{
    dynamicAtomTypes_.clear();

    if (std::all_of(types.begin(), types.end(), [&](const auto &at) { return addDynamicAtomType(at); }))
        return generateUniqueSites();
    else
        return false;
}

// Return atom types for selection as sites
const std::vector<std::shared_ptr<AtomType>> &SpeciesSite::dynamicAtomTypes() const { return dynamicAtomTypes_; }

// Return fragment definition
const NETADefinition &SpeciesSite::fragment() const { return fragment_; }

// Generate unique sites
bool SpeciesSite::generateUniqueSites()
{
    sitesOriginAtomsIndices_.clear();
    sitesXAxisAtomsIndices_.clear();
    sitesYAxisAtomsIndices_.clear();

    switch (type_)
    {
        case (SiteType::Static):
        {
            sitesOriginAtomsIndices_.push_back(staticOriginAtomIndices());
            sitesXAxisAtomsIndices_.push_back(staticXAxisAtomIndices());
            sitesYAxisAtomsIndices_.push_back(staticYAxisAtomIndices());
        }
        case (SiteType::Dynamic):
        {
            for (auto &i : parent_->atoms())
            {
                // Valid element or atom type?
                if ((std::find(dynamicElements_.begin(), dynamicElements_.end(), i.Z()) != dynamicElements_.end()) ||
                    std::find(dynamicAtomTypes_.begin(), dynamicAtomTypes_.end(), i.atomType()) != dynamicAtomTypes_.end())
                    sitesOriginAtomsIndices_.push_back({i.index()});
            }
            return true;
        }
        case (SiteType::Fragment):
        {
            std::vector<std::vector<int>> matchedIndices;
            for (auto &i : parent_->atoms())
            {
                if (fragment_.matches(&i))
                {
                    // Determine the path of matched atoms - i.e. the atoms in the fragment.
                    auto matchedGroup = fragment_.matchedPath(&i);
                    auto matchedAtoms = matchedGroup.set();

                    // Create vector of indices of the matched atoms.
                    std::vector<int> matchedAtomIndices(matchedAtoms.size());
                    std::transform(matchedAtoms.begin(), matchedAtoms.end(), matchedAtomIndices.begin(),
                                   [](const auto &atom) { return atom->index(); });

                    // Check if the fragment we have found is unique.
                    std::sort(matchedAtomIndices.begin(), matchedAtomIndices.end());
                    if (std::find(matchedIndices.begin(), matchedIndices.end(), matchedAtomIndices) != matchedIndices.end())
                        continue;

                    // If it's unique, remember it and proceed.
                    matchedIndices.push_back(std::move(matchedAtomIndices));

                    auto identifiers = matchedGroup.identifiers();

                    // Determine origin atoms
                    sitesOriginAtomsIndices_.emplace_back(identifiers["origin"].size());
                    std::transform(identifiers["origin"].begin(), identifiers["origin"].end(),
                                   sitesOriginAtomsIndices_.back().begin(), [](const auto &atom) { return atom->index(); });
                    if (hasAxes())
                    {
                        // Determine x axis atoms.
                        sitesXAxisAtomsIndices_.emplace_back(identifiers["x"].size());
                        std::transform(identifiers["x"].begin(), identifiers["x"].end(), sitesXAxisAtomsIndices_.back().begin(),
                                       [](const auto &atom) { return atom->index(); });
                        // Determine y axis atoms.
                        sitesYAxisAtomsIndices_.emplace_back(identifiers["y"].size());
                        std::transform(identifiers["y"].begin(), identifiers["y"].end(), sitesYAxisAtomsIndices_.back().begin(),
                                       [](const auto &atom) { return atom->index(); });
                    }
                }
            }
            return true;
        }
        default:
        {
            Messenger::print("Can't generate unique sites for site '{}'.", name());
            return false;
        }
    }
}

// Return number of unique sites
const int SpeciesSite::nSites() const { return sitesOriginAtomsIndices_.size(); }
// Return atom indices contributing to unique site origins
const std::vector<std::vector<int>> &SpeciesSite::sitesOriginAtomsIndices() const { return sitesOriginAtomsIndices_; }
// Return atom indices indicating the x axis with the origins of unique sites.
const std::vector<std::vector<int>> &SpeciesSite::sitesXAxisAtomsIndices() const { return sitesXAxisAtomsIndices_; }
// Return atom indices indicating the y axis with the origins of unique sites.
const std::vector<std::vector<int>> &SpeciesSite::sitesYAxisAtomsIndices() const { return sitesYAxisAtomsIndices_; }

// Return fragment definition
const NETADefinition &SpeciesSite::fragment() const { return fragment_; }

// Update fragment definition
bool SpeciesSite::setFragmentDefinitionString(std::string_view definitionString) { return fragment_.create(definitionString); }

/*
 * Generation from Parent
 */

// Create and return site description from parent Species
std::vector<std::shared_ptr<Site>> SpeciesSite::createFromParent() const
{
    std::vector<std::shared_ptr<Site>> sites;

    auto originAtomsIndices = sitesOriginAtomsIndices();
    auto xAxisAtomsIndices = sitesXAxisAtomsIndices();
    auto yAxisAtomsIndices = sitesYAxisAtomsIndices();

    for (auto i = 0; i < nSites(); ++i)
    {
        Vec3<double> origin, x, y, z;
        if (originMassWeighted_)
        {
            auto massNorm = 0.0;
            for (const auto &idx : originAtomsIndices.at(i))
            {
                auto mass = AtomicMass::mass(parent_->atom(idx).Z());
                origin += parent_->atom(idx).r() * mass;
                massNorm += mass;
            }
            origin /= massNorm;
        }
        else
        {
            for (const auto &idx : originAtomsIndices.at(i))
                origin += parent_->atom(idx).r();
            origin /= originAtomsIndices.at(i).size();
        }

        if (hasAxes())
        {
            Vec3<double> v;
            for (const auto &idx : xAxisAtomsIndices.at(i))
                v += parent_->atom(idx).r();
            v /= xAxisAtomsIndices.at(i).size();

            auto x = v - origin;
            x.normalise();

            v.zero();
            for (const auto &idx : yAxisAtomsIndices.at(i))
                v += parent_->atom(idx).r();
            v /= yAxisAtomsIndices.at(i).size();

            auto y = v - origin;
            y.orthogonalise(x);
            y.normalise();

            sites.push_back(std::make_shared<OrientedSite>(nullptr, origin, x, y, x * y));
        }
        else
            sites.push_back(std::make_shared<Site>(nullptr, origin));
    }
    else if (type_ == SiteType::Fragment)
    {
        std::vector<std::shared_ptr<Site>> sites;
        double mass;
        std::vector<std::vector<int>> matchedIndices;
        for (auto &i : parent_->atoms())
        {
            std::vector<int> xAxisIndices, yAxisIndices;
            Vec3<double> v, origin, x, y, z;
            if (fragment_.matches(&i))
            {
                // Determine the path of matched atoms - i.e. the atoms in the fragment.
                auto matchedAtoms = fragment_.matchedPath(&i).set();

                // Create vector of indices of the matched atoms.
                std::vector<int> matchedAtomIndices(matchedAtoms.size());
                std::transform(matchedAtoms.begin(), matchedAtoms.end(), matchedAtomIndices.begin(),
                               [](const auto &atom) { return atom->index(); });

                // Check if the fragment we have found is unique.
                std::sort(matchedAtomIndices.begin(), matchedAtomIndices.end());
                if (std::find(matchedIndices.begin(), matchedIndices.end(), matchedAtomIndices) != matchedIndices.end())
                    continue;

                // If it's unique, remember it and proceed.
                matchedIndices.push_back(std::move(matchedAtomIndices));

                // Identifiers which label origin, x and y axis atoms.
                auto identifiers = fragment_.matchedPath(&i).identifiers();

                // Compute the origin.
                auto originAtoms = identifiers["origin"];

                if (originMassWeighted_)
                {
                    double massNorm = 0.0;
                    for (const auto &atom : originAtoms)
                    {
                        mass = AtomicMass::mass(atom->Z());
                        origin += atom->r() * mass;
                        massNorm += mass;
                    }
                    origin /= massNorm;
                }
                else
                {
                    for (const auto &atom : originAtoms)
                    {
                        origin += atom->r();
                    }
                    origin /= originAtoms.size();
                }
            }
        }
    }
    return sites;
}

/*
 * Read / Write
 */

// Return enum option info for SiteKeyword
EnumOptions<SpeciesSite::SiteKeyword> SpeciesSite::keywords()
{
    return EnumOptions<SpeciesSite::SiteKeyword>("SiteKeyword",
                                                 {{SpeciesSite::AtomTypeKeyword, "AtomType", OptionArguments::OneOrMore},
                                                  {SpeciesSite::DynamicKeyword, "Dynamic"},
                                                  {SpeciesSite::ElementKeyword, "Element", OptionArguments::OneOrMore},
                                                  {SpeciesSite::FragmentKeyword, "Fragment"},
                                                  {SpeciesSite::DescriptionKeyword, "Description", 1},
                                                  {SpeciesSite::EndSiteKeyword, "EndSite"},
                                                  {SpeciesSite::OriginKeyword, "Origin", OptionArguments::OneOrMore},
                                                  {SpeciesSite::OriginMassWeightedKeyword, "OriginMassWeighted", 1},
                                                  {SpeciesSite::XAxisKeyword, "XAxis", OptionArguments::OneOrMore},
                                                  {SpeciesSite::YAxisKeyword, "YAxis", OptionArguments::OneOrMore}});
}

// Read site definition from specified LineParser
bool SpeciesSite::read(LineParser &parser, const CoreData &coreData)
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
            case (SpeciesSite::AtomTypeKeyword):
                for (auto n = 1; n < parser.nArgs(); ++n)
                {
                    auto at = coreData.findAtomType(parser.args(n));
                    if (!at || !addDynamicAtomType(at))
                    {
                        Messenger::error("Failed to add target atom type for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            case (SpeciesSite::DynamicKeyword):
                if (staticOriginAtoms_.empty() && staticXAxisAtoms_.empty() && staticYAxisAtoms_.empty())
                    type_ = SiteType::Dynamic;
                else
                {
                    Messenger::error(
                        "Can't set '{}' to be dynamic as origin, x-axis, or y-axis atoms have already been defined.\n", name());
                    error = true;
                }
                break;
            case (SpeciesSite::ElementKeyword):
                for (auto n = 1; n < parser.nArgs(); ++n)
                {
                    auto el = Elements::element(parser.args(n));
                    if (el == Elements::Unknown || !addDynamicElement(el))
                    {
                        Messenger::error("Failed to add target element for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            case (SpeciesSite::FragmentKeyword):
                type_ = SiteType::Fragment;
                break;
            case (SpeciesSite::DescriptionKeyword):
                if (!fragment_.create(parser.args(1)))
                {
                    Messenger::error("Failed to parse NETA description for site '{}'.\n", name());
                    error = true;
                }
                break;
            case (SpeciesSite::EndSiteKeyword):
                if (type_ == SiteType::Fragment || type_ == SiteType::Dynamic)
                {
                    if (!generateUniqueSites())
                    {
                        Messenger::error("Failed to generate unique sites for site '{}'.\n", name());
                        error = true;
                    }
                }
                Messenger::print("Found end of Site '{}'.\n", name());
                blockDone = true;
                break;
            case (SpeciesSite::OriginKeyword):
                for (auto n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addStaticOriginAtom(parser.argi(n) - 1))
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
                    if (!addStaticXAxisAtom(parser.argi(n) - 1))
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
                    if (!addStaticYAxisAtom(parser.argi(n) - 1))
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

    // Site type
    if (type_ == SiteType::Dynamic)
    {
        if (!parser.writeLineF("{}  {}\n", prefix, keywords().keyword(DynamicKeyword)))
            return false;
    }
    else if (type_ == SiteType::Fragment)
    {
        if (!parser.writeLineF("{}  {}\n", prefix, keywords().keyword(FragmentKeyword)))
            return false;
    }

    // Origin atom indices
    if (!staticOriginAtoms_.empty() &&
        !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(OriginKeyword),
                           joinStrings(staticOriginAtomIndices(), "  ", [](const auto i) { return i + 1; })))
        return false;

    if (!fragment_.definitionString().empty() &&
        !parser.writeLineF("{}  {}  \"{}\"\n", prefix, keywords().keyword(DescriptionKeyword), fragment_.definitionString()))
        return false;

    if (!fragment_.definitionString().empty() &&
        !parser.writeLineF("{}  {}  \"{}\"\n", prefix, keywords().keyword(DescriptionKeyword), fragment_.definitionString()))
        return false;

    // Origin mass weighted?
    if (originMassWeighted_ && (!parser.writeLineF("{}  {}  True\n", prefix, keywords().keyword(OriginMassWeightedKeyword))))
        return false;

    // X-Axis atom indices
    if (!staticXAxisAtoms_.empty() &&
        !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(XAxisKeyword),
                           joinStrings(staticXAxisAtomIndices(), "  ", [](const auto i) { return i + 1; })))
        return false;

    // Y-Axis atom indices
    if (!staticYAxisAtoms_.empty() &&
        !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(YAxisKeyword),
                           joinStrings(staticYAxisAtomIndices(), "  ", [](const auto i) { return i + 1; })))
        return false;

    // Elements
    if (!dynamicElements_.empty() &&
        !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(ElementKeyword),
                           joinStrings(dynamicElements_, "  ", [](const auto el) { return Elements::symbol(el); })))
        return false;

    // Atom Types
    if (!dynamicAtomTypes_.empty() &&
        !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(AtomTypeKeyword),
                           joinStrings(dynamicAtomTypes_, "  ", [](const auto &at) { return at->name(); })))
        return false;

    // Write end of site definition
    if (!parser.writeLineF("{}{}\n", prefix, keywords().keyword(EndSiteKeyword)))
        return false;

    return true;
}

// Express as a tree node
SerialisedValue SpeciesSite::serialise() const
{
    SerialisedValue site;
    if (type_ == SiteType::Dynamic)
        site["dynamic"] = true;
    Serialisable::fromVector(staticOriginAtoms_, "originAtoms", site, [](const auto &item) { return item->index(); });
    Serialisable::fromVector(staticXAxisAtoms_, "xAxisAtoms", site, [](const auto &item) { return item->index(); });
    Serialisable::fromVector(staticYAxisAtoms_, "yAxisAtoms", site, [](const auto &item) { return item->index(); });
    Serialisable::fromVector(dynamicElements_, "elements", site, [](const auto &item) { return Elements::symbol(item); });
    Serialisable::fromVector(dynamicAtomTypes_, "atomTypes", site, [](const auto &item) { return item->name(); });
    site["originMassWeighted"] = originMassWeighted_;
    return site;
}

void SpeciesSite::deserialise(SerialisedValue &node, CoreData &coreData)
{
    if (node.contains("dynamic"))
        type_ = SiteType::Dynamic;

    toVector(node, "originAtoms", [this](const auto &originAtom) { addStaticOriginAtom(originAtom.as_integer()); });
    toVector(node, "xAxisAtoms", [this](const auto &xAxisAtom) { addStaticXAxisAtom(xAxisAtom.as_integer()); });
    toVector(node, "yAxisAtoms", [this](const auto &yAxisAtom) { addStaticYAxisAtom(yAxisAtom.as_integer()); });
    toVector(node, "elements", [this](const auto &el) { addDynamicElement(Elements::element(std::string(el.as_string()))); });
    toVector(node, "atomTypes",
             [&, this](const auto &at) { addDynamicAtomType(coreData.findAtomType(std::string(at.as_string()))); });

    if (node.contains("originMassWeighted"))
        originMassWeighted_ = node["originMassWeighted"].as_boolean();
}
