// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "keywords/data.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class NodeValue;
class ProcedureNode;

// Keyword with ProcedureNode base class
class NodeKeywordBase
{
    public:
    NodeKeywordBase(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope);
    virtual ~NodeKeywordBase();

    /*
     * Parent Node
     */
    private:
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

    public:
    // Return parent ProcedureNode
    ProcedureNode *parentNode() const;

    /*
     * Target Node
     */
    private:
    // Target node type to allow
    ProcedureNode::NodeType nodeType_;
    // Whether to accept nodes within scope only
    bool onlyInScope_;

    public:
    // Return target node type to allow
    ProcedureNode::NodeType nodeType() const;
    // Return whether to accept nodes within scope only
    bool onlyInScope() const;
    // Set the target node
    virtual bool setNode(ProcedureNode *node) = 0;
    // Return the current target node as the base class
    virtual const ProcedureNode *procedureNode() const = 0;

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    virtual int optionMask() const = 0;
};

// Keyword with ProcedureNode
template <class N> class NodeKeyword : public NodeKeywordBase, public KeywordData<N *>
{
    public:
    NodeKeyword(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope, N *node = nullptr)
        : NodeKeywordBase(parentNode, nodeType, onlyInScope), KeywordData<N *>(KeywordBase::NodeData, node)
    {
    }
    ~NodeKeyword() {}

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override { return 1; }
    // Return maximum number of arguments accepted
    int maxArguments() const override { return 1; }
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData)
    {
        if (!parentNode())
            return Messenger::error("Can't read keyword {} since the parent ProcedureNode has not been set.\n",
                                    KeywordBase::name());

        // Locate the named node - don't prune by type yet (we'll check that in setNode())
        ProcedureNode *node = onlyInScope() ? parentNode()->nodeInScope(parser.argsv(startArg))
                                            : parentNode()->nodeExists(parser.argsv(startArg));
        if (!node)
            return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg),
                                    KeywordBase::name());

        return setNode(node);
    }
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        // No need to write the keyword if the node pointer is null
        if (KeywordData<N *>::data_ == nullptr)
            return true;

        if (!parser.writeLineF("{}{}  '{}'\n", prefix, KeywordBase::name(), KeywordData<N *>::data_->name()))
            return false;

        return true;
    }

    /*
     * Target Node
     */
    public:
    // Set the target node
    bool setNode(ProcedureNode *node)
    {
        if (!node)
            return false;

        if (!node->isType(nodeType()))
            return Messenger::error("Node '{}' is of type {}, but the {} keyword requires a node of type {}.\n", node->name(),
                                    ProcedureNode::nodeTypes().keyword(node->type()), KeywordBase::name(),
                                    ProcedureNode::nodeTypes().keyword(nodeType()));

        KeywordData<N *>::data_ = dynamic_cast<N *>(node);

        KeywordData<N *>::set_ = true;

        return true;
    }
    // Return the current target node as the base class
    const ProcedureNode *procedureNode() const { return KeywordData<N *>::data_; }
    // Return the current target node
    N *node() const { return KeywordData<N *>::data_; }

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    int optionMask() const { return KeywordBase::optionMask(); }

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied ProcedureNode in the contained data
    void removeReferencesTo(N *node)
    {
        if (KeywordData<N *>::data_ == node)
            KeywordData<N *>::data_ = nullptr;
    }
};
