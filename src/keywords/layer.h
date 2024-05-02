// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class ModuleLayer;

// Keyword managing Layer data
class LayerKeyword : public KeywordBase
{
    public:
    explicit LayerKeyword(const ModuleLayer *&data);
    ~LayerKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    const ModuleLayer *&data_;

    public:
    // Return reference to data
    const ModuleLayer *&data();
    const ModuleLayer *&data() const;
    void setData(ModuleLayer *data);

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
};
