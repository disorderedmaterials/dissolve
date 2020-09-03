/*
    *** NETA Visitor
    *** src/neta/NETAVisitor.h
    Copyright T. Youngs 2019-2020

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

#include "neta/NETAVisitor.h"
#include "data/ff.h"
#include "neta/presence.h"
#include "neta/ring.h"
#include "templates/optionalref.h"

/*
 * Creation Entry Point
 */

// Return the topmost context in the stack
NETANode *NETAVisitor::currentNETAContext() const
{
    // TODO Assert that we have a valid context?
    return contextStack_.back();
}

// Construct description within supplied object, from given tree
bool NETAVisitor::create(NETADefinition &neta, NETAParser::NetaContext *tree, const Forcefield *associatedForcefield)
{
    // Store pointer to definition and forcefield. and clear the passed definition
    neta_ = &neta;
    associatedForcefield_ = associatedForcefield;

    // Add the definition's root node as the topmost context in the stack
    contextStack_.push_back(neta_->rootNode());

    // Traverse the AST
    errorCounter_ = 0;
    visitChildren(tree);
    return errorCounter_ == 0;
}

/*
 * Visitor Overrides
 */

antlrcpp::Any NETAVisitor::visitNeta(NETAParser::NetaContext *context) { return visitChildren(context); }

antlrcpp::Any NETAVisitor::visitNode(NETAParser::NodeContext *context) { return visitChildren(context); }

antlrcpp::Any NETAVisitor::visitNodeSequence(NETAParser::NodeSequenceContext *context) { return visitChildren(context); }

antlrcpp::Any NETAVisitor::visitRingOnlyNode(NETAParser::RingOnlyNodeContext *context) { return visitChildren(context); }

antlrcpp::Any NETAVisitor::visitRingNodeSequence(NETAParser::RingNodeSequenceContext *context)
{
    return visitChildren(context);
}

antlrcpp::Any NETAVisitor::visitConnectionNode(NETAParser::ConnectionNodeContext *context)
{
    NETAConnectionNode *connection = currentNETAContext()->createConnectionNode();
    contextStack_.push_back(connection);

    auto result = visitChildren(context);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any NETAVisitor::visitPresenceNode(NETAParser::PresenceNodeContext *context)
{
    NETAPresenceNode *presence = currentNETAContext()->createPresenceNode();
    contextStack_.push_back(presence);

    auto result = visitChildren(context);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any NETAVisitor::visitRingNode(NETAParser::RingNodeContext *context)
{
    NETARingNode *ring = currentNETAContext()->createRingNode();
    contextStack_.push_back(ring);

    auto result = visitChildren(context);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any NETAVisitor::visitElementOrType(NETAParser::ElementOrTypeContext *context)
{
    if (context->Element())
    {
        Element &el = Elements::element(context->Element()->getText().c_str());
        return std::reference_wrapper(el);
    }
    else if (context->FFTypeName())
    {
        // Is a forcefield available to search?
        if (!associatedForcefield_)
        {
            ++errorCounter_;
            return Messenger::error("No associated forcefield, so can't use atom type targets in NETA definition.\n");
        }

        // Search for the atom type with the name specified (after removing leading '&' from string)
        const auto &at = associatedForcefield_->atomTypeByName(context->FFTypeName()->getText().substr(1).c_str());
        if (!at)
        {
            ++errorCounter_;
            return Messenger::error(
                "No forcefield atom type with name {} exists in forcefield '{}', so can't add it as a target.\n",
                context->FFTypeName()->getText().c_str(), associatedForcefield_->name());
        }

        return std::reference_wrapper(*at);
    }
    else if (context->FFTypeIndex())
    {
        // Is a forcefield available to search?
        if (!associatedForcefield_)
        {
            ++errorCounter_;
            return Messenger::error("No associated forcefield, so can't use atom type targets in NETA definition.\n");
        }

        // Search for the atom type with the index specified (after removing leading '&' from string)
        auto id = std::stoi(context->FFTypeIndex()->getText().substr(1));
        const auto &at = associatedForcefield_->atomTypeById(id);
        if (!at)
        {
            ++errorCounter_;
            return Messenger::error(
                "No forcefield atom type with id {} exists in forcefield '{}', so can't add it as a target.\n", id,
                associatedForcefield_->name());
        }

        return std::reference_wrapper(*at);
    }

    return nullptr;
}

antlrcpp::Any NETAVisitor::visitTargetList(NETAParser::TargetListContext *context)
{
    for (auto elementOrType : context->targets)
    {
        antlrcpp::Any target = visitElementOrType(elementOrType);
        if (target.is<std::reference_wrapper<Element>>())
        {
            auto elRef = target.as<std::reference_wrapper<Element>>();
            if (!currentNETAContext()->addElementTarget(elRef))
                return false;
        }
        else if (target.is<std::reference_wrapper<const ForcefieldAtomType>>())
        {
            auto atomTypeRef = target.as<std::reference_wrapper<const ForcefieldAtomType>>();
            if (!currentNETAContext()->addFFTypeTarget(atomTypeRef.get()))
                return false;
        }
        else
        {
            ++errorCounter_;
            return Messenger::error("Unrecognised item in target list.\n");
        }
    }

    return true;
}

antlrcpp::Any NETAVisitor::visitModifier(NETAParser::ModifierContext *context)
{
    // Check comparison operator
    if (!NETANode::comparisonOperators().isValid(context->ComparisonOperator()->getText().c_str()))
        return Messenger::error("'{}' is not a valid comparison operator.\n", context->ComparisonOperator()->getText().c_str());
    NETANode::ComparisonOperator op =
        NETANode::comparisonOperators().enumeration(context->ComparisonOperator()->getText().c_str());
    if (currentNETAContext()->isValidModifier(context->Keyword()->getText().c_str()))
    {
        if (!currentNETAContext()->setModifier(context->Keyword()->getText().c_str(), op, std::stoi(context->value->getText())))
            return Messenger::error("Failed to set modifier '{}' for the current context ({}).\n",
                                    context->Keyword()->getText().c_str(),
                                    NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()));
    }
    else
    {
        ++errorCounter_;
        return Messenger::error("'{}' is not a valid modifier keyword for the current context ({}).\n",
                                context->Keyword()->getText().c_str(),
                                NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()));
    }

    return visitChildren(context);
}

antlrcpp::Any NETAVisitor::visitFlag(NETAParser::FlagContext *context)
{
    if (currentNETAContext()->isValidFlag(context->Keyword()->getText().c_str()))
    {
        if (!currentNETAContext()->setFlag(context->Keyword()->getText().c_str(), true))
            return Messenger::error("Failed to set flag '{}' for the current context ({}).\n",
                                    context->Keyword()->getText().c_str(),
                                    NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()));
    }
    else
    {
        ++errorCounter_;
        return Messenger::error("'{}' is not a valid flag for the current context ({}).\n",
                                context->Keyword()->getText().c_str(),
                                NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()));
    }

    return visitChildren(context);
}
