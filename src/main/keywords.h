/*
	*** Keyword Definitions
	*** src/lib/main/keywords.h
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

#ifndef DUQ_KEYWORDS_H
#define DUQ_KEYWORDS_H

// Forward Declarations
/* None */

/*!
 * \brief Keyword Definition
 * \details All allowable block keywords for the main dUQ input file.
 */
class Keywords
{
	/*!
	 * \name Input Block Keywords
	 */
	///@{
	public:
	/// Input File Block Keyword Enum
	enum InputBlock
	{
		AtomTypesBlock,			/**> 'AtomTypes' - Contains definitions of AtomTypes over all Species */
		PairPotentialsBlock,		/**> 'PairPotentials' - Contains definitions of the PairPotentials for the system */
		SampleBlock,			/**> 'Sample' - Begins a definition of a Sample */
		SetupBlock,			/**> 'Setup' - Setup of all control variables for the calculation */
		SimulationBlock,		/**> 'Simulation' - Defines a series of calculation steps to perform on the system */
		SpeciesBlock,			/**> 'Species' - Begins a definition of a Species */
		SystemBlock,			/**> 'System' - Defines the composition of the System, from defined Species */
		nInputBlocks			/**> Number of defined InputBlock keywords */
	};
	// Convert text string to InputBlock
	static InputBlock inputBlock(const char* s);
	// Convert InputBlock to text string
	static const char* inputBlock(InputBlock id);
	// Print list of valid keywords for InputBlock specified
	static void printValidKeywords(Keywords::InputBlock block);
	///@}


	/*!
	 * \name AtomTypes Block Keywords
	 */
	///@{
	public:
	/// AtomTypes Block Keyword Enum
	enum AtomTypesKeyword
	{
		AtomTypeKeyword,		/**> 'AtomType' - Specifies an AtomType definition  */
		EndAtomTypesKeyword,		/**> 'EndAtomTypes' - Signals the end of the AtomTypes block */
		nAtomTypesKeywords		/**> Number of keywords defined for this block */
	};
	// Convert text string to AtomTypesKeyword
	static AtomTypesKeyword atomTypesKeyword(const char* s);
	// Convert AtomTypesKeyword to text string
	static const char* atomTypesKeyword(AtomTypesKeyword id);
	// Return minimum number of expected arguments
	static int atomTypesBlockNArguments(AtomTypesKeyword id);
	///@}


	/*!
	 * \name Species Block Keywords
	 */
	///@{
	public:
	/// Species Block Keyword Enum
	enum SpeciesKeyword
	{
		AngleKeyword,			/**> 'Angle' - Defines an Angle joining three atoms */
		AtomKeyword,			/**> 'Atom' - Specifies an Atom in the Species */
		BondKeyword,			/**> 'Bond' - Defines a Bond joining two atoms */
		EndSpeciesKeyword,		/**> 'EndSpecies' - Signals the end of the current Species */
		GrainKeyword,			/**> 'Grain' - Defines a Grain containing a number of Atoms */
		IsotopologueKeyword,		/**> 'Isotopologue' - Defines isotope information for the Atoms */
		nSpeciesKeywords		/**> Number of keywords defined for this block */
	};
	// Convert text string to SpeciesKeyword
	static SpeciesKeyword speciesKeyword(const char* s);
	// Convert SpeciesKeyword to text string
	static const char* speciesKeyword(SpeciesKeyword id);
	// Return minimum number of expected arguments
	static int speciesBlockNArguments(SpeciesKeyword id);
	///@}


	/*!
	 * \name System Block Keywords
	 */
	///@{
	public:
	/// System Block Keyword Enum
	enum SystemKeyword
	{
		CellAnglesKeyword,		/**> 'CellAngles' - Gives the angles of the unit cell */
		CellLengthsKeyword,		/**> 'CellLengths' - Gives the relative lengths of the unit cell */
		ComponentKeyword,		/**> 'Component' - Specifies a Species and its relative population in the System */
		DensityKeyword,			/**> 'Density' - Specifies the density of the system, along with its units */
		EndSystemKeyword,		/**> 'EndSystem' - Signals the end of the System block */
		FileModelKeyword,		/**> 'File' - Specifies the file which contains the starting/current coordinates */
		MultiplierKeyword,		/**> 'Multiplier' - Specifies the factor by which relative populations are multiplied when generating the Model */
		NonPeriodicKeyword,		/**> 'NonPeriodic' - States that the system should be treated as non-periodic */
		nSystemKeywords			/**> Number of keywords defined for this block */
	};
	// Convert text string to SystemKeyword
	static SystemKeyword systemKeyword(const char* s);
	// Convert SystemKeyword to text string
	static const char* systemKeyword(SystemKeyword id);
	// Return minimum number of expected arguments
	static int systemBlockNArguments(SystemKeyword id);
	///@}


	/*!
	 * \name Sample Block Keywords
	 */
	///@{
	public:
	/// Sample Block Keyword Enum
	enum SampleKeyword
	{
		EndSampleKeyword,			/**> 'EndSample' - Signals the end of the Sample block */
		FitQMaxKeyword,				/**> 'FitQMax' - Upper Q-limit for empirical fitting procedure for this Sample */
		FitQMinKeyword,				/**> 'FitQMin' - Lower Q-limit for empirical fitting procedure for this Sample */
		IsotopologueSampleKeyword,		/**> 'Isotopologue' - Specifies a Species, Isotopologue, and relative population in this Sample */
		NormalisedToAverageSquaredKeyword,	/**> 'NormalisedToAvSq' - States that the reference F(Q) has been normalised to < b >**2 */
		NormalisedToSquaredAverageKeyword,	/**> 'NormalisedToSqAv' - States that the reference F(Q) has been normalised to < b**2 > */
		ReferenceDataKeyword,			/**> 'ReferenceData' - Specifies an F(Q) datafile which represents this Sample */
		SubtractSelfKeyword,			/**> 'SubtractSelf' - States that the self scattering (determined from the high-Q region) should be subtracted from the reference data */
		nSampleKeywords				/**> Number of keywords defined for this block */
	};
	// Convert text string to SampleKeyword
	static SampleKeyword sampleKeyword(const char* s);
	// Convert SampleKeyword to text string
	static const char* sampleKeyword(SampleKeyword id);
	// Return minimum number of expected arguments
	static int sampleBlockNArguments(SampleKeyword id);
	///@}


	/*!
	 * \name PairPotential Block Keywords
	 */
	///@{
	public:
	/// PairPotential Block Keyword Enum
	enum PairPotentialsKeyword
	{
		CoulombKeyword,			/**> 'Coulomb' - Specifies a PairPotential with Coulomb contributions only */
		DeltaKeyword,			/**> 'Delta' - Gives the spacing between points in the tabulated potentials */
		DispersionKeyword,		/**> 'Dispersion' - Specifies a PairPotential with LJ contributions only */
		EndPairPotentialsKeyword,	/**> 'EndPairPotentials' - Signals the end of the PairPotentials block */
		FullKeyword,			/**> 'Full' - Specifies a full PairPotential with LJ and charge contributions */
		RangeKeyword,			/**> 'Range' - Specifies the total range (inc. truncation width) over which to generate potentials */
		TruncationWidthKeyword,		/**> 'TruncationWidth' - Width of potential tail over which to reduce to zero */
		nPairPotentialsKeywords		/**> Number of keywords defined for this block */
	};
	// Convert text string to PairPotentialKeyword
	static PairPotentialsKeyword pairPotentialsKeyword(const char* s);
	// Convert PairPotentialsKeyword to text string
	static const char* pairPotentialsKeyword(PairPotentialsKeyword id);
	// Return minimum number of expected arguments
	static int pairPotentialsBlockNArguments(PairPotentialsKeyword id);
	///@}


	/*!
	 * \name Setup Block Keywords
	 */
	///@{
	public:
	/// Setup Block Keyword Enum
	enum SetupKeyword
	{
		BoxNormalisationFileKeyword,	/**> 'BoxNormalisationFile' - Specifies a file from which to load the RDF normalisation array */
		BoxNormalisationPointsKeyword,	/**> 'BoxNormalisationPoints' - Specifies how many random insertions to use when generating the normalisation array */
		BraggKeyword,			/**> 'Bragg' - Turns on (or explicitly off) calculation of Bragg scattering */
		BraggBroadeningKeyword,		/**> 'BraggBroadening' - Specifies the degree of broadening applied to calculated Bragg features */
		BraggMaximumQKeyword,		/**> 'BraggMaximumQ' - Sets the maximum Q value for Bragg calculation */
		BroadeningKeyword,		/**> 'Broadening' - Specifies the Q-independent and Q-dependent (in that order) Gaussian broadening function FWHM to apply when transforming simulated RDF data to Q-space */
		EndSetupKeyword,		/**> 'EndSetup' - Signals the end of the Setup block */
		QDeltaKeyword,			/**> 'QDelta' - Q delta to use in S(Q) calculation */
		QMaxKeyword,			/**> 'QMax' - Maximum Q up to which S(Q) calculation will be performed */
		RDFBinWidthKeyword,		/**> 'RDFBinWidth' - Specified bin width for all RDF generation */
		RDFMethodKeyword,		/**> 'RDFMethod' - Chooses the RDF calculation method to use */
		RDFRangeKeyword,		/**> 'RDFRange' - Requested extent for RDF (and subsequent S(Q)) calculation */
		RDFSmoothingKeyword,		/**> 'RDFSmoothing' - Specifies the degree of smoothing 'n' to apply to calculated RDFs, where 2n+1 controls the length in the applied Spline smooth */
		RMSEDeltaQKeyword,		/**> 'RMSEDeltaQ' - Controls bin width used in Sample/reference data RMSE calculation */
		SeedKeyword,			/**> 'Seed' - Sets the random seed to use */
		SimplexNCyclesKeyword,		/**> 'SimplexNCycles' - Sets the maximum number of cycles allowed by the Simplex minimiser when fitting potential coefficients */
		SimplexNMovesKeyword,		/**> 'SimplexNMoves' - Sets the maximum number of moves per cycle allowed by the Simplex minimiser when fitting potential coefficients */
		SimplexTemperatureKeyword,	/**> 'SimplexTemperature' - Sets the temperature used in the Simplex minimiser when fitting potential coefficients */
		SimplexToleranceKeyword,	/**> 'SimplexTolerance' - Sets the convergence tolerance used in the Simplex minimiser when fitting potential coefficients */
		TemperatureKeyword,		/**> 'Temperature' - Defines the temperature of the simulation */
		WindowFunctionKeyword,		/**> 'WindowFunction' - Specifies the windowing function to use in all Fourier transforms */
		nSetupKeywords			/**> Number of keywords defined for this block */
	};
	// Convert text string to SetupKeyword
	static SetupKeyword setupKeyword(const char* s);
	// Convert SetupKeyword to text string
	static const char* setupKeyword(SetupKeyword id);
	// Return minimum number of expected arguments
	static int setupBlockNArguments(SetupKeyword id);
	///@}


	/*!
	 * \name Simulation Block Keywords
	 */
	///@{
	public:
	/// Simulation Block Keyword Enum
	enum SimulationKeyword
	{
		EndSimulationKeyword,		/**> 'EndSimulation' - Signals the end of the Simulation block */
		EquilibrationKeyword,		/**> '@Equilibration' - States that all commands following will be part of the equilibration stage of the simulation (until another @ keyword is found) */
		ShakeKeyword,			/**> '@Shake' - States that all commands following will be part of the MC shake stage of the simulation (until another @ keyword is found) */
		StrategyKeyword,		/**> '@Strategy' - States that all commands following will be part of the main simulation strategy (until another @ keyword is found) */
		nSimulationKeywords		/**> Number of keywords defined for this block */
	};
	// Convert text string to SimulationKeyword
	static SimulationKeyword simulationKeyword(const char* s);
	// Convert SimulationKeyword to text string
	static const char* simulationKeyword(SimulationKeyword id);
	// Return minimum number of expected arguments
	static int simulationBlockNArguments(SimulationKeyword id);
	///@}
};

#endif
