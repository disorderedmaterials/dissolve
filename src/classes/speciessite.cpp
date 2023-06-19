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
#include <memory>
#include <numeric>

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
    originAtoms_.clear();
    xAxisAtoms_.clear();
    yAxisAtoms_.clear();
    elements_.clear();
    atomTypes_.clear();

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
    return (type_ == SiteType::Static && !(xAxisAtoms_.empty() && yAxisAtoms_.empty())) ||
           (type_ == SiteType::Fragment && fragment_.hasAxes());
}

/*
 * Static Site Definition
 */

// Add origin atom
bool SpeciesSite::addOriginAtom(const SpeciesAtom *originAtom)
{
    assert(originAtom);

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting origin atoms for a non-static site is not permitted.\n");

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

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting origin atoms for a non-static site is not permitted.\n");

    return addOriginAtom(&parent_->atom(atomIndex));
}

// Set origin atoms
bool SpeciesSite::setOriginAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting origin atoms for a non-static site is not permitted.\n");

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

// Return origin atom vector
const std::vector<const SpeciesAtom *> &SpeciesSite::originAtoms() const { return originAtoms_; }

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

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static site is not permitted.\n");

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

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static site is not permitted.\n");

    return addXAxisAtom(&parent_->atom(atomIndex));
}

// Set x-axis atoms
bool SpeciesSite::setXAxisAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static site is not permitted.\n");

    xAxisAtoms_.clear();

    ++version_;

    if (!std::all_of(atoms.begin(), atoms.end(), [&](const auto *i) { return addXAxisAtom(i); }))
    {
        xAxisAtoms_.clear();
        return false;
    }

    return true;
}

// Return x-axis atom vector
const std::vector<const SpeciesAtom *> &SpeciesSite::xAxisAtoms() const { return xAxisAtoms_; }

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

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static site is not permitted.\n");

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

    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static site is not permitted.\n");

    return addYAxisAtom(&parent_->atom(atomIndex));
}

// Set y-axis atoms
bool SpeciesSite::setYAxisAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesSite::SiteType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static site is not permitted.\n");

    yAxisAtoms_.clear();

    ++version_;

    if (!std::all_of(atoms.begin(), atoms.end(), [&](const auto *i) { return addYAxisAtom(i); }))
    {
        yAxisAtoms_.clear();
        return false;
    }

    return true;
}

// Return y-axis atom vector
const std::vector<const SpeciesAtom *> &SpeciesSite::yAxisAtoms() const { return yAxisAtoms_; }

// Return integer array of indices from which y-axis should be formed
std::vector<int> SpeciesSite::yAxisAtomIndices() const
{
    std::vector<int> indices;
    std::transform(yAxisAtoms_.begin(), yAxisAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

/*
 * Dynamic Site Definition
 */

// Add target Elements for selection as sites
bool SpeciesSite::addElement(Elements::Element el)
{
    if (type_ != SiteType::Dynamic)
        return Messenger::error("Setting element targets for a non-dynamic site is not permitted.\n");

    if (std::find(elements_.begin(), elements_.end(), el) != elements_.end())
        return Messenger::error("Element '{}' is already defined as a target for site '{}'.\n", Elements::symbol(el), name_);

    elements_.push_back(el);

    return true;
}

// Set target Elements for selection as sites
bool SpeciesSite::setElements(const std::vector<Elements::Element> &els)
{
    elements_.clear();

    return std::all_of(els.begin(), els.end(), [&](const auto el) { return addElement(el); });
}

// Return elements for selection as sites
const std::vector<Elements::Element> SpeciesSite::elements() const { return elements_; }

// Add target atom type for selection as sites
bool SpeciesSite::addAtomType(const std::shared_ptr<AtomType> &at)
{
    if (type_ != SiteType::Dynamic)
        return Messenger::error("Setting atom type targets for a non-dynamic site is not permitted.\n");

    if (std::find(atomTypes_.begin(), atomTypes_.end(), at) != atomTypes_.end())
        return Messenger::error("Atom type '{}' is already defined as a target for site '{}'.\n", at->name(), name_);

    atomTypes_.push_back(at);

    return true;
}

// Set target atom types for selection as sites
bool SpeciesSite::setAtomTypes(const std::vector<std::shared_ptr<AtomType>> &types)
{
    atomTypes_.clear();

    return std::all_of(types.begin(), types.end(), [&](const auto &at) { return addAtomType(at); });
}

// Return atom types for selection as sites
const std::vector<std::shared_ptr<AtomType>> &SpeciesSite::atomTypes() const { return atomTypes_; }

// Return fragment definition
const NETADefinition &SpeciesSite::fragment() const { return fragment_; }

/*
 * Generation from Parent
 */

// Create and return site description from parent Species
std::vector<std::shared_ptr<Site>> SpeciesSite::createFromParent() const
{
    if (type_ == SiteType::Static)
    {
        // Get origin atom indices from site
        std::vector<int> originIndices = originAtomIndices();
        if (originIndices.empty())
            return {};

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
            auto z = x * y;

            return {std::make_shared<OrientedSite>(nullptr, origin, x, y, z)};
        }
        else
            return {std::make_shared<Site>(nullptr, origin)};
    }
    else if (type_ == SiteType::Dynamic)
    {
        std::vector<std::shared_ptr<Site>> sites;
        for (auto &i : parent_->atoms())
        {
            // Valid element or atom type?
            if ((std::find(elements_.begin(), elements_.end(), i.Z()) != elements_.end()) ||
                std::find(atomTypes_.begin(), atomTypes_.end(), i.atomType()) != atomTypes_.end())
                sites.push_back(std::make_shared<Site>(nullptr, i.r()));
        }
        return sites;
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

                // Fragment site definition has orientation.
                if (hasAxes())
                {

                    auto xAxisAtoms = identifiers["x"];
                    auto yAxisAtoms = identifiers["y"];

                    Vec3<double> v;

                    // Get average position of supplied x-axis atoms
                    for (const auto &atom : xAxisAtoms)
                        v += atom->r();
                    v /= xAxisAtoms.size();

                    // Get vector from site origin and normalise it
                    auto x = v - origin;
                    x.normalise();

                    // Get average position of supplied y-axis atoms
                    v.zero();
                    for (const auto &atom : yAxisAtoms)
                        v += atom->r();
                    v /= yAxisAtoms.size();

                    // Get vector from site origin, normalise it, and orthogonalise
                    auto y = v - origin;
                    y.orthogonalise(x);
                    y.normalise();

                    // Calculate z vector from cross product of x and y
                    auto z = x * y;

                    sites.push_back(std::make_shared<OrientedSite>(nullptr, origin, x, y, z));
                }
                else
                    sites.push_back(std::make_shared<Site>(nullptr, origin));
            }
        }
        return sites;
    }
    return {};
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
                    if (!at || !addAtomType(at))
                    {
                        Messenger::error("Failed to add target atom type for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            case (SpeciesSite::DynamicKeyword):
                if (originAtoms_.empty() && xAxisAtoms_.empty() && yAxisAtoms_.empty())
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
                    if (el == Elements::Unknown || !addElement(el))
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
    if (!originAtoms_.empty() && !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(OriginKeyword),
                                                    joinStrings(originAtomIndices(), "  ", [](const auto i) { return i + 1; })))
        return false;

    if (!fragment_.definitionString().empty() &&
        !parser.writeLineF("{}  {}  \"{}\"\n", prefix, keywords().keyword(DescriptionKeyword), fragment_.definitionString()))
        return false;

    // Origin mass weighted?
    if (originMassWeighted_ && (!parser.writeLineF("{}  {}  True\n", prefix, keywords().keyword(OriginMassWeightedKeyword))))
        return false;

    // X-Axis atom indices
    if (!xAxisAtoms_.empty() && !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(XAxisKeyword),
                                                   joinStrings(xAxisAtomIndices(), "  ", [](const auto i) { return i + 1; })))
        return false;

    // Y-Axis atom indices
    if (!yAxisAtoms_.empty() && !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(YAxisKeyword),
                                                   joinStrings(yAxisAtomIndices(), "  ", [](const auto i) { return i + 1; })))
        return false;

    // Elements
    if (!elements_.empty() &&
        !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(ElementKeyword),
                           joinStrings(elements_, "  ", [](const auto el) { return Elements::symbol(el); })))
        return false;

    // Atom Types
    if (!atomTypes_.empty() && !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(AtomTypeKeyword),
                                                  joinStrings(atomTypes_, "  ", [](const auto &at) { return at->name(); })))
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
    Serialisable::fromVector(originAtoms_, "originAtoms", site, [](const auto &item) { return item->index(); });
    Serialisable::fromVector(xAxisAtoms_, "xAxisAtoms", site, [](const auto &item) { return item->index(); });
    Serialisable::fromVector(yAxisAtoms_, "yAxisAtoms", site, [](const auto &item) { return item->index(); });
    Serialisable::fromVector(elements_, "elements", site, [](const auto &item) { return Elements::symbol(item); });
    Serialisable::fromVector(atomTypes_, "atomTypes", site, [](const auto &item) { return item->name(); });
    site["originMassWeighted"] = originMassWeighted_;
    return site;
}

void SpeciesSite::deserialise(SerialisedValue &node, CoreData &coreData)
{
    if (node.contains("dynamic"))
        type_ = SiteType::Dynamic;

    toVector(node, "originAtoms", [this](const auto &originAtom) { addOriginAtom(originAtom.as_integer()); });
    toVector(node, "xAxisAtoms", [this](const auto &xAxisAtom) { addXAxisAtom(xAxisAtom.as_integer()); });
    toVector(node, "yAxisAtoms", [this](const auto &yAxisAtom) { addYAxisAtom(yAxisAtom.as_integer()); });
    toVector(node, "elements", [this](const auto &el) { addElement(Elements::element(std::string(el.as_string()))); });
    toVector(node, "atomTypes", [&, this](const auto &at) { addAtomType(coreData.findAtomType(std::string(at.as_string()))); });

    if (node.contains("originMassWeighted"))
        originMassWeighted_ = node["originMassWeighted"].as_boolean();
}
