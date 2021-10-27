// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/atomtype.h"
#include "keywords/base.h"

// Keyword with vector of AtomType pointers
class AtomTypeVectorKeyword : public KeywordBase
{
    public:
    explicit AtomTypeVectorKeyword(std::vector<std::shared_ptr<AtomType>> &data);
    ~AtomTypeVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data vector
    std::vector<std::shared_ptr<AtomType>> &data_;

    public:
    // Return reference to data vector
    std::vector<std::shared_ptr<AtomType>> &data();
    const std::vector<std::shared_ptr<AtomType>> &data() const;
    // Determine whether current data is 'empty', and should be considered as 'not set'
    bool isDataEmpty() const override;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied AtomType in the contained data
    void removeReferencesTo(std::shared_ptr<AtomType> at) override;
};
