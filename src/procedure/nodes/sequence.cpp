// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/sequence.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "procedure/nodes/registry.h"

SequenceProcedureNode::SequenceProcedureNode(ProcedureNode::NodeContext context, const Procedure *procedure, NodeRef owner,
                                             std::string_view blockKeyword)
    : ProcedureNode(ProcedureNode::NodeType::Sequence)
{
    context_ = context;
    procedure_ = procedure;
    owner_ = std::move(owner);
    blockKeyword_ = blockKeyword;
}

SequenceProcedureNode::~SequenceProcedureNode() { clear(); }

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool SequenceProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context != ProcedureNode::NoContext);
}

/*
 * Node Keywords
 */

// Return enum option info for SequenceNodeKeyword
EnumOptions<SequenceProcedureNode::SequenceNodeKeyword> SequenceProcedureNode::sequenceNodeKeywords()
{
    return EnumOptions<SequenceProcedureNode::SequenceNodeKeyword>("SequenceNodeKeyword", {});
}

/*
 * Node Contents
 */

// Clear all data
void SequenceProcedureNode::clear() { sequence_.clear(); }

// Add (own) node into sequence, checking the context
void SequenceProcedureNode::addNode(NodeRef nodeToAdd)
{
    assert(nodeToAdd);

    // Set us as its scope
    nodeToAdd->setScope(std::dynamic_pointer_cast<SequenceProcedureNode>(shared_from_this()));

    // Check context
    if (!nodeToAdd->isContextRelevant(context_))
        throw(std::runtime_error(fmt::format("Node '{}' (type = '{}') is not relevant to the '{}' context.\n",
                                             nodeToAdd->name(), ProcedureNode::nodeTypes().keyword(nodeToAdd->type()),
                                             ProcedureNode::nodeContexts().keyword(context_))));

    // If the node hasn't been given a name, generate a unique one for it now based on its type. Otherwise, check for a clash
    if (nodeToAdd->name().empty())
    {
        auto n = 1;
        while (nodeExists(fmt::format("{}{:02d}", ProcedureNode::nodeTypes().keyword(nodeToAdd->type()), n)))
            ++n;
        nodeToAdd->setName(fmt::format("{}{:02d}", ProcedureNode::nodeTypes().keyword(nodeToAdd->type()), n));
    }
    else if (nodeExists(nodeToAdd->name()))
        throw(std::runtime_error(fmt::format(
            "Can't have duplicate node names in the same procedure - node '{}' already exists.\n", nodeToAdd->name())));

    sequence_.push_back(nodeToAdd);
}

// Return sSequential node list
const std::vector<NodeRef> &SequenceProcedureNode::sequence() const { return sequence_; }

// Return number of nodes in sequence
int SequenceProcedureNode::nNodes() const { return sequence_.size(); }

/*
 * Scope
 */

// Return named node if it exists anywhere in our sequence or below, and optionally matches the type given
NodeRef SequenceProcedureNode::searchNodes(std::string_view name, NodeRef excludeNode,
                                           std::optional<ProcedureNode::NodeType> optNodeType,
                                           std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    for (auto node : sequence_)
    {
        // Does this node match the supplied name?
        if (node != excludeNode)
        {
            if (DissolveSys::sameString(node->name(), name))
            {
                // Check type / class
                if ((!optNodeType && !optNodeClass) || (optNodeType && optNodeType.value() == node->type()) ||
                    (optNodeClass && optNodeClass.value() == node->nodeClass()))
                    return node;
            }
        }

        // If the node has a branch, descend into it
        if (node->hasBranch())
        {
            auto result = node->branch()->searchNodes(name, excludeNode, optNodeType, optNodeClass);
            if (result)
                return result;
        }
    }

    return nullptr;
}

// Search through the Procedure for the named parameter
std::shared_ptr<ExpressionVariable>
SequenceProcedureNode::searchParameters(std::string_view name,
                                        const std::shared_ptr<ExpressionVariable> &excludeParameter) const
{
    for (auto node : sequence_)
    {
        // Does this node have a parameter by this name?
        auto result = node->getParameter(name, excludeParameter);
        if (result)
            return result;

        // If the node has a branch, descend into it
        if (node->hasBranch())
        {
            result = node->branch()->searchParameters(name, excludeParameter);
            if (result)
                return result;
        }
    }

    return nullptr;
}

// Return parent Procedure to which this sequence belongs
const Procedure *SequenceProcedureNode::procedure() const { return procedure_; }

// Return this sequences owner
NodeRef SequenceProcedureNode::owner() const { return owner_; }

// Return the context of the sequence
ProcedureNode::NodeContext SequenceProcedureNode::sequenceContext() const { return context_; }

// Return named node if present in this sequence, and which matches the (optional) type given
ConstNodeRef SequenceProcedureNode::node(std::string_view name, std::optional<ProcedureNode::NodeType> optNodeType,
                                         std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    for (auto node : sequence_)
    {
        if (DissolveSys::sameString(node->name(), name))
        {
            // Check type / class
            if ((!optNodeType && !optNodeClass) || (optNodeType && optNodeType.value() == node->type()) ||
                (optNodeClass && optNodeClass.value() == node->nodeClass()))
                return node;
        }

        // If the node has a branch, recurse in to that
        if (node->hasBranch())
        {
            auto branchNode = node->branch()->node(name, optNodeType, optNodeClass);
            if (branchNode)
                return branchNode;
        }
    }

    return nullptr;
}

// Return list of nodes (of specified type / class) present in the Procedure
std::vector<ConstNodeRef> SequenceProcedureNode::nodes(std::optional<ProcedureNode::NodeType> optNodeType,
                                                       std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    std::vector<ConstNodeRef> matches;

    for (auto node : sequence_)
    {
        // Check type / class
        if ((!optNodeType && !optNodeClass) || (optNodeType && optNodeType.value() == node->type()) ||
            (optNodeClass && optNodeClass.value() == node->nodeClass()))
            matches.push_back(node);

        // If the node has a branch, recurse in to that
        if (node->hasBranch())
        {
            auto branchNodes = node->branch()->nodes(optNodeType, optNodeClass);
            std::copy(branchNodes.begin(), branchNodes.end(), std::back_inserter(matches));
        }
    }

    return matches;
}

// Return named node if it is currently in scope (and matches the type / class given)
ConstNodeRef SequenceProcedureNode::nodeInScope(ConstNodeRef queryingNode, std::string_view name, ConstNodeRef excludeNode,
                                                std::optional<ProcedureNode::NodeType> optNodeType,
                                                std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    // If one was given, start from the querying node and work backwards...
    if (queryingNode)
    {
        auto range = QueryRange(queryingNode, sequence_);
        assert(!range.empty());

        for (auto node : range)
        {
            if (node == excludeNode)
                continue;

            if (DissolveSys::sameString(node->name(), name))
            {
                // Check type / class
                if ((!optNodeType && !optNodeClass) || (optNodeType && optNodeType.value() == node->type()) ||
                    (optNodeClass && optNodeClass.value() == node->nodeClass()))
                    return node;
            }
        }
    }

    // Not in our list. Recursively check our owner
    if (owner_)
        return owner_->nodeInScope(name, excludeNode, optNodeType, optNodeClass);

    // Not found
    return nullptr;
}

// Return list of nodes in scope (and matching the type / class given)
std::vector<ConstNodeRef> SequenceProcedureNode::nodesInScope(ConstNodeRef queryingNode,
                                                              std::optional<ProcedureNode::NodeType> optNodeType,
                                                              std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    std::vector<ConstNodeRef> matches;

    // If one was given, start from the querying node and work backwards...
    if (queryingNode)
    {
        auto range = QueryRange(queryingNode, sequence_);
        assert(!range.empty());

        for (auto node : range)
        {
            // Check type / class
            if ((!optNodeType && !optNodeClass) || (optNodeType && optNodeType.value() == node->type()) ||
                (optNodeClass && optNodeClass.value() == node->nodeClass()))
                matches.push_back(node);
        }
    }

    // Not in our list. Recursively check our owner
    if (owner_)
    {
        auto parentMatches = owner_->nodesInScope(optNodeType, optNodeClass);
        std::copy(parentMatches.begin(), parentMatches.end(), std::back_inserter(matches));
    }

    return matches;
}

// Return named node if it exists anywhere in the same Procedure (and matches the type / class given)
ConstNodeRef SequenceProcedureNode::nodeExists(std::string_view name, NodeRef excludeNode,
                                               std::optional<ProcedureNode::NodeType> optNodeType,
                                               std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    // First, bubble up to the topmost sequence (which should be the Procedure's rootSequence_)
    if (owner_)
        return owner_->scope()->nodeExists(name, excludeNode, optNodeType, optNodeClass);

    // No parent node, so we must be the topmost sequence - run the search from here
    return searchNodes(name, excludeNode, optNodeType, optNodeClass);
}

// Return the named parameter if it is currently in scope
std::shared_ptr<ExpressionVariable>
SequenceProcedureNode::parameterInScope(NodeRef queryingNode, std::string_view name,
                                        const std::shared_ptr<ExpressionVariable> &excludeParameter)
{
    auto range = QueryRange(queryingNode, sequence_);
    if (queryingNode)
        assert(!range.empty());

    for (auto node : range)
    {
        auto param = node->getParameter(name, excludeParameter);
        if (param)
            return param;
    }

    // Not in our list. Recursively check our owner
    if (owner_)
        return owner_->parameterInScope(name, excludeParameter);

    // Not found
    return nullptr;
}

// Return whether the named parameter exists in this sequence or its children (branches)
std::shared_ptr<ExpressionVariable>
SequenceProcedureNode::parameterExists(std::string_view name, const std::shared_ptr<ExpressionVariable> &excludeParameter) const
{
    // First, bubble up to the topmost sequence (which should be the Procedure's rootSequence_)
    if (owner_)
        return owner_->scope()->parameterExists(name, excludeParameter);

    // No parent node, so we must be the topmost sequence - run the search from here
    return searchParameters(name, excludeParameter);
}

// Create and return reference list of parameters in scope
std::vector<std::shared_ptr<ExpressionVariable>> SequenceProcedureNode::parametersInScope(ConstNodeRef queryingNode)
{
    auto range = QueryRange(queryingNode, sequence_);
    if (queryingNode)
        assert(!range.empty());

    std::vector<std::shared_ptr<ExpressionVariable>> parameters;

    // Start from the target node and work backwards...
    for (auto node : range)
    {
        auto optOtherParams = node->parameters();
        if (optOtherParams)
        {
            const std::vector<std::shared_ptr<ExpressionVariable>> otherParams = (*optOtherParams);
            parameters.insert(parameters.end(), otherParams.begin(), otherParams.end());
        }
    }

    // Recursively check our owner
    if (owner_)
    {
        auto optOtherParams = owner_->parameters();
        if (optOtherParams)
        {
            const std::vector<std::shared_ptr<ExpressionVariable>> otherParams = (*optOtherParams);
            parameters.insert(parameters.end(), otherParams.begin(), otherParams.end());
        }
    }

    return parameters;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool SequenceProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Loop over nodes in the list, preparing each in turn
    for (auto node : sequence_)
        if (!node->prepare(procedureContext))
            return false;

    return true;
}

// Execute node
bool SequenceProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // If there are no nodes, just exit now
    if (sequence_.empty())
        return true;

    // Loop over nodes in the list, executing each in turn
    for (auto node : sequence_)
        if (!node->execute(procedureContext))
            return false;

    return true;
}

// Finalise any necessary data after execution
bool SequenceProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // Loop over nodes in the list, finalising each in turn
    for (auto node : sequence_)
        if (!node->finalise(procedureContext))
            return false;

    return true;
}

/*
 * Read / Write
 */

// Return block keyword for current context
std::string_view SequenceProcedureNode::blockKeyword() const { return blockKeyword_; }

// Read structure from specified LineParser
bool SequenceProcedureNode::deserialise(LineParser &parser, const CoreData &coreData)
{
    const auto blockTerminationKeyword = fmt::format("End{}", blockKeyword_);

    // Read until we encounter the block-ending keyword, or we fail for some reason
    while (!parser.eofOrBlank())
    {
        // Read and parse the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Is the first argument the block termination keyword for the current context?
        if (DissolveSys::sameString(parser.argsv(0), blockTerminationKeyword))
            break;

        // Is the first argument on the current line a valid control keyword?
        if (sequenceNodeKeywords().isValid(parser.argsv(0)))
        {
            SequenceNodeKeyword nk = sequenceNodeKeywords().enumeration(parser.argsv(0));
            switch (nk)
            {
                case (SequenceProcedureNode::nSequenceNodeKeywords):
                    break;
            }
        }

        // Not a control keyword, so must be a node type
        if (!ProcedureNode::nodeTypes().isValid(parser.argsv(0)))
            return Messenger::error("Unrecognised node type '{}' found.\n", parser.argsv(0));

        // Create a new node of the specified type
        auto newNode = ProcedureNodeRegistry::create(ProcedureNode::nodeTypes().enumeration(parser.argsv(0)));

        // Set the name of the node if it is required / provided
        if (newNode->mustBeNamed() && !parser.hasArg(1))
            Messenger::error("A name must be given explicitly to a node of type {}.\n",
                             ProcedureNode::nodeTypes().keyword(newNode->type()));

        if (parser.hasArg(1))
        {
            // Set the name (to nice-ify it)
            newNode->setName(parser.argsv(1));

            // Check for clash of names with existing node in scope
            if (!sequence_.empty() && nodeInScope(sequence_.back(), newNode->name()))
                return Messenger::error("A node named '{}' is already in scope.\n", newNode->name());
        }

        // Add the node
        addNode(newNode);

        // Read the new node
        if (!newNode->deserialise(parser, coreData))
            return Messenger::error("Failed to read node sequence.\n");
    }

    return true;
}

// Write structure to specified LineParser
bool SequenceProcedureNode::serialise(LineParser &parser, std::string_view prefix)
{
    // Block Start - should have already been written by the calling function, since we don't know the keyword we are linked
    // to

    // Loop over nodes in this sequence
    for (auto node : sequence_)
        if (!node->serialise(parser, prefix))
            return false;

    // Block End - will be written by the calling function, since we don't know the keyword we are linked to

    return true;
}

SequenceProcedureNode::QueryRange::QueryRange(ConstNodeRef queryingNode, const std::vector<NodeRef> &seq)
{
    start_ = std::find(seq.rbegin(), seq.rend(), queryingNode);
    stop_ = seq.rend();
}
std::vector<NodeRef>::const_reverse_iterator SequenceProcedureNode::QueryRange::begin() { return start_; }
std::vector<NodeRef>::const_reverse_iterator SequenceProcedureNode::QueryRange::end() { return stop_; }
bool SequenceProcedureNode::QueryRange::empty() { return start_ == stop_; }
void SequenceProcedureNode::QueryRange::next() { start_++; }

// Express as a serialisable value
SerialisedValue SequenceProcedureNode::serialise() const
{
    SerialisedValue node;
    for (auto n : sequence_)
    {
        // node.push_back(n->serialise());
        SerialisedValue inner = n->serialise();
        inner["type"] = n->nodeTypes().serialise(n->type());
        node[std::string(n->name())] = inner;
    }
    return node;
}

// Read values from a serialisable value
void SequenceProcedureNode::deserialise(const SerialisedValue &node, const CoreData &data)
{
    for (auto &[k, v] : node.as_table())
    {
        ProcedureNode::NodeType type = ProcedureNode::nodeTypes().deserialise(v.at("type"));
        auto result = ProcedureNodeRegistry::create(type);
        addNode(result);
        result->deserialise(v, data);
        result->setName(k);
    }
}
