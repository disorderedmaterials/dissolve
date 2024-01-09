// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"
#include "keywords/base.h"
#include "procedure/nodeValue.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class ProcedureNode;

// Keyword managing NodeValue and EnumOptions base class
class NodeValueEnumOptionsBaseKeyword : public KeywordBase
{
    public:
    NodeValueEnumOptionsBaseKeyword(EnumOptionsBase &baseOptions) : KeywordBase(typeid(this)), baseOptions_(baseOptions) {}

    /*
     * Data
     */
    protected:
    // Reference to EnumBaseOptions
    EnumOptionsBase &baseOptions_;

    public:
    // Return data
    virtual const NodeValue &value() const = 0;
    virtual int enumerationIndex() const = 0;
    // Set the value from supplied expression text
    bool setData(std::string_view expressionText);
    // Return EnumBaseOptions
    const EnumOptionsBase &baseOptions() const { return baseOptions_; }
    // Set node value from expression text, informing KeywordBase
    virtual bool setValue(std::string_view expressionText) = 0;
    // Set new option by index
    virtual void setEnumeration(int optionIndex) = 0;
    // Set new option by name
    virtual bool setEnumeration(std::string_view keyword) = 0;
};

// Keyword managing NodeValue and EnumOptions
template <class E> class NodeValueEnumOptionsKeyword : public NodeValueEnumOptionsBaseKeyword
{
    public:
    NodeValueEnumOptionsKeyword(std::pair<NodeValue, E> &data, ProcedureNode *parentNode, EnumOptions<E> optionData)
        : NodeValueEnumOptionsBaseKeyword(optionData_), data_(data), default_(data), parentNode_(parentNode),
          optionData_(optionData)
    {
    }
    ~NodeValueEnumOptionsKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::pair<NodeValue, E> &data_;
    // Initial Value
    const std::pair<NodeValue, E> default_;
    // Parent ProcedureNode
    const ProcedureNode *parentNode_;
    // Related EnumOptions data
    EnumOptions<E> optionData_;

    public:
    const NodeValue &value() const override { return data_.first; }
    int enumerationIndex() const override { return data_.second; }
    // Set node value from expression text
    bool setValue(std::string_view expressionText) override
    {
        assert(parentNode_);

        if (!data_.first.set(expressionText, parentNode_->getParameters()))
            return false;

        return true;
    }
    // Set new option by index
    void setEnumeration(int optionIndex) override { data_.second = optionData_.enumerationByIndex(optionIndex); }
    // Set new option by name
    bool setEnumeration(std::string_view keyword) override
    {
        if (!optionData_.isValid(keyword))
            return optionData_.errorAndPrintValid(keyword);

        data_.second = optionData_.enumeration(keyword);

        return true;
    }

    /*
     * Arguments
     */
    public:
    // Default
    bool isDefault() const override { return data_ == default_; }
    // Return minimum number of arguments accepted
    int minArguments() const override { return 2; }
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override { return 2; }
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        // Need two args...
        if (!parser.hasArg(startArg + 1))
            return false;

        // Parse the value to start with...
        if (!setValue(parser.argsv(startArg)))
            return false;

        // Now the enum option
        return setEnumeration(parser.argsv(startArg + 1));
    }
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        return parser.writeLineF("{}{}  '{}'  {}\n", prefix, KeywordBase::name(), data_.first.asString(),
                                 optionData_.keyword(data_.second));
    }

    // Read values from a serialisable value
    SerialisedValue serialise() const override
    {
        return {{"value", data_.first.serialise()}, {"option", optionData_.serialise(data_.second)}};
    }

    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override
    {
        data_.first.deserialise(node.at("value"), parentNode_->getParameters());
        data_.second = optionData_.deserialise(node.at("option"));
    }
};
