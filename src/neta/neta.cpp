// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "neta/neta.h"
#include "NETALexer.h"
#include "NETAParser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "data/ff/atomtype.h"
#include "neta/NETAErrorListeners.h"
#include "neta/NETAVisitor.h"
#include "neta/neta.h"
#include "neta/node.h"
#include <antlr4-runtime.h>
#include <cstdarg>
#include <cstring>

NETADefinition::NETADefinition(std::string_view definition) : rootNode_(nullptr), valid_(false)
{
    if (!definition.empty())
        create(definition);
}

NETADefinition::NETADefinition(const SpeciesAtom *i, int maxDepth) : rootNode_(nullptr), valid_(false) { create(i, maxDepth); }

/*
 * Data
 */

// Return root node pointer
std::shared_ptr<NETANode> NETADefinition::rootNode() { return rootNode_; }

// Create definition from stored definition string
bool NETADefinition::create(const Forcefield *associatedFF)
{
    // Create a new root node, overwriting the old one
    rootNode_ = std::make_shared<NETANode>(this);
    valid_ = false;

    // Create string stream and set up ANTLR input stream
    std::stringstream stream;
    stream << definitionString_;
    antlr4::ANTLRInputStream input(stream);

    // Create ANTLR lexer and set-up error listener
    NETALexer lexer(&input);
    NETALexerErrorListener lexerErrorListener(*this);
    lexer.removeErrorListeners();
    lexer.addErrorListener(&lexerErrorListener);

    // Generate tokens from input stream
    antlr4::CommonTokenStream tokens(&lexer);

    // Create ANTLR parser and set-up error listeners
    NETAParser parser(&tokens);
    NETAParserErrorListener parserErrorListener(*this);
    parser.removeErrorListeners();
    parser.removeParseListeners();
    parser.addErrorListener(&lexerErrorListener);
    parser.addErrorListener(&parserErrorListener);

    // Generate the AST
    NETAParser::NetaContext *tree = nullptr;
    try
    {
        tree = parser.neta();
    }
    catch (NETAExceptions::NETASyntaxException &ex)
    {
        return Messenger::error(ex.what());
    };

    // Visit the nodes in the AST
    NETAVisitor visitor;
    try
    {
        visitor.create(*this, tree, associatedFF);
    }
    catch (NETAExceptions::NETASyntaxException &ex)
    {
        return Messenger::error(ex.what());
    }

    valid_ = true;

    return true;
}

// Set definition string and create definition
bool NETADefinition::create(std::string_view definition, const Forcefield *associatedFF)
{
    definitionString_ = definition;
    return create(associatedFF);
}

// Recursively create a NETA string for the specified atom
std::string netaString(const SpeciesAtom *i, int currentDepth, const int maxDepth, std::vector<const SpeciesAtom *> &path)
{
    // Add this atom to the path
    path.push_back(i);

    auto neta = fmt::format("nbonds={}", i->nBonds());

    // Add on each connected atom, provided it is not already in the path
    auto nH = 0;
    for (auto &b : i->bonds())
    {
        auto j = b.get().partner(i);

        // Check for H
        if (j->Z() == Elements::H)
        {
            ++nH;
            continue;
        }

        // Don't add a connectivity term if 'j' is already in the path
        if (std::find(path.begin(), path.end(), j) != path.end())
            continue;

        if (currentDepth < maxDepth)
            neta += fmt::format(",-{}({})", Elements::symbol(j->Z()), netaString(j, currentDepth + 1, maxDepth, path));
        else
            neta += fmt::format(",-{}", Elements::symbol(j->Z()));
    }

    if (i->Z() != Elements::H)
        neta += fmt::format(",nh={}", nH);
    return neta;
}

// Create from specified atom and its connectivity
bool NETADefinition::create(const SpeciesAtom *i, int maxDepth)
{
    std::vector<const SpeciesAtom *> path;
    definitionString_ = netaString(i, 0, maxDepth, path);
    return create();
}

// Set generating string
void NETADefinition::setDefinitionString(std::string_view definition) { definitionString_ = definition; }

// Return original generating string
std::string_view NETADefinition::definitionString() const { return definitionString_; }

// Return whether the definition is valid
bool NETADefinition::isValid() const { return valid_; }

void NETADefinition::setIdentifiers(std::vector<std::string> identifiers) { identifiers_ = identifiers; }
std::vector<std::string> NETADefinition::identifiers() { return identifiers_; }

/*
 * Matching
 */

// Return score of supplied atom for this definition
int NETADefinition::score(const SpeciesAtom *i) const
{
    NETAMatchedGroup matchPath(i);
    return rootNode_->score(i, matchPath);
}

// Return whether the supplied atom matches the definition
bool NETADefinition::matches(const SpeciesAtom *i) const { return score(i) != NETANode::NoMatch; }

// Return the path of matched atoms, including the target atom, if the definition matches
NETAMatchedGroup NETADefinition::matchedPath(const SpeciesAtom *i) const
{
    NETAMatchedGroup matchPath(i);
    if (rootNode_->score(i, matchPath) == NETANode::NoMatch)
        return {};

    return matchPath;
}
