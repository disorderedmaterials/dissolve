// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

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
    KeywordStoreData(KeywordBase *keyword, KeywordType type);

    private:
    // Pointer to keyword base object
    KeywordBase *keyword_;
    // Basic type for keyword
    KeywordType type_;

    public:
    // Return pointer to keyword base object
    KeywordBase *keyword();
    const KeywordBase *keyword() const;
    // Return basic type for keyword
    KeywordType type() const;
};
