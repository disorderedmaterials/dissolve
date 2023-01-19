// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <optional>

// Keyword managing optional Double data
class OptionalDoubleKeyword : public KeywordBase
{
    public:
    explicit OptionalDoubleKeyword(std::optional<double> &data, double minValue, std::optional<double> maxValue,
                                   double valueDelta, std::string_view textWhenNull);
    ~OptionalDoubleKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    std::optional<double> &data_;
    // Minimum allowed value, below which equates to nullopt
    double minimumLimit_;
    // Maximum limit to apply (if any)
    std::optional<double> maximumLimit_;
    // Step size for widget
    double valueDelta_;
    // Text to display in widget when null
    std::string textWhenNull_;
    // Whether the data has been set
    bool set_{false};

    public:
    // Set data
    bool setData(std::optional<double> value);
    // Return data
    std::optional<double> data() const;
    // Return validation minimum limit
    double validationMin() const;
    // Return validation maximum limit
    std::optional<double> validationMax() const;
    // Return step size for widget
    double valueDelta() const;
    // Return text to display in widget  when value is null
    std::string textWhenNull() const;

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Has not changed from initial value
    bool isDefault() const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
