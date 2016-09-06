/*
	*** dUQ I/O - Input File
	*** src/lib/main/io_input.cpp
	Copyright T. Youngs 2012-2014

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

#include "main/duq.h"
#include "main/flags.h"
#include "main/keywords.h"
#include "main/command.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "classes/graindefinition.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <string.h>
#include <QtCore/QDate>

/*!
 * \brief Load datafiles
 */
bool DUQ::loadDataFiles()
{
	// Get basic path for data files
	Dnchar fileName, dataPath = getenv("DUQ_DATA");
	if (dataPath.isEmpty())
	{
		msg.print("Environment variable DUQ_DATA not set - using './data' as the default.\n");
		dataPath = "./data";
	}
	else msg.print("Looking for datafiles in '%s'...\n", dataPath.get());
	
	// Load elements data
	fileName.sprintf("%s/elements.dat", dataPath.get());
	if (!periodicTable.loadElements(fileName)) return false;
	
	// Load isotope data
	fileName.sprintf("%s/sears91_gudrun.dat", dataPath.get());
	if (!periodicTable.loadIsotopes(fileName)) return false;
	
	// Load parameter data
	fileName.sprintf("%s/atomtypes.dat", dataPath.get());
	if (!periodicTable.loadParameters(fileName)) return false;
	
	return true;
}

/*!
 * \brief Load Species from specified file
 */
bool DUQ::loadSpecies(const char* fileName)
{
	Species* newSpecies = addSpecies();
	if (!newSpecies->loadFromXYZ(fileName))
	{
		msg.print("Error loading from XYZ file.\n");
		removeSpecies(newSpecies);
		return false;
	}
	
	// Succesfully loaded the species from file, so perform post-load operations

	// Set initial composition
	newSpecies->setRelativePopulation(1.0);

	// Centre coordinates at the origin
	newSpecies->centreAtOrigin();

	// Update the AtomTypes list
	updateAtomTypes();

	// Calculate bonds and angles
	msg.print("Recalculating bonds/angles for '%s'...\n", newSpecies->name());
	newSpecies->recalculateIntramolecular();
	
	// Add an automatic GrainDefinition
	msg.print("Adding automatic GrainDefinitions for '%s'...\n", newSpecies->name());
	newSpecies->autoAddGrains();
	
	// Add the default (natural) Isotopologue
	msg.print("Adding natural isotopologue...\n");
	Isotopologue* iso = newSpecies->addIsotopologue("Natural");
	updateIsotopologues(newSpecies, iso);

	// Must update all existing Samples, so that this Species is added to them...
	updateSamples();

	return true;
}

/*!
 * \brief Load input file
 */
bool DUQ::loadInput(const char* fileName)
{
	// Open file and check that we're OK to proceed reading from it
	LineParser parser;
	parser.openInput(fileName);
	if (!parser.isFileGoodForReading())
	{
		msg.error("Couldn't open file '%s' for reading.\n", fileName);
		return false;
	}
	
	// Clear all existing data before we begin
	clear();
	
	// Variables
	int el, n;
	AtomType* at, *at2;
	Isotopologue* iso;
	Isotope* tope;
	Parameters* params;
	PairPotential* pot;
	Sample* sam;
	Species* sp;
	SpeciesAngle* a;
	SpeciesAtom* i;
	SpeciesBond* b;
	SpeciesGrain* sg;
	Keywords::InputBlock block;
	Keywords::SpeciesKeyword spkeyword;
	Keywords::AtomTypesKeyword atkeyword;
	Keywords::SampleKeyword samkeyword;
	Keywords::SystemKeyword syskeyword;
	Keywords::PairPotentialsKeyword ppkeyword;
	Keywords::SetupKeyword setkeyword;
	Keywords::SimulationKeyword simkeyword;
	List<Step>* targetSteps = NULL;
	Step* step;
	Dnchar arg1, arg2;
	bool error = false, blockDone;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a block keyword
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
		block = Keywords::inputBlock(parser.argc(0));
		switch (block)
		{
			case (Keywords::AtomTypesBlock):
				msg.print("Found %s block\n", Keywords::inputBlock(block));
				blockDone = false;
				while (!parser.eofOrBlank())
				{
					// Read in a line, which should contain a keyword and a minimum number of arguments
					parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
					atkeyword = Keywords::atomTypesKeyword(parser.argc(0));
					if ((atkeyword != Keywords::nAtomTypesKeywords) && ((parser.nArgs()-1) < Keywords::atomTypesBlockNArguments(atkeyword)))
					{
						msg.error("Not enough arguments given to '%s' keyword.\n", Keywords::atomTypesKeyword(atkeyword));
						error = true;
						break;
					}
					switch (atkeyword)
					{
						case (Keywords::AtomTypeKeyword):
							el = PeriodicTable::find(parser.argc(2));
							if (el == -1)
							{
								msg.error("Unrecognised element symbol '%s' found in %s keyword.\n", parser.argc(2), Keywords::atomTypesKeyword(Keywords::AtomTypeKeyword));
								el = 0;
								error = true;
								break;
							}
							params = PeriodicTable::element(el).findParameters(parser.argc(3));
							if (!params)
							{
								msg.error("Couldn't find Parameters called '%s' when adding AtomType '%s'.\n", parser.argc(3), parser.argc(1));
								params = PeriodicTable::element(el).parameters();
								error = true;
								break;
							}
							at = addAtomType(el);
							at->setName(parser.argc(1));
							at->setParameters(params);
							break;
						case (Keywords::EndAtomTypesKeyword):
							blockDone = true;
							break;
						case (Keywords::nAtomTypesKeywords):
							msg.error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(block), parser.argc(0));
							Keywords::printValidKeywords(block);
							error = true;
							break;
						default:
							printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(block), Keywords::atomTypesKeyword(atkeyword));
							error = true;
							break;
					}

					// Error encountered?
					if (error) break;
					
					// End of block?
					if (blockDone) break;
				}
				break;
			case (Keywords::SpeciesBlock):
				msg.print("Found %s '%s'\n", Keywords::inputBlock(block), parser.argc(1));
				blockDone = false;
				sp = addSpecies();
				sp->setName(uniqueSpeciesName(parser.argc(1)));
				while (!parser.eofOrBlank())
				{
					// Read in a line, which should contain a keyword and a minimum number of arguments
					parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
					spkeyword = Keywords::speciesKeyword(parser.argc(0));
					if ((spkeyword != Keywords::nSpeciesKeywords) && ((parser.nArgs()-1) < Keywords::speciesBlockNArguments(spkeyword)))
					{
						msg.error("Not enough arguments given to '%s' keyword.\n", Keywords::speciesKeyword(spkeyword));
						error = true;
						break;
					}
					switch (spkeyword)
					{
						case (Keywords::AngleKeyword):
							a = sp->addAngle(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1);
							if (a)
							{
								a->setEquilibrium(parser.argd(4));
								a->setForceConstant(parser.argd(5));
							}
							else error = true;
							break;
						case (Keywords::AtomKeyword):
							el = PeriodicTable::find(parser.argc(2));
							if (el == -1)
							{
								msg.error("Unrecognised element symbol '%s' found in %s keyword.\n", parser.argc(2), Keywords::speciesKeyword(Keywords::AtomKeyword));
								el = 0;
								error = true;
								break;
							}
							i = sp->addAtom(el, parser.argd(3), parser.argd(4), parser.argd(5));
							i->setCharge(parser.argd(6));
							at = findAtomType(parser.argc(7));
							if (!at)
							{
								msg.print("Warning: AtomType '%s' has not been defined in Species '%s'...\n", parser.argc(7), sp->name());
								at = atomTypeForElement(el);
								if (at) msg.print("--> Using first available AtomType ('%s') for element.\n", at->name());
								else
								{
									msg.error("No available AtomTypes.\n");
									error = true;
									break;
								}
							}
							i->setAtomType(at);
							break;
						case (Keywords::BondKeyword):
							b = sp->addBond(parser.argi(1)-1, parser.argi(2)-1);
							if (b)
							{
								b->setEquilibrium(parser.argd(3));
								b->setForceConstant(parser.argd(4));
							}
							else error = true;
							break;
						case (Keywords::EndSpeciesKeyword):
							sp->updateGrains();
							sp->centreAtOrigin();
							sp->orderAtomsWithinGrains();
							sp->calculateIndexLists();
							sp->identifyInterGrainTerms();
							sp->createScalingMatrix();
							msg.print("Found end of Species '%s'.\n", sp->name());
							blockDone = true;
							break;
						case (Keywords::GrainKeyword):
							sg = sp->addGrain();
							sg->setName(sp->uniqueGrainName(parser.argc(1)));
							msg.print("--> Added grain definition '%s' to Species '%s'\n", sg->name(), sp->name());
							for (n=2; n<parser.nArgs(); ++n)
							{
								i = sp->atom(parser.argi(n)-1);
								if (i == NULL)
								{
									msg.error("Failed to find atom with index %i in Species '%s'\n", parser.argi(n), sp->name());
									error = true;
								}
								else sp->addAtomToGrain(i, sg);
							}
							break;
						case (Keywords::IsotopologueKeyword):
							iso = sp->addIsotopologue(sp->uniqueIsotopologueName(parser.argc(1)));
							updateIsotopologues(sp, iso);
							msg.print("--> Added Isotopologue '%s' to Species '%s'\n", iso->name(), sp->name());
							// Each parser argument is a string of the form ATOMTYPE=ISO
							for (n=2; n<parser.nArgs(); ++n)
							{
								// Split argument into parts before and after '='
								arg1 = beforeChar(parser.argc(n),'=');
								arg2 = afterChar(parser.argc(n),'=');
								
								at = findAtomType(arg1.get());
								if (at == NULL)
								{
									msg.error("Failed to find AtomType '%s', referred to in Isotopologue '%s', Species '%s'\n", arg1.get(), iso->name(), sp->name());
									error = true;
									break;
								}

								// Is supplied isotope valid for the AtomType's element?
								el = at->element();
								tope = PeriodicTable::element(el).hasIsotope(arg2.asInteger());
								if (tope == NULL)
								{
									msg.error("No such Isotope (%i) for element %s (AtomType '%s') in Isotopologue '%s', Species '%s'\n", arg2.asInteger(), PeriodicTable::element(el).symbol(), at->name(), iso->name(), sp->name());
									error = true;
									break;
								}
								
								// Assign isotope to AtomType
								iso->setAtomTypeIsotope(at, tope);
							}
							break;
						case (Keywords::nSpeciesKeywords):
							msg.error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(block), parser.argc(0));
							Keywords::printValidKeywords(block);
							error = true;
							break;
						default:
							printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(block), Keywords::speciesKeyword(spkeyword));
							error = true;
							break;
					}

					// Error encountered?
					if (error) break;
					
					// End of block?
					if (blockDone) break;
				}
				break;
			case (Keywords::SystemBlock):
				msg.print("Found %s block\n", Keywords::inputBlock(block));
				blockDone = false;
				while (!parser.eofOrBlank())
				{
					// Read in a line, which should contain a keyword and a minimum number of arguments
					parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
					syskeyword = Keywords::systemKeyword(parser.argc(0));
					if ((syskeyword != Keywords::nSystemKeywords) && ((parser.nArgs()-1) < Keywords::systemBlockNArguments(syskeyword)))
					{
						msg.error("Not enough arguments given to '%s' keyword.\n", Keywords::systemKeyword(syskeyword));
						error = true;
						break;
					}
					switch (syskeyword)
					{
						case (Keywords::CellAnglesKeyword):
							boxAngles_.set(parser.argd(1), parser.argd(2), parser.argd(3));
							break;
						case (Keywords::CellLengthsKeyword):
							relativeBoxLengths_.set(parser.argd(1), parser.argd(2), parser.argd(3));
							break;
						case (Keywords::ComponentKeyword):
							sp = findSpecies(parser.argc(1));
							if (sp == NULL)
							{
								msg.error("System refers to Species '%s', but no such Species is defined.\n", parser.argc(1));
								error = true;
							}
							else
							{
								if (sp->relativePopulation() > 0.0)
								{
									msg.error("Composition for Species '%s' has already been set.\n", sp->name());
									error = true;
								}
								else
								{
									sp->setRelativePopulation(parser.argd(2));
									msg.print("--> Set composition for Species '%s' (%f relative population).\n", sp->name(), sp->relativePopulation());
								}
							}
							break;
						case (Keywords::DensityKeyword):
							// Determine units given
							if (strcmp(parser.argc(2),"atoms/A3") == 0) setAtomicDensity(parser.argd(1));
							else if (strcmp(parser.argc(2),"g/cm3") == 0) setChemicalDensity(parser.argd(1));
							else
							{
								msg.error("Unrecognised density unit given - '%s'\nValid values are 'atoms/A3' or 'g/cm3'.\n", parser.argc(2));
								error = true;
							}
							break;
						case (Keywords::EndSystemKeyword):
							msg.print("Found end of %s block.\n", Keywords::inputBlock(block));
							blockDone = true;
							break;
						case (Keywords::FileModelKeyword):
							initialCoordinatesFile_ = parser.argc(1);
							randomConfiguration_ = false;
							msg.print("Initial coordinates will be loaded from file '%s'\n", initialCoordinatesFile_.get());
							break;
						case (Keywords::MultiplierKeyword):
							setMultiplier(parser.argd(1));
							msg.print("--> Set system multiplier to %i\n", multiplier_);
							break;
						case (Keywords::NonPeriodicKeyword):
							nonPeriodic_ = true;
							msg.print("--> Flag set for a non-periodic calculation.\n");
							break;
						case (Keywords::nSystemKeywords):
							msg.print("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(block), parser.argc(0));
							Keywords::printValidKeywords(block);
							error = true;
							break;
						default:
							printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(block), Keywords::systemKeyword(syskeyword));
							error = true;
							break;
					}
					
					// Error encountered?
					if (error) break;
					
					// End of block?
					if (blockDone) break;
				}
				break;
			case (Keywords::SampleBlock):
				msg.print("Found %s '%s'\n", Keywords::inputBlock(block), parser.argc(1));
				blockDone = false;
				sam = addSample(uniqueSampleName(parser.argc(1)));
				while (!parser.eofOrBlank())
				{
					// Read in a line, which should contain a keyword and a minimum number of arguments
					parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
					samkeyword = Keywords::sampleKeyword(parser.argc(0));
					if ((samkeyword != Keywords::nSampleKeywords) && ((parser.nArgs()-1) < Keywords::sampleBlockNArguments(samkeyword)))
					{
						msg.error("Not enough arguments given to '%s' keyword.\n", Keywords::sampleKeyword(samkeyword));
						error = true;
						break;
					}
					switch (samkeyword)
					{
						case (Keywords::FitQMaxKeyword):
							sam->setReferenceFitQMax(parser.argd(1));
							break;
						case (Keywords::FitQMinKeyword):
							sam->setReferenceFitQMin(parser.argd(1));
							break;
						case (Keywords::IsotopologueSampleKeyword):
							// Locate Species first...
							sp = findSpecies(parser.argc(1));
							if (sp == NULL)
							{
								msg.error("Sample refers to Species '%s', but no such Species is defined.\n", parser.argc(1));
								error = true;
								break;
							}
							
							// Now locate Isotopologue
							iso = sp->findIsotopologue(parser.argc(2));
							if (iso == NULL)
							{
								msg.error("Sample refers to Isotopologue '%s' in Species '%s', but no such Isotopologue is defined.\n", parser.argc(2), parser.argc(1));
								error = true;
								break;
							}

							// OK to add 
							if (!sam->addIsotopologueToMixture(sp, iso, parser.argd(3))) error = true;
							else msg.print("--> Added Isotopologue '%s' (Species '%s') to Sample '%s' (%f relative population).\n", iso->name(), sp->name(), sam->name(), parser.argd(3));
							break;
						case (Keywords::NormalisedToAverageSquaredKeyword):
							if (sam->referenceDataNormalisation() != Sample::NoNormalisation)
							{
								msg.error("Normalisation has already been set for Sample '%s'.\n", sam->name());
								error = true;
							}
							else sam->setReferenceDataNormalisation(Sample::AverageSquaredNormalisation);
							break;
						case (Keywords::NormalisedToSquaredAverageKeyword):
							if (sam->referenceDataNormalisation() != Sample::NoNormalisation)
							{
								msg.error("Normalisation has already been set for Sample '%s'.\n", sam->name());
								error = true;
							}
							else sam->setReferenceDataNormalisation(Sample::SquaredAverageNormalisation);
							break;
						case (Keywords::ReferenceDataKeyword):
							if (!sam->loadReferenceData(parser.argc(1)))
							{
								error = true;
								break;
							}
							break;
						case (Keywords::SubtractSelfKeyword):
							sam->setReferenceSubtractSelf(true);
							break;
						case (Keywords::EndSampleKeyword):
							msg.print("Found end of %s block '%s'.\n", Keywords::inputBlock(block), sam->name());
							blockDone = true;
							break;
						case (Keywords::nSampleKeywords):
							msg.error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(block), parser.argc(0));
							Keywords::printValidKeywords(block);
							error = true;
							break;
						default:
							printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(block), Keywords::sampleKeyword(samkeyword));
							error = true;
							break;
					}
					
					// Error encountered?
					if (error) break;
					
					// End of block?
					if (blockDone) break;
				}
				break;
			case (Keywords::PairPotentialsBlock):
				msg.print("Found %s block\n", Keywords::inputBlock(block));
				blockDone = false;
				while (!parser.eofOrBlank())
				{
					// Read in a line, which should contain a keyword and a minimum number of arguments
					parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
					ppkeyword = Keywords::pairPotentialsKeyword(parser.argc(0));
					if ((ppkeyword != Keywords::nPairPotentialsKeywords) && ((parser.nArgs()-1) < Keywords::pairPotentialsBlockNArguments(ppkeyword)))
					{
						msg.error("Not enough arguments given to '%s' keyword.\n", Keywords::pairPotentialsKeyword(ppkeyword));
						error = true;
						break;
					}
					switch (ppkeyword)
					{
						case (Keywords::CoulombKeyword):
						case (Keywords::DispersionKeyword):
						case (Keywords::FullKeyword):
							// Grab AtomTypes first...
							at = findAtomType(parser.argc(1));
							at2 = findAtomType(parser.argc(2));
							if ((!at) || (!at2))
							{
								msg.error("Eror: AtomType '%s' used in PairPotential has not been defined.\n", at ? parser.argc(2) : parser.argc(1));
								error = true;
								break;
							}
							pot = pairPotentials_.add();
							pot->setParameters(at, at2);
							// Pair potential now contains values from the Parameters associated with the AtomTypes, so we will overwrite them...
							if (ppkeyword == Keywords::CoulombKeyword)
							{
								if (!parser.hasArg(4))
								{
									msg.error("Not enough charge parameters supplied in PairPotential definition.\n");
									error = true;
									break;
								}
								pot->setType(PairPotential::CoulombType);
								pot->setChargeI(parser.argd(3));
								pot->setChargeJ(parser.argd(4));
							}
							else if (ppkeyword == Keywords::DispersionKeyword)
							{
								if (!parser.hasArg(4))
								{
									msg.error("Not enough LJ parameters supplied in PairPotential definition.\n");
									error = true;
									break;
								}
								pot->setType(PairPotential::DispersionType);
								pot->setSigmaIJ(parser.argd(3));
								pot->setEpsilonIJ(parser.argd(4));
							}
							else
							{
								if (!parser.hasArg(6))
								{
									msg.error("Not enough LJ/charge parameters supplied in PairPotential definition.\n");
									error = true;
									break;
								}
								pot->setType(PairPotential::FullType);
								pot->setSigmaIJ(parser.argd(3));
								pot->setEpsilonIJ(parser.argd(4));
								pot->setChargeI(parser.argd(5));
								pot->setChargeJ(parser.argd(6));
							}
							break;
						case (Keywords::DeltaKeyword):
							setPairPotentialDelta(parser.argd(1));
							break;
						case (Keywords::EndPairPotentialsKeyword):
							msg.print("Found end of %s block.\n", Keywords::inputBlock(block));
							blockDone = true;
							break;
						case (Keywords::RangeKeyword):
							setPairPotentialRange(parser.argd(1));
							break;
						case (Keywords::TruncationWidthKeyword):
							setPairPotentialTruncationWidth(parser.argd(1));
							break;
						case (Keywords::nPairPotentialsKeywords):
							msg.error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(block), parser.argc(0));
							Keywords::printValidKeywords(block);
							error = true;
							break;
						default:
							printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(block), Keywords::pairPotentialsKeyword(ppkeyword));
							error = true;
							break;
					}
					
					// Error encountered?
					if (error) break;
					
					// End of block?
					if (blockDone) break;
				}
				break;
			case (Keywords::SetupBlock):
				msg.print("Found %s block\n", Keywords::inputBlock(block));
				blockDone = false;
				while (!parser.eofOrBlank())
				{
					// Read in a line, which should contain a keyword and a minimum number of arguments
					parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
					setkeyword = Keywords::setupKeyword(parser.argc(0));
					if ((setkeyword != Keywords::nSetupKeywords) && ((parser.nArgs()-1) < Keywords::setupBlockNArguments(setkeyword)))
					{
						msg.error("Not enough arguments given to '%s' keyword.\n", Keywords::setupKeyword(setkeyword));
						error = true;
						break;
					}
					switch (setkeyword)
					{
						case (Keywords::BoxNormalisationFileKeyword):
							if (!loadBoxNormalisationFile(parser.argc(1))) error = true;
							break;
						case (Keywords::BoxNormalisationPointsKeyword):
							boxNormalisationPoints_ = parser.argi(1);
							break;
						case (Keywords::BraggKeyword):
							braggCalculationOn_ = parser.argb(1);
							break;
						case (Keywords::BraggBroadeningKeyword):
							braggBroadening_ = parser.argd(1);
							break;
						case (Keywords::BraggMaximumQKeyword):
							braggMaximumQ_ = parser.argd(1);
							break;
						case (Keywords::BroadeningKeyword):
							qDependentFWHM_ = parser.argd(1);
							qIndependentFWHM_ = parser.argd(2);
							break;
						case (Keywords::EndSetupKeyword):
							msg.print("Found end of %s block.\n", Keywords::inputBlock(block));
							blockDone = true;
							break;
						case (Keywords::QDeltaKeyword):
							qDelta_ = parser.argd(1);
							break;
						case (Keywords::QMaxKeyword):
							qMax_ = parser.argd(1);
							break;
						case (Keywords::RDFBinWidthKeyword):
							rdfBinWidth_ = parser.argd(1);
							break;
						case (Keywords::RDFMethodKeyword):
							if (rdfMethod(parser.argc(1)) == DUQ::nRDFMethods)
							{
								msg.error("'%s' is not a valid calculation method.\n", parser.argc(1));
								error = true;
							}
							else rdfMethod_ = rdfMethod(parser.argc(1));
							break;
						case (Keywords::RDFRangeKeyword):
							requestedRDFRange_ = parser.argd(1);
							break;
						case (Keywords::RDFSmoothingKeyword):
							rdfSmoothing_ = parser.argi(1);
							break;
						case (Keywords::SeedKeyword):
							seed_ = parser.argi(1);
							break;
						case (Keywords::SimplexNCyclesKeyword):
							simplexNCycles_ = parser.argi(1);
							msg.print("--> Maximum number of cycles in Simplex minimiser set to %i\n", simplexNCycles_);
							break;
						case (Keywords::SimplexNMovesKeyword):
							simplexNMoves_ = parser.argi(1);
							msg.print("--> Maximum number of moves per cycle in Simplex minimiser set to %i\n", simplexNMoves_);
							break;
						case (Keywords::SimplexTemperatureKeyword):
							simplexTemperature_ = parser.argd(1);
							break;
						case (Keywords::SimplexToleranceKeyword):
							simplexTolerance_ = parser.argd(1);
							break;
						case (Keywords::TemperatureKeyword):
							temperature_ = parser.argd(1);
							break;
						case (Keywords::WindowFunctionKeyword):
							if (Data2D::windowFunction(parser.argc(1)) == Data2D::nWindowFunctions)
							{
								msg.error("Unrecognised window function = '%s'.\n", parser.argc(1));
								error = true;
							}
							else windowFunction_ = Data2D::windowFunction(parser.argc(1));
							break;
						case (Keywords::nSetupKeywords):
							msg.error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(block), parser.argc(0));
							Keywords::printValidKeywords(block);
							error = true;
							break;
						default:
							printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(block), Keywords::setupKeyword(setkeyword));
							error = true;
							break;
					}
					
					// Error encountered?
					if (error) break;
					
					// End of block?
					if (blockDone) break;
				}
				break;
			case (Keywords::SimulationBlock):
				msg.print("Found %s block\n", Keywords::inputBlock(block));
				blockDone = false;
				while (!parser.eofOrBlank())
				{
					// Read in a line, which should contain a keyword and a minimum number of arguments
					parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
					simkeyword = Keywords::simulationKeyword(parser.argc(0));
					if ((simkeyword != Keywords::nSimulationKeywords) && ((parser.nArgs()-1) < Keywords::simulationBlockNArguments(simkeyword)))
					{
						msg.error("Not enough arguments given to '%s' keyword.\n", Keywords::simulationKeyword(simkeyword));
						error = true;
						break;
					}
					switch (simkeyword)
					{
						case (Keywords::EndSimulationKeyword):
							msg.print("Found end of %s block.\n", Keywords::inputBlock(block));
							blockDone = true;
							break;
						case (Keywords::EquilibrationKeyword):
							targetSteps = &equilibrationSteps_;
							break;
						case (Keywords::ShakeKeyword):
							targetSteps = &shakeSteps_;
							break;
						case (Keywords::StrategyKeyword):
							targetSteps = &strategySteps_;
							break;
						case (Keywords::nSimulationKeywords):
							// Check for a valid target Step List
							if (targetSteps == NULL)
							{
								msg.error("Simulation commands must be added to a specific stage - select one with an '@' keyword first.\n");
								error = true;
								break;
							}
							
							// Add a new Step and attempt to parse the line into a Command
							step = targetSteps->add();
							if (!parseStep(step, parser.line()))
							{
								msg.error("Failed to add simulation step.\n");
								error = true;
							}
							break;
						default:
							printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(block), Keywords::simulationKeyword(simkeyword));
							error = true;
							break;
					}
					
					// Error encountered?
					if (error) break;
					
					// End of block?
					if (blockDone) break;
				}
				break;
			case (Keywords::nInputBlocks):
				msg.error("Unrecognised input block found - '%s'\n", parser.argc(0));
				error = true;
				break;
			default:
				printf("DEV_OOPS - Input block keyword '%s' not accounted for.\n", Keywords::inputBlock(block));
				error = true;
				break;
		}
		
		// Error encountered?
		if (error) break;
	}
	
	if (!error) msg.print("Finished reading input file.\n");

	// Update necessary objects
	fileName_ = fileName;
	regeneratePairPotentials();

	// Error encountered?
	if (error)
	{
		msg.print("Errors encountered while loading input file.\nLoad aborted.\n");
		clear();
	}
	
	// Done
	parser.closeFiles();
	return (!error);
}

/*!
 * \brief Save input file
 */
bool DUQ::saveInput(const char* fileName)
{
	// Open file and check that we're OK to proceed writing to it
	LineParser parser;
	msg.print("Writing input file '%s'...\n", fileName);

	parser.openOutput(fileName, true);
	if (!parser.isFileGoodForWriting())
	{
		msg.error("Couldn't open file '%s' for writing.\n", fileName);
		return false;
	}
	
	int count;

	// Write title comment
	parser.writeLineF("# File: '%s'\n", fileName);
// 	parser.writeLineF("# Saved at %s on %s\n\n", qPrintable(QTime::currentTime().toString()), qPrintable(QDate::currentDate().toString()));
	// TODO Need standard C version of date and time routines.
	
	// Write AtomTypes block
	parser.writeLineF("# AtomType Definitions\n");
	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::AtomTypesBlock));
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next)
	{
		parser.writeLineF("  %s  '%s'  %3s  '%s'\n", Keywords::atomTypesKeyword(Keywords::AtomTypeKeyword), at->name(), periodicTable.element(at->element()).symbol(), at->parameters()->name());
	}
	parser.writeLineF("%s\n\n", Keywords::atomTypesKeyword(Keywords::EndAtomTypesKeyword));

	// Write Species data to File
	parser.writeLineF("# Species Definitions\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		parser.writeLineF("%s '%s'\n", Keywords::inputBlock(Keywords::SpeciesBlock), sp->name());
		
		// Atoms
		parser.writeLineF("  # Atoms\n");
		count = 0;
		for (SpeciesAtom* i = sp->atoms(); i != NULL; i = i->next)
		{
			++count;
			parser.writeLineF("  %s  %3i  %3s  %8.3f %8.3f %8.3f %8.3f '%s'\n", Keywords::speciesKeyword(Keywords::AtomKeyword), count, periodicTable.element(i->element()).symbol(), i->r().x, i->r().y, i->r().z, i->charge(), i->atomType() == NULL ? "???" : i->atomType()->name());
		}
		
		// Bonds
		parser.writeLineF("\n  # Bonds\n");
		for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
		{
			parser.writeLineF("  %s  %3i  %3i  %8.3f %8.3f\n", Keywords::speciesKeyword(Keywords::BondKeyword), b->indexI()+1, b->indexJ()+1, b->equilibrium(), b->forceConstant());
		}
		
		// Angles
		parser.writeLineF("\n  # Angles\n");
		for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
		{
			parser.writeLineF("  %s  %3i  %3i  %3i  %8.3f %8.3f\n", Keywords::speciesKeyword(Keywords::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->equilibrium(), a->forceConstant());
		}
		
		// Grains
		parser.writeLineF("\n  # Grain Definitions\n");
		for (SpeciesGrain* sg = sp->grains(); sg != NULL; sg = sg->next)
		{
			parser.writeLineF("  %s  '%s'", Keywords::speciesKeyword(Keywords::GrainKeyword), sg->name());
			for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next) parser.writeLineF("  %i", ri->item->userIndex());
			parser.writeLineF("\n");
		}

		// Isotopologues
		parser.writeLineF("\n  # Isotopologues\n");
		for (Isotopologue* iso = sp->isotopologues(); iso != NULL; iso = iso->next)
		{
			parser.writeLineF("  %s  '%s'", Keywords::speciesKeyword(Keywords::IsotopologueKeyword), iso->name());
			for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
			{
				parser.writeLineF("  %s=%i", ri->item->name(), ri->data->A());
			}
			parser.writeLineF("\n");
		}
		
		// Done with this species
		parser.writeLineF("%s\n\n", Keywords::speciesKeyword(Keywords::EndSpeciesKeyword));
	}

	// Write System block
	parser.writeLineF("# System Composition\n");
	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::SystemBlock));
	parser.writeLineF("  %s  %i\n", Keywords::systemKeyword(Keywords::MultiplierKeyword), multiplier_);
	parser.writeLineF("  %s  %f  %s\n", Keywords::systemKeyword(Keywords::DensityKeyword), density_, densityIsAtomic_ ? "atoms/A3" : "g/cm3");
	parser.writeLineF("  %s  %f  %f  %f\n", Keywords::systemKeyword(Keywords::CellLengthsKeyword), relativeBoxLengths_.x, relativeBoxLengths_.y, relativeBoxLengths_.z);
	parser.writeLineF("  %s  %f  %f  %f\n", Keywords::systemKeyword(Keywords::CellAnglesKeyword), boxAngles_.x, boxAngles_.y, boxAngles_.z);
	if (nonPeriodic_) parser.writeLineF("  %s\n", Keywords::systemKeyword(Keywords::NonPeriodicKeyword));
	double sum = totalRelative();
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
// 		parser.writeLineF("  %s  '%s'  %f  %f  %i\n", Keywords::systemKeyword(Keywords::ComponentKeyword), sp->name(), sp->relativePopulation(), sp->relativePopulation()/sum, sp->relativePopulation()*multiplier_);
		parser.writeLineF("  %s  '%s'  %f\n", Keywords::systemKeyword(Keywords::ComponentKeyword), sp->name(), sp->relativePopulation());
	}
	parser.writeLineF("%s\n\n", Keywords::systemKeyword(Keywords::EndSystemKeyword));

	// Write Samples blocks
	parser.writeLineF("# Samples\n");
	for (Sample* s = samples_.first(); s != NULL; s = s->next)
	{
		parser.writeLineF("%s  '%s'\n", Keywords::inputBlock(Keywords::SampleBlock), s->name()); 
		for (IsotopologueMix *iso = s->isotopologueMixtures(); iso != NULL; iso = iso->next)
		{
			double sum = iso->totalRelative();
			for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
			{
				parser.writeLineF("  %s  '%s'  '%s'  %f\n", Keywords::sampleKeyword(Keywords::IsotopologueSampleKeyword), iso->species()->name(), ri->item->name(), ri->data);
			}
		}
		// Reference data present?
		if (!s->referenceDataFileName().isEmpty())
		{
			parser.writeLineF("  %s '%s'\n", Keywords::sampleKeyword(Keywords::ReferenceDataKeyword), s->referenceDataFileName().get());
			if (s->referenceDataNormalisation() == Sample::AverageSquaredNormalisation) parser.writeLineF("  %s\n", Keywords::sampleKeyword(Keywords::NormalisedToAverageSquaredKeyword));
			else if (s->referenceDataNormalisation() == Sample::SquaredAverageNormalisation) parser.writeLineF("  %s\n", Keywords::sampleKeyword(Keywords::NormalisedToSquaredAverageKeyword));
			if (s->referenceSubtractSelf()) parser.writeLineF("  %s\n", Keywords::sampleKeyword(Keywords::SubtractSelfKeyword));
		}
		parser.writeLineF("%s\n\n", Keywords::sampleKeyword(Keywords::EndSampleKeyword));
	}
	
	// Write PairPotentials block
	parser.writeLineF("# Pair Potentials\n");
	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::PairPotentialsBlock));
	parser.writeLineF("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::RangeKeyword), pairPotentialRange_);
	parser.writeLineF("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::DeltaKeyword), pairPotentialDelta_);
	parser.writeLineF("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::TruncationWidthKeyword), pairPotentialTruncationWidth_);
	for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
	{
		if (pot->type() == PairPotential::CoulombType) parser.writeLineF("  %s  '%s'  '%s'  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->chargeI(), pot->chargeJ());
		else if (pot->type() == PairPotential::DispersionType) parser.writeLineF("  %s  '%s'  '%s'  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->sigmaIJ(), pot->epsilonIJ());
		else parser.writeLineF("  %s  '%s'  '%s'  %f  %f  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->sigmaIJ(), pot->epsilonIJ(), pot->chargeI(), pot->chargeJ());
	}
	parser.writeLineF("%s\n\n", Keywords::pairPotentialsKeyword(Keywords::EndPairPotentialsKeyword));

	// Write Setup block
	parser.writeLineF("# Setup\n");
	//parser.writeLineF("  %s  '%s'\n", Keywords::setupKeyword(Keywords::BoxNormalisationFileKeyword), boxNormalisationFile_);
	parser.writeLineF("  %s  %i\n", Keywords::setupKeyword(Keywords::BoxNormalisationPointsKeyword), boxNormalisationPoints_);
	parser.writeLineF("  %s  %s\n", Keywords::setupKeyword(Keywords::BraggKeyword), braggCalculationOn_ ? "on" : "off");
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::BraggBroadeningKeyword), braggBroadening_);
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::BraggMaximumQKeyword), braggMaximumQ_);
	parser.writeLineF("  %s  %f  %f\n", Keywords::setupKeyword(Keywords::BroadeningKeyword), qDependentFWHM_, qIndependentFWHM_);
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::QDeltaKeyword), qDelta_);
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::QMaxKeyword), qMax_);
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::RDFBinWidthKeyword), rdfBinWidth_);
	parser.writeLineF("  %s  %s\n", Keywords::setupKeyword(Keywords::RDFMethodKeyword), rdfMethod(rdfMethod_));
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::RDFRangeKeyword), rdfRange_);
	parser.writeLineF("  %s  %i\n", Keywords::setupKeyword(Keywords::RDFSmoothingKeyword), rdfSmoothing_);
	parser.writeLineF("  %s  %i\n", Keywords::setupKeyword(Keywords::SeedKeyword), seed_);
	parser.writeLineF("  %s  %i\n", Keywords::setupKeyword(Keywords::SimplexNCyclesKeyword), simplexNCycles_);
	parser.writeLineF("  %s  %i\n", Keywords::setupKeyword(Keywords::SimplexNMovesKeyword), simplexNMoves_);
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::SimplexTemperatureKeyword), simplexTemperature_);
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::SimplexToleranceKeyword), simplexTolerance_);
	parser.writeLineF("  %s  %f\n", Keywords::setupKeyword(Keywords::TemperatureKeyword), temperature_);
	parser.writeLineF("  %s  %s\n", Keywords::setupKeyword(Keywords::WindowFunctionKeyword), Data2D::windowFunction(windowFunction_));
	parser.writeLineF("%s\n\n", Keywords::setupKeyword(Keywords::EndSetupKeyword));

	// Write Simulation block
	parser.writeLineF("# Simulation\n");
	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::SimulationBlock));
	// -- Equilibration Steps
	if (equilibrationSteps_.nItems() > 0)
	{
		parser.writeLineF("\n%s\n", Keywords::SimulationKeyword(Keywords::EquilibrationKeyword));
		for (Step* step = equilibrationSteps_.first(); step != NULL; step = step->next)
		{
			if (step->iterations() == 1) parser.writeLineF("%s", step->command()->name());
			else parser.writeLineF("%i %s", step->iterations(), step->command()->name());
			for (RefListItem<Argument,Dnchar>* argRef = step->argumentValues(); argRef != NULL; argRef = argRef->next)
			{
				parser.writeLineF(" %s=%s", argRef->item->name(), argRef->data.get());
			}
			parser.writeLineF("\n");
		}
	}
	// -- Shake Steps
	if (shakeSteps_.nItems() > 0)
	{
		parser.writeLineF("\n%s\n", Keywords::SimulationKeyword(Keywords::ShakeKeyword));
		for (Step* step = shakeSteps_.first(); step != NULL; step = step->next)
		{
			if (step->iterations() == 1) parser.writeLineF("%s", step->command()->name());
			else parser.writeLineF("%i %s", step->iterations(), step->command()->name());
			for (RefListItem<Argument,Dnchar>* argRef = step->argumentValues(); argRef != NULL; argRef = argRef->next)
			{
				parser.writeLineF(" %s=%s", argRef->item->name(), argRef->data.get());
			}
			parser.writeLineF("\n");
		}
	}
	// -- Strategy Steps
	if (strategySteps_.nItems() > 0)
	{
		parser.writeLineF("\n%s\n", Keywords::SimulationKeyword(Keywords::StrategyKeyword));
		for (Step* step = strategySteps_.first(); step != NULL; step = step->next)
		{
			if (step->iterations() == 1) parser.writeLineF("%s", step->command()->name());
			else parser.writeLineF("%i %s", step->iterations(), step->command()->name());
			for (RefListItem<Argument,Dnchar>* argRef = step->argumentValues(); argRef != NULL; argRef = argRef->next)
			{
				parser.writeLineF(" %s=%s", argRef->item->name(), argRef->data.get());
			}
			parser.writeLineF("\n");
		}
	}
	parser.writeLineF("%s\n\n", Keywords::simulationKeyword(Keywords::EndSimulationKeyword));

	msg.print("Finished writing input file.\n");

	// Close file
	parser.closeFiles();

	// Update logpoint
	fileName_ = fileName;
	Flags::clearAll();

	return true;
}

/*!
 * \brief Return whether a fileName has been set
 */
bool DUQ::hasFileName() const
{
	return (!fileName_.isEmpty());
}

/*!
 * \brief Return filename of current input file
 */
const char *DUQ::fileName() const
{
	return fileName_.get();
}
