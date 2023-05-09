// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/potentials/base.h"

ExternalPotential::ExternalPotential(ExternalPotentialTypes::ExternalPotentialType type) : type_(type) {}

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
    // Block Start
    if (!parser.writeLineF("{}{}\n", prefix, ExternalPotentialTypes::keyword(type_)))
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
