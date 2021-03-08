// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/node.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "classes/site.h"
#include "procedure/nodes/sequence.h"
#include "procedure/procedure.h"

// Return enum option info for NodeType
EnumOptions<ProcedureNode::NodeType> ProcedureNode::nodeTypes()
{
    return EnumOptions<ProcedureNode::NodeType>(
        "NodeType", {{ProcedureNode::AddSpeciesNode, "AddSpecies"},
                     {ProcedureNode::BoxNode, "Box"},
                     {ProcedureNode::CalculateAngleNode, "CalculateAngle"},
                     {ProcedureNode::CalculateAxisAngleNode, "CalculateAxisAngle"},
                     {ProcedureNode::CalculateDistanceNode, "CalculateDistance"},
                     {ProcedureNode::CalculateBaseNode, "Calculate(Any)"},
                     {ProcedureNode::CalculateVectorNode, "CalculateVector"},
                     {ProcedureNode::Collect1DNode, "Collect1D"},
                     {ProcedureNode::Collect2DNode, "Collect2D"},
                     {ProcedureNode::Collect3DNode, "Collect3D"},
                     {ProcedureNode::DynamicSiteNode, "DynamicSite"},
                     {ProcedureNode::ExcludeNode, "Exclude"},
                     {ProcedureNode::Fit1DNode, "Fit1D"},
                     {ProcedureNode::Integrate1DNode, "Integrate1D"},
                     {ProcedureNode::OperateBaseNode, "Operate(Any)"},
                     {ProcedureNode::OperateDivideNode, "OperateDivide"},
                     {ProcedureNode::OperateExpressionNode, "OperateExpression"},
                     {ProcedureNode::OperateMultiplyNode, "OperateMultiply"},
                     {ProcedureNode::OperateNormaliseNode, "OperateNormalise"},
                     {ProcedureNode::OperateNumberDensityNormaliseNode, "OperateNumberDensityNormalise"},
                     {ProcedureNode::OperateSitePopulationNormaliseNode, "OperateSitePopulationNormalise"},
                     {ProcedureNode::OperateSphericalShellNormaliseNode, "OperateSphericalShellNormalise"},
                     {ProcedureNode::ParametersNode, "Parameters"},
                     {ProcedureNode::Process1DNode, "Process1D"},
                     {ProcedureNode::Process2DNode, "Process2D"},
                     {ProcedureNode::Process3DNode, "Process3D"},
                     {ProcedureNode::SelectNode, "Select"},
                     {ProcedureNode::SequenceNode, "Sequence"},
                     {ProcedureNode::Sum1DNode, "Sum1D"}});
}

// Return enum option info for NodeContext
EnumOptions<ProcedureNode::NodeContext> ProcedureNode::nodeContexts()
{
    return EnumOptions<ProcedureNode::NodeContext>("NodeContext", {{ProcedureNode::NoContext, "None"},
                                                                   {ProcedureNode::AnalysisContext, "Analysis"},
                                                                   {ProcedureNode::GenerationContext, "Generation"},
                                                                   {ProcedureNode::OperateContext, "Operate"}});
}

ProcedureNode::ProcedureNode(ProcedureNode::NodeType nodeType) : ListItem<ProcedureNode>()
{
    type_ = nodeType;
    scope_ = nullptr;

    // Assign default, unique name to the node
    static int nodeCount = 0;
    name_ = fmt::format("Node{:04d}", ++nodeCount);
}

ProcedureNode::~ProcedureNode() {}

/*
 * Identity
 */

// Return node type
ProcedureNode::NodeType ProcedureNode::type() const { return type_; }

// Return whether the node is of the specified type (detecting derived node classes as well)
bool ProcedureNode::isType(ProcedureNode::NodeType thisType) const
{
    // Handle derived node types
    if (thisType == ProcedureNode::CalculateBaseNode)
        return ((type_ > ProcedureNode::BEGIN_CalculateNodes) && (type_ < ProcedureNode::END_CalculateNodes));
    else if (thisType == ProcedureNode::OperateBaseNode)
        return ((type_ > ProcedureNode::BEGIN_OperateNodes) && (type_ < ProcedureNode::END_OperateNodes));

    return (thisType == type_);
}

// Return whether a name for the node must be provided
bool ProcedureNode::mustBeNamed() const { return true; }

// Set node name (and nice name)
void ProcedureNode::setName(std::string_view name)
{
    name_ = name;

    // Generate a nice name (i.e. no spaces, slashes etc.)
    niceName_ = DissolveSys::niceName(name);
}

// Return node name
std::string_view ProcedureNode::name() const { return name_; }

// Return nice node name
std::string_view ProcedureNode::niceName() const { return niceName_; }

/*
 * Keywords
 */

// Return keywords for this node
const KeywordList &ProcedureNode::keywords() const { return keywords_; }

/*
 * Scope
 */

// Set scope
void ProcedureNode::setScope(SequenceProcedureNode *scopeNode) { scope_ = scopeNode; }

// Return scope (SequenceNode) in which this node exists
SequenceProcedureNode *ProcedureNode::scope() const { return scope_; }

// Return Procedure in which this node exists
const Procedure *ProcedureNode::procedure() const
{
    if (!scope_)
        return nullptr;

    return scope_->procedure();
}

// Return context of scope in which this node exists
ProcedureNode::NodeContext ProcedureNode::scopeContext() const
{
    if (!scope_)
        return ProcedureNode::NoContext;

    return scope_->sequenceContext();
}

// Return named node if it is currently in scope, and optionally matches the type given
ProcedureNode *ProcedureNode::nodeInScope(std::string_view name, ProcedureNode::NodeType nt)
{
    if (!scope_)
        return nullptr;

    return scope_->nodeInScope(this, name, nt);
}

// Return list of nodes of specified type present in this node's scope
RefList<ProcedureNode> ProcedureNode::nodesInScope(ProcedureNode::NodeType nt)
{
    if (!scope_)
        return RefList<ProcedureNode>();

    return scope_->nodesInScope(this, nt);
}

// Return named node if it exists anywhere in the same Procedure, and optionally matches the type given
ProcedureNode *ProcedureNode::nodeExists(std::string_view name, ProcedureNode *excludeNode, ProcedureNode::NodeType nt) const
{
    if (!scope_)
        return nullptr;

    return scope_->nodeExists(name, excludeNode, nt);
}

// Return list of nodes of specified type present in the Procedure
RefList<ProcedureNode> ProcedureNode::nodes(ProcedureNode::NodeType nt)
{
    if (!scope_)
        return RefList<ProcedureNode>();

    return scope_->nodes(this, nt);
}

// Return the named parameter if it is currently in scope
std::shared_ptr<ExpressionVariable> ProcedureNode::parameterInScope(std::string_view name,
                                                                    std::shared_ptr<ExpressionVariable> excludeParameter)
{
    if (!scope_)
        return nullptr;

    return scope_->parameterInScope(this, name, excludeParameter);
}

// Return the named parameter if it exists anywhere in the same Procedure
std::shared_ptr<ExpressionVariable> ProcedureNode::parameterExists(std::string_view name,
                                                                   std::shared_ptr<ExpressionVariable> excludeParameter) const
{
    if (!scope_)
        return nullptr;

    return scope_->parameterExists(name, excludeParameter);
}

// Create and return reference list of parameters in scope
std::vector<std::shared_ptr<ExpressionVariable>> ProcedureNode::parametersInScope()
{
    if (!scope_)
        return {};

    return scope_->parametersInScope(this);
}

/*
 * Branch
 */

// Return whether this node has a branch
bool ProcedureNode::hasBranch() const { return false; }

// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode *ProcedureNode::branch() { return nullptr; }

/*
 * Parameters
 */

// Return whether this node has the named parameter specified
std::shared_ptr<ExpressionVariable> ProcedureNode::hasParameter(std::string_view name,
                                                                std::shared_ptr<ExpressionVariable> excludeParameter)
{
    return nullptr;
}

// Return references to all parameters for this node
OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> ProcedureNode::parameters() const
{
    return std::nullopt;
}

/*
 * Execution
 */

// Prepare any necessary data, ready for execution
bool ProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Finalise any necessary data after execution
bool ProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    return true;
}

/*
 * Read / Write
 */

// Read node data from specified LineParser
bool ProcedureNode::read(LineParser &parser, CoreData &coreData)
{
    // Read until we encounter the ending keyword (derived from the node type), or we fail for some reason
    while (!parser.eofOrBlank())
    {
        // Read and parse the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Is this the end of the node block?
        if (DissolveSys::sameString(parser.argsv(0), fmt::format("End{}", nodeTypes().keyword(type_))))
            return true;

        // Try to parse this line as a keyword
        KeywordBase::ParseResult result = keywords_.parse(parser, coreData);
        if (result == KeywordBase::Failed)
            return Messenger::error("Failed to parse keyword '{}'.\n", parser.argsv(0));
        else if (result == KeywordBase::Success)
            continue;
        else if (result == KeywordBase::Unrecognised)
            return Messenger::error("Unrecognised keyword '{}' found while parsing {} node.\n", parser.argsv(0),
                                    nodeTypes().keyword(type_));
    }

    return true;
}

// Write node data to specified LineParser
bool ProcedureNode::write(LineParser &parser, std::string_view prefix)
{
    // Block Start - does this node have a required name?
    if (mustBeNamed())
    {
        if (!parser.writeLineF("{}{}  '{}'\n", prefix, ProcedureNode::nodeTypes().keyword(type_), name()))
            return false;
    }
    else
    {
        if (!parser.writeLineF("{}{}\n", prefix, ProcedureNode::nodeTypes().keyword(type_)))
            return false;
    }

    // Create new prefix
    std::string newPrefix = fmt::format("  {}", prefix);

    // Write keywords
    if (!keywords_.write(parser, newPrefix, false))
        return false;

    // Block End
    if (!parser.writeLineF("{}End{}\n", prefix, nodeTypes().keyword(type_)))
        return false;

    return true;
}
