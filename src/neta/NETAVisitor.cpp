// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "neta/NETAVisitor.h"
#include "data/ff/ff.h"
#include "neta/NETAErrorListeners.h"
#include "neta/or.h"
#include "neta/presence.h"
#include "neta/ring.h"
#include "templates/optionalref.h"

/*
 * Creation Entry Point
 */

// Return the topmost context in the stack
std::shared_ptr<NETANode> NETAVisitor::currentNETAContext() const
{
    assert(!contextStack_.empty());
    return contextStack_.back();
}

// Construct description within supplied object, from given tree
void NETAVisitor::create(NETADefinition &neta, NETAParser::NetaContext *tree, const Forcefield *associatedForcefield)
{
    // Store pointer to definition and forcefield. and clear the passed definition
    neta_ = &neta;
    associatedForcefield_ = associatedForcefield;

    // Add the definition's root node as the topmost context in the stack
    contextStack_.clear();
    contextStack_.push_back(neta_->rootNode());

    // Traverse the AST
    visitChildren(tree);
}

/*
 * Visitor Overrides
 */

antlrcpp::Any NETAVisitor::visitOrNode(NETAParser::OrNodeContext *context)
{
    auto orLogic = currentNETAContext()->createOrNode();
    contextStack_.push_back(orLogic);

    auto result = visitChildren(context);

    contextStack_.pop_back();

    return true;
}

antlrcpp::Any NETAVisitor::visitConnectionNode(NETAParser::ConnectionNodeContext *context)
{
    auto connection = currentNETAContext()->createConnectionNode();
    contextStack_.push_back(connection);

    if (context->Not())
        connection->setReverseLogic();

    auto result = visitChildren(context);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any NETAVisitor::visitPresenceNode(NETAParser::PresenceNodeContext *context)
{
    auto presence = currentNETAContext()->createPresenceNode();
    contextStack_.push_back(presence);

    if (context->Not())
        presence->setReverseLogic();

    auto result = visitChildren(context);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any NETAVisitor::visitRingNode(NETAParser::RingNodeContext *context)
{
    auto ring = currentNETAContext()->createRingNode();
    contextStack_.push_back(ring);

    if (context->Not())
        ring->setReverseLogic();

    auto result = visitChildren(context);

    contextStack_.pop_back();

    return result;
}

antlrcpp::Any NETAVisitor::visitElementOrType(NETAParser::ElementOrTypeContext *context)
{
    if (context->Element())
    {
        auto el = Elements::element(context->Element()->getText());
        if (!el)
            throw(NETAExceptions::NETASyntaxException(
                fmt::format("'{}' is not a recognised element.\n", context->Element()->getText())));
        return el;
    }
    else if (context->FFTypeName())
    {
        // Is a forcefield available to search?
        if (!associatedForcefield_)
            throw(NETAExceptions::NETASyntaxException(
                "No associated forcefield, so can't use atom type targets in NETA definition."));

        // Search for the atom type with the name specified (after removing leading '&' from string)
        const auto &at = associatedForcefield_->atomTypeByName(context->FFTypeName()->getText().substr(1));
        if (!at)
            throw(NETAExceptions::NETASyntaxException(
                fmt::format("No forcefield atom type with name {} exists in forcefield '{}', so can't add it as a target.",
                            context->FFTypeName()->getText(), associatedForcefield_->name())));

        return std::reference_wrapper(*at);
    }
    else if (context->FFTypeIndex())
    {
        // Is a forcefield available to search?
        if (!associatedForcefield_)
            throw(NETAExceptions::NETASyntaxException(
                "No associated forcefield, so can't use atom type targets in NETA definition."));

        // Search for the atom type with the index specified (after removing leading '&' from string)
        auto id = std::stoi(context->FFTypeIndex()->getText().substr(1));
        const auto &at = associatedForcefield_->atomTypeById(id);
        if (!at)
            throw(NETAExceptions::NETASyntaxException(
                fmt::format("No forcefield atom type with id {} exists in forcefield '{}', so can't add it as a target.", id,
                            associatedForcefield_->name())));

        return std::reference_wrapper(*at);
    }

    throw(NETAExceptions::NETASyntaxException(
        fmt::format("'{]' is not an element symbol, type name, or type index.", context->getText())));
}

antlrcpp::Any NETAVisitor::visitTargetList(NETAParser::TargetListContext *context)
{
    for (auto elementOrType : context->targets)
    {
        antlrcpp::Any target = visitElementOrType(elementOrType);
        if (target.is<Elements::Element>())
        {
            auto Z = target.as<Elements::Element>();
            if (!currentNETAContext()->addElementTarget(Z))
                throw(NETAExceptions::NETASyntaxException("Failed to add element to target list."));
        }
        else if (target.is<std::reference_wrapper<const ForcefieldAtomType>>())
        {
            auto atomTypeRef = target.as<std::reference_wrapper<const ForcefieldAtomType>>();
            if (!currentNETAContext()->addFFTypeTarget(atomTypeRef.get()))
                throw(NETAExceptions::NETASyntaxException("Failed to add atom type to target list."));
        }
        else
            throw(NETAExceptions::NETASyntaxException("Unrecognised item in target list."));
    }

    return true;
}

antlrcpp::Any NETAVisitor::visitModifier(NETAParser::ModifierContext *context)
{
    // Check comparison operator
    if (!NETANode::comparisonOperators().isValid(context->ComparisonOperator()->getText()))
        return Messenger::error("'{}' is not a valid comparison operator.\n", context->ComparisonOperator()->getText());
    NETANode::ComparisonOperator op = NETANode::comparisonOperators().enumeration(context->ComparisonOperator()->getText());
    if (currentNETAContext()->isValidModifier(context->Keyword()->getText()))
    {
        if (!currentNETAContext()->setModifier(context->Keyword()->getText(), op, std::stoi(context->value->getText())))
            throw(NETAExceptions::NETASyntaxException(
                fmt::format("Failed to set modifier '{}' for the current context ({}).", context->Keyword()->getText(),
                            NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()))));
    }
    else
        throw(NETAExceptions::NETASyntaxException(
            fmt::format("'{}' is not a valid modifier keyword for the current context ({}).", context->Keyword()->getText(),
                        NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()))));

    return visitChildren(context);
}

antlrcpp::Any NETAVisitor::visitOption(NETAParser::OptionContext *context)
{
    // Check comparison operator - must be either '=' or '!='
    if (!NETANode::comparisonOperators().isValid(context->ComparisonOperator()->getText()))
        return Messenger::error("'{}' is not a valid comparison operator.\n", context->ComparisonOperator()->getText());
    NETANode::ComparisonOperator op = NETANode::comparisonOperators().enumeration(context->ComparisonOperator()->getText());
    if (op != NETANode::ComparisonOperator::EqualTo && op != NETANode::ComparisonOperator::NotEqualTo)
        throw(NETAExceptions::NETASyntaxException(fmt::format(
            "Option '{}' may only use the equal to ('=') or not equal to ('!=') operators.", context->opt->getText())));

    if (currentNETAContext()->isValidOption(context->opt->getText()))
    {
        if (!currentNETAContext()->setOption(context->opt->getText(), op, context->value->getText()))
            throw(NETAExceptions::NETASyntaxException(
                fmt::format("Failed to set option '{}' for the current context ({}).", context->opt->getText(),
                            NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()))));
    }
    else
        throw(NETAExceptions::NETASyntaxException(
            fmt::format("'{}' is not a valid option keyword for the current context ({}).", context->opt->getText(),
                        NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()))));

    return visitChildren(context);
}

antlrcpp::Any NETAVisitor::visitFlag(NETAParser::FlagContext *context)
{
    if (currentNETAContext()->isValidFlag(context->Keyword()->getText()))
    {
        if (!currentNETAContext()->setFlag(context->Keyword()->getText(), true))
            throw(NETAExceptions::NETASyntaxException(
                fmt::format("Failed to set flag '{}' for the current context ({}).", context->Keyword()->getText(),
                            NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()))));
    }
    else
        throw(NETAExceptions::NETASyntaxException(
            fmt::format("'{}' is not a valid flag for the current context ({}).", context->Keyword()->getText(),
                        NETANode::nodeTypes().keyword(currentNETAContext()->nodeType()))));

    return visitChildren(context);
}
