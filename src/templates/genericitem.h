/*
	*** Generic Item
	*** src/templates/genericitem.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_GENERICITEM_H
#define DUQ_GENERICITEM_H

#include "base/processpool.h"
#include "base/sysfunc.h"

/*
 * GenericItem - Base class for all GenericItemContainers
 */
class GenericItem
{
	public:
	// Constructor
	GenericItem(const char* name, int flags)
	{
		name_ = name;
		flags_ = flags;
		itemClass_ = nItemClasses;
		prev = NULL;
		next = NULL;
	}
	virtual ~GenericItem()
	{
	};
	// Item Flags
	enum Flags
	{
		ModuleOptionFlag		= 1,	/* Specifies that the item represents a Module option (i.e. a PlainValue in Module::options_) */
		NoOutputFlag			= 2	/* The item should not be read or written by input / output routines */
	};
	// Item Classes
	enum ItemClass
	{
		BoolClass,				/* Item is of 'bool' type */
		IntClass,				/* Item is of 'int' type */
		DoubleClass,				/* Item is of 'double' type */
		CharStringClass,			/* Item is a 'CharString' */
		ArrayIntClass,				/* Item is a 'Array<int>' */
		ArrayDoubleClass,			/* Item is a 'Array<double>' */
		ArrayVec3IntClass,			/* Item is a 'Array< Vec3<int> >' */
		ArrayVec3DoubleClass,			/* Item is a 'Array< Vec3<double> >' */
		Array2DDoubleClass,			/* Item is a 'Array2D<double>' */
		nItemClasses
	};
	// Convert item class to char string
	static const char* itemClass(ItemClass c)
	{
		static const char* classKeywords[] = { "Bool", "Int", "Double", "CharString", "Array<int>", "Array<double>", "Array<Vec3<int>>", "Array<Vec3<double>>", "Array2D<double>" };
		return classKeywords[c];
	}
	// Convert char string to item class
	static ItemClass itemClass(const char* s)
	{
		for (int n=0; n<nItemClasses; ++n) if (DUQSys::sameString(itemClass( (ItemClass) n), s)) return (ItemClass) n;
		return nItemClasses;
	}
	// List Pointers
	GenericItem* prev, *next;

	private:
	// Name associated to the item
	CharString name_;
	// Description, if any, for the item
	CharString description_;
	// Flags
	int flags_;

	protected:
	// Item class
	ItemClass itemClass_;

	public:
	// Return item name
	const char* name()
	{
		return name_.get();
	}
	// Set item description
	void setDescription(const char* description)
	{
		description_ = description_;
	}
	// Return item description
	const char* description()
	{
		return description_.get();
	}
	// Set flags for item
	void setFlags(int flags)
	{
		flags_ = flags;
	}
	// Return flags
	int flags()
	{
		return flags_;
	}
	// Return item class
	ItemClass itemClass()
	{
		return itemClass_;
	}


	/*
	 * Read / Write
	 */
	public:
	// Read item contents from specified LineParser
	virtual bool read(LineParser& parser) = 0;
	// Write item contents to specified LineParser
	virtual bool write(LineParser& parser) = 0;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contsnts
	virtual bool broadcast(ProcessPool& procPool, int root) = 0;
};

#endif
