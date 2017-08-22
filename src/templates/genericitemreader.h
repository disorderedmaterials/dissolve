/*
	*** Generic Item Reader
	*** src/templates/genericitemreader.h
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

#ifndef DUQ_GENERICITEMREADER_H
#define DUQ_GENERICITEMREADER_H

#include "base/lineparser.h"
#include "templates/genericitemcontainer.h"

/*
 * GenericItemDataReaders
 * Constructor-only template classes to read contents of a GenericItemContainer with the specified LineParser.
 */

// For General Case
template <class T> class GenericItemDataReader
{
	public:
	GenericItemDataReader<T>(LineParser& parser, GenericItemContainer<int>* container, bool& success)
	{
		Messenger::error("GenericItemDataReader - No specialisation for this class type.\n");
		success = false;
	}
};

// For GenericItemContainer<int>
template <> class GenericItemDataReader<int>
{
	public:
	// Constructor
	GenericItemDataReader<int>(LineParser& parser, GenericItemContainer<int>* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		container->data = parser.argi(0);
	}
};

// For GenericItemContainer<bool>
template <> class GenericItemDataReader<bool>
{
	public:
	// Constructor
	GenericItemDataReader<bool>(LineParser& parser, GenericItemContainer<bool>* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		container->data = parser.argb(0);
	}
};

// For GenericItemContainer<double>
template <> class GenericItemDataReader<double>
{
	public:
	// Constructor
	GenericItemDataReader<double>(LineParser& parser, GenericItemContainer<double>* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		container->data = parser.argd(0);
	}
};

// For GenericItemContainer<CharString>
template <> class GenericItemDataReader<CharString>
{
	public:
	// Constructor
	GenericItemDataReader<CharString>(LineParser& parser, GenericItemContainer<CharString>* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		container->data = parser.line();
	}
};

// For GenericItemContainer< Vec3<int> >
template <> class GenericItemDataReader< Vec3<int> >
{
	public:
	// Constructor
	GenericItemDataReader< Vec3<int> >(LineParser& parser, GenericItemContainer< Vec3<int> >* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		container->data = parser.arg3i(0);
	}
};

// For GenericItemContainer< Vec3<double> >
template <> class GenericItemDataReader< Vec3<double> >
{
	public:
	// Constructor
	GenericItemDataReader< Vec3<double> >(LineParser& parser, GenericItemContainer< Vec3<double> >* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		container->data = parser.argd(0);
	}
};

// For GenericItemContainer< Array<int> >
template <> class GenericItemDataReader< Array<int> >
{
	public:
	// Constructor
	GenericItemDataReader< Array<int> >(LineParser& parser, GenericItemContainer< Array<int> >* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		if (!success) return;
		int nItems = parser.argi(0);
		container->data.initialise(nItems);
		for (int n=0; n<nItems; ++n)
		{
			success = (parser.getArgsDelim(LineParser::Defaults) == 0);
			if (!success) return;
			container->data.add(parser.argi(0));
		}
	}
};

// For GenericItemContainer< Array<double> >
template <> class GenericItemDataReader< Array<double> >
{
	public:
	// Constructor
	GenericItemDataReader< Array<double> >(LineParser& parser, GenericItemContainer< Array<double> >* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		if (!success) return;
		int nItems = parser.argi(0);
		container->data.initialise(nItems);
		for (int n=0; n<nItems; ++n)
		{
			success = (parser.getArgsDelim(LineParser::Defaults) == 0);
			if (!success) return;
			container->data.add(parser.argd(0));
		}
	}
};

// For GenericItemContainer< Array<int> >
template <> class GenericItemDataReader< Array< Vec3<int> > >
{
	public:
	// Constructor
	GenericItemDataReader< Array< Vec3<int> > >(LineParser& parser, GenericItemContainer< Array< Vec3<int> > >* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		if (!success) return;
		int nItems = parser.argi(0);
		container->data.initialise(nItems);
		for (int n=0; n<nItems; ++n)
		{
			success = (parser.getArgsDelim(LineParser::Defaults) == 0);
			if (!success) return;
			container->data.add(parser.arg3i(0));
		}
	}
};

// For GenericItemContainer< Array<double> >
template <> class GenericItemDataReader< Array< Vec3<double> > >
{
	public:
	// Constructor
	GenericItemDataReader< Array< Vec3<double> > >(LineParser& parser, GenericItemContainer< Array< Vec3<double> > >* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		if (!success) return;
		int nItems = parser.argi(0);
		container->data.initialise(nItems);
		for (int n=0; n<nItems; ++n)
		{
			success = (parser.getArgsDelim(LineParser::Defaults) == 0);
			if (!success) return;
			container->data.add(parser.arg3d(0));
		}
	}
};

// For GenericItemContainer< Array2D<int> >
template <> class GenericItemDataReader< Array2D<int> >
{
	public:
	// Constructor
	GenericItemDataReader< Array2D<int> >(LineParser& parser, GenericItemContainer< Array2D<int> >* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		if (!success) return;
		int nRows = parser.argi(0), nColumns = parser.argi(1);
		container->data.initialise(nRows, nColumns);
		// Read in data
		for (int row=0; row<nRows; ++row)
		{
			for (int column=0; column<nColumns; ++column)
			{
				success = (parser.getArgsDelim(LineParser::Defaults) == 0);
				if (!success) return;
				container->data.ref(row, column) = parser.argi(0);
			}
		}
	}
};

// For GenericItemContainer< Array2D<double> >
template <> class GenericItemDataReader< Array2D<double> >
{
	public:
	// Constructor
	GenericItemDataReader< Array2D<double> >(LineParser& parser, GenericItemContainer< Array2D<double> >* container, bool& success)
	{
		success = (parser.getArgsDelim(LineParser::Defaults) == 0);
		if (!success) return;
		int nRows = parser.argi(0), nColumns = parser.argi(1);
		container->data.initialise(nRows, nColumns);

		// Read in data
		for (int row=0; row<nRows; ++row)
		{
			for (int column=0; column<nColumns; ++column)
			{
				success = (parser.getArgsDelim(LineParser::Defaults) == 0);
				if (!success) return;
				container->data.ref(row, column) = parser.argd(0);
			}
		}
	}
};

/*
 * GenericItemReader
 * Constructor-only class used to instantiate the correct templated GenericItemDataReader<> for the supplied GenericItem
 */
class GenericItemReader
{
	public:
	// Constructor
	GenericItemReader(LineParser& parser, GenericItem* item)
	{
		result_ = false;

		// Need to check classType of item, and then instantiate the relevant GenericItemDataReader<>
		if (item->itemClass() == GenericItem::BoolClass) GenericItemDataReader<bool> writer(parser, (GenericItemContainer<bool>*)item, result_);
		else if (item->itemClass() == GenericItem::IntClass) GenericItemDataReader<int> writer(parser, (GenericItemContainer<int>*)item, result_);
		else if (item->itemClass() == GenericItem::DoubleClass) GenericItemDataReader<double> writer(parser, (GenericItemContainer<double>*)item, result_);
		else if (item->itemClass() == GenericItem::CharStringClass) GenericItemDataReader<CharString> writer(parser, (GenericItemContainer<CharString>*)item, result_);
		else if (item->itemClass() == GenericItem::Vec3IntClass) GenericItemDataReader< Vec3<int> > writer(parser, (GenericItemContainer< Vec3<int> >*)item, result_);
		else if (item->itemClass() == GenericItem::Vec3DoubleClass) GenericItemDataReader< Vec3<double> > writer(parser, (GenericItemContainer< Vec3<double> >*)item, result_);
		else if (item->itemClass() == GenericItem::ArrayIntClass) GenericItemDataReader< Array<int> > writer(parser, (GenericItemContainer< Array<int> >*)item, result_);
		else if (item->itemClass() == GenericItem::ArrayDoubleClass) GenericItemDataReader< Array<double> > writer(parser, (GenericItemContainer< Array<double> >*)item, result_);
		else if (item->itemClass() == GenericItem::ArrayVec3IntClass) GenericItemDataReader< Array< Vec3<int> > > writer(parser, (GenericItemContainer< Array< Vec3<int> > >*)item, result_);
		else if (item->itemClass() == GenericItem::ArrayVec3DoubleClass) GenericItemDataReader< Array< Vec3<double> > > writer(parser, (GenericItemContainer< Array< Vec3<double> > >*)item, result_);
		else if (item->itemClass() == GenericItem::Array2DDoubleClass) GenericItemDataReader< Array2D<double> > writer(parser, (GenericItemContainer< Array2D<double> >*)item, result_);
// 		else if (item->itemClass() == GenericItem::ListClass) GenericItemDataReader< List > writer(parser, (GenericItemContainer<bool>*)item, result_);
// 		else if (item->itemClass() == GenericItem::OrderedListClass) GenericItemDataReader<bool> writer(parser, (GenericItemContainer<bool>*)item, result_);
		else
		{
			Messenger::error("GenericItemWriter doesn't know how to handle class type %i (item '%s').\n", item->itemClass(), item->name());
			result_ = false;
		}
	}

	private:
	// Result of the read
	bool result_;

	public:
	// Return if the read succeeded
	bool success()
	{
		return result_;
	}
	// Return if the read failed
	bool failed()
	{
		return (!result_);
	}
};

#endif
