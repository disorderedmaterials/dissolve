/*
    *** Keyword - Link To Other Keyword
    *** src/keywords/linkto.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
bool LinkToKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    Messenger::warn("Don't call LinkToKeyword::read() - go through base().\n");
    return data_->read(parser, startArg, coreData);
}

// Write keyword data to specified LineParser
bool LinkToKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    Messenger::warn("Don't call LinkToKeyword::write() - go through base().\n");
    return data_->write(parser, data_->name(), prefix);
}
