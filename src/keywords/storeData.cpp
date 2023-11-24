// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/storeData.h"

KeywordStoreData::KeywordStoreData(KeywordBase *keyword, KeywordType type) : keyword_(keyword), type_(type) {}

// Return pointer to keyword base object
KeywordBase *KeywordStoreData::keyword() { return keyword_; }
const KeywordBase *KeywordStoreData::keyword() const { return keyword_; }

// Return basic type for keyword
KeywordStoreData::KeywordType KeywordStoreData::type() const { return type_; }
