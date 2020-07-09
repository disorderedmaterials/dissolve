/*
    *** NETA Definition Generator
    *** src/neta/generator.cpp
    Copyright T. Youngs 2015-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "neta/generator.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "data/elements.h"
#include "data/ff.h"
#include "neta/neta.h"
#include "neta/neta_grammar.hh"
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

// Static members
const Forcefield *NETADefinitionGenerator::associatedForcefield_ = NULL;
NETADefinition *NETADefinitionGenerator::definition_ = NULL;
NETADefinitionGenerator *NETADefinitionGenerator::generator_ = NULL;
std::vector<Element *> NETADefinitionGenerator::targetElements_;
std::vector<std::reference_wrapper<const ForcefieldAtomType>> NETADefinitionGenerator::targetAtomTypes_;
RefList<NETANode> NETADefinitionGenerator::contextStack_;
bool NETADefinitionGenerator::expectName_ = false;

NETADefinitionGenerator::NETADefinitionGenerator(NETADefinition &definition, const char *definitionText,
                                                 const Forcefield *associatedFF)
{
    // Clear any possible old data
    contextStack_.clear();
    clearTargets();
    expectName_ = false;

    definition_ = &definition;
    associatedForcefield_ = associatedFF;
    generator_ = this;

    // Add the rootNode of the definition as the first context
    contextStack_.append(definition.rootNode());

    // Initialise generator
    setSource(definitionText);
}

NETADefinitionGenerator::~NETADefinitionGenerator() {}

/*
 * Target Objects
 */

// Return target NETADefinition (static to allow NETADefinitionGenerator_parse() to use it)
NETADefinition *NETADefinitionGenerator::definition() { return definition_; }

// Return current NETADefinitionGenerator (static to allow NETADefinitionGenerator_parse() to use it)
NETADefinitionGenerator *NETADefinitionGenerator::generator() { return generator_; }

/*
 * Lexer
 */

// Set string source for lexer
void NETADefinitionGenerator::setSource(const char *definitionText)
{
    // Set parsing source, always ensuring that we have a terminating ';'
    definitionString_ = definitionText;
    definitionString_ += ';';

    stringPos_ = 0;
    stringLength_ = strlen(definitionString_);
    tokenStart_ = 0;
    functionStart_ = -1;

    Messenger::printVerbose("Parser source string is '%s', length is %i\n", definitionString_.get(), stringLength_);
}

// Get next character from current input stream
char NETADefinitionGenerator::getChar()
{
    char c = 0;

    // Are we at the end of the current string?
    if (stringPos_ == stringLength_)
        return 0;

    // Return current char
    c = definitionString_[stringPos_];
    stringPos_++;
    return c;
}

// Peek next character from current input stream
char NETADefinitionGenerator::peekChar() { return (stringPos_ == stringLength_ ? 0 : definitionString_[stringPos_]); }

// 'Replace' last character read from current input stream
void NETADefinitionGenerator::unGetChar() { --stringPos_; }

// Bison-required NETADefinitionGenerator_lex()
int NETADefinitionGenerator_lex()
{
    if (!NETADefinitionGenerator::definition())
        return 0;
    if (!NETADefinitionGenerator::generator())
        return 0;
    return NETADefinitionGenerator::generator()->lex();
}

// Parser lexer, called by yylex()
int NETADefinitionGenerator::lex()
{
    int n;
    bool done, hasExp;
    static CharString token;
    char c;
    token.clear();

    // Skip over whitespace
    while ((c = getChar()) == ' ' || c == '\t' || c == '\r' || c == '\n')
        ;

    if (c == 0)
        return 0;

    // Set this point as the start of our new token (for error reporting)
    tokenStart_ = stringPos_ - 1;

    /*
     * Number Detection - Either '.' or a digit begins a number
     */
    if (c == '.' || isdigit(c))
    {
        Messenger::printVerbose("NETA (%p): found the start of a number...\n", definition_);
        auto isInteger = (c != '.');
        hasExp = false;
        token += c;
        done = false;
        do
        {
            c = getChar();
            if (isdigit(c))
                token += c;
            else if (c == '.')
            {
                token += '.';
                isInteger = false;
            }
            else if ((c == 'e') || (c == 'E'))
            {
                // Check for previous exponential in number
                if (hasExp)
                {
                    printf("Error: Number has two exponentiations (e/E).\n");
                    return 0;
                }
                token += 'E';
                hasExp = true;
            }
            else if ((c == '-') || (c == '+'))
            {
                // We allow '-' or '+' only as part of an exponentiation, so if it is not preceeded by 'E' we
                // stop parsing
                if ((!token.isEmpty()) && (!(token.lastChar() == 'E')))
                {
                    unGetChar();
                    done = true;
                }
                else
                    token += c;
            }
            else
            {
                unGetChar();
                done = true;
            }
        } while (!done);

        // We now have the number as a text token...
        if (isInteger)
        {
            NETADefinitionGenerator_lval.integerConst = token.asInteger();
            Messenger::printVerbose("NETA (%p): found an integer constant [%s] [%i]\n", definition_, token.get(),
                                    NETADefinitionGenerator_lval.integerConst);
            return DISSOLVE_NETA_INTEGERCONSTANT;
        }
        else
        {
            NETADefinitionGenerator_lval.doubleConst = token.asDouble();
            Messenger::printVerbose("NETA (%p): found a double constant [%s] [%e]\n", definition_, token.get(),
                                    NETADefinitionGenerator_lval.doubleConst);
            return DISSOLVE_NETA_DOUBLECONSTANT;
        }
    }

    /*
     * Alpha-token
     */
    if (isalpha(c))
    {
        do
        {
            token += c;
            c = getChar();
        } while (isalnum(c) || (c == '_'));
        unGetChar();
        Messenger::printVerbose("NETA (%p): found an alpha token [%s]...\n", definition_, token.get());

        // Extended context?
        if (DissolveSys::sameString(token, "ring"))
        {
            Messenger::printVerbose("NETA : ...which is the ring keyword.\n");
            return DISSOLVE_NETA_RING;
        }

        // Modifier for the current context?
        if (context() && context()->isValidModifier(token))
        {
            // Check next character - if it looks like it's part of an operator, we'll assume it is a modifier
            char c2 = peekChar();
            if ((c2 == '=') || (c2 == '>') || (c2 == '<') || (c2 == '!'))
            {
                Messenger::printVerbose("NETA : ...which is a valid modifier for this context.\n");
                NETADefinitionGenerator_lval.name = token.get();
                return DISSOLVE_NETA_MODIFIER;
            }
        }

        // Flag for the current context?
        if (context() && context()->isValidFlag(token))
        {
            Messenger::printVerbose("NETA : ...which is a valid flag for this context.\n");
            NETADefinitionGenerator_lval.name = token.get();
            return DISSOLVE_NETA_FLAG;
        }

        // Chemical Element?
        if (!expectName_)
        {
            const Element &el = Elements::element(token);
            if (!el.isUnknown())
            {
                NETADefinitionGenerator_lval.elementZ = el.Z();
                Messenger::printVerbose("NETA (%p): ...which is an element symbol (Z=%i)", definition_, el.Z());
                return DISSOLVE_NETA_ELEMENT;
            }

            // If we get to here then we have found an unrecognised alphanumeric token
            Messenger::printVerbose("NETA (%p): ...which is unrecognised.\n", definition_);
            NETADefinitionGenerator_lval.name = token.get();
        }
        else
        {
            // Assumed generic name
            Messenger::printVerbose("NETA (%p): ...which is a generic name.\n", definition_);
            NETADefinitionGenerator_lval.name = token.get();
            return DISSOLVE_NETA_NAME;
        }

        Messenger::error("Unknown token '%s' encountered in NETA definition.\n", token.get());
        return 0;
    }

    // We have found a symbolic character (or a pair) that corresponds to an operator
    // Return immediately in the case of brackets, dash, comma, and ampersand
    if ((c == '(') || (c == ')') || (c == '-') || (c == ',') || (c == '&'))
    {
        Messenger::printVerbose("NETA (%p): found symbol [%c]\n", definition_, c);
        return c;
    }
    token += c;

    // Similarly, if the next character is a period, bracket or double quotes, return immediately
    char c2 = peekChar();
    if ((c2 == '.') || (c2 == '(') || (c2 == ')') || (c2 == ';') || (c2 == '{') || (c2 == '}') || (c2 == '"'))
        return c;

    // If next character is '-', return now if previous char was *not* another '-'
    if ((c2 == '-') && (c != '-'))
        return c;

    // If it is 'punctuation', add this second character to our operator and search for it
    if (ispunct(c2))
    {
        c = getChar();
        token += c;
    }

    Messenger::printVerbose("NETA (%p): found symbol [%s]\n", definition_, token.get());
    if (NETANode::comparisonOperators().isValid(token.get()))
    {
        NETADefinitionGenerator_lval.valueOperator = NETANode::comparisonOperators().enumeration(token.get());
        return DISSOLVE_NETA_OPERATOR;
    }

    return 0;
}

/*
 * Creation
 */

// Add element target to array (by Z)
bool NETADefinitionGenerator::addElementTarget(int elementZ)
{
    Element &el = Elements::element(elementZ);
    if (el.isUnknown())
        return Messenger::error("Unknown element Z %i passed to NETADefinitionGenerator::addTarget().\n", elementZ);

    targetElements_.push_back(&el);

    return true;
}

// Add atomtype target to array (by index)
bool NETADefinitionGenerator::addAtomTypeTarget(int id)
{
    // Is a forcefield available to search?
    if (!associatedForcefield_)
        return false;

    auto optRef = associatedForcefield_->atomTypeById(id);
    if (!optRef)
        return Messenger::error(
            "No forcefield atom type with index %i exists in forcefield '%s', so can't add it as a target.\n", id,
            associatedForcefield_->name());

    targetAtomTypes_.push_back(*optRef);

    return true;
}

// Add atomtype target to array (by name)
bool NETADefinitionGenerator::addAtomTypeTarget(const char *typeName)
{
    // Is a forcefield available to search?
    if (!associatedForcefield_)
        return false;

    auto optTypeRef = associatedForcefield_->atomTypeByName(typeName);
    if (!optTypeRef)
        return Messenger::error("Unknown forcefield atom type '%s' passed to NETADefinitionGenerator::addTarget().\n",
                                typeName);

    targetAtomTypes_.push_back(*optTypeRef);

    return true;
}

// Return target Elements array
std::vector<Element *> NETADefinitionGenerator::targetElements() { return targetElements_; }

// Return target ForcefieldAtomTypes array
std::vector<std::reference_wrapper<const ForcefieldAtomType>> NETADefinitionGenerator::targetAtomTypes() { return targetAtomTypes_; }

// Clear element / atomtype targets
void NETADefinitionGenerator::clearTargets()
{
    targetElements_.clear();
    targetAtomTypes_.clear();
}

// Return topmost context
NETANode *NETADefinitionGenerator::context() { return contextStack_.lastItem(); }

// Push branch in last node of topmost context onto the context stack
bool NETADefinitionGenerator::pushContext()
{
    if (!context())
        return Messenger::error("No context exists, so can't push any branch.\n");

    if (!context()->lastBranchNode())
        return Messenger::error("No nodes exist in the current context, so can't push any branch.\n");

    contextStack_.append(context()->lastBranchNode());

    return true;
}

// Pop topmost context
void NETADefinitionGenerator::popContext() { contextStack_.removeLast(); }

// Set whether to recognise text elements as generic names
void NETADefinitionGenerator::setExpectName(bool b) { expectName_ = b; }

// Static generation functions
bool NETADefinitionGenerator::generate(NETADefinition &neta, const char *netaDefinition, const Forcefield *associatedFF)
{
    // Set the source string
    neta.setDefinitionString(netaDefinition);
    // 	if ((netaDefinition == NULL) || (netaDefinition[0] == '\0')) return true;

    // Create a generator
    NETADefinitionGenerator generator(neta, neta.definitionString(), associatedFF);

    // Generate definition
    auto result = NETADefinitionGenerator_parse() == 0;
    if (!result)
        definition_->clear();

    return result;
}
