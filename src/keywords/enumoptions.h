/*
    *** Keyword - EnumOptions
    *** src/keywords/enumoptions.h
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
        : KeywordData<EnumOptions<E>>(KeywordBase::EnumOptionsData, options),
          EnumOptionsBaseKeyword(KeywordData<EnumOptions<E>>::data_)
    {
        // Set our array of valid values
        for (int n = 0; n < KeywordData<EnumOptions<E>>::data_.nOptions(); ++n)
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
    int minArguments() const { return 1; }
    // Return maximum number of arguments accepted
    int maxArguments() const { return 1; }
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData)
    {
        if (parser.hasArg(startArg))
        {
            // Check validity of the supplied keyword...
            if (!KeywordData<EnumOptions<E>>::data_.isValid(parser.argsv(startArg)))
                return KeywordData<EnumOptions<E>>::data_.errorAndPrintValid(parser.argsv(startArg));

            // Keyword recognised...
            EnumOptions<E> newOptions(KeywordData<EnumOptions<E>>::data_);
            newOptions.setCurrentOption(parser.argsv(startArg));
            if (!KeywordData<EnumOptions<E>>::setData(newOptions))
                return Messenger::error("An odd thing happened....\n");

            return true;
        }

        return false;
    }
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
    {
        return parser.writeLineF("{}{}  {}\n", prefix, keywordName, KeywordData<EnumOptions<E>>::data_.currentOptionKeyword());
    }

    /*
     * Set (implementing pure virtual from EnumOptionsBaseKeyword)
     */
    public:
    // Set new option index, informing KeywordBase
    void setEnumerationByIndex(int optionIndex)
    {
        KeywordData<EnumOptions<E>>::data_.setCurrentOptionIndex(optionIndex);
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
    std::string asString() { return std::string(KeywordData<EnumOptions<E>>::data_.currentOptionKeyword()); }
};
