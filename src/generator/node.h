// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "generator/aliases.h"
#include "generator/context.h"
#include "keywords/store.h"
#include "templates/optionalRef.h"

// Forward Declarations
class GeneratorNodeSequence;

// Generator Node
class GeneratorNode : public std::enable_shared_from_this<GeneratorNode>, public Serialisable<const CoreData &>
{
    public:
    // Node Types
    enum class NodeType
    {
        Add,
        AddPair,
        Box,
        CoordinateSets,
        Copy,
        CustomRegion,
        CylindricalGlobalPotential,
        CylindricalRegion,
        DynamicSite,
        GeneralRegion,
        ImportCoordinates,
        IterateSelection,
        Parameters,
        Pick,
        PickProximity,
        PickRegion,
        RegionalGlobalPotential,
        Remove,
        RestraintPotential,
        RotateFragment,
        Select,
        Sequence,
        SphericalGlobalPotential,
        SizeFactor,
        Temperature,
        Transmute
    };
    // Typedef for node type vector
    using NodeTypeVector = std::vector<GeneratorNode::NodeType>;
    // Return enum option info for NodeType
    static EnumOptions<NodeType> nodeTypes();

    GeneratorNode(NodeType nodeType);
    virtual ~GeneratorNode() = default;

    /*
     * Identity
     */
    protected:
    // Node type
    NodeType type_;
    // Node name
    std::string name_;

    public:
    // Return node type
    NodeType type() const;
    // Return whether a name for the node must be provided
    virtual bool mustBeNamed() const;
    // Set node name
    void setName(std::string_view name);
    // Return node name
    std::string_view name() const;

    /*
     * Keywords
     */
    protected:
    // Keywords for this node
    KeywordStore keywords_;

    public:
    // Return keywords for this node
    KeywordStore &keywords();
    const KeywordStore &keywords() const;

    /*
     * Scope
     */
    private:
    // Scope (SequenceNode) in which this node exists
    OptionalReferenceWrapper<GeneratorNodeSequence> scope_;

    public:
    // Return the parent node which owns this node
    GeneratorNode *parent() const;
    // Set scope
    void setScope(GeneratorNodeSequence &scopeNode);
    // Return scope (GeneratorNodeSequence) in which this node exists
    OptionalReferenceWrapper<GeneratorNodeSequence> scope() const;
    // Return named node which must optionally match one of the types given, in scope
    ConstNodeRef getNodeInScope(std::string_view name, const ConstNodeRef &excludeNode = {},
                                const NodeTypeVector &allowedNodeTypes = {}) const;
    // Return nodes of the specified types in scope
    std::vector<ConstNodeRef> getNodesInScope(const NodeTypeVector &allowedNodeTypes = {}) const;
    // Return the named parameter in scope
    std::shared_ptr<ExpressionVariable>
    getParameterInScope(std::string_view name, const std::shared_ptr<ExpressionVariable> &excludeParameter = {}) const;
    // Return all parameters in scope
    std::vector<std::shared_ptr<ExpressionVariable>> getParametersInScope() const;

    /*
     * Branch
     */
    public:
    // Return the branch from this node (if it has one)
    virtual OptionalReferenceWrapper<GeneratorNodeSequence> branch();

    /*
     * Parameters
     */
    protected:
    // Defined parameters
    std::vector<std::shared_ptr<ExpressionVariable>> parameters_;
    // Set named parameter in supplied vector
    bool setParameter(std::vector<std::shared_ptr<ExpressionVariable>> &parameters, std::string_view parameter,
                      ExpressionValue value);

    public:
    // Add new parameter
    std::shared_ptr<ExpressionVariable> addParameter(std::string_view name, const ExpressionValue &initialValue = {});
    // Return the named parameter (if it exists)
    std::shared_ptr<ExpressionVariable> getParameter(std::string_view name,
                                                     const std::shared_ptr<ExpressionVariable> &excludeParameter = {});
    // Return references to all parameters for this node
    const std::vector<std::shared_ptr<ExpressionVariable>> &parameters() const;

    /*
     * Execution
     */
    public:
    // Prepare any necessary data, ready for execution
    virtual bool prepare(const GeneratorContext &generatorContext);
    // Execute node
    virtual bool execute(const GeneratorContext &generatorContext);

    /*
     * Read / Write
     */
    public:
    // Read node data from specified LineParser
    virtual bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write node data to specified LineParser
    virtual bool serialise(LineParser &parser, std::string_view prefix);
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
