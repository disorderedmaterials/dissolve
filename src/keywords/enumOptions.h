// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/lineParser.h"
#include "keywords/base.h"

// EnumOptionsKeyword base class
class EnumOptionsBaseKeyword : public KeywordBase
{
    public:
    explicit EnumOptionsBaseKeyword(EnumOptionsBase &baseOptions) : KeywordBase(typeid(this)), baseOptions_(baseOptions) {}

    /*
     * Source Options
     */
    private:
    // Source EnumBaseOptions
    EnumOptionsBase &baseOptions_;

    public:
    // Return EnumBaseOptions
    const EnumOptionsBase &baseOptions() const { return baseOptions_; }

    /*
     * Set
     */
    public:
    // Return data as integer index
    virtual int enumerationByIndex() const = 0;
    // Set new option index, informing KeywordBase
    virtual void setEnumerationByIndex(int optionIndex) = 0;
};

// Keyword based on EnumOptions
template <class E> class EnumOptionsKeyword : public EnumOptionsBaseKeyword
{
    public:
    explicit EnumOptionsKeyword(E &data, EnumOptions<E> optionData)
        : EnumOptionsBaseKeyword(optionData_), data_(data), default_(data), optionData_(optionData)
    {
        // Set our array of valid values
        for (auto n = 0; n < optionData_.nOptions(); ++n)
            validKeywords_.emplace_back(std::string(optionData_.keywordByIndex(n)));
    }
    ~EnumOptionsKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    E &data_;
    // Initial value
    const E default_;
    // Related EnumOptions data
    EnumOptions<E> optionData_;
    // List of valid keyword values
    std::vector<std::string> validKeywords_;

    public:
    // Return reference to data
    E &data() { return data_; }
    const E &data() const { return data_; }
    // Validate supplied value
    bool isValid(std::string_view value) { return optionData_.isValid(value); }

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        if (parser.hasArg(startArg))
        {
            // Check validity of the supplied keyword...
            if (!optionData_.isValid(parser.argsv(startArg)))
                return optionData_.errorAndPrintValid(parser.argsv(startArg));

            data_ = optionData_.enumeration(parser.argsv(startArg));

            return true;
        }

        return false;
    }
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        return parser.writeLineF("{}{}  {}\n", prefix, keywordName, optionData_.keyword(data_));
    }

    /*
     * Set (implementing pure virtual from EnumOptionsBaseKeyword)
     */
    public:
    // Return data as integer index
    int enumerationByIndex() const override
    {
        for (auto n = 0; n < optionData_.nOptions(); ++n)
            if (optionData_.enumerationByIndex(n) == data_)
                return n;
        throw(std::runtime_error("Couldn't retrieve index for enumeration as it doesn't exist.\n"));
    };
    // Set new option index
    void setEnumerationByIndex(int optionIndex) override { data_ = optionData_.enumerationByIndex(optionIndex); }

    // Has not changed from initial value
    bool isDefault() const override { return data_ == default_; }
    // Express as a serialisable value
    SerialisedValue serialise() const override { return optionData_.keyword(data_); }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override { data_ = optionData_.deserialise(node); }
};
