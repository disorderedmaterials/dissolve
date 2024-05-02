// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "templates/algorithms.h"
#include <string>
#include <vector>

// Interaction Potential Storage
template <class Functions> class InteractionPotential
{
    public:
    explicit InteractionPotential(typename Functions::Form form) { setForm(form); };
    InteractionPotential(typename Functions::Form form, std::string_view params)
    {
        setForm(form);
        parseParameters(params);
    };
    virtual ~InteractionPotential() = default;
    InteractionPotential(const InteractionPotential &source) = default;
    InteractionPotential(InteractionPotential &&source) = delete;
    InteractionPotential &operator=(const InteractionPotential &source) = default;
    InteractionPotential &operator=(InteractionPotential &&source) = delete;

    /*
     * Parameter Data
     */
    protected:
    // Functional form of interaction
    typename Functions::Form form_;
    // InteractionPotential for interaction
    std::vector<double> parameters_;

    public:
    // Set functional form of interaction
    void setForm(typename Functions::Form form)
    {
        form_ = form;

        // Resize parameter vector if necessary
        if (!Functions::forms().validNArgs(form_, parameters_.size(), true))
            parameters_.resize(Functions::forms().minArgs(form_).value_or(0), 0.0);
    }
    // Return functional form of interaction
    typename Functions::Form form() const { return form_; }
    // Parse supplied vector of terms
    bool parseParameters(std::vector<std::string> terms)
    {
        // Do we have a suitable number of parameters
        if (!Functions::forms().validNArgs(form(), terms.size()))
            return false;

        // We allow either a set of plain values or a set of name=value assignments - we don't allow mixing of the two
        auto nAssigned =
            std::count_if(terms.begin(), terms.end(), [](const auto &s) { return s.find('=') != std::string::npos; });
        if (nAssigned == 0)
        {
            // Plain values
            parameters_.resize(terms.size(), 0.0);
            std::transform(terms.begin(), terms.end(), parameters_.begin(), [](const auto &term) { return std::stod(term); });
        }
        else if (nAssigned == terms.size())
        {
            // Name = value assignments
            // The parameters may not have been given in the expected order, so maintain/resize a value vector
            parameters_.clear();
            for (const auto &term : terms)
            {
                // Split the string into name and value parts
                auto name = DissolveSys::beforeChar(term, '=');
                if (name.empty())
                    return Messenger::error("Bad assignment found in parameters - no name present in '{}'.\n", term);
                auto value = DissolveSys::afterChar(term, '=');
                if (value.empty())
                    return Messenger::error("Bad assignment found in parameters - no value present in '{}'.\n", term);

                // Get the index of the named parameter
                auto index = Functions::parameterIndex(form(), name);
                if (!index)
                    return Messenger::error(
                        "Bad assignment found in parameters - '{}' is not a valid parameter for this interaction.\n", name);

                // Resize vector if necessary
                if (index.value() >= parameters_.size())
                    parameters_.resize(index.value() + 1, 0.0);
                parameters_[index.value()] = std::stod(std::string(value));
            }
        }
        else
            return Messenger::error(
                "Failed to parse parameters string - provide either plain values or name=value, but don't mix both.\n");

        return true;
    }
    // Parse parameters from specified string
    bool parseParameters(std::string_view paramString) { return parseParameters(DissolveSys::splitString(paramString)); }
    // Parse parameters from current line
    bool parseParameters(const LineParser &parser, int startArg)
    {
        // Construct a vector of all remaining arguments on the line, starting from the argument offset
        std::vector<std::string> terms;
        for (auto n = startArg; n < parser.nArgs(); ++n)
            terms.emplace_back(parser.args(n));
        return parseParameters(terms);
    }
    // Set form and parameters
    void setFormAndParameters(typename Functions::Form form, const std::vector<double> &params)
    {
        form_ = form;
        parameters_ = params;
    }
    bool setFormAndParameters(typename Functions::Form form, std::string_view paramString)
    {
        form_ = form;
        return parseParameters(paramString);
    }
    // Return number of parameters defined
    int nParameters() const { return parameters_.size(); }
    // Return array of parameters
    const std::vector<double> &parameters() const { return parameters_; }
    // Return parameters as name=value string
    std::string parametersAsString() const
    {
        auto id = 0;
        return joinStrings(parameters(), " ",
                           [&](const auto &value) { return fmt::format("{}={}", Functions::parameter(form(), id++), value); });
    }
};
