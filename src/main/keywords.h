/*
	*** Keyword Definitions
	*** src/main/keywords.h
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

#ifndef DUQ_KEYWORDS_H
#define DUQ_KEYWORDS_H

// Forward Declarations
class LineParser;
class DUQ;
class Sample;
class Configuration;
class Species;
class Module;
class GenericList;
class Data;
class SpeciesInfo;

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
 * Main Keyword Blocks
 */
namespace InputBlocks
{
	/*
	 * Input Block Keywords
	 */
	// Input File Block Keyword Enum
	enum InputBlock
	{
		ConfigurationBlock,		/* 'Configuration' - Defines a single Configuration for use in the simulation */
		DataBlock,			/* 'Data' - Links a reference dataset to a calculated value */
		ModuleBlock,			/* 'Module' - Sets up a Module to run after Configuration processing */
		PairPotentialsBlock,		/* 'PairPotentials' - Contains definitions of the PairPotentials for the simulation */
		SimulationBlock,		/* 'Simulation' - Setting of simulation variables affecting the calculation */
		SpeciesBlock,			/* 'Species' - Begins a definition of a Species */
		SpeciesInfoBlock,		/* 'SpeciesInfo' - Defines a Species for inclusion into a Configuration */
		nInputBlocks			/* Number of defined InputBlock keywords */
	};
	// Convert text string to InputBlock
	InputBlock inputBlock(const char* s);
	// Convert InputBlock to text string
	const char* inputBlock(InputBlock id);
	// Print list of valid keywords for InputBlock specified
	void printValidKeywords(InputBlock block);
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
		OutputCoordinatesKeyword,	/* 'OutputCoordinates' - Specifies the file which should contain output coordinates */
		RDFBinWidthKeyword,		/* 'RDFBinWidth' - Specified bin width for all RDF generation */
		RDFRangeKeyword,		/* 'RDFRange' - Requested extent for RDF calculation */
		SpeciesInfoKeyword,		/* 'SpeciesInfo' - Specifies a Species to add to this Configuration */
		TemperatureKeyword,		/* 'Temperature' - Defines the temperature of the simulation */
		UseOutputAsInputKeyword,	/* 'UseOutputAsInput' - "Use output coordinates file as input (if it exists) */
		nConfigurationKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to ConfigurationKeyword
	ConfigurationKeyword keyword(const char* s);
	// Convert ConfigurationKeyword to text string
	const char* keyword(ConfigurationKeyword id);
	// Return expected number of expected arguments
	int nArguments(ConfigurationKeyword id);
	// Parse Configuration block
	bool parse(LineParser& parser, DUQ* duq, Configuration* cfg);
};


/*
 * Data Block Keywords
 */
namespace DataBlock
{
	// Data Block Keyword Enum
	enum DataKeyword
	{
		AssociatedToKeyword,		/* 'AssociatedTo' - Name of Module to which this data is associated, if any */
		EndDataKeyword,			/* 'EndData' - Signals the end of the Data block */
		FileKeyword,			/* 'File' - Datafile containing reference data */
		NeutronNormalisationKeyword,	/* 'NeutronNormalisation' - Normalisation to apply to reference neutron data */
		SubtractAverageLevelKeyword,	/* 'SubtractAverageLevel' - Minimum x value from which to calculate and subtract mean value from y data */
		TypeKeyword,			/* 'Type' - Type of the supplied reference data */
		nDataKeywords			/* Number of keywords defined for this block */
	};
	// Convert text string to DataKeyword
	DataKeyword keyword(const char* s);
	// Convert DataKeyword to text string
	const char* keyword(DataKeyword id);
	// Return expected number of expected arguments
	int nArguments(DataKeyword id);
	// Parse Data block
	bool parse(LineParser& parser, DUQ* duq, Data* data);
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
		DataKeyword,			/* 'Data' - Define / link Data that we are targetting for use by this Module */
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
	bool parse(LineParser& parser, DUQ* duq, Module* module, GenericList& targetList, bool moduleInConfiguration);
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
		ParametersKeyword,		/* 'Parameters' - Sets or re-sets the LJ and charge parameters for a specific AtomType */
		RangeKeyword,			/* 'Range' - Specifies the total range (inc. truncation width) over which to generate potentials */
		TruncationWidthKeyword,		/* 'TruncationWidth' - Width of potential tail over which to reduce LJ term to zero */
		nPairPotentialsKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to PairPotentialKeyword
	PairPotentialsKeyword keyword(const char* s);
	// Convert PairPotentialsKeyword to text string
	const char* keyword(PairPotentialsKeyword id);
	// Return expected number of expected arguments
	int nArguments(PairPotentialsKeyword id);
	// Parse PairPotentials block
	bool parse(LineParser& parser, DUQ* duq);
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
		MaxIterationsKeyword,		/* 'MaxIterations' - Maximum number of main loop iterations to perform, or -1 for no limit */
		ParallelStrategyKeyword,	/* 'ParallelStrategy' - Determines the distribution of processes across Configurations */
		SeedKeyword,			/* 'Seed' - Random seed to use */
		WindowFunctionKeyword,		/* 'WindowFunction' - Window function to use in all Fourier transforms */
		WriteFrequencyKeyword,		/* 'WriteFrequency' - Write various data at the end of each iteration */
		nSimulationKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to SimulationKeyword
	SimulationKeyword keyword(const char* s);
	// Convert SimulationKeyword to text string
	const char* keyword(SimulationKeyword id);
	// Return expected number of expected arguments
	int nArguments(SimulationKeyword id);
	// Parse Simulation block
	bool parse(LineParser& parser, DUQ* duq);
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
		MasterAngleKeyword,		/* 'MasterAngle' - Define master Angle parameters that can be referred to */
		MasterBondKeyword,		/* 'MasterBond' - Define master Bond parameters that can be referred to */
		MasterTorsionKeyword,		/* 'MasterTorsion' - Define master Torsion parameters that can be referred to */
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
	bool parse(LineParser& parser, DUQ* duq, Species* species);
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
	bool parse(LineParser& parser, DUQ* duq, SpeciesInfo* speciesInfo);
};

#endif
