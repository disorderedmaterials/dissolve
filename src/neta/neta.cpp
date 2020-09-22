// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "neta/neta.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "data/ffatomtype.h"
#include "neta/generator.h"
#include "neta/node.h"
#include <stdarg.h>
#include <string.h>

NETADefinition::NETADefinition() : rootNode_(this) {}

NETADefinition::~NETADefinition() { rootNode_.clear(); }

/*
 * Data
 */

// Return root node pointer
NETARootNode *NETADefinition::rootNode() { return &rootNode_; }

// Create definition from stored string
bool NETADefinition::create(const Forcefield *associatedFF)
{
    rootNode_.clear();

    return NETADefinitionGenerator::generate(*this, definitionString_, associatedFF);
}

// Set generating string
void NETADefinition::setDefinitionString(std::string_view definition) { definitionString_ = definition; }

// Return original generating string
std::string_view NETADefinition::definitionString() const { return definitionString_; }

/*
 * Matching
 */

// Check supplied atom to see if it matches this NETA description
int NETADefinition::score(const SpeciesAtom *i) const
{
    RefList<const SpeciesAtom> matchPath;
    return rootNode_.score(i, matchPath);
}
