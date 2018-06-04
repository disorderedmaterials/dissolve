/*
	*** Generic Item
	*** src/base/genericitem.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_GENERICITEM_H
#define DISSOLVE_GENERICITEM_H

#include "base/processpool.h"
#include "base/sysfunc.h"

/*
 * GenericItem - Base class for all GenericItemContainers
 */
class GenericItem
{
	public:
	// Constructor
	GenericItem(const char* name, int flags);
	virtual ~GenericItem();
	// Item Flags
	enum Flags
	{
		NoFlag				= -1,	/* Null flag */
		InRestartFileFlag		= 1	/* The item should be written to the restart file */
	};
	// List Pointers
	GenericItem* prev, *next;


	/*
	 * Item Class
	 */
	protected:
	// List of all available class names (as GenericItems)
	static List<GenericItem> itemClasses_;

	protected:
	// Create a new GenericItem containing same class as current type
	virtual GenericItem* createItem(const char* className, const char* name, int flags = 0) = 0;

	public:
	// Return class name contained in item
	virtual const char* itemClassName() = 0;
	// Add class to list of representative itemClasses_
	static void addItemClass(GenericItem* item);
	// Return new, empty GenericItem containing the class specified
	static GenericItem* newItem(const char* className, const char* name, int flags = 0);


	/*
	 * Item Contents
	 */
	private:
	// Name associated to the item
	CharString name_;
	// Description, if any, for the item
	CharString description_;
	// Version of the item
	int version_;
	// Flags
	int flags_;

	public:
	// Set item name
	void setName(const char* name);
	// Return item name
	const char* name();
	// Set item description
	void setDescription(const char* description);
	// Return item description
	const char* description();
	// Return version of the item
	int version() const;
	// Set version of the item
	void setVersion(int version);
	// Bump the version of the item
	void bumpVersion();
	// Set flags for item
	void setFlags(int flags);
	// Return flags
	int flags();


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	virtual bool write(LineParser& parser) = 0;
	// Read data through specified parser
	virtual bool read(LineParser& parser) = 0;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	virtual bool broadcast(ProcessPool& procPool, int root = 0) = 0;
	// Return equality between items
	virtual bool equality(ProcessPool& procPool) = 0;
};

#endif
