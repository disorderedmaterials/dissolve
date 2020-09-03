/*
    *** Keyword - NodeValueEnum
    *** src/keywords/nodevalueenum.h
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
    virtual bool setEnumerationByIndex(int optionIndex) = 0;

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
        : KeywordData<Venum<NodeValue, E>>(KeywordBase::NodeValueEnumOptionsData, Venum<NodeValue, E>(value, enumOptions)),
          NodeValueEnumOptionsBaseKeyword(KeywordData<Venum<NodeValue, E>>::data_.value(),
                                          KeywordData<Venum<NodeValue, E>>::data_.baseOptions())
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
    int minArguments() const { return 2; }
    // Return maximum number of arguments accepted
    int maxArguments() const { return 2; }
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData)
    {
        // Check that the parent node has been set
        if (!parentNode_)
            return Messenger::error("Parent node in NodeValueEnumOptions keyword '{}' not set. Can't read data.\n",
                                    KeywordData<Venum<NodeValue, E>>::name());

        // Need two args...
        if (parser.hasArg(startArg + 1))
        {
            // Parse the value to start with...
            if (!KeywordData<Venum<NodeValue, E>>::data_.value().set(parser.argsv(startArg), parentNode_->parametersInScope()))
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
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
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

        bool result = KeywordData<Venum<NodeValue, E>>::data_.value().set(expressionText, parentNode_->parametersInScope());

        KeywordData<Venum<NodeValue, E>>::hasBeenSet();

        return result;
    }
    // Set new option index, informing KeywordBase
    bool setEnumerationByIndex(int optionIndex)
    {
        bool result = KeywordData<Venum<NodeValue, E>>::data_.setEnumerationByIndex(optionIndex);

        KeywordData<Venum<NodeValue, E>>::hasBeenSet();

        return result;
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
