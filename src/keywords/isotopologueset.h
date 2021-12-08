// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueset.h"
#include "keywords/base.h"

// Keyword managing IsotopologueSet data
class IsotopologueSetKeyword : public KeywordBase
{
    public:
    explicit IsotopologueSetKeyword(IsotopologueSet &data);
    ~IsotopologueSetKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    IsotopologueSet &data_;

    public:
    // Return reference to data
    dissolve::any_ptr data() const override;

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
