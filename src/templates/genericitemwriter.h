/*
	*** Generic Item Writer
	*** src/templates/genericitemwriter.h
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

#ifndef DUQ_GENERICITEMWRITER_H
#define DUQ_GENERICITEMWRITER_H

#include "base/messenger.h"
#include "base/lineparser.h"
#include "templates/genericitemcontainer.h"

/*
 * GenericItemDataWriters
 * Constructor-only template class to write contents of a GenericItemContainer with the specified LineParser.
 */
// For General Case
template <class T> class GenericItemDataWriter
{
	// Constructor
	GenericItemDataWriter<T>(LineParser& parser, GenericItemContainer<T>& container, bool& success)
	{
		Messenger::error("GenericItemDataWriter - No specialisation for this class type.\n");
		success = false;
	}
};

// For GenericItemContainer<int>
template <> class GenericItemDataWriter<int>
{
	public:
	// Constructor
	GenericItemDataWriter<int>(LineParser& parser, GenericItemContainer<int>& container, bool& success)
	{
		success = parser.writeLineF("%i\n", container.data);
	}
};

// For GenericItemContainer<bool>
template <> class GenericItemDataWriter<bool>
{
	public:
	// Constructor
	GenericItemDataWriter<bool>(LineParser& parser, GenericItemContainer<bool>& container, bool& success)
	{
		success = parser.writeLineF("%s\n", DUQSys::btoa(container.data));
	}
};

// For GenericItemContainer<double>
template <> class GenericItemDataWriter<double>
{
	public:
	// Constructor
	GenericItemDataWriter<double>(LineParser& parser, GenericItemContainer<double>& container, bool& success)
	{
		success = parser.writeLineF("%16.9e\n", container.data);
	}
};

// For GenericItemContainer<CharString>
template <> class GenericItemDataWriter<CharString>
{
	public:
	// Constructor
	GenericItemDataWriter<CharString>(LineParser& parser, GenericItemContainer<CharString>& container, bool& success)
	{
		success = parser.writeLineF("%s\n", container.data.get());
	}
};

// For GenericItemContainer< Vec3<int> >
template <> class GenericItemDataWriter< Vec3<int> >
{
	public:
	// Constructor
	GenericItemDataWriter< Vec3<int> >(LineParser& parser, GenericItemContainer< Vec3<int> >& container, bool& success)
	{
		success = parser.writeLineF("%i  %i  %i\n", container.data.x, container.data.y, container.data.z);
	}
};

// For GenericItemContainer< Vec3<double> >
template <> class GenericItemDataWriter< Vec3<double> >
{
	public:
	// Constructor
	GenericItemDataWriter< Vec3<double> >(LineParser& parser, GenericItemContainer< Vec3<double> >& container, bool& success)
	{
		success = parser.writeLineF("%16.9e  %16.9e  %16.9e\n", container.data.x, container.data.y, container.data.z);
	}
};

// For GenericItemContainer< Array<int> >
template <> class GenericItemDataWriter< Array<int> >
{
	public:
	// Constructor
	GenericItemDataWriter< Array<int> >(LineParser& parser, GenericItemContainer< Array<int> >& container, bool& success)
	{
		parser.writeLineF("%i\n", container.data.nItems());
		int* array = container.data.array();
		for (int n=0; n<container.data.nItems(); ++n)
		{
			success = parser.writeLineF("%i\n", array[n]);
			if (!success) return;
		}
	}
};

// For GenericItemContainer< Array<double> >
template <> class GenericItemDataWriter< Array<double> >
{
	public:
	// Constructor
	GenericItemDataWriter< Array<double> >(LineParser& parser, GenericItemContainer< Array<double> >& container, bool& success)
	{
		parser.writeLineF("%16.9e\n", container.data.nItems());
		double* array = container.data.array();
		for (int n=0; n<container.data.nItems(); ++n)
		{
			success = parser.writeLineF("%16.9e\n", array[n]);
			if (!success) return;
		}
	}
};

// For GenericItemContainer< Array2D<int> >
template <> class GenericItemDataWriter< Array2D<int> >
{
	public:
	// Constructor
	GenericItemDataWriter< Array2D<int> >(LineParser& parser, GenericItemContainer< Array2D<int> >& container, bool& success)
	{
		parser.writeLineF("%i  %i\n", container.data.nRows(), container.data.nColumns());
		for (int row=0; row<container.data.nRows(); ++row)
		{
			for (int column=0; column<container.data.nColumns(); ++column)
			{
				success = parser.writeLineF("%i\n", container.data.value(row, column));
				if (!success) return;
			}
		}
	}
};

// For GenericItemContainer< Array2D<double> >
template <> class GenericItemDataWriter< Array2D<double> >
{
	public:
	// Constructor
	GenericItemDataWriter< Array2D<double> >(LineParser& parser, GenericItemContainer< Array2D<double> >& container, bool& success)
	{
		parser.writeLineF("%i  %i\n", container.data.nRows(), container.data.nColumns());
		for (int row=0; row<container.data.nRows(); ++row)
		{
			for (int column=0; column<container.data.nColumns(); ++column)
			{
				success = parser.writeLineF("%16.9e\n", container.data.value(row, column));
				if (!success) return;
			}
		}
	}
};


/*
 * GenericItemWriter
 * Constructor-only class used to instantiate the correct templated GenericItemDataWriter<> for the supplied GenericItem
 */
class GenericItemWriter
{
	public:
	// Constructor
	GenericItemWriter(LineParser& parser, GenericItem* item, bool& success)
	{
		// Need to check classType of item, and then instantiate the relevant GenericItemDataReader<>
		switch (item->itemClass())
		{
			case (GenericItem::BoolClass):
				GenericItemDataReader<bool> reader();
				break;
			case (GenericItem::IntClass):
				break;
			case (GenericItem::DoubleClass):
				break;
			case (GenericItem::CharStringClass):
				break;
			case (GenericItem::Vec3IntClass):
				break;
			case (GenericItem::Vec3DoubleClass):
				break;
			case (GenericItem::ArrayIntClass):
				break;
			case (GenericItem::ArrayDoubleClass):
				break;
			case (GenericItem::ArrayVec3IntClass):
				break;
			case (GenericItem::ArrayVec3DoubleClass):
				break;
			case (GenericItem::Array2DDoubleClass):
				break;
			case (GenericItem::ListClass):
				break;
			case (GenericItem::OrderedListClass):
				break;
			default:
				Messenger::error("GenericItemReader doesn't know how to handle class type %i (item '%s').\n", item->itemClass(), item->name());
				success = false;
		}
	}
};

#endif
