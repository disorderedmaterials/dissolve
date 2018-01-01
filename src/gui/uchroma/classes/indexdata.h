/*
	*** Index Data
	*** src/gui/uchroma/classes/indexdata.h
	Copyright T. Youngs 2013-2018

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

#ifndef DUQ_UCHROMAINDEXDATA_H
#define DUQ_UCHROMAINDEXDATA_H

#include <QString>

// Forward Declarations
/* none */

// Index Data
class IndexData
{
	public:
	// Constructor / Destructor
	IndexData();
	~IndexData();
	// Reference Type
	enum IndexType { NormalIndex, FixedIndex, RelativeIndex, nIndexTypes };
	// Convert text string to IndexType
	static IndexType indexType(QString s);
	// Convert IndexType to text string
	static const char* indexType(IndexType id);


	/*
	 * Definition
	 */
	private:
	// Index type
	IndexType type_;
	// Integer index (for FixedIndex)
	int index_;
	// Index offset (for RelativeIndex)
	int offset_;

	public:
	// Set index type
	void setType(IndexData::IndexType type);
	// Return index type
	IndexData::IndexType type();
	// Set index
	void setIndex(int index);
	// Return index
	int index();
	// Set offset
	void setOffset(int offset);
	// Return offset
	int offset();
};

#endif
