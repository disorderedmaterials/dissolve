/*
	*** Keyword Definitions
	*** src/main/keywords.h
	Copyright T. Youngs 2012-2016

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
/* None */

/*
 * \brief Keyword Definition
 * \details All allowable block keywords for the main dUQ input file.
 */
class Keywords
{
	/*
	 * Input Block Keywords
	 */
	public:
	// Input File Block Keyword Enum
	enum InputBlock
	{
		AtomTypesBlock,			/* 'AtomTypes' - Contains definitions of AtomTypes over all Species */
		ConfigurationBlock,		/* 'Configuration' - Defines a single configuration for use in the simulation */
		PairPotentialsBlock,		/* 'PairPotentials' - Contains definitions of the PairPotentials for the simulation */
		SampleBlock,			/* 'Sample' - Begins a definition of a Sample */
		SpeciesBlock,			/* 'Species' - Begins a definition of a Species */
		SimulationBlock,		/* 'Simulation' - Setting of simulation variables affecting the calculation */
		nInputBlocks			/* Number of defined InputBlock keywords */
	};
	// Convert text string to InputBlock
	static InputBlock inputBlock(const char* s);
	// Convert InputBlock to text string
	static const char* inputBlock(InputBlock id);
	// Print list of valid keywords for InputBlock specified
	static void printValidKeywords(Keywords::InputBlock block);


	/*
	 * AtomTypes Block Keywords
	 */
	public:
	// AtomTypes Block Keyword Enum
	enum AtomTypesKeyword
	{
		AtomTypeKeyword,		/* 'AtomType' - Specifies an AtomType definition  */
		EndAtomTypesKeyword,		/* 'EndAtomTypes' - Signals the end of the AtomTypes block */
		nAtomTypesKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to AtomTypesKeyword
	static AtomTypesKeyword atomTypesKeyword(const char* s);
	// Convert AtomTypesKeyword to text string
	static const char* atomTypesKeyword(AtomTypesKeyword id);
	// Return minimum number of expected arguments
	static int atomTypesBlockNArguments(AtomTypesKeyword id);


	/*
	 * Configuration Block Keywords
	 */
	public:
	// Configuration Block Keyword Enum
	enum ConfigurationKeyword
	{
		BoxNormalisationFileKeyword,	/* 'BoxNormalisationFile' - Specifies a file from which to load the RDF normalisation array */
		BraggKeyword,			/* 'Bragg' - Turns on (or explicitly off) calculation of Bragg scattering */
		BraggBroadeningKeyword,		/* 'BraggBroadening' - Specifies the degree of broadening applied to calculated Bragg features */
		BraggMaximumQKeyword,		/* 'BraggMaximumQ' - Sets the maximum Q value for Bragg calculation */
		CellAnglesKeyword,		/* 'CellAngles' - Gives the angles of the unit cell */
		CellLengthsKeyword,		/* 'CellLengths' - Gives the relative lengths of the unit cell */
		DensityKeyword,			/* 'Density' - Specifies the density of the simulation, along with its units */
		EndConfigurationKeyword,	/* 'EndConfiguration' - Signals the end of the Configuration block */
		FileModelKeyword,		/* 'File' - Specifies the file which contains the starting/current coordinates */
		MultiplierKeyword,		/* 'Multiplier' - Specifies the factor by which relative populations are multiplied when generating the Model */
		NonPeriodicKeyword,		/* 'NonPeriodic' - States that the simulation should be treated as non-periodic */
		QDeltaKeyword,			/* 'QDelta' - Q delta to use in S(Q) calculation */
		QMaxKeyword,			/* 'QMax' - Maximum Q up to which S(Q) calculation will be performed */
		RDFBinWidthKeyword,		/* 'RDFBinWidth' - Specified bin width for all RDF generation */
		RDFMethodKeyword,		/* 'RDFMethod' - Chooses the RDF calculation method to use */
		RDFRangeKeyword,		/* 'RDFRange' - Requested extent for RDF (and subsequent S(Q)) calculation */
		RDFSmoothingKeyword,		/* 'RDFSmoothing' - Specifies the degree of smoothing 'n' to apply to calculated RDFs, where 2n+1 controls the length in the applied Spline smooth */
		RMSEDeltaQKeyword,		/* 'RMSEDeltaQ' - Controls bin width used in Sample/reference data RMSE calculation */
		SampleAddKeyword,		/* 'Sample' - Specifies that the Sample data is relevant to this Configuration, and should be calculated / compared */
		SpeciesAddKeyword,		/* 'Species' - Specifies a Species and its relative population to add to this Configuration */
		TemperatureKeyword,		/* 'Temperature' - Defines the temperature of the simulation */
		WindowFunctionKeyword,		/* 'WindowFunction' - Specifies the windowing function to use in all Fourier transforms */
		nConfigurationKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to ConfigurationKeyword
	static ConfigurationKeyword configurationKeyword(const char* s);
	// Convert ConfigurationKeyword to text string
	static const char* configurationKeyword(ConfigurationKeyword id);
	// Return minimum number of expected arguments
	static int configurationBlockNArguments(ConfigurationKeyword id);


	/*
	 * PairPotential Block Keywords
	 */
	public:
	// PairPotential Block Keyword Enum
	enum PairPotentialsKeyword
	{
		CoulombKeyword,			/* 'Coulomb' - Specifies a PairPotential with Coulomb contributions only */
		DeltaKeyword,			/* 'Delta' - Gives the spacing between points in the tabulated potentials */
		DispersionKeyword,		/* 'Dispersion' - Specifies a PairPotential with LJ contributions only */
		EndPairPotentialsKeyword,	/* 'EndPairPotentials' - Signals the end of the PairPotentials block */
		FullKeyword,			/* 'Full' - Specifies a full PairPotential with LJ and charge contributions */
		RangeKeyword,			/* 'Range' - Specifies the total range (inc. truncation width) over which to generate potentials */
		TruncationWidthKeyword,		/* 'TruncationWidth' - Width of potential tail over which to reduce to zero */
		nPairPotentialsKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to PairPotentialKeyword
	static PairPotentialsKeyword pairPotentialsKeyword(const char* s);
	// Convert PairPotentialsKeyword to text string
	static const char* pairPotentialsKeyword(PairPotentialsKeyword id);
	// Return minimum number of expected arguments
	static int pairPotentialsBlockNArguments(PairPotentialsKeyword id);


	/*
	 * Sample Block Keywords
	 */
	public:
	// Sample Block Keyword Enum
	enum SampleKeyword
	{
		BroadeningKeyword,		/* 'Broadening' - Specifies the Q-independent and Q-dependent (in that order) Gaussian broadening function FWHM to apply when transforming simulated RDF data to Q-space */
		EndSampleKeyword,			/* 'EndSample' - Signals the end of the Sample block */
		FitMaxKeyword,				/* 'FitMax' - Upper limit for empirical fitting procedure for this Sample */
		FitMinKeyword,				/* 'FitMin' - Lower limit for empirical fitting procedure for this Sample */
		IsotopologueSampleKeyword,		/* 'Isotopologue' - Specifies a Species, Isotopologue, and relative population in this Sample */
		NormalisedToAverageSquaredKeyword,	/* 'NormalisedToAvSq' - States that the reference F(Q) has been normalised to < b >**2 */
		NormalisedToSquaredAverageKeyword,	/* 'NormalisedToSqAv' - States that the reference F(Q) has been normalised to < b**2 > */
		ReferenceDataKeyword,			/* 'ReferenceData' - Specifies an F(Q) datafile which represents this Sample */
		SubtractSelfKeyword,			/* 'SubtractSelf' - States that the self scattering (determined from the high-Q region) should be subtracted from the reference data */
		SampleTypeKeyword,			/* 'Type' - Specifies the type of sample data */
		nSampleKeywords				/* Number of keywords defined for this block */
	};
	// Convert text string to SampleKeyword
	static SampleKeyword sampleKeyword(const char* s);
	// Convert SampleKeyword to text string
	static const char* sampleKeyword(SampleKeyword id);
	// Return minimum number of expected arguments
	static int sampleBlockNArguments(SampleKeyword id);


	/*
	 * Simulation Block Keywords
	 */
	public:
	// Simulation Block Keyword Enum
	enum SimulationKeyword
	{
		BoxNormalisationPointsKeyword,	/* 'BoxNormalisationPoints' - Specifies how many random insertions to use when generating the normalisation array */
		EndSimulationKeyword,		/* 'EndSimulation' - Signals the end of the Simulation block */
		SeedKeyword,			/* 'Seed' - Sets the random seed to use */
		SimplexNCyclesKeyword,		/* 'SimplexNCycles' - Sets the maximum number of cycles allowed by the Simplex minimiser when fitting potential coefficients */
		SimplexNMovesKeyword,		/* 'SimplexNMoves' - Sets the maximum number of moves per cycle allowed by the Simplex minimiser when fitting potential coefficients */
		SimplexTemperatureKeyword,	/* 'SimplexTemperature' - Sets the temperature used in the Simplex minimiser when fitting potential coefficients */
		SimplexToleranceKeyword,	/* 'SimplexTolerance' - Sets the convergence tolerance used in the Simplex minimiser when fitting potential coefficients */
		nSimulationKeywords			/* Number of keywords defined for this block */
	};
	// Convert text string to SimulationKeyword
	static SimulationKeyword simulationKeyword(const char* s);
	// Convert SimulationKeyword to text string
	static const char* simulationKeyword(SimulationKeyword id);
	// Return minimum number of expected arguments
	static int simulationBlockNArguments(SimulationKeyword id);


	/*
	 * Species Block Keywords
	 */
	public:
	// Species Block Keyword Enum
	enum SpeciesKeyword
	{
		AngleKeyword,			/* 'Angle' - Defines an Angle joining three atoms */
		AtomKeyword,			/* 'Atom' - Specifies an Atom in the Species */
		BondKeyword,			/* 'Bond' - Defines a Bond joining two atoms */
		EndSpeciesKeyword,		/* 'EndSpecies' - Signals the end of the current Species */
		GrainKeyword,			/* 'Grain' - Defines a Grain containing a number of Atoms */
		IsotopologueKeyword,		/* 'Isotopologue' - Defines isotope information for the Atoms */
		nSpeciesKeywords		/* Number of keywords defined for this block */
	};
	// Convert text string to SpeciesKeyword
	static SpeciesKeyword speciesKeyword(const char* s);
	// Convert SpeciesKeyword to text string
	static const char* speciesKeyword(SpeciesKeyword id);
	// Return minimum number of expected arguments
	static int speciesBlockNArguments(SpeciesKeyword id);
};

#endif
