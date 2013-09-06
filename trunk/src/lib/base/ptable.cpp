/*
	*** Periodic Table (Element and Isotope information)
	*** src/lib/base/ptable.cpp
	Copyright T. Youngs 2012-2013

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

#include "base/ptable.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/comms.h"
#include <string.h>

// Static singleton
PeriodicTable periodicTable;

// Static Members
int PeriodicTable::nElements_;
Element *PeriodicTable::elements_;
int *PeriodicTable::elementCount_;

/*!
 * \brief Constructor
 * \details Constructor for PeriodicTable. 
 */
PeriodicTable::PeriodicTable()
{
	nElements_ = 0;
	elements_ = NULL;
	elementCount_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for PeriodicTable. 
 */
PeriodicTable::~PeriodicTable()
{
	clear();
}

/*!
 * \brief Clear
 * \details Clear all data
 */
void PeriodicTable::clear()
{
	if (elements_ != NULL) delete[] elements_;
	elements_ = NULL;
	if (elementCount_ != NULL) delete[] elementCount_;
	elementCount_ = NULL;
	nElements_ = 0;
}

/*
// Element Data
*/

/*!
 * \brief Load basic element information from file specified
 */
bool PeriodicTable::loadElements(const char* fileName)
{
	msg.print("Loading elements from file '%s'\n", fileName);
	
	// Open the specified file...
	LineParser parser;
	parser.openInput(fileName);
	if (!parser.isFileGoodForReading())
	{
		msg.error("Couldn't open elements file.\n");
		return false;
	}
	
	// First line contains number of comment lines to follow
	parser.getArgsDelim(LineParser::Defaults);
	int nSkip = parser.argi(0);
	msg.printVerbose("Skipping %i lines at start of elements definitions file...\n", nSkip);
	parser.skipLines(nSkip);
	
	// Next line will contain number of elements to read in
	parser.getArgsDelim(LineParser::Defaults);
	nElements_ = parser.argi(0);
	elements_ = new Element[nElements_];
	elementCount_ = new int[nElements_];
	int success;
	for (int n=0; n<nElements_; ++n)
	{
		success = parser.getArgsDelim(LineParser::Defaults);
		if (success != 0)
		{
			msg.error("Error reading element %i from elements file.\n", n+1);
			parser.closeFiles();
			return false;
		}
		elements_[n].set(parser.argi(0), parser.argc(1), parser.argc(2), parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6), parser.argd(7));
	}
	parser.closeFiles();
	
	msg.print("Loaded %i elements from file.\n", nElements_);
	return true;
}

/*!
 * \brief Load isotope information from file specified
 */
bool PeriodicTable::loadIsotopes(const char* fileName)
{
	msg.print("Loading isotopes from file '%s'\n", fileName);
	
	// Open the specified file...
	LineParser parser;
	parser.openInput(fileName);
	if (!parser.isFileGoodForReading())
	{
		msg.error("Couldn't open isotopes file.\n");
		return false;
	}
	
	// First line contains number of comment lines to follow
	parser.getArgsDelim(LineParser::Defaults);
	int nSkip = parser.argi(0);
	msg.printVerbose("Skipping %i lines at start of isotopes definitions file...\n", nSkip);
	parser.skipLines(nSkip);
	
	// Read data until EOF
	int Z = 0, A, count = 0;
	Dnchar arg, arg2, el;
	double weight, bc, bi, sc, si, ss, sa;
	Isotope* isotope;
	bool failed = false;
	while (parser.readNextLine(LineParser::Defaults) == 0)
	{
		// Split up read line into component parts...
		// Fortran format of line is: format(a2,1x,i2,1x,i3,1x,a6,1x,f9.5,4(1x,f8.4),1x,f9.4,1x,e10.3)
		// Corresponding to: El  Z   A  I(p)     Atwt       bc       bi       sc       si        ss     sa
		// Element (characters 1-3) (a2,1x)
		if (!parser.getNextN(LineParser::Defaults, 3, &arg)) failed = true;
		// Z (characters 4-6) (i2,1x)
		if (!parser.getNextN(LineParser::Defaults, 3, &arg2)) failed = true;
		// -- el/Z data might be empty - if it is, keep el and Z from last time
		if (arg2.isNumeric())
		{
			el = arg;
			Z = arg2.asInteger();
			// Check that el and Z match...
			if (find(el.get()) != Z)
			{
				msg.error("Element symbol for isotope (%s) doesn't match Z number (%i)\n", el.get(), Z);
				parser.closeFiles();
				return false;
			}
		}
		// A (characters 7-10) (i3,1x)
		if (!parser.getNextN(LineParser::Defaults, 3, &arg)) failed = true;
		A = arg.asInteger();
		// I(p) - Unused (characters 11-17) (a6,1x)
		if (!parser.getNextN(LineParser::Defaults, 7, &arg)) failed = true;
		// Atwt (characters 18-27) (f9.5,1x)
		if (!parser.getNextN(LineParser::Defaults, 10, &arg)) failed = true;
		weight = arg.asDouble();
		// bc (characters 28-36) (f8.4,1x)
		if (!parser.getNextN(LineParser::Defaults, 9, &arg)) failed = true;
		bc = arg.asDouble();
		// bi (characters 37-45) (f8.4,1x)
		if (!parser.getNextN(LineParser::Defaults, 9, &arg)) failed = true;
		bi = arg.asDouble();
		// sc (characters 46-54) (f8.4,1x)
		if (!parser.getNextN(LineParser::Defaults, 9, &arg)) failed = true;
		sc = arg.asDouble();
		// si (characters 55-63) (f8.4,1x)
		if (!parser.getNextN(LineParser::Defaults, 9, &arg)) failed = true;
		si = arg.asDouble();
		// ss (characters 64-73) (f9.4,1x)
		if (!parser.getNextN(LineParser::Defaults, 10, &arg)) failed = true;
		ss = arg.asDouble();
		// sa (characters 74-83) (e10.4)
		if (!parser.getNextN(LineParser::Defaults, 10, &arg)) failed = true;
		sa = arg.asDouble();
		
		// Did we succeed?
		if (failed)
		{
			msg.error("Error reading isotope data from file, line = '%s'.\n", parser.line());
			parser.closeFiles();
			return false;
		}
		
		// Create new isotope definition for target element
		isotope = elements_[Z].addIsotope();
		isotope->set(A, weight, bc, bi, sc, si, ss, sa);
		
		++count;
		
		if (parser.eofOrBlank()) break;;
	}
	parser.closeFiles();
	
	msg.print("Loaded %i isotope definitions from file.\n", count);
	
	// Go through Elements and check for no natural isotope defined (and add it if necessary)
	for (int n=0; n<nElements_; ++n)
	{
		// Search defined isotopes for natural definition (A = 0)
		for (isotope = elements_[n].isotopes(); isotope != NULL; isotope = isotope->next) if (isotope->A() == 0) break;
		if (isotope != NULL) msg.printVerbose("Found natural isotope for element %i (%s) - bc = %f\n", n, elements_[n].name(), isotope->boundCoherent());
		else
		{
			msg.print("Creating natural isotope for element %i (%s) with bc = 0.0.\n", n, elements_[n].name());
			isotope = elements_->addIsotope();
		}
	}

	return true;
}

/*!
 * \brief Load atomtype information from file specified
 */
bool PeriodicTable::loadParameters(const char* fileName)
{
	msg.print("Loading atomtypes from file '%s'\n", fileName);
	
	// Open the specified file...
	LineParser parser;
	parser.openInput(fileName);
	if (!parser.isFileGoodForReading())
	{
		msg.print("Couldn't open atomtypes file.\n");
		return false;
	}
	
	// First line contains number of comment lines to follow
	parser.getArgsDelim(LineParser::Defaults);
	int nSkip = parser.argi(0);
	msg.printVerbose("Skipping %i lines at start of atomtypes definitions file...\n", nSkip);
	parser.skipLines(nSkip);
	
	// Read data until EOF
	int Z, count = 0;
	Parameters* params;
	bool failed = false;
	while (parser.getArgsDelim(LineParser::SkipBlanks | LineParser::StripComments | LineParser::UseQuotes) == 0)
	{
		// First, convert argument 1 into an element
		Z = find(parser.argc(0));
		if (Z == 0)
		{
			msg.error("Element '%s' unrecognised in atomtype definitions file.\n", parser.argc(0));
			parser.closeFiles();
			return false;
		}

		// Did we succeed?
		if (failed)
		{
			msg.error("Error reading atomtype data from file, line = '%s'.\n", parser.line());
			parser.closeFiles();
			return false;
		}
		
		// Create new isotope definition for target element
		params = elements_[Z].addParameters();
		params->setName(parser.argc(1));
		params->setDescription(parser.argc(5));
		params->set(parser.argd(2), parser.argd(3), parser.argd(4));
		
		++count;
		
		if (parser.eofOrBlank()) break;;
	}
	parser.closeFiles();
	
	msg.print("Loaded %i atomtype definitions from file.\n", count);
	
	// Go through Elements and check for no atomtype definigions, adding a basic one if necessary
	for (Z=0; Z<nElements_; ++Z)
	{
		// Search defined isotopes for natural definition (A = 0)
		if (elements_[Z].nParameters() != 0) continue;

		msg.print("Creating generic atomtype for element %i (%s).\n", Z, elements_[Z].name());
		params = elements_[Z].addParameters();
		params->setName(elements_[Z].symbol());
		params->setDescription(elements_[Z].name());
		params->set(2.0, 0.1, 0.0);
	}

	return true;
}

/*!
 * \brief Return atomic number of element in string
 */
int PeriodicTable::find(const char* query)
{
	static Dnchar cleaned;
	cleaned.clear();
	int n, nDigits = 0, count = 0;
	for (n=0; query[n] != '\0'; n++)
	{
		if (query[n] == ' ') continue;
		else if ((query[n] > 64) && (query[n] < 91)) cleaned += (count == 0 ? query[n] : tolower(query[n]));
		else if ((query[n] > 96) && (query[n] < 123)) cleaned += (count == 0 ? toupper(query[n]) : query[n]);
		else if ((query[n] > 47) && (query[n] < 58)) { cleaned += query[n]; ++nDigits; }
		else break;
		++count;
	}
	// Pure digit given?
	if (count == nDigits) return atoi(cleaned.get());
	else for (n=0; n<nElements_; n++) if (cleaned == elements_[n].symbol()) return n;
	return -1;
}

/*!
 * \brief Return number of defined elements
 */
int PeriodicTable::nElements()
{
	return nElements_;
}

/*!
 * \brief Return element specified
 */
Element &PeriodicTable::element(int z)
{
#ifdef CHECKS
	if ((z < 0) || (z > nElements_))
	{
		msg.print("OUT_OF_RANGE - Bad element ID requested (%i) in PeriodicTable::element().\n", z);
		return elements_[0];
	}
#endif
	return elements_[z];
}

/*
// Empirical Formula Generation
*/

/*!
 * \brief Clear counts array
 * \details Clear the element counts array, ready for generation of a new empirical formula.
 */
void PeriodicTable::resetEmpiricalFormula()
{
	for (int n=0; n<nElements_; ++n) elementCount_[n] = 0;
}

/*!
 * \brief Add Atom (element) to empirical formula
 */
void PeriodicTable::addToEmpirical(int z, int count)
{
	if ((z < 0) || (z >= nElements_)) msg.print("OUT_OF_RANGE - Element index %i is out of range.\n", z);
	else elementCount_[z] += count;
}

/*!
 * \brief Return current empirical formula
 * \details Generate (and return) an empirical formula based on the current element counts.
 */
const char* PeriodicTable::empiricalFormula()
{
	static Dnchar formula(1024);
	formula.clear();

	// Loop over elements in descending order
	for (int n=nElements_-1; n>=0; --n)
	{
		if (elementCount_[n] == 0) continue;
		else if (elementCount_[n] > 1) formula.strcatf("%s%i", elements_[n].symbol(), elementCount_[n]);
		else formula.strcatf("%s", elements_[n].symbol());
	}
	return formula.get();
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool PeriodicTable::broadcast()
{
#ifdef PARALLEL
	// Clear slave data
	if (Comm.slave()) clear();

	// Send number of elements
	Comm.broadcast(&nElements_, 1);
	if (Comm.slave())
	{
		elements_ = new Element[nElements_];
		elementCount_ = new int[nElements_];
	}

	// Send elements
	for (int n = 0; n<nElements_; ++n) elements_[n].broadcast();
#endif
	return true;
}
