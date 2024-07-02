// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesBead.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include "classes/bead.h"
#include "classes/species.h"
#include "data/atomicMasses.h"
#include "neta/matchedGroup.h"
#include "neta/neta.h"
#include "templates/algorithms.h"
#include <numeric>
#include <string_view>

SpeciesBead::SpeciesBead(const Species *parent, BeadType type) : parent_(parent), type_(type), originMassWeighted_(false) {}
SpeciesBead::SpeciesBead(const Species *parent, std::string name, BeadType type)
    : name_(name), parent_(parent), type_(type), originMassWeighted_(false)
{
}

/*
 * Basic Information
 */

// Enum of types
EnumOptions<SpeciesBead::BeadType> SpeciesBead::beadTypes()
{
    return EnumOptions<BeadType>(
        "BeadType", {{BeadType::Static, "Static"}, {BeadType::Dynamic, "Dynamic"}, {BeadType::Fragment, "Fragment"}});
}

// Set name of bead
void SpeciesBead::setName(std::string_view newName) { name_ = newName; }

// Return name of bead
std::string_view SpeciesBead::name() const { return name_; }

// Return species parent
const Species *SpeciesBead::parent() const { return parent_; }

// Return version
int SpeciesBead::version() const { return version_; }

/*
 * Basic Definition
 */

// Clear definition data from bead
void SpeciesBead::clearDefinition()
{
    staticOriginAtoms_.clear();
    staticXAxisAtoms_.clear();
    staticYAxisAtoms_.clear();
    dynamicElements_.clear();
    dynamicAtomTypes_.clear();
    fragment_ = NETADefinition{};
    instances_.clear();

    ++version_;
}

// Set type of bead
void SpeciesBead::setType(SpeciesBead::BeadType type)
{
    type_ = type;

    clearDefinition();
}

// Return type of bead
SpeciesBead::BeadType SpeciesBead::type() const { return type_; }

// Return whether the bead has defined axes beads
bool SpeciesBead::hasAxes() const
{
    return (type_ == BeadType::Static && !(staticXAxisAtoms_.empty() || staticYAxisAtoms_.empty())) ||
           (type_ == BeadType::Fragment && fragment_.hasAxes());
}

/*
 * Static Bead Definition
 */

// Add origin atom
bool SpeciesBead::addStaticOriginAtom(const SpeciesAtom *originAtom)
{
    assert(originAtom);

    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting origin atoms for a non-static bead is not permitted.\n");

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(staticOriginAtoms_.begin(), staticOriginAtoms_.end(), originAtom) != staticOriginAtoms_.end())
        return Messenger::error("Origin atom index {} specified twice for bead '{}'.\n", originAtom->index(), name_);

    staticOriginAtoms_.push_back(originAtom);

    ++version_;

    return generateInstances();
}

// Add origin atom from index
bool SpeciesBead::addStaticOriginAtom(int atomIndex)
{
    assert(parent_);

    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting origin atoms for a non-static bead is not permitted.\n");

    return addStaticOriginAtom(&parent_->atom(atomIndex));
}

// Set origin atoms
bool SpeciesBead::setStaticOriginAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting origin atoms for a non-static bead is not permitted.\n");

    staticOriginAtoms_.clear();

    ++version_;

    for (auto *i : atoms)
        if (!addStaticOriginAtom(i))
        {
            staticOriginAtoms_.clear();
            return false;
        }

    return generateInstances();
}

// Return origin atom vector
const std::vector<const SpeciesAtom *> &SpeciesBead::staticOriginAtoms() const { return staticOriginAtoms_; }

// Return integer array of indices from which the origin should be formed
std::vector<int> SpeciesBead::staticOriginAtomIndices() const
{
    std::vector<int> indices;
    std::transform(staticOriginAtoms_.begin(), staticOriginAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

// Set whether the origin should be calculated with mass-weighted positions
void SpeciesBead::setOriginMassWeighted(bool b)
{
    originMassWeighted_ = b;

    ++version_;
}

// Return whether the origin should be calculated with mass-weighted positions
bool SpeciesBead::originMassWeighted() const { return originMassWeighted_; }

// Add x-axis atom
bool SpeciesBead::addStaticXAxisAtom(const SpeciesAtom *xAxisAtom)
{
    assert(xAxisAtom);

    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static bead is not permitted.\n");

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(staticXAxisAtoms_.begin(), staticXAxisAtoms_.end(), xAxisAtom) != staticXAxisAtoms_.end())
        return Messenger::error("X-axis atom index {} specified twice for bead '{}'.\n", xAxisAtom->index(), name_);

    staticXAxisAtoms_.push_back(xAxisAtom);

    ++version_;

    return generateInstances();
}

// Add x-axis atom from index
bool SpeciesBead::addStaticXAxisAtom(int atomIndex)
{
    assert(parent_);

    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static bead is not permitted.\n");

    return addStaticXAxisAtom(&parent_->atom(atomIndex));
}

// Set x-axis atoms
bool SpeciesBead::setStaticXAxisAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting x-axis atoms for a non-static bead is not permitted.\n");

    staticXAxisAtoms_.clear();

    ++version_;

    if (!std::all_of(atoms.begin(), atoms.end(), [&](const auto *i) { return addStaticXAxisAtom(i); }))
    {
        staticXAxisAtoms_.clear();
        return false;
    }

    return generateInstances();
}

// Return x-axis atom vector
const std::vector<const SpeciesAtom *> &SpeciesBead::staticXAxisAtoms() const { return staticXAxisAtoms_; }

// Return integer array of indices from which x-axis should be formed
std::vector<int> SpeciesBead::staticXAxisAtomIndices() const
{
    std::vector<int> indices;
    std::transform(staticXAxisAtoms_.begin(), staticXAxisAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

// Add y-axis atom
bool SpeciesBead::addStaticYAxisAtom(const SpeciesAtom *yAxisAtom)
{
    assert(yAxisAtom);

    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static bead is not permitted.\n");

    // If the SpeciesAtom already exists in the vector, complain
    if (std::find(staticYAxisAtoms_.begin(), staticYAxisAtoms_.end(), yAxisAtom) != staticYAxisAtoms_.end())
        return Messenger::error("Y-axis atom index {} specified twice for bead '{}'.\n", yAxisAtom->index(), name_);

    staticYAxisAtoms_.push_back(yAxisAtom);

    ++version_;

    return generateInstances();
}

// Add y-axis atom from index
bool SpeciesBead::addStaticYAxisAtom(int atomIndex)
{
    assert(parent_);

    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static bead is not permitted.\n");

    return addStaticYAxisAtom(&parent_->atom(atomIndex));
}

// Set y-axis atoms
bool SpeciesBead::setStaticYAxisAtoms(const std::vector<const SpeciesAtom *> &atoms)
{
    if (type_ != SpeciesBead::BeadType::Static)
        return Messenger::error("Setting y-axis atoms for a non-static bead is not permitted.\n");

    staticYAxisAtoms_.clear();

    ++version_;

    if (!std::all_of(atoms.begin(), atoms.end(), [&](const auto *i) { return addStaticYAxisAtom(i); }))
    {
        staticYAxisAtoms_.clear();
        return false;
    }

    return generateInstances();
}

// Return y-axis atom vector
const std::vector<const SpeciesAtom *> &SpeciesBead::staticYAxisAtoms() const { return staticYAxisAtoms_; }

// Return integer array of indices from which y-axis should be formed
std::vector<int> SpeciesBead::staticYAxisAtomIndices() const
{
    std::vector<int> indices;
    std::transform(staticYAxisAtoms_.begin(), staticYAxisAtoms_.end(), std::back_inserter(indices),
                   [](auto *atom) { return atom->index(); });
    return indices;
}

/*
 * Dynamic Bead Definition
 */

// Add target Elements for selection as beads
bool SpeciesBead::addDynamicElement(Elements::Element el)
{
    if (type_ != BeadType::Dynamic)
        return Messenger::error("Setting element targets for a non-dynamic bead is not permitted.\n");

    if (std::find(dynamicElements_.begin(), dynamicElements_.end(), el) != dynamicElements_.end())
        return Messenger::error("Element '{}' is already defined as a target for bead '{}'.\n", Elements::symbol(el), name_);

    dynamicElements_.push_back(el);

    return generateInstances();
}

// Set target Elements for selection as beads
bool SpeciesBead::setDynamicElements(const std::vector<Elements::Element> &els)
{
    dynamicElements_.clear();

    return std::all_of(els.begin(), els.end(), [&](const auto el) { return addDynamicElement(el); }) && generateInstances();
}

// Return elements for selection as beads
const std::vector<Elements::Element> SpeciesBead::dynamicElements() const { return dynamicElements_; }

// Add target atom type for selection as beads
bool SpeciesBead::addDynamicAtomType(const std::shared_ptr<AtomType> &at)
{
    if (type_ != BeadType::Dynamic)
        return Messenger::error("Setting atom type targets for a non-dynamic bead is not permitted.\n");

    if (std::find(dynamicAtomTypes_.begin(), dynamicAtomTypes_.end(), at) != dynamicAtomTypes_.end())
        return Messenger::error("Atom type '{}' is already defined as a target for bead '{}'.\n", at->name(), name_);

    dynamicAtomTypes_.push_back(at);

    return generateInstances();
}

// Set target atom types for selection as beads
bool SpeciesBead::setDynamicAtomTypes(const std::vector<std::shared_ptr<AtomType>> &types)
{
    dynamicAtomTypes_.clear();

    return std::all_of(types.begin(), types.end(), [&](const auto &at) { return addDynamicAtomType(at); }) &&
           generateInstances();
}

// Return atom types for selection as beads
const std::vector<std::shared_ptr<AtomType>> &SpeciesBead::dynamicAtomTypes() const { return dynamicAtomTypes_; }

// Return fragment definition
const NETADefinition &SpeciesBead::fragment() const { return fragment_; }

// Update fragment definition
bool SpeciesBead::setFragmentDefinitionString(std::string_view definitionString)
{
    instances_.clear();

    return fragment_.create(definitionString) && generateInstances();
}

// Generate unique beads
bool SpeciesBead::generateInstances()
{
    instances_.clear();

    switch (type_)
    {
        case (BeadType::Static):
        {
            std::vector<int> atomIndices;
            auto originAtomIndices = staticOriginAtomIndices();
            auto xAxisAtomIndices = staticXAxisAtomIndices();
            auto yAxisAtomIndices = staticYAxisAtomIndices();
            atomIndices.insert(atomIndices.end(), originAtomIndices.begin(), originAtomIndices.end());
            atomIndices.insert(atomIndices.end(), xAxisAtomIndices.begin(), xAxisAtomIndices.end());
            atomIndices.insert(atomIndices.end(), yAxisAtomIndices.begin(), yAxisAtomIndices.end());
            instances_.emplace_back(atomIndices, originAtomIndices, xAxisAtomIndices, yAxisAtomIndices);
            break;
        }
        case (BeadType::Dynamic):
            for (auto &i : parent_->atoms())
            {
                // Valid element or atom type?
                if ((std::find(dynamicElements_.begin(), dynamicElements_.end(), i.Z()) != dynamicElements_.end()) ||
                    std::find(dynamicAtomTypes_.begin(), dynamicAtomTypes_.end(), i.atomType()) != dynamicAtomTypes_.end())
                {
                    instances_.emplace_back(std::vector<int>{i.index()}, std::vector<int>{i.index()});
                }
            }
            break;
        case (BeadType::Fragment):
            if (!fragment_.isValid())
                return false;

            for (auto &i : parent_->atoms())
            {
                if (fragment_.matches(&i))
                {
                    // Determine the path of matched atoms - i.e. the atoms in the fragment.
                    auto matchedGroup = fragment_.matchedPath(&i);
                    const auto &matchedAtoms = matchedGroup.set();

                    // Create vector of indices of all matched atoms
                    std::vector<int> matchedAtomIndices(matchedAtoms.size());
                    std::transform(matchedAtoms.begin(), matchedAtoms.end(), matchedAtomIndices.begin(),
                                   [](const auto &atom) { return atom->index(); });

                    // Check if the fragment we have found is unique.
                    std::sort(matchedAtomIndices.begin(), matchedAtomIndices.end());
                    if (std::find_if(instances_.begin(), instances_.end(),
                                     [&](const auto &instance)
                                     { return matchedAtomIndices == instance.allIndices(); }) != instances_.end())
                        continue;

                    // Get identifier from match and check for origin and axis definitions
                    auto identifiers = matchedGroup.identifiers();

                    // Check origin atoms, if there are none then the bead is not valid
                    auto originAtoms = identifiers["origin"];
                    if (originAtoms.empty())
                        continue;

                    std::vector<int> originAtomIndices;
                    std::transform(originAtoms.begin(), originAtoms.end(), std::back_inserter(originAtomIndices),
                                   [](const auto &atom) { return atom->index(); });

                    if (hasAxes())
                    {
                        // Determine x axis atoms
                        const auto xAxisAtoms = identifiers["x"];
                        std::vector<int> xAxisAtomIndices;
                        std::transform(xAxisAtoms.begin(), xAxisAtoms.end(), std::back_inserter(xAxisAtomIndices),
                                       [](const auto &atom) { return atom->index(); });

                        // Determine y axis atoms
                        const auto yAxisAtoms = identifiers["y"];
                        std::vector<int> yAxisAtomIndices;
                        std::transform(yAxisAtoms.begin(), yAxisAtoms.end(), std::back_inserter(yAxisAtomIndices),
                                       [](const auto &atom) { return atom->index(); });

                        instances_.emplace_back(matchedAtomIndices, originAtomIndices, xAxisAtomIndices, yAxisAtomIndices);
                    }
                    else
                        instances_.emplace_back(matchedAtomIndices, originAtomIndices);
                }
            }
            break;
        default:
            return Messenger::error("Can't generate unique beads for bead '{}'.", name());
    }

    return true;
}

// Return bead instances
const std::vector<SpeciesBeadInstance> &SpeciesBead::instances() const { return instances_; }

/*
 * Generation
 */

// Calculate geometric centre of atoms in the given molecule
Vec3<double> SpeciesBead::centreOfGeometry(const std::vector<int> &indices) const
{
    const auto ref = parent_->atom(indices.front()).r();
    return std::accumulate(std::next(indices.begin()), indices.end(), ref,
                           [&ref, this](const auto &acc, const auto idx)
                           { return acc + parent_->box()->minimumImage(parent_->atom(idx).r(), ref); }) /
           indices.size();
}

// Calculate (mass-weighted) coordinate centre of atoms in the given molecule
Vec3<double> SpeciesBead::centreOfMass(const std::vector<int> &indices) const
{
    auto mass = AtomicMass::mass(parent_->atom(indices.front()).Z());
    const auto ref = parent_->atom(indices.front()).r();
    auto sums = std::accumulate(std::next(indices.begin()), indices.end(), std::pair<Vec3<double>, double>(ref * mass, mass),
                                [&ref, this](const auto &acc, const auto idx)
                                {
                                    auto mass = AtomicMass::mass(parent_->atom(idx).Z());
                                    return std::pair<Vec3<double>, double>(
                                        acc.first + parent_->box()->minimumImage(parent_->atom(idx).r(), ref) * mass,
                                        acc.second + mass);
                                });
    return sums.first / sums.second;
}

// Create and return bead description from parent Species
std::vector<std::shared_ptr<Bead>> SpeciesBead::createFromParent() const
{
    std::vector<std::shared_ptr<Bead>> beads;

    auto index = 0;
    for (const auto &instance : instances_)
    {
        // Determine origin
        auto origin = originMassWeighted_ ? centreOfMass(instance.originIndices()) : centreOfGeometry(instance.originIndices());

        if (hasAxes())
        {
            // Get vector from bead origin to x-axis reference point and normalise it
            auto x = parent_->box()->minimumVector(origin, centreOfGeometry(instance.xAxisIndices()));
            x.normalise();

            // Get vector from bead origin to y-axis reference point, normalise it, and orthogonalise
            auto y = parent_->box()->minimumVector(origin, centreOfGeometry(instance.yAxisIndices()));
            y.orthogonalise(x);
            y.normalise();

            beads.push_back(std::make_shared<OrientedBead>(this, index, nullptr, origin, x, y, x * y));
        }
        else
            beads.push_back(std::make_shared<Bead>(this, index, nullptr, origin));

        ++index;
    }
    return beads;
}

/*
 * Read / Write
 */

// Return enum option info for BeadKeyword
EnumOptions<SpeciesBead::BeadKeyword> SpeciesBead::keywords()
{
    return EnumOptions<SpeciesBead::BeadKeyword>("BeadKeyword",
                                                 {{SpeciesBead::AtomTypeKeyword, "AtomType", OptionArguments::OneOrMore},
                                                  {SpeciesBead::DynamicKeyword, "Dynamic"},
                                                  {SpeciesBead::ElementKeyword, "Element", OptionArguments::OneOrMore},
                                                  {SpeciesBead::FragmentKeyword, "Fragment"},
                                                  {SpeciesBead::DescriptionKeyword, "Description", 1},
                                                  {SpeciesBead::EndBeadKeyword, "EndBead"},
                                                  {SpeciesBead::OriginKeyword, "Origin", OptionArguments::OneOrMore},
                                                  {SpeciesBead::OriginMassWeightedKeyword, "OriginMassWeighted", 1},
                                                  {SpeciesBead::XAxisKeyword, "XAxis", OptionArguments::OneOrMore},
                                                  {SpeciesBead::YAxisKeyword, "YAxis", OptionArguments::OneOrMore}});
}

// Read bead definition from specified LineParser
bool SpeciesBead::read(LineParser &parser, const CoreData &coreData)
{
    Messenger::printVerbose("\nReading information for Bead '{}'...\n", name());

    auto blockDone = false, errorsEncountered = false;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
        {
            keywords().errorAndPrintValid(parser.argsv(0));
            errorsEncountered = true;
            continue;
        }
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
        {
            errorsEncountered = true;
            continue;
        }

        // All OK, so process the keyword
        switch (kwd)
        {
            case (SpeciesBead::AtomTypeKeyword):
                for (auto n = 1; n < parser.nArgs(); ++n)
                {
                    auto at = coreData.findAtomType(parser.args(n));
                    if (!at || !addDynamicAtomType(at))
                    {
                        Messenger::error("Failed to add target atom type for bead '{}'.\n", name());
                        errorsEncountered = true;
                        break;
                    }
                }
                break;
            case (SpeciesBead::DynamicKeyword):
                if (staticOriginAtoms_.empty() && staticXAxisAtoms_.empty() && staticYAxisAtoms_.empty())
                    type_ = BeadType::Dynamic;
                else
                {
                    Messenger::error(
                        "Can't set '{}' to be dynamic as origin, x-axis, or y-axis atoms have already been defined.\n", name());
                    errorsEncountered = true;
                }
                break;
            case (SpeciesBead::ElementKeyword):
                for (auto n = 1; n < parser.nArgs(); ++n)
                {
                    auto el = Elements::element(parser.args(n));
                    if (el == Elements::Unknown || !addDynamicElement(el))
                    {
                        Messenger::error("Failed to add target element for bead '{}'.\n", name());
                        errorsEncountered = true;
                        break;
                    }
                }
                break;
            case (SpeciesBead::FragmentKeyword):
                type_ = BeadType::Fragment;
                break;
            case (SpeciesBead::DescriptionKeyword):
                if (!fragment_.create(parser.args(1)))
                {
                    Messenger::error("Failed to parse NETA description for bead '{}'.\n", name());
                    errorsEncountered = true;
                }
                break;
            case (SpeciesBead::EndBeadKeyword):
                if (type_ == BeadType::Fragment || type_ == BeadType::Dynamic)
                {
                    if (!generateInstances())
                    {
                        Messenger::error("Failed to generate unique beads for bead '{}'.\n", name());
                        errorsEncountered = true;
                    }
                }
                Messenger::print("Found end of Bead '{}'.\n", name());
                blockDone = true;
                break;
            case (SpeciesBead::OriginKeyword):
                for (auto n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addStaticOriginAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add origin atom for bead '{}'.\n", name());
                        errorsEncountered = true;
                        break;
                    }
                }
                break;
            case (SpeciesBead::OriginMassWeightedKeyword):
                setOriginMassWeighted(parser.argb(1));
                break;
            case (SpeciesBead::XAxisKeyword):
                for (int n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addStaticXAxisAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add x-axis atom for bead '{}'.\n", name());
                        errorsEncountered = true;
                        break;
                    }
                }
                break;
            case (SpeciesBead::YAxisKeyword):
                for (int n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addStaticYAxisAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add y-axis atom for bead '{}'.\n", name());
                        errorsEncountered = true;
                        break;
                    }
                }
                break;
            default:
                Messenger::error("Bead block keyword '{}' not accounted for.\n", keywords().keyword(kwd));
                errorsEncountered = true;
                break;
        }

        // Error encountered?
        if (errorsEncountered)
            break;

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no errorsEncountered and the blockDone flag isn't set, return an errorsEncountered
    if (!errorsEncountered && !blockDone)
    {
        Messenger::error("Unterminated Bead block found.\n");
        errorsEncountered = true;
    }

    return (!errorsEncountered);
}

// Write bead definition to specified LineParser
bool SpeciesBead::write(LineParser &parser, std::string_view prefix)
{
    // Write start of bead definition
    if (!parser.writeLineF("{}Bead  '{}'\n", prefix, name()))
        return false;

    // Bead type
    if (type_ == BeadType::Dynamic)
    {
        if (!parser.writeLineF("{}  {}\n", prefix, keywords().keyword(DynamicKeyword)))
            return false;
    }
    else if (type_ == BeadType::Fragment)
    {
        if (!parser.writeLineF("{}  {}\n", prefix, keywords().keyword(FragmentKeyword)))
            return false;
    }

    // Origin atom indices
    if (!staticOriginAtoms_.empty() &&
        !parser.writeLineF("{}  {}  {}\n", prefix, keywords().keyword(OriginKeyword),
                           joinStrings(staticOriginAtomIndices(), "  ", [](const auto i) { return i + 1; })))
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

    // Fragment definition string
    if (!fragment_.definitionString().empty() &&
        !parser.writeLineF("{}  {}  \"{}\"\n", prefix, keywords().keyword(DescriptionKeyword), fragment_.definitionString()))
        return false;

    // Write end of bead definition
    if (!parser.writeLineF("{}{}\n", prefix, keywords().keyword(EndBeadKeyword)))
        return false;

    return true;
}

// Express as a serialisable value
SerialisedValue SpeciesBead::serialise() const
{
    SerialisedValue bead;
    if (type_ != BeadType::Static)
        bead["type"] = beadTypes().serialise(type_);
    if (originMassWeighted_)
        bead["originMassWeighted"] = originMassWeighted_;
    switch (type_)
    {
        case BeadType::Dynamic:
            bead["element"] = dynamicElements_;
            break;
        case BeadType::Fragment:
            bead["description"] = fragment_.definitionString();
            break;
        case BeadType::Static:
            Serialisable::fromVector(staticOriginAtoms_, "originAtoms", bead, [](const auto &item) { return item->index(); });
            Serialisable::fromVector(staticXAxisAtoms_, "xAxisAtoms", bead, [](const auto &item) { return item->index(); });
            Serialisable::fromVector(staticYAxisAtoms_, "yAxisAtoms", bead, [](const auto &item) { return item->index(); });
            Serialisable::fromVector(dynamicElements_, "elements", bead,
                                     [](const auto &item) { return Elements::symbol(item); });
            Serialisable::fromVector(dynamicAtomTypes_, "atomTypes", bead, [](const auto &item) { return item->name(); });
            break;
    }
    return bead;
}

void SpeciesBead::deserialise(const SerialisedValue &node, CoreData &coreData)
{
    type_ = beadTypes().deserialise(toml::find_or(node, "type", "static"));

    switch (type_)
    {
        case BeadType::Static:
            toVector(node, "originAtoms", [this](const auto &originAtom) { addStaticOriginAtom(originAtom.as_integer()); });
            toVector(node, "xAxisAtoms", [this](const auto &xAxisAtom) { addStaticXAxisAtom(xAxisAtom.as_integer()); });
            toVector(node, "yAxisAtoms", [this](const auto &yAxisAtom) { addStaticYAxisAtom(yAxisAtom.as_integer()); });
            toVector(node, "elements",
                     [this](const auto &el) { addDynamicElement(Elements::element(std::string(el.as_string()))); });
            toVector(node, "atomTypes",
                     [&, this](const auto &at) { addDynamicAtomType(coreData.findAtomType(std::string(at.as_string()))); });

            break;
        case BeadType::Fragment:
            fragment_.create(toml::find<std::string>(node, "description"));
            break;
        case BeadType::Dynamic:
            toVector(node, "element",
                     [this](const auto &element) { addDynamicElement(toml::get<Elements::Element>(element)); });
            break;
    }

    originMassWeighted_ = toml::find_or<bool>(node, "originMassWeighted", false);

    generateInstances();
}
