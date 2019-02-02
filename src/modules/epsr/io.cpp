/*
	*** EPSR Module - I/O
	*** src/modules/epsr/io.cpp
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

#include "main/dissolve.h"
#include "module/keywordtypes.h"
#include "modules/epsr/epsr.h"
#include "classes/atomtype.h"
#include "templates/genericlisthelper.h"

// EPSR PCof Keywords
const char* EPSRPCofKeywordKeywords[] = { "addpottype", "expecf", "gaussian", "ncoeffp",  "npitss", "paccept", "pdmax", "pdstep", "power", "psigma2", "q", "rbroad", "rcharge",  "refpotfac",  "reppottype", "rmaxpt", "rminfac", "rminpt", "roverlap" };

// Convert text string to EPSRPCofKeyword
EPSRModule::EPSRPCofKeyword EPSRModule::epsrPCofKeyword(const char* s)
{
	for (int n=0; n<EPSRModule::nEPSRPCofKeywords; ++n) if (DissolveSys::sameString(s, EPSRPCofKeywordKeywords[n])) return (EPSRModule::EPSRPCofKeyword) n;
	return EPSRModule::nEPSRPCofKeywords;
}

// Convert EPSRPCofKeyword to text string
const char* EPSRModule::epsrPCofKeyword(EPSRModule::EPSRPCofKeyword pcofkwd)
{
	return EPSRPCofKeywordKeywords[pcofkwd];
}

// Read data from supplied pcof file
bool EPSRModule::readPCof(Dissolve& dissolve, ProcessPool& procPool, const char* filename)
{
	/*
	 * Read EPSR potential coefficients from supplied file.
	 * All potential coefficients matching pair potentials that exist in the simulation will be set.
	 * All others will be set to zero.
	 */

	LineParser parser(&procPool);
	if (!parser.openInput(filename)) return Messenger::error("Couldn't open pcof file for reading.\n");

	// Read keyword section (terminated by the 'q' command)
	bool done = false;
	int ncoeffp = 0;
	while (!parser.eofOrBlank())
	{
		// Parse arguments, and attempt to convert the first into a keyword
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return Messenger::error("Failed to read pcof file.\n");

		EPSRModule::EPSRPCofKeyword keyword = epsrPCofKeyword(parser.argc(0));
		switch (keyword)
		{
			case (EPSRModule::AddPotTypePCofKeyword):
				break;
			case (EPSRModule::ExpecFPCofKeyword):
				break;
			case (EPSRModule::GaussianPCofKeyword):
				KeywordListHelper<CharString>::set(keywords_, "expansionfunction", (DissolveSys::sameString(parser.argc(1), "Poisson") || DissolveSys::sameString(parser.argc(1), "T") ? "Poisson" : "Gaussian"));
				break;
			case (EPSRModule::NCoeffPPCofKeyword):
				ncoeffp = parser.argi(1);
				KeywordListHelper<int>::set(keywords_, "ncoeffp", ncoeffp);
				break;
			case (EPSRModule::NPItSSPCofKeyword):
				KeywordListHelper<int>::set(keywords_, "npitss", parser.argi(1));
				break;
			case (EPSRModule::PAcceptPCofKeyword):
				break;
			case (EPSRModule::PDMaxPCofKeyword):
				break;
			case (EPSRModule::PDStepPCofKeyword):
				break;
			case (EPSRModule::PowerPCofKeyword):
				break;
			case (EPSRModule::PSigma2PCofKeyword):
				KeywordListHelper<double>::set(keywords_, "psigma1", parser.argd(1));
				KeywordListHelper<double>::set(keywords_, "psigma2", parser.argd(1));
				KeywordListHelper<double>::set(keywords_, "gsigma1", parser.argd(1));
				// Note - the factor of two applied here is used to reproduce the broadening applied by ESPR to the r-space Gaussian transformation
				KeywordListHelper<double>::set(keywords_, "gsigma2", parser.argd(1) * 2.0);
				break;
			case (EPSRModule::QuitPCofKeyword):
				done = true;
				break;
			case (EPSRModule::RBroadPCofKeyword):
				break;
			case (EPSRModule::RChargePCofKeyword):
				break;
			case (EPSRModule::RefPotFacPCofKeyword):
				break;
			case (EPSRModule::RepPotTypePCofKeyword):
				break;
			case (EPSRModule::RMaxPtPCofKeyword):
				KeywordListHelper<double>::set(keywords_, "rmaxpt", parser.argd(1));
				break;
			case (EPSRModule::RMinFacPCofKeyword):
				break;
			case (EPSRModule::RMinPtPCofKeyword):
				KeywordListHelper<double>::set(keywords_, "rminpt", parser.argd(1));
				break;
			case (EPSRModule::ROverlapPCofKeyword):
				break;
			default:
				Messenger::warn("Unrecognised pcof file keyword '%s'...\n", parser.argc(0));
				continue;
		}

		// If we have found the 'q' keyword, exit the loop
		if (done) break;
	}

	// Retrieve and zero the current potential coefficients file
	Array2D< Array<double> >& potentialCoefficients = GenericListHelper< Array2D< Array<double> > >::realise(dissolve.processingModuleData(), "PotentialCoefficients", uniqueName_, GenericItem::InRestartFileFlag);
	potentialCoefficients.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
	for (int n=0; n<potentialCoefficients.linearArraySize(); ++n)
	{
		potentialCoefficients.linearArray()[n].initialise(ncoeffp);
		potentialCoefficients.linearArray()[n] = 0.0;
	}

	// Get density to apply to coefficients (as 1.0/rho)
	const double rho = keywords_.asDouble("PCofRho");

	// Now we are ready to read in the potential coefficients - first line contains the number of pair potentials to expect coefficients for
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return Messenger::error("Failed to read number of pair potentials from pcof file.\n");
	int nPots = parser.argi(0);
	Messenger::print("Number of potentials in pcof file = %i\n", nPots);
	for (int n=0; n<nPots; ++n)
	{
		// First line of potential contains the two atom types it is related to, and its index (in EPSR)
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return Messenger::error("Failed to read pair potential atom types from pcof file.\n");

		// Find the atom types to which these cofficients relate
		AtomType* at1 = dissolve.findAtomType(parser.argc(0));
		if (!at1) return Messenger::error("Unrecognised AtomType '%s' referenced in pcof file.\n", parser.argc(0));
		AtomType* at2 = dissolve.findAtomType(parser.argc(1));
		if (!at2) return Messenger::error("Unrecognised AtomType '%s' referenced in pcof file.\n", parser.argc(1));
		Messenger::print("Found %s-%s potential...\n", at1->name(), at2->name());

		// Next line contains ??? and ??? TODO
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return Messenger::error("Failed to read ??? and ??? from pcof file.\n");

		// Grab the coefficient storage from the module data and read the coefficients in - they will all be on one single line in the file.
		Array<double>& coefficients = potentialCoefficients.at(at1->index(), at2->index());
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return Messenger::error("Failed to read coefficients from pcof file.\n");
		if (parser.nArgs() != ncoeffp) return Messenger::error("Number of potential coefficients (%i) does not match ncoeffp (%i).\n", parser.nArgs(), ncoeffp);
		for (int i=0; i<ncoeffp; ++i) coefficients[i] = parser.argd(i);

		// Zero the first coefficient, which EPSR ignores
		coefficients[0] = 0.0;

		// Apply factor of (1.0/rho), since our Poisson function normalisations do not contain this term
		coefficients /= rho;
	}

	return true;
}
