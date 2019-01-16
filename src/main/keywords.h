/*
	*** Keyword Definitions
	*** src/main/keywords.h
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

#ifndef DISSOLVE_KEYWORDS_H
#define DISSOLVE_KEYWORDS_H

// Forward Declarations
class LineParser;
class Dissolve;
class Sample;
class Configuration;
class Species;
class Module;
class GenericList;
class Data;
class SpeciesInfo;
class SpeciesSite;

// Keyword Data
class KeywordData
{
	public:
	// Keyword name
	const char* name;
	// Number of arguments expected by keyword
	int nArguments;
	// Argument description
	const char* argumentDescription;
};

/*
 * Block Keywords
 */
namespace BlockKeywords
{
	/*
	 * Block Keywords
	 */
	// Block Keyword Enum
	enum BlockKeyword
	{
		ConfigurationBlockKeyword,		/* 'Configuration' - Defines a single Configuration for use in the simulation */
		MasterBlockKeyword,			/* 'Master' - Contains master intramolecular terms for use in Species */
		ModuleBlockKeyword,			/* 'Module' - Sets up a Module to run after Configuration processing */
		PairPotentialsBlockKeyword,		/* 'PairPotentials' - Contains definitions of the PairPotentials for the simulation */
		SimulationBlockKeyword,			/* 'Simulation' - Setting of simulation variables affecting the calculation */
		SiteBlockKeyword,			/* 'Site' - Defines an analysis site within a Species */
		SpeciesBlockKeyword,			/* 'Species' - Begins a definition of a Species */
		SpeciesInfoBlockKeyword,		/* 'SpeciesInfo' - Defines a Species for inclusion into a Configuration */
		nBlockKeywords				/* Number of defined BlockKeyword keywords */
	};
	// Convert text string to BlockKeyword
	BlockKeyword blockKeyword(const char* s);
	// Convert BlockKeyword to text string
	const char* blockKeyword(BlockKeyword id);
	// Print list of valid keywords for the block specified
	void printValidKeywords(BlockKeyword block);
};


/*
 * Configuration Block Keywords
 */
namespace ConfigurationBlock
{
	// Configuration Block Keyword Enum
	enum ConfigurationKeyword
	{
		BoxNormalisationFileKeyword,	/* 'BoxNormalisationFile' - Specifies a file from which to load the RDF normalisation array */
		BraggMultiplicityKeyword,	/* 'BraggMultiplicity' - Multiplicity of a primitive cell within the unit cell */
		BraggQMaxKeyword,		/* 'BraggQMax' - Maximum Q value for Bragg calculation */
		BraggQMinKeyword,		/* 'BraggQMin' - Minimum Q value for Bragg calculation */
		CellAnglesKeyword,		/* 'CellAngles' - Gives the angles of the unit cell */
		CellDivisionLengthKeyword,	/* 'CellDivisionLength' - Set the requested side length for regions when partitioning the unit cell */
		CellLengthsKeyword,		/* 'CellLengths' - Gives the relative lengths of the unit cell */
		DensityKeyword,			/* 'Density' - Specifies the density of the simulation, along with its units */
		EndConfigurationKeyword,	/* 'EndConfiguration' - Signals the end of the Configuration block */
		EnsembleKeyword,		/* 'Ensemble' - Whether to append to an ensemble file */
		EnsembleFrequencyKeyword,	/* 'EnsembleFrequency' - Frequency with which to append to ensemble file */
		InputCoordinatesKeyword,	/* 'InputCoordinates' - Specifies the file which contains the starting coordinates */
		ModuleKeyword,			/* 'Module' - Starts the set up of a Module for this configuration */
		MultiplierKeyword,		/* 'Multiplier' - Specifies the factor by which relative populations are multiplied when generating the Configuration data */
		NonPeriodicKeyword,		/* 'NonPeriodic' - States that the simulation should be treated as non-periodic */
		RDFBinWidthKeyword,		/* 'RDFBinWidth' - Specified bin width for all RDF generation */
		RDFRangeKeyword,		/* 'RDFRange' - Requested extent for RDF calculation */
		SizeFactorKeyword,		/* 'SizeFactor' - Scaling factor for Box lengths, Cell size, and Molecule centres-of-geometry */
		SpeciesInfoKeyword,		/* 'SpeciesInfo' - Specifies a Species to add to this Configuration */
		TemperatureKeyword,		/* 'Temperature' - Defines the temperature of the simulation */
		nConfigurationKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to ConfigurationKeyword
	ConfigurationKeyword keyword(const char* s);
	// Convert ConfigurationKeyword to text string
	const char* keyword(ConfigurationKeyword id);
	// Return expected number of expected arguments
	int nArguments(ConfigurationKeyword id);
	// Parse Configuration block
	bool parse(LineParser& parser, Dissolve* dissolve, Configuration* cfg);
};


/*
 * Master Block Keywords
 */
namespace MasterBlock
{
	// Master Block Keyword Enum
	enum MasterKeyword
	{
		AngleKeyword,			/* 'Angle' - Define master Angle parameters that can be referred to */
		BondKeyword,			/* 'Bond' - Define master Bond parameters that can be referred to */
		EndMasterKeyword,		/* 'EndModule' - Signals the end of the Module block */
		TorsionKeyword,			/* 'Torsion' - Define master Torsion parameters that can be referred to */
		nMasterKeywords			/* Number of keywords defined for this block */
	};
	// Convert text string to MasterKeyword
	MasterKeyword keyword(const char* s);
	// Convert MasterKeyword to text string
	const char* keyword(MasterKeyword id);
	// Return expected number of expected arguments
	int nArguments(MasterKeyword id);
	// Parse Module block
	bool parse(LineParser& parser, Dissolve* dissolve);
};


/*
 * Module Block Keywords
 */
namespace ModuleBlock
{
	// Module Block Keyword Enum
	enum ModuleKeyword
	{
		ConfigurationKeyword,		/* 'Configuration' - Associates the specified Configuration to this Module */
		DisableKeyword,			/* 'Disable' - Disables the module, preventing it from running */
		EndModuleKeyword,		/* 'EndModule' - Signals the end of the Module block */
		FrequencyKeyword,		/* 'Frequency' - Frequency at which the Module is run */
		nModuleKeywords			/* Number of keywords defined for this block */
	};
	// Convert text string to ModuleKeyword
	ModuleKeyword keyword(const char* s);
	// Convert ModuleKeyword to text string
	const char* keyword(ModuleKeyword id);
	// Return expected number of expected arguments
	int nArguments(ModuleKeyword id);
	// Parse Module block
	bool parse(LineParser& parser, Dissolve* dissolve, Module* module, GenericList& targetList, bool moduleInConfiguration);
};


/*
 * PairPotential Block Keywords
 */
namespace PairPotentialsBlock
{
	// PairPotential Block Keyword Enum
	enum PairPotentialsKeyword
	{
		CoulombTruncationKeyword,	/* 'CoulombTruncation' - Truncation scheme to apply to Coulomb potential */
		DeltaKeyword,			/* 'Delta' - Gives the spacing between points in the tabulated potentials */
		EndPairPotentialsKeyword,	/* 'EndPairPotentials' - Signals the end of the PairPotentials block */
		GenerateKeyword,		/* 'Generate' - Generates a single PairPotential with the specified contributions */
		GenerateAllKeyword,		/* 'GenerateAll' - Generates all required PairPotentials with the specified contributions */
		IncludeCoulombKeyword,		/* 'IncludeCoulomb' - Include Coulomb term in tabulated pair potentials" */
		ParametersKeyword,		/* 'Parameters' - Sets or re-sets the short-range and charge parameters for a specific AtomType */
		RangeKeyword,			/* 'Range' - Specifies the total range (inc. truncation width) over which to generate potentials */
		ShortRangeTruncationKeyword,	/* 'ShortRangeTruncation' - Truncation scheme to apply to short-range potential */
		ShortRangeTruncationWidthKeyword,/* 'ShortRangeTruncationWidth' - Width of potential tail over which to reduce short-range term to zero */
		nPairPotentialsKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to PairPotentialKeyword
	PairPotentialsKeyword keyword(const char* s);
	// Convert PairPotentialsKeyword to text string
	const char* keyword(PairPotentialsKeyword id);
	// Return expected number of expected arguments
	int nArguments(PairPotentialsKeyword id);
	// Parse PairPotentials block
	bool parse(LineParser& parser, Dissolve* dissolve);
};


/*
 * Simulation Block Keywords
 */
namespace SimulationBlock
{
	// Simulation Block Keyword Enum
	enum SimulationKeyword
	{
		BoxNormalisationPointsKeyword,	/* 'BoxNormalisationPoints' - Number of random insertions to use when generating the normalisation array */
		EndSimulationKeyword,		/* 'EndSimulation' - Signals the end of the Simulation block */
		ParallelStrategyKeyword,	/* 'ParallelStrategy' - Determines the distribution of processes across Configurations */
		RestartFileFrequencyKeyword,	/* 'RestartFileFrequency' - Frequency at which to write restart file */
		SeedKeyword,			/* 'Seed' - Random seed to use */
		nSimulationKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to SimulationKeyword
	SimulationKeyword keyword(const char* s);
	// Convert SimulationKeyword to text string
	const char* keyword(SimulationKeyword id);
	// Return expected number of expected arguments
	int nArguments(SimulationKeyword id);
	// Parse Simulation block
	bool parse(LineParser& parser, Dissolve* dissolve);
};


/*
 * Site Block Keywords
 */
namespace SiteBlock
{
	// Site Block Keyword Enum
	enum SiteKeyword
	{
		EndSiteKeyword,			/* 'EndSite' - Signals the end of the Site */
		OriginKeyword,			/* 'Origin' - Set the atom indices whose average coordinates reflect the site origin */
		OriginMassWeightedKeyword,	/* 'OriginMassWeighted' - Control whether the origin should be calculated with mass-weighted coordinates */
		XAxisKeyword,			/* 'XAxis' - Define one or more atoms whose average coordinates reflect the direction of the x axis */
		YAxisKeyword,			/* 'YAxis' - Define one or more atoms whose average coordinates reflect the direction of the y axis */
		nSiteKeywords			/* Number of keywords defined for this block */
	};
	// Convert text string to SiteKeyword
	SiteKeyword keyword(const char* s);
	// Convert SiteKeyword to text string
	const char* keyword(SiteKeyword id);
	// Return expected number of expected arguments
	int nArguments(SiteKeyword id);
	// Parse Site block
	bool parse(LineParser& parser, Dissolve* dissolve, SpeciesSite* site);
};


/*
 * Species Block Keywords
 */
namespace SpeciesBlock
{
	// Species Block Keyword Enum
	enum SpeciesKeyword
	{
		AngleKeyword,			/* 'Angle' - Defines an Angle joining three atoms */
		AtomKeyword,			/* 'Atom' - Specifies an Atom in the Species */
		AutoAddGrainsKeyword,		/* 'AutoAddGrains' - Automatically add Grains to cover all atoms in the Species */
		BondKeyword,			/* 'Bond' - Defines a Bond joining two atoms */
		ChargeKeyword,			/* 'Charge' - Specifies the atomic charge for an individual atom */
		EndSpeciesKeyword,		/* 'EndSpecies' - Signals the end of the current Species */
		GrainKeyword,			/* 'Grain' - Defines a Grain containing a number of Atoms */
		IsotopologueKeyword,		/* 'Isotopologue' - Add an isotopologue to the Species */
		SiteKeyword,			/* 'Site' - Define an analysis site within the Species */
		TorsionKeyword,			/* 'Torsion' - Define a Torsion interaction between four atoms */
		nSpeciesKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to SpeciesKeyword
	SpeciesKeyword keyword(const char* s);
	// Convert SpeciesKeyword to text string
	const char* keyword(SpeciesKeyword id);
	// Return expected number of expected arguments
	int nArguments(SpeciesKeyword id);
	// Parse Species block
	bool parse(LineParser& parser, Dissolve* dissolve, Species* species);
};


/*
 * SpeciesInfo Block Keywords
 */
namespace SpeciesInfoBlock
{
	// SpeciesInfo Block Keyword Enum
	enum SpeciesInfoKeyword
	{
		EndSpeciesInfoKeyword,		/* 'EndSpeciesInfo' - Signals the end of the SpeciesInfo */
		NoRotationKeyword,		/* Flag that the Species should not be rotated when making a random configuration */
		NoTranslationKeyword,		/* Flag that the Species should not be translated when making a random configuration */
		PopulationKeyword,		/* Relative population of the Species */
		nSpeciesInfoKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to SpeciesInfoKeyword
	SpeciesInfoKeyword keyword(const char* s);
	// Convert SpeciesInfoKeyword to text string
	const char* keyword(SpeciesInfoKeyword id);
	// Return expected number of expected arguments
	int nArguments(SpeciesInfoKeyword id);
	// Parse SpeciesInfo block
	bool parse(LineParser& parser, Dissolve* dissolve, SpeciesInfo* speciesInfo);
};

#endif
