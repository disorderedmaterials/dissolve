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
	public:
	// Constructor
	GenericItemDataWriter<T>(LineParser& parser, GenericItemContainer<T>* container, bool& success)
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
	GenericItemDataWriter<int>(LineParser& parser, GenericItemContainer<int>* container, bool& success)
	{
		success = parser.writeLineF("%i\n", container->data);
	}
};

// For GenericItemContainer<bool>
template <> class GenericItemDataWriter<bool>
{
	public:
	// Constructor
	GenericItemDataWriter<bool>(LineParser& parser, GenericItemContainer<bool>* container, bool& success)
	{
		success = parser.writeLineF("%s\n", DUQSys::btoa(container->data));
	}
};

// For GenericItemContainer<double>
template <> class GenericItemDataWriter<double>
{
	public:
	// Constructor
	GenericItemDataWriter<double>(LineParser& parser, GenericItemContainer<double>* container, bool& success)
	{
		success = parser.writeLineF("%16.9e\n", container->data);
	}
};

// For GenericItemContainer<CharString>
template <> class GenericItemDataWriter<CharString>
{
	public:
	// Constructor
	GenericItemDataWriter<CharString>(LineParser& parser, GenericItemContainer<CharString>* container, bool& success)
	{
		success = parser.writeLineF("%s\n", container->data.get());
	}
};

// For GenericItemContainer< Vec3<int> >
template <> class GenericItemDataWriter< Vec3<int> >
{
	public:
	// Constructor
	GenericItemDataWriter< Vec3<int> >(LineParser& parser, GenericItemContainer< Vec3<int> >* container, bool& success)
	{
		success = parser.writeLineF("%i  %i  %i\n", container->data.x, container->data.y, container->data.z);
	}
};

// For GenericItemContainer< Vec3<double> >
template <> class GenericItemDataWriter< Vec3<double> >
{
	public:
	// Constructor
	GenericItemDataWriter< Vec3<double> >(LineParser& parser, GenericItemContainer< Vec3<double> >* container, bool& success)
	{
		success = parser.writeLineF("%16.9e  %16.9e  %16.9e\n", container->data.x, container->data.y, container->data.z);
	}
};

// For GenericItemContainer< Array<int> >
template <> class GenericItemDataWriter< Array<int> >
{
	public:
	// Constructor
	GenericItemDataWriter< Array<int> >(LineParser& parser, GenericItemContainer< Array<int> >* container, bool& success)
	{
		parser.writeLineF("%i\n", container->data.nItems());
		int* array = container->data.array();
		for (int n=0; n<container->data.nItems(); ++n)
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
	GenericItemDataWriter< Array<double> >(LineParser& parser, GenericItemContainer< Array<double> >* container, bool& success)
	{
		parser.writeLineF("%16.9e\n", container->data.nItems());
		double* array = container->data.array();
		for (int n=0; n<container->data.nItems(); ++n)
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
	GenericItemDataWriter< Array2D<int> >(LineParser& parser, GenericItemContainer< Array2D<int> >* container, bool& success)
	{
		parser.writeLineF("%i  %i\n", container->data.nRows(), container->data.nColumns());
		for (int row=0; row<container->data.nRows(); ++row)
		{
			for (int column=0; column<container->data.nColumns(); ++column)
			{
				success = parser.writeLineF("%i\n", container->data.value(row, column));
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
	GenericItemDataWriter< Array2D<double> >(LineParser& parser, GenericItemContainer< Array2D<double> >* container, bool& success)
	{
		parser.writeLineF("%i  %i\n", container->data.nRows(), container->data.nColumns());
		for (int row=0; row<container->data.nRows(); ++row)
		{
			for (int column=0; column<container->data.nColumns(); ++column)
			{
				success = parser.writeLineF("%16.9e\n", container->data.value(row, column));
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
	GenericItemWriter(LineParser& parser, GenericItem* item)
	{
		result_ = false;

		// Need to check classType of item, and then instantiate the relevant GenericItemDataWriter<>
		if (item->itemClass() == GenericItem::BoolClass) GenericItemDataWriter<bool> writer(parser, (GenericItemContainer<bool>*)item, result_);
		else if (item->itemClass() == GenericItem::IntClass) GenericItemDataWriter<int> writer(parser, (GenericItemContainer<int>*)item, result_);
		else if (item->itemClass() == GenericItem::DoubleClass) GenericItemDataWriter<double> writer(parser, (GenericItemContainer<double>*)item, result_);
		else if (item->itemClass() == GenericItem::CharStringClass) GenericItemDataWriter<CharString> writer(parser, (GenericItemContainer<CharString>*)item, result_);
		else if (item->itemClass() == GenericItem::Vec3IntClass) GenericItemDataWriter< Vec3<int> > writer(parser, (GenericItemContainer< Vec3<int> >*)item, result_);
		else if (item->itemClass() == GenericItem::Vec3DoubleClass) GenericItemDataWriter< Vec3<double> > writer(parser, (GenericItemContainer< Vec3<double> >*)item, result_);
		else if (item->itemClass() == GenericItem::ArrayIntClass) GenericItemDataWriter< Array<int> > writer(parser, (GenericItemContainer< Array<int> >*)item, result_);
		else if (item->itemClass() == GenericItem::ArrayDoubleClass) GenericItemDataWriter< Array<double> > writer(parser, (GenericItemContainer< Array<double> >*)item, result_);
		else if (item->itemClass() == GenericItem::ArrayVec3IntClass) GenericItemDataWriter< Array< Vec3<int> > > writer(parser, (GenericItemContainer< Array< Vec3<int> > >*)item, result_);
		else if (item->itemClass() == GenericItem::ArrayVec3DoubleClass) GenericItemDataWriter< Array< Vec3<double> > > writer(parser, (GenericItemContainer< Array< Vec3<double> > >*)item, result_);
		else if (item->itemClass() == GenericItem::Array2DDoubleClass) GenericItemDataWriter< Array2D<double> > writer(parser, (GenericItemContainer< Array2D<double> >*)item, result_);
// 		else if (item->itemClass() == GenericItem::ListClass) GenericItemDataWriter< List > writer(parser, (GenericItemContainer<bool>*)item, result_);
// 		else if (item->itemClass() == GenericItem::OrderedListClass) GenericItemDataWriter<bool> writer(parser, (GenericItemContainer<bool>*)item, result_);
		else
		{
			Messenger::error("GenericItemWriter doesn't know how to handle class type %i (item '%s').\n", item->itemClass(), item->name());
			result_ = false;
		}
	}

	private:
	// Result of the write
	bool result_;

	public:
	// Return if the write succeeded
	bool success()
	{
		return result_;
	}
	// Return if the write failed
	bool failed()
	{
		return (!result_);
	}
};

#endif
