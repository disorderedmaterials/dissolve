// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class ValueStore;

// Keyword managing a store of values
class ValueStoreKeyword : public KeywordBase
{
    public:
    explicit ValueStoreKeyword(ValueStore &data);
    ~ValueStoreKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    ValueStore &data_;

    public:
    // Return reference to data
    ValueStore &data();
    const ValueStore &data() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Has not changed from initial value
    bool isDefault() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
