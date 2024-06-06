// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/node.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "classes/site.h"
#include "expression/variable.h"
#include "procedure/nodes/sequence.h"
#include <algorithm>
#include <memory>
#include <utility>

// Return enum option info for NodeType
EnumOptions<ProcedureNode::NodeType> ProcedureNode::nodeTypes()
{
    return EnumOptions<ProcedureNode::NodeType>(
        "NodeType", {{ProcedureNode::NodeType::Add, "Add"},
                     {ProcedureNode::NodeType::AddPair, "AddPair"},
                     {ProcedureNode::NodeType::Box, "Box"},
                     {ProcedureNode::NodeType::CoordinateSets, "CoordinateSets"},
                     {ProcedureNode::NodeType::Copy, "Copy"},
                     {ProcedureNode::NodeType::CustomRegion, "CustomRegion"},
                     {ProcedureNode::NodeType::CylindricalGlobalPotential, "CylindricalGlobalPotential"},
                     {ProcedureNode::NodeType::CylindricalRegion, "CylindricalRegion"},
                     {ProcedureNode::NodeType::DynamicSite, "DynamicSite"},
                     {ProcedureNode::NodeType::GeneralRegion, "GeneralRegion"},
                     {ProcedureNode::NodeType::ImportCoordinates, "ImportCoordinates"},
                     {ProcedureNode::NodeType::IterateSelection, "IterateSelection"},
                     {ProcedureNode::NodeType::Parameters, "Parameters"},
                     {ProcedureNode::NodeType::Pick, "Pick"},
                     {ProcedureNode::NodeType::PickProximity, "PickProximity"},
                     {ProcedureNode::NodeType::PickRegion, "PickRegion"},
                     {ProcedureNode::NodeType::RegionalGlobalPotential, "RegionalGlobalPotential"},
                     {ProcedureNode::NodeType::Remove, "Remove"},
                     {ProcedureNode::NodeType::RestraintPotential, "RestraintPotential"},
                     {ProcedureNode::NodeType::RotateFragment, "RotateFragment"},
                     {ProcedureNode::NodeType::RunLayer, "RunLayer"},
                     {ProcedureNode::NodeType::Select, "Select"},
                     {ProcedureNode::NodeType::Sequence, "Sequence"},
                     {ProcedureNode::NodeType::SphericalGlobalPotential, "SphericalGlobalPotential"},
                     {ProcedureNode::NodeType::SizeFactor, "SizeFactor"},
                     {ProcedureNode::NodeType::Temperature, "Temperature"},
                     {ProcedureNode::NodeType::Transmute, "Transmute"}});
}

// Return the lowerCamelCase name of the node type provided
std::string ProcedureNode::lccNodeType(NodeType nodeType)
{
    auto lccNodeName = ProcedureNode::nodeTypes().keyword(nodeType);
    lccNodeName.front() = tolower(lccNodeName.front());
    return lccNodeName;
}

// Return enum option info for NodeContext
EnumOptions<ProcedureNode::NodeContext> ProcedureNode::nodeContexts()
{
    return EnumOptions<ProcedureNode::NodeContext>("NodeContext", {{ProcedureNode::NoContext, "None"},
                                                                   {ProcedureNode::AnalysisContext, "Analysis"},
                                                                   {ProcedureNode::GenerationContext, "Generation"},
                                                                   {ProcedureNode::ControlContext, "Control"},
                                                                   {ProcedureNode::OperateContext, "Operate"},
                                                                   {ProcedureNode::AnyContext, "Any"},
                                                                   {ProcedureNode::InheritContext, "Inherit"}});
}

ProcedureNode::ProcedureNode(ProcedureNode::NodeType nodeType, std::vector<NodeContext> relevantContexts)
    : type_(nodeType), relevantContexts_(std::move(relevantContexts))
{
}

/*
 * Identity
 */

// Return node type
ProcedureNode::NodeType ProcedureNode::type() const { return type_; }

// Return whether the supplied context is relevant for the current node
bool ProcedureNode::isContextRelevant(NodeContext targetContext) const
{
    // Check for spurious context checks
    if (targetContext == ProcedureNode::NodeContext::InheritContext)
        throw(std::runtime_error(fmt::format("Attempted to context check node '{}' with an InheritedContext.\n",
                                             name_.empty() ? nodeTypes().keyword(type_) : name_)));

    // If the node is suitable in Any context, or if there is no context (None) return immediately
    if (targetContext == ProcedureNode::NodeContext::NoContext || targetContext == ProcedureNode::NodeContext::AnyContext ||
        std::find(relevantContexts_.begin(), relevantContexts_.end(), ProcedureNode::NodeContext::AnyContext) !=
            relevantContexts_.end())
        return true;

    return std::find(relevantContexts_.begin(), relevantContexts_.end(), targetContext) != relevantContexts_.end();
}

// Return whether a name for the node must be provided
bool ProcedureNode::mustBeNamed() const { return true; }

// Set node name
void ProcedureNode::setName(std::string_view name)
{
    name_ = DissolveSys::niceName(name);

    // Re-set prefixes with the new node name, except if this is a Parameters-type node
    if (type_ != ProcedureNode::NodeType::Parameters)
        for (auto &par : parameters_)
            par->setNamePrefix(name_);
}

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
void ProcedureNode::setScope(ProcedureNodeSequence &scopeNode) { scope_ = scopeNode; }

// Return the parent node which owns this node
ProcedureNode *ProcedureNode::parent() const
{
    return scope_ && (*scope_).get().owner() ? &((*scope_).get().owner()->get()) : nullptr;
}

// Return scope (ProcedureNodeSequence) in which this node exists
OptionalReferenceWrapper<ProcedureNodeSequence> ProcedureNode::scope() const { return scope_; }

// Return context of scope in which this node exists
ProcedureNode::NodeContext ProcedureNode::scopeContext() const
{
    if (!scope_)
        return ProcedureNode::NoContext;

    auto context = (*scope_).get().context();
    return context == ProcedureNode::NodeContext::InheritContext ? parent()->scopeContext() : context;
}

// Return named node, optionally matching the type / class given, in or out of scope
ConstNodeRef ProcedureNode::getNode(std::string_view name, bool onlyInScope, ConstNodeRef excludeNode,
                                    const NodeTypeVector &allowedNodeTypes) const
{
    if (!scope_)
        return nullptr;
    const auto &scope = (*scope_).get();

    return onlyInScope ? scope.nodeInScope(shared_from_this(), name, excludeNode, allowedNodeTypes)
                       : scope.nodeExists(name, excludeNode, allowedNodeTypes);
}

// Return nodes, optionally matching the type / class given, in or out of scope
std::vector<ConstNodeRef> ProcedureNode::getNodes(bool onlyInScope, const NodeTypeVector &allowedNodeTypes) const
{
    if (!scope_)
        return {};

    if (onlyInScope)
        return (*scope_).get().nodesInScope(shared_from_this(), allowedNodeTypes);

    // Find the topmost (root) scope and search from there.
    auto optScope = scope_;
    while (optScope->get().owner() && optScope->get().owner()->get().scope())
        optScope = optScope->get().owner()->get().scope();

    return optScope->get().nodes(allowedNodeTypes);
}

// Return the named parameter, in or out of scope
std::shared_ptr<ExpressionVariable> ProcedureNode::getParameter(std::string_view name, bool onlyInScope,
                                                                std::shared_ptr<ExpressionVariable> excludeParameter) const
{
    if (!scope_)
        return nullptr;
    auto &scope = (*scope_).get();

    return onlyInScope ? scope.parameterInScope(shared_from_this(), name, std::move(excludeParameter))
                       : scope.parameterExists(name, std::move(excludeParameter));
}

// Return all parameters in scope
std::vector<std::shared_ptr<ExpressionVariable>> ProcedureNode::getParameters() const
{
    if (!scope_)
        return {};

    return (*scope_).get().parametersInScope(shared_from_this());
}

/*
 * Branch
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> ProcedureNode::branch() { return {}; }

/*
 * Parameters
 */

// Add new parameter
std::shared_ptr<ExpressionVariable> ProcedureNode::addParameter(std::string_view name, const ExpressionValue &initialValue)
{
    auto &newVar = parameters_.emplace_back(std::make_shared<ExpressionVariable>(name, initialValue));
    if (type_ != ProcedureNode::NodeType::Parameters)
        newVar->setNamePrefix(name_);
    return newVar;
}

// Set named parameter in supplied vector
bool ProcedureNode::setParameter(std::vector<std::shared_ptr<ExpressionVariable>> &parameters, std::string_view parameter,
                                 ExpressionValue value)
{
    auto it = std::find_if(parameters.begin(), parameters.end(),
                           [parameter](const auto &p) { return DissolveSys::sameString(p->name(), parameter); });
    if (it == parameters.end())
        return false;

    (*it)->setValue(value);

    return true;
}

// Return the named parameter (if it exists)
std::shared_ptr<ExpressionVariable> ProcedureNode::getParameter(std::string_view name,
                                                                std::shared_ptr<ExpressionVariable> excludeParameter)
{
    for (auto var : parameters_)
        if ((var != excludeParameter) && (DissolveSys::sameString(var->name(), name)))
            return var;

    return nullptr;
}

// Return references to all parameters for this node
const std::vector<std::shared_ptr<ExpressionVariable>> &ProcedureNode::parameters() const { return parameters_; }

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
    auto errorsEncountered = false;
    while (!parser.eofOrBlank())
    {
        // Read and parse the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Is this the end of the node block?
        if (DissolveSys::sameString(parser.argsv(0), fmt::format("End{}", nodeTypes().keyword(type_))))
            return !errorsEncountered;

        // Try to parse this line as a keyword
        KeywordBase::ParseResult result = keywords_.deserialise(parser, coreData);
        if (result == KeywordBase::ParseResult::Unrecognised)
            return Messenger::error("Unrecognised keyword '{}' found while parsing {} node.\n", parser.argsv(0),
                                    nodeTypes().keyword(type_));
        else if (result == KeywordBase::ParseResult::Deprecated)
            Messenger::warn("The '{}' keyword is deprecated and will be removed in a future version.\n", parser.argsv(0));
        else if (result == KeywordBase::ParseResult::Failed)
            errorsEncountered = true;
    }

    return (!errorsEncountered);
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

// Express as a serialisable value
SerialisedValue ProcedureNode::serialise() const
{
    SerialisedValue result = {{"type", nodeTypes().keyword(type_)}};
    if (mustBeNamed())
        result["name"] = name_;
    return keywords_.serialiseOnto(result);
}

// Read values from a serialisable value
void ProcedureNode::deserialise(const SerialisedValue &node, const CoreData &data)
{
    if (mustBeNamed())
        setName(toml::find<std::string>(node, "name"));
    keywords_.deserialiseFrom(node, data);
}
