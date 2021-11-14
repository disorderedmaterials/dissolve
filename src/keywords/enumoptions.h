// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "base/lineparser.h"
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
    // Set new option index, informing KeywordBase
    virtual void setEnumerationByIndex(int optionIndex) = 0;

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    virtual int optionMask() const = 0;
};

// Keyword based on EnumOptions
template <class E> class EnumOptionsKeyword : public EnumOptionsBaseKeyword
{
    public:
    explicit EnumOptionsKeyword(E &data, EnumOptions<E> optionData)
        : EnumOptionsBaseKeyword(optionData_), data_(data), optionData_(optionData)
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
            setAsModified();

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
    // Set new option index, informing KeywordBase
    void setEnumerationByIndex(int optionIndex) override
    {
        data_ = optionData_.enumerationByIndex(optionIndex);
        setAsModified();
    }

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    int optionMask() const override { return KeywordBase::optionMask(); }
};
