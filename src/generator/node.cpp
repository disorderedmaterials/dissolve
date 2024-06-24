// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/node.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "classes/site.h"
#include "expression/variable.h"
#include "generator/sequence.h"
#include <algorithm>
#include <memory>

// Return enum option info for NodeType
EnumOptions<GeneratorNode::NodeType> GeneratorNode::nodeTypes()
{
    return EnumOptions<GeneratorNode::NodeType>(
        "NodeType", {{GeneratorNode::NodeType::Add, "Add"},
                     {GeneratorNode::NodeType::AddPair, "AddPair"},
                     {GeneratorNode::NodeType::Box, "Box"},
                     {GeneratorNode::NodeType::CoordinateSets, "CoordinateSets"},
                     {GeneratorNode::NodeType::Copy, "Copy"},
                     {GeneratorNode::NodeType::CustomRegion, "CustomRegion"},
                     {GeneratorNode::NodeType::CylindricalGlobalPotential, "CylindricalGlobalPotential"},
                     {GeneratorNode::NodeType::CylindricalRegion, "CylindricalRegion"},
                     {GeneratorNode::NodeType::DynamicSite, "DynamicSite"},
                     {GeneratorNode::NodeType::GeneralRegion, "GeneralRegion"},
                     {GeneratorNode::NodeType::ImportCoordinates, "ImportCoordinates"},
                     {GeneratorNode::NodeType::IterateSelection, "IterateSelection"},
                     {GeneratorNode::NodeType::Parameters, "Parameters"},
                     {GeneratorNode::NodeType::Pick, "Pick"},
                     {GeneratorNode::NodeType::PickProximity, "PickProximity"},
                     {GeneratorNode::NodeType::PickRegion, "PickRegion"},
                     {GeneratorNode::NodeType::RegionalGlobalPotential, "RegionalGlobalPotential"},
                     {GeneratorNode::NodeType::Remove, "Remove"},
                     {GeneratorNode::NodeType::RestraintPotential, "RestraintPotential"},
                     {GeneratorNode::NodeType::RotateFragment, "RotateFragment"},
                     {GeneratorNode::NodeType::Select, "Select"},
                     {GeneratorNode::NodeType::Sequence, "Sequence"},
                     {GeneratorNode::NodeType::SphericalGlobalPotential, "SphericalGlobalPotential"},
                     {GeneratorNode::NodeType::SizeFactor, "SizeFactor"},
                     {GeneratorNode::NodeType::Temperature, "Temperature"},
                     {GeneratorNode::NodeType::Transmute, "Transmute"}});
}

GeneratorNode::GeneratorNode(NodeType nodeType) : type_(nodeType) {}

/*
 * Identity
 */

// Return node type
GeneratorNode::NodeType GeneratorNode::type() const { return type_; }

// Return whether a name for the node must be provided
bool GeneratorNode::mustBeNamed() const { return true; }

// Set node name
void GeneratorNode::setName(std::string_view name)
{
    name_ = DissolveSys::niceName(name);

    // Re-set prefixes with the new node name, except if this is a Parameters-type node
    if (type_ != GeneratorNode::NodeType::Parameters)
        for (auto &par : parameters_)
            par->setNamePrefix(name_);
}

// Return node name
std::string_view GeneratorNode::name() const { return name_; }

/*
 * Keywords
 */

// Return keywords for this node
KeywordStore &GeneratorNode::keywords() { return keywords_; }
const KeywordStore &GeneratorNode::keywords() const { return keywords_; }

/*
 * Scope
 */

// Set scope
void GeneratorNode::setScope(GeneratorNodeSequence &scopeNode) { scope_ = scopeNode; }

// Return the parent node which owns this node
GeneratorNode *GeneratorNode::parent() const
{
    return scope_ && (*scope_).get().owner() ? &((*scope_).get().owner()->get()) : nullptr;
}

// Return scope (GeneratorNodeSequence) in which this node exists
OptionalReferenceWrapper<GeneratorNodeSequence> GeneratorNode::scope() const { return scope_; }

// Return named node, optionally matching the type / class given, in or out of scope
ConstNodeRef GeneratorNode::getNodeInScope(std::string_view name, const ConstNodeRef &excludeNode,
                                           const NodeTypeVector &allowedNodeTypes) const
{
    if (!scope_)
        return nullptr;
    const auto &scope = (*scope_).get();

    return scope.nodeInScope(shared_from_this(), name, excludeNode, allowedNodeTypes);
}

// Return nodes in scope, optionally matching the type / class given
std::vector<ConstNodeRef> GeneratorNode::getNodesInScope(const NodeTypeVector &allowedNodeTypes) const
{
    if (!scope_)
        return {};

    return (*scope_).get().nodesInScope(shared_from_this(), allowedNodeTypes);
}

// Return the named parameter in scope
std::shared_ptr<ExpressionVariable>
GeneratorNode::getParameterInScope(std::string_view name, const std::shared_ptr<ExpressionVariable> &excludeParameter) const
{
    if (!scope_)
        return nullptr;
    auto &scope = (*scope_).get();

    return scope.parameterInScope(shared_from_this(), name, excludeParameter);
}

// Return all parameters in scope
std::vector<std::shared_ptr<ExpressionVariable>> GeneratorNode::getParametersInScope() const
{
    if (!scope_)
        return {};

    return (*scope_).get().parametersInScope(shared_from_this());
}

/*
 * Branch
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<GeneratorNodeSequence> GeneratorNode::branch() { return {}; }

/*
 * Parameters
 */

// Add new parameter
std::shared_ptr<ExpressionVariable> GeneratorNode::addParameter(std::string_view name, const ExpressionValue &initialValue)
{
    auto &newVar = parameters_.emplace_back(std::make_shared<ExpressionVariable>(name, initialValue));
    if (type_ != GeneratorNode::NodeType::Parameters)
        newVar->setNamePrefix(name_);
    return newVar;
}

// Set named parameter in supplied vector
bool GeneratorNode::setParameter(std::vector<std::shared_ptr<ExpressionVariable>> &parameters, std::string_view parameter,
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
std::shared_ptr<ExpressionVariable> GeneratorNode::getParameter(std::string_view name,
                                                                const std::shared_ptr<ExpressionVariable> &excludeParameter)
{
    for (auto var : parameters_)
        if ((var != excludeParameter) && (DissolveSys::sameString(var->name(), name)))
            return var;

    return nullptr;
}

// Return references to all parameters for this node
const std::vector<std::shared_ptr<ExpressionVariable>> &GeneratorNode::parameters() const { return parameters_; }

/*
 * Execution
 */

// Prepare any necessary data, ready for execution
bool GeneratorNode::prepare(const ProcedureContext &procedureContext) { return true; }

// Execute node
bool GeneratorNode::execute(const ProcedureContext &procedureContext) { return true; }

/*
 * Read / Write
 */

// Read node data from specified LineParser
bool GeneratorNode::deserialise(LineParser &parser, const CoreData &coreData)
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
bool GeneratorNode::serialise(LineParser &parser, std::string_view prefix)
{
    // Block Start - node type and name (if specified)
    if (name_.empty())
    {
        if (!parser.writeLineF("{}{}\n", prefix, GeneratorNode::nodeTypes().keyword(type_)))
            return false;
    }
    else
    {
        if (!parser.writeLineF("{}{}  '{}'\n", prefix, GeneratorNode::nodeTypes().keyword(type_), name()))
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
SerialisedValue GeneratorNode::serialise() const
{
    SerialisedValue result = {{"type", nodeTypes().keyword(type_)}};
    if (mustBeNamed())
        result["name"] = name_;
    return keywords_.serialiseOnto(result);
}

// Read values from a serialisable value
void GeneratorNode::deserialise(const SerialisedValue &node, const CoreData &data)
{
    if (mustBeNamed())
        setName(toml::find<std::string>(node, "name"));
    keywords_.deserialiseFrom(node, data);
}
