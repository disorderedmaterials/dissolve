// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/node.h"

#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "classes/site.h"
#include "procedure/nodes/sequence.h"
#include "procedure/procedure.h"
#include <algorithm>
#include <utility>

// Return enum option info for NodeClass
EnumOptions<ProcedureNode::NodeClass> ProcedureNode::nodeClasses()
{
    return EnumOptions<ProcedureNode::NodeClass>("NodeClass", {{ProcedureNode::NodeClass::None, "None"},
                                                               {ProcedureNode::NodeClass::Calculate, "Calculate"},
                                                               {ProcedureNode::NodeClass::Operate, "Operate"},
                                                               {ProcedureNode::NodeClass::Region, "Region"}});
}

// Return enum option info for NodeType
EnumOptions<ProcedureNode::NodeType> ProcedureNode::nodeTypes()
{
    return EnumOptions<ProcedureNode::NodeType>(
        "NodeType", {{ProcedureNode::NodeType::Add, "Add"},
                     {ProcedureNode::NodeType::AddPair, "AddPair"},
                     {ProcedureNode::NodeType::Box, "Box"},
                     {ProcedureNode::NodeType::CalculateAngle, "CalculateAngle"},
                     {ProcedureNode::NodeType::CalculateAxisAngle, "CalculateAxisAngle"},
                     {ProcedureNode::NodeType::CalculateDistance, "CalculateDistance"},
                     {ProcedureNode::NodeType::CalculateVector, "CalculateVector"},
                     {ProcedureNode::NodeType::Collect1D, "Collect1D"},
                     {ProcedureNode::NodeType::Collect2D, "Collect2D"},
                     {ProcedureNode::NodeType::Collect3D, "Collect3D"},
                     {ProcedureNode::NodeType::CoordinateSets, "CoordinateSets"},
                     {ProcedureNode::NodeType::CylindricalRegion, "CylindricalRegion"},
                     {ProcedureNode::NodeType::DynamicSite, "DynamicSite"},
                     {ProcedureNode::NodeType::Fit1D, "Fit1D"},
                     {ProcedureNode::NodeType::GeneralRegion, "GeneralRegion"},
                     {ProcedureNode::NodeType::Integrate1D, "Integrate1D"},
                     {ProcedureNode::NodeType::OperateDivide, "OperateDivide"},
                     {ProcedureNode::NodeType::OperateExpression, "OperateExpression"},
                     {ProcedureNode::NodeType::OperateGridNormalise, "OperateGridNormalise"},
                     {ProcedureNode::NodeType::OperateMultiply, "OperateMultiply"},
                     {ProcedureNode::NodeType::OperateNormalise, "OperateNormalise"},
                     {ProcedureNode::NodeType::OperateNumberDensityNormalise, "OperateNumberDensityNormalise"},
                     {ProcedureNode::NodeType::OperateSitePopulationNormalise, "OperateSitePopulationNormalise"},
                     {ProcedureNode::NodeType::OperateSphericalShellNormalise, "OperateSphericalShellNormalise"},
                     {ProcedureNode::NodeType::Parameters, "Parameters"},
                     {ProcedureNode::NodeType::Pick, "Pick"},
                     {ProcedureNode::NodeType::PickProximity, "PickProximity"},
                     {ProcedureNode::NodeType::PickRegion, "PickRegion"},
                     {ProcedureNode::NodeType::Process1D, "Process1D"},
                     {ProcedureNode::NodeType::Process2D, "Process2D"},
                     {ProcedureNode::NodeType::Process3D, "Process3D"},
                     {ProcedureNode::NodeType::Remove, "Remove"},
                     {ProcedureNode::NodeType::Select, "Select"},
                     {ProcedureNode::NodeType::Sequence, "Sequence"},
                     {ProcedureNode::NodeType::Sum1D, "Sum1D"},
                     {ProcedureNode::NodeType::Transmute, "Transmute"}});
}

// Return enum option info for NodeContext
EnumOptions<ProcedureNode::NodeContext> ProcedureNode::nodeContexts()
{
    return EnumOptions<ProcedureNode::NodeContext>("NodeContext", {{ProcedureNode::NoContext, "None"},
                                                                   {ProcedureNode::AnalysisContext, "Analysis"},
                                                                   {ProcedureNode::GenerationContext, "Generation"},
                                                                   {ProcedureNode::OperateContext, "Operate"}});
}

ProcedureNode::ProcedureNode(ProcedureNode::NodeType nodeType, ProcedureNode::NodeClass classType)
    : class_(classType), type_(nodeType), scope_(nullptr)
{
}

/*
 * Identity
 */

// Return node type
ProcedureNode::NodeType ProcedureNode::type() const { return type_; }

// Return whether the node is of the specified class
ProcedureNode::NodeClass ProcedureNode::nodeClass() const { return class_; }

// Return whether specified context is relevant for this node type
bool ProcedureNode::isContextRelevant(NodeContext context) { return false; }

// Return whether a name for the node must be provided
bool ProcedureNode::mustBeNamed() const { return true; }

// Set node name
void ProcedureNode::setName(std::string_view name) { name_ = DissolveSys::niceName(name); }

// Return node name
std::string_view ProcedureNode::name() const { return name_; }

/*
 * Keywords
 */

// Return keywords for this node
KeywordStore &ProcedureNode::keywords() { return keywords_; }
const KeywordStore &ProcedureNode::keywords() const { return keywords_; }

/*
 * Scope
 */

// Set scope
void ProcedureNode::setScope(std::shared_ptr<SequenceProcedureNode> scopeNode) { scope_ = scopeNode; }

// Return the parent non-sequence node which owns this node
NodeRef ProcedureNode::parent() const { return scope_ ? scope_->owner() : NodeRef(); }

// Find the nodes owned by this node
std::vector<ConstNodeRef> ProcedureNode::children() const { return {}; }

// Return scope (SequenceNode) in which this node exists
std::shared_ptr<SequenceProcedureNode> ProcedureNode::scope() const { return scope_; }

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

// Return named node if it is currently in scope (and matches the type / class given)
ConstNodeRef ProcedureNode::nodeInScope(std::string_view name, ConstNodeRef excludeNode,
                                        std::optional<ProcedureNode::NodeType> optNodeType,
                                        std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    if (!scope_)
        return nullptr;

    return scope_->nodeInScope(shared_from_this(), name, excludeNode, optNodeType, optNodeClass);
}

// Return list of nodes in this node's scope (and matches the type / class given)
std::vector<ConstNodeRef> ProcedureNode::nodesInScope(std::optional<ProcedureNode::NodeType> optNodeType,
                                                      std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    if (!scope_)
        return {};

    return scope_->nodesInScope(shared_from_this(), optNodeType, optNodeClass);
}

// Return named node if it exists anywhere in the same Procedure (and matches the type / class given)
ConstNodeRef ProcedureNode::nodeExists(std::string_view name, NodeRef excludeNode,
                                       std::optional<ProcedureNode::NodeType> optNodeType,
                                       std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    if (!scope_)
        return nullptr;

    return scope_->nodeExists(name, excludeNode, optNodeType, optNodeClass);
}

// Return list of nodes (of specified type / class) present in the Procedure
std::vector<ConstNodeRef> ProcedureNode::nodes(std::optional<ProcedureNode::NodeType> optNodeType,
                                               std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    if (!scope_)
        return {};

    return scope_->nodes(optNodeType, optNodeClass);
}

// Return the named parameter if it is currently in scope
std::shared_ptr<ExpressionVariable> ProcedureNode::parameterInScope(std::string_view name,
                                                                    std::shared_ptr<ExpressionVariable> excludeParameter)
{
    if (!scope_)
        return nullptr;

    return scope_->parameterInScope(shared_from_this(), name, std::move(excludeParameter));
}

// Return the named parameter if it exists anywhere in the same Procedure
std::shared_ptr<ExpressionVariable> ProcedureNode::parameterExists(std::string_view name,
                                                                   std::shared_ptr<ExpressionVariable> excludeParameter) const
{
    if (!scope_)
        return nullptr;

    return scope_->parameterExists(name, std::move(excludeParameter));
}

// Create and return reference list of parameters in scope
std::vector<std::shared_ptr<ExpressionVariable>> ProcedureNode::parametersInScope() const
{
    if (!scope_)
        return {};

    return scope_->parametersInScope(shared_from_this());
}

/*
 * Branch
 */

// Return whether this node has a branch
bool ProcedureNode::hasBranch() const { return false; }

// Return SequenceNode for the branch (if it exists)
std::shared_ptr<SequenceProcedureNode> ProcedureNode::branch() { return nullptr; }

/*
 * Parameters
 */

// Return the named parameter (if it exists)
std::shared_ptr<ExpressionVariable> ProcedureNode::getParameter(std::string_view name,
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
bool ProcedureNode::prepare(const ProcedureContext &procedureContext) { return true; }

// Execute node
bool ProcedureNode::execute(const ProcedureContext &procedureContext) { return true; }

// Finalise any necessary data after execution
bool ProcedureNode::finalise(const ProcedureContext &procedureContext) { return true; }

/*
 * Read / Write
 */

// Read node data from specified LineParser
bool ProcedureNode::deserialise(LineParser &parser, const CoreData &coreData)
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
        KeywordBase::ParseResult result = keywords_.deserialise(parser, coreData);
        if (result == KeywordBase::ParseResult::Unrecognised)
            return Messenger::error("Unrecognised keyword '{}' found while parsing {} node.\n", parser.argsv(0),
                                    nodeTypes().keyword(type_));
        else if (result == KeywordBase::ParseResult::Deprecated)
            Messenger::warn("The '{}' keyword is deprecated and will be removed in a future version.\n", parser.argsv(0));
        else if (result == KeywordBase::ParseResult::Failed)
            return Messenger::error("Failed to parse keyword '{}'.\n", parser.argsv(0));
    }

    return true;
}

// Write node data to specified LineParser
bool ProcedureNode::serialise(LineParser &parser, std::string_view prefix)
{
    // Block Start - node type and name (if specified)
    if (name_.empty())
    {
        if (!parser.writeLineF("{}{}\n", prefix, ProcedureNode::nodeTypes().keyword(type_)))
            return false;
    }
    else
    {
        if (!parser.writeLineF("{}{}  '{}'\n", prefix, ProcedureNode::nodeTypes().keyword(type_), name()))
            return false;
    }

    // Create new prefix
    std::string newPrefix = fmt::format("  {}", prefix);

    // Write keywords
    if (!keywords_.serialise(parser, newPrefix, true))
        return false;

    // Block End
    if (!parser.writeLineF("{}End{}\n", prefix, nodeTypes().keyword(type_)))
        return false;

    return true;
}

SerialisedValue ProcedureNode::serialise() const
{
    SerialisedValue result;
    result["type"] = nodeTypes().keyword(type_);
    if (mustBeNamed())
        result["name"] = name_;
    for (auto &[k, v] : keywords_.keywords())
        result[std::string(k)] = v->serialise();
    return result;
}

void ProcedureNode::deserialise(const SerialisedValue &node, const CoreData &data)
{
    if (mustBeNamed())
        name_ = toml::find<std::string>(node, "name");
    for (auto &[k, v] : keywords_.keywords())
        if (node.contains(std::string(k)))
            v->deserialise(node.at(std::string(k)));
}
