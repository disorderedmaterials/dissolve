// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
#include <stdarg.h>
#include <string.h>

NETADefinition::NETADefinition() : rootNode_(nullptr) {}

/*
 * Data
 */

// Return root node pointer
std::shared_ptr<NETARootNode> NETADefinition::rootNode() { return rootNode_; }

// Create definition from stored definition string
bool NETADefinition::create(const Forcefield *associatedFF)
{
    // Create a new root node, overwriting the old one
    rootNode_ = std::make_shared<NETARootNode>(this);

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

    return true;
}

// Set definition string and create definition
bool NETADefinition::create(std::string_view definition, const Forcefield *associatedFF)
{
    definitionString_ = definition;
    return create(associatedFF);
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
    std::vector<const SpeciesAtom *> matchPath;
    return rootNode_->score(i, matchPath);
}
