/*
	*** Index Data
	*** src/gui/uchroma/classes/indexdata.cpp
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/classes/indexdata.h"
#include <string.h>

// IndexType Keywords
const char* IndexTypeKeywords[] = { "Normal", "Fixed", "Relative" };

// Convert text string to IndexType
IndexData::IndexType IndexData::indexType(QString s)
{
	for (int n=0; n<IndexData::nIndexTypes; ++n) if (s == IndexTypeKeywords[n]) return (IndexData::IndexType) n;
	return IndexData::nIndexTypes;
}

// Convert IndexType to text string
const char* IndexData::indexType(IndexData::IndexType rt)
{
	return IndexTypeKeywords[rt];
}

// Constructor
IndexData::IndexData()
{
	type_ = IndexData::NormalIndex;
	index_ = 0;
	offset_ = 0;
}

// Destructor
IndexData::~IndexData()
{
}

// Set index type
void IndexData::setType(IndexData::IndexType type)
{
	type_ = type;
}

// Return index type
IndexData::IndexType IndexData::type()
{
	return type_;
}

// Set index
void IndexData::setIndex(int index)
{
	index_ = index;
}

// Return index
int IndexData::index()
{
	return index_;
}

// Set offset
void IndexData::setOffset(int offset)
{
	offset_ = offset;
}

// Return offset
int IndexData::offset()
{
	return offset_;
}
