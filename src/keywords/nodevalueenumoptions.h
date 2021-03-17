// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "keywords/data.h"
#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"
#include "templates/venum.h"

// Forward Declarations
class ProcedureNode;

// Keyword with NodeValue and EnumOptions base class
class NodeValueEnumOptionsBaseKeyword
{
    public:
    NodeValueEnumOptionsBaseKeyword(NodeValue &nodeValue, EnumOptionsBase &baseOptions)
        : nodeValue_(nodeValue), baseOptions_(baseOptions)
    {
    }

    /*
     * Source Data
     */
    private:
    // Source NodeValue
    NodeValue &nodeValue_;
    // Source EnumBaseOptions
    EnumOptionsBase &baseOptions_;

    public:
    // Return NodeValue
    const NodeValue &value() const { return nodeValue_; }
    // Return EnumBaseOptions
    const EnumOptionsBase &baseOptions() const { return baseOptions_; }

    /*
     * Set
     */
    public:
    // Set node value from expression text, informing KeywordBase
    virtual bool setValue(std::string_view expressionText) = 0;
    // Set new option index, informing KeywordBase
    virtual void setEnumerationByIndex(int optionIndex) = 0;

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    virtual int optionMask() const = 0;
};

// Keyword with NodeValue and EnumOptions
template <class E>
class NodeValueEnumOptionsKeyword : public NodeValueEnumOptionsBaseKeyword, public KeywordData<Venum<NodeValue, E>>
{
    public:
    NodeValueEnumOptionsKeyword(ProcedureNode *parentNode, NodeValue value, EnumOptions<E> enumOptions)
        : NodeValueEnumOptionsBaseKeyword(KeywordData<Venum<NodeValue, E>>::data_.value(),
                                          KeywordData<Venum<NodeValue, E>>::data_.baseOptions()),
          KeywordData<Venum<NodeValue, E>>(KeywordBase::NodeValueEnumOptionsData, Venum<NodeValue, E>(value, enumOptions))
    {
        parentNode_ = parentNode;
    }
    ~NodeValueEnumOptionsKeyword() {}

    /*
     * Parent Node
     */
    private:
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override { return 2; }
    // Return maximum number of arguments accepted
    int maxArguments() const override { return 2; }
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData)
    {
        // Check that the parent node has been set
        if (!parentNode_)
            return Messenger::error("Parent node in NodeValueEnumOptions keyword '{}' not set. Can't read data.\n",
                                    KeywordData<Venum<NodeValue, E>>::name());

        // Need two args...
        if (parser.hasArg(startArg + 1))
        {
            // Get any variables currently in scope
            auto vars = parentNode_->parametersInScope();

            // Parse the value to start with...
            if (!KeywordData<Venum<NodeValue, E>>::data_.value().set(parser.argsv(startArg), vars))
                return false;

            // Now the enum option
            if (!KeywordData<Venum<NodeValue, E>>::data_.setEnumeration(parser.argsv(startArg + 1)))
                return false;

            KeywordData<Venum<NodeValue, E>>::hasBeenSet();

            return true;
        }

        return false;
    }
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        return parser.writeLineF("{}{}  '{}'  {}\n", prefix, KeywordBase::name(),
                                 KeywordData<Venum<NodeValue, E>>::data_.value().asString(),
                                 KeywordData<Venum<NodeValue, E>>::data_.enumerationAsString());
    }

    /*
     * Set (implementing pure virtuals from NodeValueEnumOptionsBaseKeyword)
     */
    public:
    // Set node value from expression text, informing KeywordBase
    bool setValue(std::string_view expressionText)
    {
        if (!parentNode_)
            return Messenger::error("Can't read keyword {} since the parent ProcedureNode has not been set.\n",
                                    KeywordBase::name());

        // Get any variables currently in scope
        auto vars = parentNode_->parametersInScope();

        bool result = KeywordData<Venum<NodeValue, E>>::data_.value().set(expressionText, vars);

        KeywordData<Venum<NodeValue, E>>::hasBeenSet();

        return result;
    }
    // Set new option index, informing KeywordBase
    void setEnumerationByIndex(int optionIndex)
    {
        KeywordData<Venum<NodeValue, E>>::data_.setEnumerationByIndex(optionIndex);

        KeywordData<Venum<NodeValue, E>>::hasBeenSet();
    }

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    int optionMask() const { return KeywordBase::optionMask(); }

    /*
     * Conversion
     */
    public:
    // Return value (as int)
    int asInt() { return KeywordData<Venum<NodeValue, E>>::data_.value().asInteger(); }
    // Return value (as double)
    double asDouble() { return KeywordData<Venum<NodeValue, E>>::data_.value().asDouble(); }
};
