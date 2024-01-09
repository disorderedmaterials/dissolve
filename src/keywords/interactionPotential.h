// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/lineParser.h"
#include "classes/interactionPotential.h"
#include "keywords/base.h"

// InteractionPotentialKeyword base class
class InteractionPotentialBaseKeyword : public KeywordBase
{
    public:
    InteractionPotentialBaseKeyword(EnumOptionsBase &formOptions) : KeywordBase(typeid(this)), formOptions_(formOptions) {}

    /*
     * Source Form Options
     */
    private:
    // Source EnumBaseOptions for functional form
    EnumOptionsBase &formOptions_;

    public:
    // Return EnumBaseOptions for functional form
    const EnumOptionsBase &formOptions() const { return formOptions_; }

    /*
     * Set / Get
     */
    public:
    // Return form as integer index
    virtual int formByIndex() const = 0;
    // Set new form index
    virtual void setFormByIndex(int formIndex) = 0;
    // Return parameters as string
    virtual std::string parametersAsString() const = 0;
    // Set parameters from supplied string
    virtual bool setParameters(std::string parameters) = 0;
    // Express as a serialisable value
    SerialisedValue serialise() const override { throw std::runtime_error("Cannot serialise InteractionPotentialBaseKeyword"); }
};

// Keyword based on InteractionPotential
template <class Functions> class InteractionPotentialKeyword : public InteractionPotentialBaseKeyword
{
    public:
    explicit InteractionPotentialKeyword(InteractionPotential<Functions> &data)
        : InteractionPotentialBaseKeyword(optionData_), data_(data), optionData_(Functions::forms())
    {
    }
    ~InteractionPotentialKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    InteractionPotential<Functions> &data_;
    // Related form enum data
    EnumOptions<typename Functions::Form> optionData_;

    public:
    // Return reference to data
    InteractionPotential<Functions> &data() { return data_; }
    const InteractionPotential<Functions> &data() const { return data_; }

    /*
     * Arguments
     */
    public:
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override { return {}; }
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        if (!parser.hasArg(startArg))
            return Messenger::error("Functional form of interaction must be provided.\n");

        // Check the functional form specified
        if (!Functions::forms().isValid(parser.argsv(startArg)))
            return Messenger::error("Functional form of interaction ({}) not recognised.\n", parser.argsv(startArg));
        data_.setForm(Functions::forms().enumeration(parser.argsv(startArg)));

        // Check number of args provided
        if (!Functions::forms().validNArgs(data_.form(), parser.nArgs() - startArg - 1))
            return Messenger::error("Invalid number of parameters given for functional form '{}'.\n",
                                    Functions::forms().keyword(data_.form()));

        // Set parameters
        if (!data_.parseParameters(parser, startArg + 1))
            return Messenger::error("Failed to set parameters for functional form '{}'.\n",
                                    Functions::forms().keyword(data_.form()));

        return true;
    }
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        return parser.writeLineF("{}{}  {}  {}\n", prefix, keywordName, Functions::forms().keyword(data_.form()),
                                 data_.parametersAsString());
    }

    /*
     * Set (implementing pure virtual from InteractionPotentialBaseKeyword)
     */
    public:
    // Return form as integer index
    int formByIndex() const override
    {
        for (auto n = 0; n < Functions::forms().nOptions(); ++n)
            if (Functions::forms().enumerationByIndex(n) == data_.form())
                return n;
        throw(std::runtime_error("Couldn't retrieve index for enumeration as it doesn't exist.\n"));
    };
    // Set new form index
    void setFormByIndex(int formIndex) override { data_.setForm(Functions::forms().enumerationByIndex(formIndex)); }
    // Return parameters as string
    std::string parametersAsString() const override { return data_.parametersAsString(); };
    // Set parameters from supplied string
    bool setParameters(std::string parameters) override
    {
        // Stash current parameters, just in case we fail to set from the new string
        auto currentParameters = data_.parametersAsString();

        if (data_.parseParameters(parameters))
            return true;

        data_.parseParameters(currentParameters);
        return false;
    }
};
