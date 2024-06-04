// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/base.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include "types.h"

ExternalPotential::ExternalPotential(ExternalPotentialTypes::ExternalPotentialType type) : type_(type) {}

// Create and return a copy of this potential
std::unique_ptr<ExternalPotential> ExternalPotential::duplicate() const
{
    throw(std::runtime_error(fmt::format("Can't duplicate() an ExternalPotential of this type.\n")));
}

/*
 * Target Information
 */

// Set atom indices targeted by the potential
void ExternalPotential::setTargetAtomIndices(const std::vector<int> &targets) { targetAtomIndices_ = targets; }

// Add target atom index
void ExternalPotential::addTargetAtomIndex(int index) { targetAtomIndices_.push_back(index); }

// Return atom indices targeted by the potential
const std::vector<int> &ExternalPotential::targetAtomIndices() const { return targetAtomIndices_; }

ExternalPotentialTypes::ExternalPotentialType ExternalPotential::type() const { return type_; }

// Atom types targeted by the potential
void ExternalPotential::setTargetAtomTypes(const std::vector<std::shared_ptr<AtomType>> &targets)
{
    targetAtomTypes_ = targets;
}

// Add target atom type
void ExternalPotential::addTargetAtomType(std::shared_ptr<AtomType> target)
{
    targetAtomTypes_.emplace_back(std::move(target));
}

// Return atom types targeted by the potential
const std::vector<std::shared_ptr<AtomType>> &ExternalPotential::targetAtomTypes() const { return targetAtomTypes_; }

// Species targeted by the potential
void ExternalPotential::setTargetSpecies(const std::vector<const Species *> &targets) { targetSpecies_ = targets; }

// Add target species
void ExternalPotential::addTargetSpecies(const Species *target) { targetSpecies_.emplace_back(target); }

// Return species targeted by the potential
const std::vector<const Species *> &ExternalPotential::targetSpecies() const { return targetSpecies_; }

/*
 * Keywords
 */

// Return keywords for this potential
KeywordStore &ExternalPotential::keywords() { return keywords_; }
const KeywordStore &ExternalPotential::keywords() const { return keywords_; }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double ExternalPotential::energy(const Atom &i, const Box *box) const { return 0.0; }

// Calculate force on specified atom, summing in to supplied vector
void ExternalPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const {}

/*
 * Read / Write
 */

// Read data from specified LineParser
bool ExternalPotential::deserialise(LineParser &parser, const CoreData &coreData)
{
    // Read until we encounter the ending keyword (derived from the potential type), or we fail for some reason
    while (!parser.eofOrBlank())
    {
        // Read and parse the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Is this the end of the block?
        if (DissolveSys::sameString(parser.argsv(0), fmt::format("End{}", ExternalPotentialTypes::keyword(type_))))
            return true;

        // Try to parse this line as a keyword
        KeywordBase::ParseResult result = keywords_.deserialise(parser, coreData);
        if (result == KeywordBase::ParseResult::Unrecognised)
            return Messenger::error("Unrecognised keyword '{}' found while parsing {} additional potential.\n", parser.argsv(0),
                                    ExternalPotentialTypes::keyword(type_));
        else if (result == KeywordBase::ParseResult::Deprecated)
            Messenger::warn("The '{}' keyword is deprecated and will be removed in a future version.\n", parser.argsv(0));
        else if (result == KeywordBase::ParseResult::Failed)
            return Messenger::error("Failed to parse keyword '{}'.\n", parser.argsv(0));
    }

    return true;
}

// Write data to specified LineParser
bool ExternalPotential::serialise(LineParser &parser, std::string_view prefix) const
{
    // Assemble target strings
    std::string targets;
    if (!targetAtomIndices_.empty())
        targets += fmt::format("  {}", joinStrings(targetAtomIndices_, " "));
    if (!targetAtomTypes_.empty())
        targets += fmt::format("  {}", joinStrings(targetAtomTypes_, " ", [](const auto &at) { return at->name(); }));
    if (!targetSpecies_.empty())
        targets += fmt::format("  {}", joinStrings(targetSpecies_, " ", [](const auto &sp) { return sp->name(); }));

    // Block Start
    if (!parser.writeLineF("{}{}{}\n", prefix, ExternalPotentialTypes::keyword(type_), targets))
        return false;

    // Create new prefix
    std::string newPrefix = fmt::format("  {}", prefix);

    // Write keywords
    if (!keywords_.serialise(parser, newPrefix, true))
        return false;

    // Block End
    if (!parser.writeLineF("{}End{}\n", prefix, ExternalPotentialTypes::keyword(type_)))
        return false;

    return true;
}
