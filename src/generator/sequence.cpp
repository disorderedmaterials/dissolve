// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/sequence.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "generator/registry.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/nodeValueEnumOptions.h"
#include "keywords/nodeVector.h"
#include "keywords/vec3NodeValue.h"

GeneratorNodeSequence::GeneratorNodeSequence(OptionalReferenceWrapper<GeneratorNode> owner, std::string_view blockKeyword)
    : owner_(owner), blockKeyword_(blockKeyword)
{
}

GeneratorNodeSequence::~GeneratorNodeSequence() { clear(); }

/*
 * Node Contents
 */

// Clear all data
void GeneratorNodeSequence::clear() { sequence_.clear(); }

// Append specified node to the sequence, or optionally insert at index
void GeneratorNodeSequence::appendNode(NodeRef node, std::optional<int> insertAtIndex)
{
    assert(node);

    // Set us as its scope
    node->setScope(*this);

    // If the node hasn't been given a name, generate a unique one for it now based on its type. Otherwise, check for a clash
    if (node->name().empty())
    {
        auto n = 1;
        while (nodeExists(fmt::format("{}{:02d}", GeneratorNode::nodeTypes().keyword(node->type()), n)))
            ++n;
        node->setName(fmt::format("{}{:02d}", GeneratorNode::nodeTypes().keyword(node->type()), n));
    }
    else if (nodeExists(node->name()))
        throw(std::runtime_error(
            fmt::format("Can't have duplicate node names in the same procedure - node '{}' already exists.\n", node->name())));

    if (insertAtIndex)
        sequence_.insert(sequence_.begin() + *insertAtIndex, node);
    else
        sequence_.push_back(node);
}

// Insert empty node at specified position
void GeneratorNodeSequence::insertEmpty(int index)
{
    if (index >= sequence_.size())
        sequence_.push_back(nullptr);
    else
        sequence_.insert(sequence_.begin() + index, nullptr);
}

// Return sequential node list
std::vector<NodeRef> &GeneratorNodeSequence::sequence() { return sequence_; }
const std::vector<NodeRef> &GeneratorNodeSequence::sequence() const { return sequence_; }

// Return number of nodes in sequence
int GeneratorNodeSequence::nNodes() const { return sequence_.size(); }

// Return whether the sequence is empty
bool GeneratorNodeSequence::empty() const { return sequence_.empty(); }

// Remove a node
bool GeneratorNodeSequence::removeNode(NodeRef node)
{
    // Find the node in the sequence
    auto it = std::find_if(sequence_.begin(), sequence_.end(), [node](const auto &n) { return n.get() == node.get(); });
    if (it != sequence_.end())
    {
        sequence_.erase(it);
        return true;
    }
    return false;
}

/*
 * Scope
 */

// Return named node if it exists anywhere in our sequence or below, and optionally matches the type given
NodeRef GeneratorNodeSequence::searchNodes(std::string_view name, const ConstNodeRef &excludeNode,
                                           const GeneratorNode::NodeTypeVector &allowedNodeTypes) const
{
    for (const auto &node : sequence_)
    {
        // Does this node match the supplied name?
        if (node != excludeNode)
        {
            if (DissolveSys::sameString(node->name(), name))
            {
                // Check type
                if (allowedNodeTypes.empty() ||
                    std::find(allowedNodeTypes.begin(), allowedNodeTypes.end(), node->type()) != allowedNodeTypes.end())
                    return node;
            }
        }

        // If the node has a branch, descend into it
        if (node->branch())
        {
            auto result = node->branch()->get().searchNodes(name, excludeNode, allowedNodeTypes);
            if (result)
                return result;
        }
    }

    return nullptr;
}

// Search through the sequence for the named parameter
std::shared_ptr<ExpressionVariable>
GeneratorNodeSequence::searchParameters(std::string_view name,
                                        const std::shared_ptr<ExpressionVariable> &excludeParameter) const
{
    for (const auto &node : sequence_)
    {
        // Does this node have a parameter by this name?
        auto result = node->getParameter(name, excludeParameter);
        if (result)
            return result;

        // If the node has a branch, descend into it
        if (node->branch())
        {
            result = node->branch()->get().searchParameters(name, excludeParameter);
            if (result)
                return result;
        }
    }

    return nullptr;
}

// Return this sequence's owner
OptionalReferenceWrapper<GeneratorNode> GeneratorNodeSequence::owner() const { return owner_; }

// Return named node if present in this sequence, and which matches the (optional) type given
ConstNodeRef GeneratorNodeSequence::node(std::string_view name, const ConstNodeRef &excludeNode,
                                         const GeneratorNode::NodeTypeVector &allowedNodeTypes) const
{
    for (const auto &node : sequence_)
    {
        if (node != excludeNode && DissolveSys::sameString(node->name(), name))
        {
            // Check type
            if (allowedNodeTypes.empty() ||
                std::find(allowedNodeTypes.begin(), allowedNodeTypes.end(), node->type()) != allowedNodeTypes.end())
                return node;
        }

        // If the node has a branch, recurse in to that
        if (node->branch())
        {
            auto branchNode = node->branch()->get().node(name, excludeNode, allowedNodeTypes);
            if (branchNode)
                return branchNode;
        }
    }

    return nullptr;
}

// Return list of nodes (of specified type / class) present in the sequence and its children
std::vector<ConstNodeRef> GeneratorNodeSequence::nodes(const GeneratorNode::NodeTypeVector &allowedNodeTypes) const
{
    std::vector<ConstNodeRef> matches;

    for (const auto &node : sequence_)
    {
        // Check type
        if (allowedNodeTypes.empty() ||
            std::find(allowedNodeTypes.begin(), allowedNodeTypes.end(), node->type()) != allowedNodeTypes.end())
            matches.push_back(node);

        // If the node has a branch, recurse in to that
        if (node->branch())
        {
            auto branchNodes = node->branch()->get().nodes(allowedNodeTypes);
            std::copy(branchNodes.begin(), branchNodes.end(), std::back_inserter(matches));
        }
    }

    return matches;
}

// Return named node if it is currently in scope (and matches the type / class given)
ConstNodeRef GeneratorNodeSequence::nodeInScope(ConstNodeRef queryingNode, std::string_view name,
                                                const ConstNodeRef &excludeNode,
                                                const GeneratorNode::NodeTypeVector &allowedNodeTypes) const
{
    // If one was given, start from the querying node and work backwards...
    if (queryingNode)
    {
        auto range = QueryRange(queryingNode, sequence_);
        assert(!range.empty());

        for (const auto &node : range)
        {
            if (node == excludeNode)
                continue;

            if (DissolveSys::sameString(node->name(), name))
            {
                // Check type
                if (allowedNodeTypes.empty() ||
                    std::find(allowedNodeTypes.begin(), allowedNodeTypes.end(), node->type()) != allowedNodeTypes.end())
                    return node;
            }
        }
    }

    // Not in our list. Recursively check our owner
    if (owner_)
        return owner_->get().getNodeInScope(name, excludeNode, allowedNodeTypes);

    // Not found
    return nullptr;
}

// Return list of nodes in scope (and matching the type / class given)
std::vector<ConstNodeRef> GeneratorNodeSequence::nodesInScope(ConstNodeRef queryingNode,
                                                              const GeneratorNode::NodeTypeVector &allowedNodeTypes) const
{
    std::vector<ConstNodeRef> matches;

    // If one was given, start from the querying node and work backwards...
    if (queryingNode)
    {
        auto range = QueryRange(queryingNode, sequence_);
        assert(!range.empty());

        for (const auto &node : range)
        {
            // Check type
            if (allowedNodeTypes.empty() ||
                std::find(allowedNodeTypes.begin(), allowedNodeTypes.end(), node->type()) != allowedNodeTypes.end())
                matches.push_back(node);
        }
    }

    // Not in our list. Recursively check our owner
    if (owner_)
    {
        auto parentMatches = owner_->get().getNodesInScope(allowedNodeTypes);
        std::copy(parentMatches.begin(), parentMatches.end(), std::back_inserter(matches));
    }

    return matches;
}

// Return named node if it exists anywhere in the same Generator (and matches the type / class given)
ConstNodeRef GeneratorNodeSequence::nodeExists(std::string_view name, const ConstNodeRef &excludeNode,
                                               const GeneratorNode::NodeTypeVector &allowedNodeTypes) const
{
    // First, bubble up to the topmost sequence (which should be the Generator's rootSequence_)
    if (owner_)
        return owner_->get().scope()->get().nodeExists(name, excludeNode, allowedNodeTypes);

    // No parent node, so we must be the topmost sequence - run the search from here
    return searchNodes(name, excludeNode, allowedNodeTypes);
}

// Return the named parameter if it is currently in scope
std::shared_ptr<ExpressionVariable>
GeneratorNodeSequence::parameterInScope(ConstNodeRef queryingNode, std::string_view name,
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
        return owner_->get().getParameterInScope(name, excludeParameter);

    // Not found
    return nullptr;
}

// Return whether the named parameter exists in this sequence or its children (branches)
std::shared_ptr<ExpressionVariable>
GeneratorNodeSequence::parameterExists(std::string_view name, const std::shared_ptr<ExpressionVariable> &excludeParameter) const
{
    // First, bubble up to the topmost sequence (which should be the Generator's rootSequence_)
    if (owner_)
        return owner_->get().scope()->get().parameterExists(name, excludeParameter);

    // No parent node, so we must be the topmost sequence - run the search from here
    return searchParameters(name, excludeParameter);
}

// Create and return reference list of parameters in scope
std::vector<std::shared_ptr<ExpressionVariable>> GeneratorNodeSequence::parametersInScope(ConstNodeRef queryingNode)
{
    auto range = QueryRange(queryingNode, sequence_);
    if (queryingNode)
        assert(!range.empty());

    std::vector<std::shared_ptr<ExpressionVariable>> parameters;

    // Start from the target node and work backwards...
    for (const auto &node : range)
    {
        parameters.insert(parameters.end(), node->parameters().begin(), node->parameters().end());
    }

    // Recursively check our owner
    if (owner_)
    {
        auto otherParams = owner_->get().getParametersInScope();
        parameters.insert(parameters.end(), otherParams.begin(), otherParams.end());
    }

    return parameters;
}

// Validate node-related keywords ensuring invalid (out-of-scope) data are un-set
bool GeneratorNodeSequence::validateNodeKeywords()
{
    auto result = true;

    for (auto &node : sequence_)
    {
        // NodeKeyword
        for (auto &kwd : node->keywords().allOfType<NodeKeywordBase>())
            if (!kwd->validate())
                result = false;

        // NodeVectorKeyword
        for (auto &kwd : node->keywords().allOfType<NodeVectorKeywordBase>())
            if (!kwd->validate())
                result = false;

        // Check node branch if present
        if (node->branch() && !node->branch()->get().validateNodeKeywords())
            result = false;
    }

    return result;
}

// Check for node consistency
bool GeneratorNodeSequence::check() const
{
    for (auto &node : sequence_)
    {
        // Check ownership
        if (&node->scope()->get() != this)
            return Messenger::error("Node '{}' failed parent check ({} is not this {})\n", node->name(),
                                    fmt::ptr(node->parent()), fmt::ptr(this));

        // Check node branch if present
        if (node->branch() && !node->branch()->get().check())
            return false;

        // Check validity of NodeValue-based keywords
        for (auto &kwd : node->keywords().allOfType<NodeValueKeyword>())
            if (!kwd->data().isValid())
                return false;
        for (auto &kwd : node->keywords().allOfType<Vec3NodeValueKeyword>())
            if (!kwd->data().x.isValid() || !kwd->data().y.isValid() || !kwd->data().z.isValid())
                return false;
        for (auto &kwd : node->keywords().allOfType<NodeValueEnumOptionsBaseKeyword>())
            if (!kwd->value().isValid())
                return false;
    }

    return true;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool GeneratorNodeSequence::prepare(const GeneratorContext &generatorContext)
{
    // Loop over nodes in the list, preparing each in turn
    for (const auto &node : sequence_)
        if (!node->prepare(generatorContext))
            return false;

    return true;
}

// Execute node
bool GeneratorNodeSequence::execute(const GeneratorContext &generatorContext)
{
    // If there are no nodes, just exit now
    if (sequence_.empty())
        return true;

    // Loop over nodes in the list, executing each in turn
    for (const auto &node : sequence_)
        if (!node->execute(generatorContext))
            return false;

    return true;
}

/*
 * Read / Write
 */

// Return block keyword for current context
std::string_view GeneratorNodeSequence::blockKeyword() const { return blockKeyword_; }

// Read structure from specified LineParser
bool GeneratorNodeSequence::deserialise(LineParser &parser, const CoreData &coreData)
{
    const auto blockTerminationKeyword = fmt::format("End{}", blockKeyword_);

    // Read until we encounter the block-ending keyword, or we fail for some reason
    auto errorsEncountered = false;
    while (!parser.eofOrBlank())
    {
        // Read and parse the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Is the first argument the block termination keyword for the current context?
        if (DissolveSys::sameString(parser.argsv(0), blockTerminationKeyword))
            break;

        // Not a control keyword, so must be a node type
        if (!GeneratorNode::nodeTypes().isValid(parser.argsv(0)))
        {
            Messenger::error("Unrecognised node type '{}' found.\n", parser.argsv(0));
            errorsEncountered = true;
            continue;
        }

        // Create a new node of the specified type
        auto newNode = GeneratorNodeRegistry::create(GeneratorNode::nodeTypes().enumeration(parser.argsv(0)));

        // Set the name of the node if it is required / provided
        if (newNode->mustBeNamed() && !parser.hasArg(1))
        {
            Messenger::error("A name must be given explicitly to a node of type {}.\n",
                             GeneratorNode::nodeTypes().keyword(newNode->type()));
            errorsEncountered = true;
        }

        if (parser.hasArg(1))
        {
            // Set the name (to nice-ify it)
            newNode->setName(parser.argsv(1));

            // Check for clash of names with existing node in scope
            if (!sequence_.empty() && nodeInScope(sequence_.back(), newNode->name()))
                return Messenger::error("A node named '{}' is already in scope.\n", newNode->name());
        }

        // Append the node to our sequence
        appendNode(newNode);

        // Read the new node
        if (!newNode->deserialise(parser, coreData))
        {
            Messenger::error("Failed to read node sequence.\n");
            errorsEncountered = true;
        }
    }

    return (!errorsEncountered);
}

// Write structure to specified LineParser
bool GeneratorNodeSequence::serialise(LineParser &parser, std::string_view prefix)
{
    // Block Start - should have already been written by the calling function, since we don't know the keyword we are linked
    // to

    // Loop over nodes in this sequence
    for (const auto &node : sequence_)
        if (!node->serialise(parser, prefix))
            return false;

    // Block End - will be written by the calling function, since we don't know the keyword we are linked to

    return true;
}

GeneratorNodeSequence::QueryRange::QueryRange(ConstNodeRef queryingNode, const std::vector<NodeRef> &seq)
{
    start_ = std::find(seq.rbegin(), seq.rend(), queryingNode);
    stop_ = seq.rend();
}
std::vector<NodeRef>::const_reverse_iterator GeneratorNodeSequence::QueryRange::begin() { return start_; }
std::vector<NodeRef>::const_reverse_iterator GeneratorNodeSequence::QueryRange::end() { return stop_; }
bool GeneratorNodeSequence::QueryRange::empty() { return start_ == stop_; }
void GeneratorNodeSequence::QueryRange::next() { start_++; }

// Express as a serialisable value
SerialisedValue GeneratorNodeSequence::serialise() const
{
    return fromVector(sequence_,
                      [](const auto item)
                      {
                          SerialisedValue node = item->serialise();
                          node["type"] = item->nodeTypes().serialise(item->type());
                          return node;
                      });
}

// Read values from a serialisable value
void GeneratorNodeSequence::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node,
             [this, &coreData](const auto &value)
             {
                 GeneratorNode::NodeType type = GeneratorNode::nodeTypes().deserialise(value.at("type"));
                 auto result = GeneratorNodeRegistry::create(type);
                 appendNode(result, {});
                 result->deserialise(value, coreData);
             });
}
