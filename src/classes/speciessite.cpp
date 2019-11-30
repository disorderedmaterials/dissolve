/*
	*** SpeciesSite Definition
	*** src/classes/speciessite.cpp
	Copyright T. Youngs 2012-2019

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

#include "classes/speciessite.h"
#include "classes/species.h"
#include "base/lineparser.h"

// Constructor
SpeciesSite::SpeciesSite() : ListItem<SpeciesSite>()
{
	parent_ = NULL;
	originMassWeighted_ = false;
}

// Destructor
SpeciesSite::~SpeciesSite()
{
}

/*
 * Basic Information
 */

// Set name of site
void SpeciesSite::setName(const char* newName)
{
	name_ = newName;
}

// Return anme of site
const char* SpeciesSite::name() const
{
	return name_.get();
}

// Set Species parent
void SpeciesSite::setParent(Species* sp)
{
	parent_ = sp;
}

// Return species parent
Species* SpeciesSite::parent()
{
	return parent_;
}

// Return version
int SpeciesSite::version() const
{
	return version_;
}

/*
 * Definition
 */

// Add origin atom
bool SpeciesSite::addOriginAtom(SpeciesAtom* originAtom)
{
	if (!originAtom) return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addOriginAtom().\n");

	// If the SpeciesAtom already exists in the list, complain
	if (originAtoms_.contains(originAtom)) return Messenger::error("Origin atom index %i specified twice for site '%s'.\n", originAtom->index(), name_.get());

	originAtoms_.append(originAtom);

	++version_;

	return true;
}

// Add origin atom from index
bool SpeciesSite::addOriginAtom(int atomIndex)
{
#ifdef CHECKS
	if (!parent_) return Messenger::error("Tried to add an origin atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
	return addOriginAtom(parent_->atom(atomIndex));
}

// Return list of origin atoms
const RefList<SpeciesAtom>& SpeciesSite::originAtoms()
{
	return originAtoms_;
}

// Return integer array of indices from which the origin should be formed
Array<int> SpeciesSite::originAtomIndices() const
{
	Array<int> indices;
	RefListIterator<SpeciesAtom> atomIterator(originAtoms_);
	while (SpeciesAtom* atom = atomIterator.iterate()) indices.add(atom->index());

	return indices;
}

// Set whether the origin should be calculated with mass-weighted positions
void SpeciesSite::setOriginMassWeighted(bool b)
{
	originMassWeighted_ = b;

	++version_;
}

// Return whether the origin should be calculated with mass-weighted positions
bool SpeciesSite::originMassWeighted() const
{
	return originMassWeighted_;
}

// Add x-axis atom
bool SpeciesSite::addXAxisAtom(SpeciesAtom* xAxisAtom)
{
	if (!xAxisAtom) return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addXAxisAtom().\n");

	// If the SpeciesAtom already exists in the list, complain
	if (xAxisAtoms_.contains(xAxisAtom)) return Messenger::error("X-axis atom index %i specified twice for site '%s'.\n", xAxisAtom->index(), name_.get());

	xAxisAtoms_.append(xAxisAtom);

	++version_;

	return true;
}

// Add x-axis atom from index
bool SpeciesSite::addXAxisAtom(int atomIndex)
{
#ifdef CHECKS
	if (!parent_) return Messenger::error("Tried to add an x-axis atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
	return addXAxisAtom(parent_->atom(atomIndex));
}

// Return list of x-axis atoms
const RefList<SpeciesAtom>& SpeciesSite::xAxisAtoms()
{
	return xAxisAtoms_;
}

// Return integer array of indices from which x-axis should be formed
Array<int> SpeciesSite::xAxisAtomIndices() const
{
	Array<int> indices;
	RefListIterator<SpeciesAtom> atomIterator(xAxisAtoms_);
	while (SpeciesAtom* atom = atomIterator.iterate()) indices.add(atom->index());

	return indices;
}

// Add y-axis atom
bool SpeciesSite::addYAxisAtom(SpeciesAtom* yAxisAtom)
{
	if (!yAxisAtom) return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addYAxisAtom().\n");

	// If the SpeciesAtom already exists in the list, complain
	if (yAxisAtoms_.contains(yAxisAtom)) return Messenger::error("Y-axis atom index %i specified twice for site '%s'.\n", yAxisAtom->index(), name_.get());

	yAxisAtoms_.append(yAxisAtom);

	++version_;

	return true;
}

// Add y-axis atom from index
bool SpeciesSite::addYAxisAtom(int atomIndex)
{
#ifdef CHECKS
	if (!parent_) return Messenger::error("Tried to add a y-axis atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
	return addYAxisAtom(parent_->atom(atomIndex));
}

// Return list of y-axis atoms
const RefList<SpeciesAtom>& SpeciesSite::yAxisAtoms()
{
	return yAxisAtoms_;
}

// Return integer array of indices from which y-axis should be formed
Array<int> SpeciesSite::yAxisAtomIndices() const
{
	Array<int> indices;
	RefListIterator<SpeciesAtom> atomIterator(yAxisAtoms_);
	while (SpeciesAtom* atom = atomIterator.iterate()) indices.add(atom->index());

	return indices;
}

// Return whether the site has defined axes sites
bool SpeciesSite::hasAxes() const
{
	if ((xAxisAtoms_.nItems() == 0) || (yAxisAtoms_.nItems() == 0)) return false;
	return true;
}

/*
 * Read / Write
 */

// Return enum option info for SiteKeyword
EnumOptions<SpeciesSite::SiteKeyword> SpeciesSite::keywords()
{
	static EnumOptionsList SiteKeywords = EnumOptionsList() <<
		EnumOption(SpeciesSite::EndSiteKeyword, 			"EndSite") <<
		EnumOption(SpeciesSite::OriginKeyword, 			"Origin",		EnumOption::OneOrMoreArguments) <<
		EnumOption(SpeciesSite::OriginMassWeightedKeyword,	"OriginMassWeighted",	1) <<
		EnumOption(SpeciesSite::XAxisKeyword,			"XAxis",		EnumOption::OneOrMoreArguments) <<
		EnumOption(SpeciesSite::YAxisKeyword, 			"YAxis",		EnumOption::OneOrMoreArguments);

	static EnumOptions<SpeciesSite::SiteKeyword> options("SiteKeyword", SiteKeywords);

	return options;
}

// Read site definition from specified LineParser
bool SpeciesSite::read(LineParser& parser)
{
	Messenger::printVerbose("\nReading information for Site '%s'...\n", name());

	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!keywords().isValid(parser.argc(0))) return keywords().errorAndPrintValid(parser.argc(0));
		SiteKeyword kwd = keywords().enumeration(parser.argc(0));
		if (!keywords().validNArgs(kwd, parser.nArgs()-1)) return false;

		// All OK, so process the keyword
		switch (kwd)
		{
			case (SpeciesSite::EndSiteKeyword):
				Messenger::print("Found end of Site '%s'.\n", name());
				blockDone = true;
				break;
			case (SpeciesSite::OriginKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					if (!addOriginAtom(parser.argi(n) - 1))
					{
						Messenger::error("Failed to add origin atom for site '%s'.\n", name());
						error = true;
						break;
					}
				}
				break;
			case (SpeciesSite::OriginMassWeightedKeyword):
				setOriginMassWeighted(parser.argb(1));
				break;
			case (SpeciesSite::XAxisKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					if (!addXAxisAtom(parser.argi(n) - 1))
					{
						Messenger::error("Failed to add x-axis atom for site '%s'.\n", name());
						error = true;
						break;
					}
				}
				break;
			case (SpeciesSite::YAxisKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					if (!addYAxisAtom(parser.argi(n) - 1))
					{
						Messenger::error("Failed to add y-axis atom for site '%s'.\n", name());
						error = true;
						break;
					}
				}
				break;
			default:
				printf("DEV_OOPS - Site block keyword '%s' not accounted for.\n", keywords().keyword(kwd));
				error = true;
				break;
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	// If there's no error and the blockDone flag isn't set, return an error
	if (!error && !blockDone)
	{
		Messenger::error("Unterminated Site block found.\n");
		error = true;
	}

	return (!error);
}

// Write site definition to specified LineParser
bool SpeciesSite::write(LineParser& parser, const char* prefix)
{
	// Write start of site definition
	if (!parser.writeLineF("%sSite  '%s'\n", prefix, name())) return false;

	// Origin atom indices
	if (originAtoms_.nItems() > 0)
	{
		Array<int> indices = originAtomIndices();

		CharString atomIndices;
		for (int n=0; n<indices.nItems(); ++n) atomIndices.strcatf("  %i", indices[n]+1);

		if (!parser.writeLineF("%s  %s%s\n", prefix, keywords().keyword(OriginKeyword), atomIndices.get())) return false;
	}

	// Origin mass weighted?
	if (originMassWeighted_ && (!parser.writeLineF("%s  %s  True\n", prefix, keywords().keyword(OriginMassWeightedKeyword)))) return false;

	// X-Axis atom indices
	if (xAxisAtoms_.nItems() > 0)
	{
		Array<int> indices = xAxisAtomIndices();

		CharString atomIndices;
		for (int n=0; n<indices.nItems(); ++n) atomIndices.strcatf("  %i", indices[n]+1);

		if (!parser.writeLineF("%s  %s%s\n", prefix, keywords().keyword(XAxisKeyword), atomIndices.get())) return false;
	}

	// Y-Axis atom indices
	if (yAxisAtoms_.nItems() > 0)
	{
		Array<int> indices = yAxisAtomIndices();

		CharString atomIndices;
		for (int n=0; n<indices.nItems(); ++n) atomIndices.strcatf("  %i", indices[n]+1);

		if (!parser.writeLineF("%s  %s%s\n", prefix, keywords().keyword(YAxisKeyword), atomIndices.get())) return false;
	}

	// Write start of site definition
	if (!parser.writeLineF("%s%s\n", prefix, keywords().keyword(EndSiteKeyword))) return false;

	return true;
}
