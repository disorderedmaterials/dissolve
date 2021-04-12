// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/linkto.h"

LinkToKeyword::LinkToKeyword(KeywordBase *keywordData) : KeywordData<KeywordBase *>(KeywordBase::LinkToKeywordData, keywordData)
{
}

LinkToKeyword::~LinkToKeyword() {}

/*
 * Base Pointer Return (Overloading KeywordBase virtual)
 */

// Return base pointer for this (may be overloaded to provide access to other KeywordBase instance)
KeywordBase *LinkToKeyword::base() { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int LinkToKeyword::minArguments() const
{
    Messenger::warn("Don't call LinkToKeyword::minArguments() - go through base().\n");
    return data_->minArguments();
}

// Return maximum number of arguments accepted
int LinkToKeyword::maxArguments() const
{
    Messenger::warn("Don't call LinkToKeyword::maxArguments() - go through base().\n");
    return data_->maxArguments();
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool LinkToKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    Messenger::warn("Don't call LinkToKeyword::read() - go through base().\n");
    return data_->read(parser, startArg, coreData);
}

// Write keyword data to specified LineParser
bool LinkToKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    Messenger::warn("Don't call LinkToKeyword::write() - go through base().\n");
    return data_->write(parser, data_->name(), prefix);
}
