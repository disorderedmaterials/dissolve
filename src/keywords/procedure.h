// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "procedure/procedure.h"

// Keyword managing Procedure data
class ProcedureKeyword : public KeywordBase
{
    public:
    explicit ProcedureKeyword(Procedure &data);
    ~ProcedureKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    Procedure &data_;

    public:
    // Return reference to data
    Procedure &data();
    std::any_ptr data() const override;

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
};
