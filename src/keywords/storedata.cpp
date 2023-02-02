// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/storedata.h"

KeywordStoreData::KeywordStoreData(KeywordBase *keyword, KeywordType type, std::string_view group, std::string_view section)
    : keyword_(keyword), type_(type), groupName_(group), sectionName_(section)
{
}

// Return pointer to keyword base object
KeywordBase *KeywordStoreData::keyword() { return keyword_; }
const KeywordBase *KeywordStoreData::keyword() const { return keyword_; }

// Return basic type for keyword
KeywordStoreData::KeywordType KeywordStoreData::type() const { return type_; }

// Group name the keyword exists in (if any)
std::string_view KeywordStoreData::groupName() const { return groupName_; }

// Section name the keyword exists in (if any)
std::string_view KeywordStoreData::sectionName() const { return sectionName_; }
