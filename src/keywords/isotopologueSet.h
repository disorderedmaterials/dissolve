// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "keywords/base.h"

// Keyword managing IsotopologueSet data
class IsotopologueSetKeyword : public KeywordBase
{
    public:
    explicit IsotopologueSetKeyword(IsotopologueSet &data);
    ~IsotopologueSetKeyword() override = default;
    // Has not changed from initial value
    bool isDefault() const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;

    /*
     * Data
     */
    private:
    // Reference to data
    IsotopologueSet &data_;

    public:
    // Return reference to data
    IsotopologueSet &data();
    const IsotopologueSet &data() const;

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

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Species in the contained data
    void removeReferencesTo(Species *sp) override;
    // Prune any references to the supplied Isotopologue in the contained data
    void removeReferencesTo(Isotopologue *iso) override;
};
