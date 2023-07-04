// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <string>

// Forward Declarations
class KeywordBase;

// Keyword Store Data
class KeywordStoreData
{
    public:
    // Keyword basic type
    enum class KeywordType
    {
        Standard,    /* Standard keyword */
        Restartable, /* Standard keyword, captured in restart file */
        Target,      /* Target keyword */
        Deprecated   /* Deprecated keyword (not captured in restart file) */
    };
    KeywordStoreData(KeywordBase *keyword, KeywordType type, std::string_view group = "", std::string_view section = "");

    private:
    // Pointer to keyword base object
    KeywordBase *keyword_;
    // Basic type for keyword
    KeywordType type_;
    // Group name the keyword exists in (if any)
    std::string groupName_;
    // Section name the keyword exists in (if any)
    std::string sectionName_;

    public:
    // Return pointer to keyword base object
    KeywordBase *keyword();
    const KeywordBase *keyword() const;
    // Return basic type for keyword
    KeywordType type() const;
    // Group name the keyword exists in (if any)
    std::string_view groupName() const;
    // Section name the keyword exists in (if any)
    std::string_view sectionName() const;
};
