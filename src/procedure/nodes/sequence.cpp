/*
    *** Procedure Node - Sequence
    *** src/procedure/nodes/sequence.cpp
    Copyright T. Youngs 2012-2020

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

#include "procedure/nodes/sequence.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "procedure/nodes/nodes.h"

SequenceProcedureNode::SequenceProcedureNode(ProcedureNode::NodeContext context, const Procedure *procedure,
                                             ProcedureNode *parentNode, std::string_view blockTerminationKeyword)
    : ProcedureNode(ProcedureNode::SequenceNode)
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
    static EnumOptionsList SequenceNodeTypeKeywords;

    static EnumOptions<SequenceProcedureNode::SequenceNodeKeyword> options("SequenceNodeKeyword", SequenceNodeTypeKeywords);

    return options;
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
                                                  ProcedureNode::NodeType nt) const
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
                if (nt == ProcedureNode::nNodeTypes)
                    return node;
                else if (node->type() == nt)
                    return node;
            }
        }

        // If the node has a branch, descend into it
        if (node->hasBranch())
        {
            ProcedureNode *result = node->branch()->searchNodes(name, excludeNode, nt);
            if (result)
                return result;
        }
    }

    return nullptr;
}

// Search through the Procedure for the named parameter
ExpressionVariable *SequenceProcedureNode::searchParameters(std::string_view name, ExpressionVariable *excludeParameter) const
{
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        // Does this node have a parameter by this name?
        ExpressionVariable *result = node->hasParameter(name, excludeParameter);
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
ProcedureNode *SequenceProcedureNode::node(std::string_view name, ProcedureNode::NodeType nt) const
{
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        if (DissolveSys::sameString(node->name(), name))
        {
            // Check type
            if (nt == ProcedureNode::nNodeTypes)
                return node;
            else if (node->type() == nt)
                return node;
        }

        // If the node has a branch, recurse in to that
        if (node->hasBranch())
        {
            ProcedureNode *branchNode = node->branch()->node(name, nt);
            if (branchNode)
                return branchNode;
        }
    }

    return nullptr;
}

// Return list of nodes of specified type present in the Procedure
RefList<ProcedureNode> SequenceProcedureNode::nodes(ProcedureNode *queryingNode, ProcedureNode::NodeType nt)
{
    RefList<ProcedureNode> matches;

    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        // Check type
        if (nt == ProcedureNode::nNodeTypes)
            matches.append(node);
        else if (node->type() == nt)
            matches.append(node);

        // If the node has a branch, recurse in to that
        if (node->hasBranch())
        {
            RefList<ProcedureNode> branchNodes = node->branch()->nodes(node, nt);
            matches += branchNodes;
        }
    }

    return matches;
}

// Return named node if it is currently in scope, and optionally matches the type given
ProcedureNode *SequenceProcedureNode::nodeInScope(ProcedureNode *queryingNode, std::string_view name,
                                                  ProcedureNode::NodeType nt)
{
    // Is this node present in our own sequence?
    if (queryingNode && (!sequence_.contains(queryingNode)))
    {
        Messenger::error("INTERNAL ERROR: Querying node passed to SequenceProcedureNode::nodeInScope() is not a member "
                         "of this sequence.\n");
        return nullptr;
    }

    // Start from the target node and work backwards...
    for (auto *node = queryingNode; node != nullptr; node = node->prev())
    {
        if (DissolveSys::sameString(node->name(), name))
        {
            // Check type
            if (nt == ProcedureNode::nNodeTypes)
                return node;
            else if (node->type() == nt)
                return node;
        }
    }

    // Not in our list. Recursively check our parent(s)
    if (parentNode_)
        return parentNode_->nodeInScope(name, nt);

    // Not found
    return nullptr;
}

// Return list of nodes of specified type present in scope
RefList<ProcedureNode> SequenceProcedureNode::nodesInScope(ProcedureNode *queryingNode, ProcedureNode::NodeType nt)
{
    // Is this node present in our own sequence?
    if (queryingNode && (!sequence_.contains(queryingNode)))
    {
        Messenger::error("INTERNAL ERROR: Querying node passed to SequenceProcedureNode::nodesInScope() is not a "
                         "member of this sequence.\n");
        return RefList<ProcedureNode>();
    }

    RefList<ProcedureNode> matches;

    // Start from the target node and work backwards...
    for (auto *node = queryingNode; node != nullptr; node = node->prev())
    {
        // Check type
        if (nt == ProcedureNode::nNodeTypes)
            matches.append(node);
        else if (node->type() == nt)
            matches.append(node);
    }

    // Not in our list. Recursively check our parent(s)
    if (parentNode_)
        matches += parentNode_->nodesInScope(nt);

    return matches;
}

// Return named node if it exists anywhere in the same Procedure, and optionally matches the type given
ProcedureNode *SequenceProcedureNode::nodeExists(std::string_view name, ProcedureNode *excludeNode,
                                                 ProcedureNode::NodeType nt) const
{
    // First, bubble up to the topmost sequence (which should be the Procedure's rootSequence_)
    if (parentNode_)
        return parentNode_->scope()->nodeExists(name, excludeNode, nt);

    // No parent node, so we must be the topmost sequence - run the search from here
    return searchNodes(name, excludeNode, nt);
}

// Return whether the named parameter is currently in scope
ExpressionVariable *SequenceProcedureNode::parameterInScope(ProcedureNode *queryingNode, std::string_view name,
                                                            ExpressionVariable *excludeParameter)
{
    // Is this node present in our own sequence?
    if (queryingNode && (!sequence_.contains(queryingNode)))
    {
        Messenger::error("INTERNAL ERROR: Querying node passed to SequenceProcedureNode::parameterInScope() is not a "
                         "member of this sequence.\n");
        return nullptr;
    }

    // Start from the target node and work backwards...
    for (auto *node = queryingNode; node != nullptr; node = node->prev())
    {
        ExpressionVariable *param = node->hasParameter(name, excludeParameter);
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
ExpressionVariable *SequenceProcedureNode::parameterExists(std::string_view name, ExpressionVariable *excludeParameter) const
{
    // First, bubble up to the topmost sequence (which should be the Procedure's rootSequence_)
    if (parentNode_)
        return parentNode_->scope()->parameterExists(name, excludeParameter);

    // No parent node, so we must be the topmost sequence - run the search from here
    return searchParameters(name, excludeParameter);
}

// Create and return reference list of parameters in scope
RefList<ExpressionVariable> SequenceProcedureNode::parametersInScope(ProcedureNode *queryingNode)
{
    RefList<ExpressionVariable> parameters;

    // Is this node present in our own sequence?
    if (queryingNode && (!sequence_.contains(queryingNode)))
    {
        Messenger::error("INTERNAL ERROR: Querying node passed to SequenceProcedureNode::parametersInScope() is not a "
                         "member of this sequence.\n");
        return parameters;
    }

    // Start from the target node and work backwards...
    for (auto *node = queryingNode; node != nullptr; node = node->prev())
    {
        parameters += node->parameterReferences();
    }

    // Recursively check our parent(s)
    if (parentNode_)
        parameters += parentNode_->parametersInScope();

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
ProcedureNode::NodeExecutionResult SequenceProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                  std::string_view prefix, GenericList &targetList)
{
    ProcedureNode::NodeExecutionResult result = ProcedureNode::Success;

    // If there are no nodes, just exit now
    if (sequence_.nItems() == 0)
        return ProcedureNode::Success;

    // Loop over nodes in the list, executing each in turn
    ListIterator<ProcedureNode> nodeIterator(sequence_);
    while (ProcedureNode *node = nodeIterator.iterate())
    {
        // Get the result of executing the node
        result = node->execute(procPool, cfg, prefix, targetList);

        // If the result is not Success, don't process any more nodes
        if (result != ProcedureNode::Success)
            break;
    }

    return result;
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
bool SequenceProcedureNode::read(LineParser &parser, CoreData &coreData)
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
            case (ProcedureNode::AddSpeciesNode):
                newNode = new AddSpeciesProcedureNode();
                break;
            case (ProcedureNode::BoxNode):
                newNode = new BoxProcedureNode();
                break;
            case (ProcedureNode::CalculateAngleNode):
                newNode = new CalculateAngleProcedureNode();
                break;
            case (ProcedureNode::CalculateDistanceNode):
                newNode = new CalculateDistanceProcedureNode();
                break;
            case (ProcedureNode::CalculateBaseNode):
                /* This should never be called */
                return Messenger::error("Can't create a node of CalculateBase type directly - create the "
                                        "parent node instead.\n");
                break;
            case (ProcedureNode::CalculateVectorNode):
                newNode = new CalculateVectorProcedureNode();
                break;
            case (ProcedureNode::Collect1DNode):
                newNode = new Collect1DProcedureNode();
                break;
            case (ProcedureNode::Collect2DNode):
                newNode = new Collect2DProcedureNode();
                break;
            case (ProcedureNode::Collect3DNode):
                newNode = new Collect3DProcedureNode();
                break;
            case (ProcedureNode::ExcludeNode):
                newNode = new ExcludeProcedureNode();
                break;
            case (ProcedureNode::Fit1DNode):
                newNode = new Fit1DProcedureNode();
                break;
            case (ProcedureNode::OperateBaseNode):
                /* This should never be called */
                return Messenger::error("Can't create a node of OperateBase type directly - create the parent node instead.\n");
                break;
            case (ProcedureNode::OperateDivideNode):
                newNode = new OperateDivideProcedureNode();
                break;
            case (ProcedureNode::OperateExpressionNode):
                newNode = new OperateExpressionProcedureNode();
                break;
            case (ProcedureNode::OperateMultiplyNode):
                newNode = new OperateMultiplyProcedureNode();
                break;
            case (ProcedureNode::OperateNormaliseNode):
                newNode = new OperateNormaliseProcedureNode();
                break;
            case (ProcedureNode::OperateNumberDensityNormaliseNode):
                newNode = new OperateNumberDensityNormaliseProcedureNode();
                break;
            case (ProcedureNode::OperateSitePopulationNormaliseNode):
                newNode = new OperateSitePopulationNormaliseProcedureNode();
                break;
            case (ProcedureNode::OperateSphericalShellNormaliseNode):
                newNode = new OperateSphericalShellNormaliseProcedureNode();
                break;
            case (ProcedureNode::ParametersNode):
                newNode = new ParametersProcedureNode();
                break;
            case (ProcedureNode::Process1DNode):
                newNode = new Process1DProcedureNode();
                break;
            case (ProcedureNode::Process2DNode):
                newNode = new Process2DProcedureNode();
                break;
            case (ProcedureNode::Process3DNode):
                newNode = new Process3DProcedureNode();
                break;
            case (ProcedureNode::SelectNode):
                newNode = new SelectProcedureNode();
                break;
            case (ProcedureNode::SequenceNode):
                /* This should never be called */
                newNode = new SequenceProcedureNode(ProcedureNode::NoContext, procedure(), this);
                break;
            case (ProcedureNode::nNodeTypes):
                return Messenger::error("Unrecognised procedure node type '{}' found.\n", parser.argsv(0));
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
        if (!newNode->read(parser, coreData))
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
