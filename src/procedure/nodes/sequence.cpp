// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/sequence.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "procedure/nodes/nodes.h"

SequenceProcedureNode::SequenceProcedureNode(ProcedureNode::NodeContext context, const Procedure *procedure,
                                             ProcedureNode *parentNode, std::string_view blockTerminationKeyword)
    : ProcedureNode(ProcedureNode::NodeType::Sequence)
{
    context_ = context;
    procedure_ = procedure;
    parentNode_ = parentNode;
    blockTerminationKeyword_ = blockTerminationKeyword;
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
void SequenceProcedureNode::addNode(ProcedureNode *node)
{
    if (!node)
        return;

    if (!node->isContextRelevant(context_))
        Messenger::error("Node '{}' (type = '{}') is not relevant to the '{}' context.\n", node->name(),
                         ProcedureNode::nodeTypes().keyword(node->type()), ProcedureNode::nodeContexts().keyword(context_));

    sequence_.own(node);
}

// Return sSequential node list
const List<ProcedureNode> &SequenceProcedureNode::sequence() const { return sequence_; }

// Return number of nodes in sequence
int SequenceProcedureNode::nNodes() const { return sequence_.nItems(); }

/*
 * Scope
 */

// Return named node if it exists anywhere in our sequence or below, and optionally matches the type given
ProcedureNode *SequenceProcedureNode::searchNodes(std::string_view name, ProcedureNode *excludeNode,
                                                  std::optional<ProcedureNode::NodeType> optNodeType) const
{
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        // Does this node match the supplied name?
        if (node != excludeNode)
        {
            if (DissolveSys::sameString(node->name(), name))
            {
                // Check type
                if (!optNodeType || optNodeType.value() == node->type())
                    return node;
            }
        }

        // If the node has a branch, descend into it
        if (node->hasBranch())
        {
            ProcedureNode *result = node->branch()->searchNodes(name, excludeNode, optNodeType);
            if (result)
                return result;
        }
    }

    return nullptr;
}

// Search through the Procedure for the named parameter
std::shared_ptr<ExpressionVariable>
SequenceProcedureNode::searchParameters(std::string_view name, std::shared_ptr<ExpressionVariable> excludeParameter) const
{
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        // Does this node have a parameter by this name?
        auto result = node->hasParameter(name, excludeParameter);
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

// Return the context of the sequence
ProcedureNode::NodeContext SequenceProcedureNode::sequenceContext() const { return context_; }

// Return named node if present in this sequence, and which matches the (optional) type given
ProcedureNode *SequenceProcedureNode::node(std::string_view name, std::optional<ProcedureNode::NodeType> optNodeType) const
{
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        if (DissolveSys::sameString(node->name(), name))
        {
            // Check type
            if (!optNodeType || optNodeType.value() == node->type())
                return node;
        }

        // If the node has a branch, recurse in to that
        if (node->hasBranch())
        {
            ProcedureNode *branchNode = node->branch()->node(name, optNodeType);
            if (branchNode)
                return branchNode;
        }
    }

    return nullptr;
}

// Return list of nodes of specified type present in the Procedure
RefList<ProcedureNode> SequenceProcedureNode::nodes(ProcedureNode *queryingNode,
                                                    std::optional<ProcedureNode::NodeType> optNodeType)
{
    RefList<ProcedureNode> matches;

    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        // Check type
        if (!optNodeType || optNodeType.value() == node->type())
            matches.append(node);

        // If the node has a branch, recurse in to that
        if (node->hasBranch())
        {
            RefList<ProcedureNode> branchNodes = node->branch()->nodes(node, optNodeType);
            matches += branchNodes;
        }
    }

    return matches;
}

// Return named node if it is currently in scope, and optionally matches the type given
ProcedureNode *SequenceProcedureNode::nodeInScope(ProcedureNode *queryingNode, std::string_view name,
                                                  std::optional<ProcedureNode::NodeType> optNodeType)
{
    if (queryingNode)
        assert(sequence_.contains(queryingNode));

    // Start from the target node and work backwards...
    for (auto *node = queryingNode; node != nullptr; node = node->prev())
    {
        if (DissolveSys::sameString(node->name(), name))
        {
            // Check type
            if (!optNodeType || optNodeType.value() == node->type())
                return node;
        }
    }

    // Not in our list. Recursively check our parent(s)
    if (parentNode_)
        return parentNode_->nodeInScope(name, optNodeType);

    // Not found
    return nullptr;
}

// Return list of nodes of optional specified type present in scope
RefList<ProcedureNode> SequenceProcedureNode::nodesInScope(ProcedureNode *queryingNode,
                                                           std::optional<ProcedureNode::NodeType> optNodeType)
{
    if (queryingNode)
        assert(sequence_.contains(queryingNode));

    RefList<ProcedureNode> matches;

    // Start from the target node and work backwards...
    for (auto *node = queryingNode; node != nullptr; node = node->prev())
    {
        // Check type
        if (!optNodeType || optNodeType.value() == node->type())
            matches.append(node);
    }

    // Not in our list. Recursively check our parent(s)
    if (parentNode_)
        matches += parentNode_->nodesInScope(optNodeType);

    return matches;
}

// Return named node if it exists anywhere in the same Procedure, and optionally matches the type given
ProcedureNode *SequenceProcedureNode::nodeExists(std::string_view name, ProcedureNode *excludeNode,
                                                 std::optional<ProcedureNode::NodeType> optNodeType) const
{
    // First, bubble up to the topmost sequence (which should be the Procedure's rootSequence_)
    if (parentNode_)
        return parentNode_->scope()->nodeExists(name, excludeNode, optNodeType);

    // No parent node, so we must be the topmost sequence - run the search from here
    return searchNodes(name, excludeNode, optNodeType);
}

// Return the named parameter if it is currently in scope
std::shared_ptr<ExpressionVariable>
SequenceProcedureNode::parameterInScope(ProcedureNode *queryingNode, std::string_view name,
                                        std::shared_ptr<ExpressionVariable> excludeParameter)
{
    if (queryingNode)
        assert(sequence_.contains(queryingNode));

    // Start from the target node and work backwards...
    for (auto *node = queryingNode; node != nullptr; node = node->prev())
    {
        auto param = node->hasParameter(name, excludeParameter);
        if (param)
            return param;
    }

    // Not in our list. Recursively check our parent(s)
    if (parentNode_)
        return parentNode_->parameterInScope(name, excludeParameter);

    // Not found
    return nullptr;
}

// Return whether the named parameter exists in this sequence or its children (branches)
std::shared_ptr<ExpressionVariable>
SequenceProcedureNode::parameterExists(std::string_view name, std::shared_ptr<ExpressionVariable> excludeParameter) const
{
    // First, bubble up to the topmost sequence (which should be the Procedure's rootSequence_)
    if (parentNode_)
        return parentNode_->scope()->parameterExists(name, excludeParameter);

    // No parent node, so we must be the topmost sequence - run the search from here
    return searchParameters(name, excludeParameter);
}

// Create and return reference list of parameters in scope
std::vector<std::shared_ptr<ExpressionVariable>> SequenceProcedureNode::parametersInScope(ProcedureNode *queryingNode)
{
    if (queryingNode)
        assert(sequence_.contains(queryingNode));

    std::vector<std::shared_ptr<ExpressionVariable>> parameters;

    // Start from the target node and work backwards...
    for (auto *node = queryingNode; node != nullptr; node = node->prev())
    {
        auto optOtherParams = node->parameters();
        if (optOtherParams)
        {
            const std::vector<std::shared_ptr<ExpressionVariable>> otherParams = (*optOtherParams);
            parameters.insert(parameters.end(), otherParams.begin(), otherParams.end());
        }
    }

    // Recursively check our parent(s)
    if (parentNode_)
    {
        auto optOtherParams = parentNode_->parameters();
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
bool SequenceProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Loop over nodes in the list, preparing each in turn
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
        if (!node->prepare(cfg, prefix, targetList))
            return false;

    return true;
}

// Execute node, targetting the supplied Configuration
bool SequenceProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // If there are no nodes, just exit now
    if (sequence_.nItems() == 0)
        return true;

    // Loop over nodes in the list, executing each in turn
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
        if (!node->execute(procPool, cfg, prefix, targetList))
            return false;

    return true;
}

// Finalise any necessary data after execution
bool SequenceProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                     GenericList &targetList)
{
    // Loop over nodes in the list, finalising each in turn
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
        if (!node->finalise(procPool, cfg, prefix, targetList))
            return false;

    return true;
}

/*
 * Read / Write
 */

// Set block termination keyword for current context when reading
void SequenceProcedureNode::setBlockTerminationKeyword(std::string_view endKeyword) { blockTerminationKeyword_ = endKeyword; }

// Return block termination keyword for current context
std::string_view SequenceProcedureNode::blockTerminationKeyword() const { return blockTerminationKeyword_; }

// Read structure from specified LineParser
bool SequenceProcedureNode::deserialise(LineParser &parser, const CoreData &coreData)
{
    // Read until we encounter the block-ending keyword, or we fail for some reason
    while (!parser.eofOrBlank())
    {
        // Read and parse the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Is the first argument the block termination keyword for the current context?
        if (DissolveSys::sameString(parser.argsv(0), blockTerminationKeyword_))
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
        ProcedureNode *newNode = nullptr;
        if (!ProcedureNode::nodeTypes().isValid(parser.argsv(0)))
            return Messenger::error("Unrecognised node type '{}' found.\n", parser.argsv(0));
        ProcedureNode::NodeType nt = ProcedureNode::nodeTypes().enumeration(parser.argsv(0));
        switch (nt)
        {
            case (ProcedureNode::NodeType::AddSpecies):
                newNode = new AddSpeciesProcedureNode();
                break;
            case (ProcedureNode::NodeType::Box):
                newNode = new BoxProcedureNode();
                break;
            case (ProcedureNode::NodeType::CalculateAngle):
                newNode = new CalculateAngleProcedureNode();
                break;
            case (ProcedureNode::NodeType::CalculateDistance):
                newNode = new CalculateDistanceProcedureNode();
                break;
            case (ProcedureNode::NodeType::CalculateBase):
                /* This should never be called */
                return Messenger::error("Can't create a node of CalculateBase type directly - create the "
                                        "parent node instead.\n");
                break;
            case (ProcedureNode::NodeType::CalculateVector):
                newNode = new CalculateVectorProcedureNode();
                break;
            case (ProcedureNode::NodeType::Collect1D):
                newNode = new Collect1DProcedureNode();
                break;
            case (ProcedureNode::NodeType::Collect2D):
                newNode = new Collect2DProcedureNode();
                break;
            case (ProcedureNode::NodeType::Collect3D):
                newNode = new Collect3DProcedureNode();
                break;
            case (ProcedureNode::NodeType::Fit1D):
                newNode = new Fit1DProcedureNode();
                break;
            case (ProcedureNode::NodeType::OperateBase):
                /* This should never be called */
                return Messenger::error("Can't create a node of OperateBase type directly - create the parent node instead.\n");
                break;
            case (ProcedureNode::NodeType::OperateDivide):
                newNode = new OperateDivideProcedureNode();
                break;
            case (ProcedureNode::NodeType::OperateExpression):
                newNode = new OperateExpressionProcedureNode();
                break;
            case (ProcedureNode::NodeType::OperateMultiply):
                newNode = new OperateMultiplyProcedureNode();
                break;
            case (ProcedureNode::NodeType::OperateNormalise):
                newNode = new OperateNormaliseProcedureNode();
                break;
            case (ProcedureNode::NodeType::OperateNumberDensityNormalise):
                newNode = new OperateNumberDensityNormaliseProcedureNode();
                break;
            case (ProcedureNode::NodeType::OperateSitePopulationNormalise):
                newNode = new OperateSitePopulationNormaliseProcedureNode();
                break;
            case (ProcedureNode::NodeType::OperateSphericalShellNormalise):
                newNode = new OperateSphericalShellNormaliseProcedureNode();
                break;
            case (ProcedureNode::NodeType::Parameters):
                newNode = new ParametersProcedureNode();
                break;
            case (ProcedureNode::NodeType::Process1D):
                newNode = new Process1DProcedureNode();
                break;
            case (ProcedureNode::NodeType::Process2D):
                newNode = new Process2DProcedureNode();
                break;
            case (ProcedureNode::NodeType::Process3D):
                newNode = new Process3DProcedureNode();
                break;
            case (ProcedureNode::NodeType::Select):
                newNode = new SelectProcedureNode();
                break;
            case (ProcedureNode::NodeType::Sequence):
                /* This should never be called */
                newNode = new SequenceProcedureNode(ProcedureNode::NoContext, procedure(), this);
                break;
            default:
                return Messenger::error("Epic Developer Fail - Don't know how to create a node of type '{}'.\n",
                                        parser.argsv(0));
        }

        // Check for clash of names with existing node in scope
        if (nodeInScope(sequence_.last(), parser.hasArg(1) ? parser.argsv(1) : newNode->name()))
        {
            return Messenger::error("A node named '{}' is already in scope.\n",
                                    parser.hasArg(1) ? parser.argsv(1) : newNode->name());
        }

        // Set the name of the node if it is required
        if (newNode->mustBeNamed())
        {
            if (!parser.hasArg(1))
                Messenger::error("A name must be given explicitly to a node of type {}.\n",
                                 ProcedureNode::nodeTypes().keyword(newNode->type()));
            else
                newNode->setName(parser.argsv(1));
        }

        // Is the new node permitted in our context?
        if (!newNode->isContextRelevant(context_))
        {
            return Messenger::error("'{}' node not allowed / relevant in '{}' context.\n",
                                    ProcedureNode::nodeTypes().keyword(newNode->type()),
                                    ProcedureNode::nodeContexts().keyword(context_));
        }

        // Add the new node to our list, and set ourself as its scope
        sequence_.own(newNode);
        newNode->setScope(this);

        // Read the new node
        if (!newNode->deserialise(parser, coreData))
            return Messenger::error("Failed to read node sequence.\n");
    }

    return true;
}

// Write structure to specified LineParser
bool SequenceProcedureNode::write(LineParser &parser, std::string_view prefix)
{
    // Block Start - should have already been written by the calling function, since we don't know the keyword we are linked
    // to

    // Loop over nodes in this sequence
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
        if (!node->write(parser, prefix))
            return false;

    // Block End - will be written by the calling function, since we don't know the keyword we are linked to

    return true;
}
