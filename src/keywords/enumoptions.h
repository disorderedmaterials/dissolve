// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "base/lineparser.h"
#include "keywords/data.h"

// Forward Declarations
/* none */

// EnumOptionsKeyword base class
class EnumOptionsBaseKeyword
{
    public:
    EnumOptionsBaseKeyword(EnumOptionsBase &baseOptions) : baseOptions_(baseOptions) {}

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
template <class E> class EnumOptionsKeyword : public EnumOptionsBaseKeyword, public KeywordData<EnumOptions<E>>
{
    public:
    EnumOptionsKeyword(EnumOptions<E> options)
        : EnumOptionsBaseKeyword(KeywordData<EnumOptions<E>>::data_), KeywordData<EnumOptions<E>>(KeywordBase::EnumOptionsData,
                                                                                                  options)
    {
        // Set our array of valid values
        for (auto n = 0; n < KeywordData<EnumOptions<E>>::data_.nOptions(); ++n)
            validKeywords_.emplace_back(std::string(KeywordData<EnumOptions<E>>::data_.keywordByIndex(n)));
    }
    ~EnumOptionsKeyword() {}

    /*
     * Data Validation
     */
    private:
    // List of valid keyword values
    std::vector<std::string> validKeywords_;

    public:
    // Return validation list
    const std::vector<std::string> &validationList() { return validKeywords_; }
    // Validate supplied value
    bool isValid(std::string_view value) { return KeywordData<EnumOptions<E>>::data_.isValid(value); }

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
        if (parser.hasArg(startArg))
        {
            // Check validity of the supplied keyword...
            if (!KeywordData<EnumOptions<E>>::data_.isValid(parser.argsv(startArg)))
                return KeywordData<EnumOptions<E>>::data_.errorAndPrintValid(parser.argsv(startArg));

            KeywordData<EnumOptions<E>>::data_.set(parser.argsv(startArg));
            KeywordData<EnumOptions<E>>::hasBeenSet();

            return true;
        }

        return false;
    }
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        return parser.writeLineF("{}{}  {}\n", prefix, keywordName, KeywordData<EnumOptions<E>>::data_.keyword());
    }

    /*
     * Set (implementing pure virtual from EnumOptionsBaseKeyword)
     */
    public:
    // Set new option index, informing KeywordBase
    void setEnumerationByIndex(int optionIndex)
    {
        KeywordData<EnumOptions<E>>::data_.setIndex(optionIndex);
        KeywordData<EnumOptions<E>>::hasBeenSet();
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
    // Return value (as string)
    std::string asString() { return std::string(KeywordData<EnumOptions<E>>::data_.keyword()); }
};
