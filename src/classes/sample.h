/*
	*** Sample Definition
	*** src/classes/sample.h
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

#ifndef DUQ_SAMPLE_H
#define DUQ_SAMPLE_H

#include "classes/isotopologuemix.h"
#include "classes/atomtypeindex.h"
#include "classes/histogram.h"
#include "base/dnchar.h"
#include "templates/reflist.h"

// Forward Declarations
class Species;

/*
 * Sample Definition
 */
class Sample : public ListItem<Sample>
{
	public:
	// Constructor
	Sample();
	// Copy Constructor
	Sample(const Sample& source);
	// Assignment Operator
	void operator=(const Sample& source);
	// Sample Types
	enum SampleType { NeutronFQ, nSampleTypes };
	// Convert text string to InputBlock
	static SampleType sampleType(const char* s);
	// Convert InputBlock to text string
	static const char* sampleType(SampleType st);


	/*
	 * Basic Information
	 */
	private:
	// Descriptive name
	Dnchar name_;
	// Data type
	SampleType type_;
	
	public:
	// Set name of Sample
	void setName(const char* name);
	// Return name of Sample
	const char* name() const;
	// Set data type
	void setType(SampleType type);
	// Return data type
	SampleType type();
	
	
	/*
	 * Isotopologue Definition
	 */
	private:
	// List of IsotopologueMix-tures for Species in this Sample
	List<IsotopologueMix> isotopologueMixtures_;
	
	public:
	// Update IsotopologueMix data
	void updateIsotopologueMixtures(const List<Species>& species);
	// Return whether the Sample contains a mixtures definition for the provided Species
	IsotopologueMix* hasSpeciesIsotopologueMixture(Species* sp) const;
	// Add Isotopologue to mixture
	bool addIsotopologueToMixture(Species* sp, Isotopologue* iso, double relPop);
	// Return first IsotopologueMix
	IsotopologueMix* isotopologueMixtures() const;
	// Return nth IsotopologueMix
	IsotopologueMix* isotopologueMixture(int n);
	// Assign default (first) Isotopologues for all Species
	void assignDefaultIsotopes();


	/*
	 * RDF / S(Q) Data
	 */
	private:
	// Type index for this Sample
	AtomTypeIndex typeIndex_;
	// Average scattering lengths of sample
	double boundCoherentAverageSquared_, boundCoherentSquaredAverage_;
	// Partial S(Q) matrix
	Array2D<Data2D> pairSQMatrix_;
	// Bragg S(Q) matrix
	Array2D<Data2D> braggSQMatrix_;
	// Weights matrix (bb * cc)
	Array2D<double> weightsMatrix_;
	// Scattering weights matrix (bb)
	Array2D<double> scatteringMatrix_;
	// Total neutron-weighted g(r)
	Data2D totalGR_;
	// Total neutron-weighted F(Q)
	Data2D totalFQ_;

	public:
	// Create type index and RDF / S(Q) storage
	bool createTypeIndex(const RefList<Species,double>& species, int multiplier, int nExpectedAtoms, const AtomTypeIndex& masterIndex);
	// Return <b>**2
	double boundCoherentAverageSquared();
	// Return <b**2>
	double boundCoherentSquaredAverage();
	// Setup pair correlations
	bool setupPairCorrelations(double volume, double range, double binWidth, Data2D& boxNormalisation, double rho);
	// Calculate weighted pair correlations
	bool calculatePairCorrelations(Array2D< Histogram >& masterRDFs, Array2D< Data2D >& masterPairSQ, Array2D< Data2D >& masterBraggSQ, Array2D< Data2D >& masterPartialSQ);
	// Return reference to array of partials
	Array2D<Data2D>& partialSQMatrix();
	// Return reference to array of Bragg partials
	Array2D<Data2D>& braggSQMatrix();
	// Return reference to weights matrix
	Array2D<double>& weightsMatrix();
	// Return total F(Q)
	Data2D& totalFQ();
	// Return total G(R)
	Data2D& totalGR();
	// Save RDFs
	void saveRDFs(const char* baseName);
	// Save S(Q)
	void saveSQ(const char* baseName);


	/*
	 * Reference Data
	 */
	public:
	// Reference data normalisation types
	enum NormalisationType
	{
		NoNormalisation,		/* Data has not been normalised */
		AverageSquaredNormalisation,	/* Data has been normalised to < b >**2 */
		SquaredAverageNormalisation	/* Data has been normalised to < b**2 > */
	};

	private:
	// Whether reference data exists
	bool hasReferenceData_;
	// Filename of reference F(Q) (if present)
	Dnchar referenceDataFileName_;
	// Reference data
	Data2D referenceData_;
	// FWHM of Gaussian for Q-dependent instrument broadening function (if required)
	double qDependentFWHM_;
	// FWHM of Gaussian for Q-independent instrument broadening function (if required)
	double qIndependentFWHM_;
	// Reference data normalisation style (if required)
	NormalisationType referenceDataNormalisation_;
	// Whether self-scattering should be subtracted (and is necessary)
	bool referenceDataSubtractSelf_;
	// Fourier transform of reference data
	Data2D referenceDataFT_;
	// Minimum abscissa limit for empirical fitting
	double referenceFitMin_;
	// Maximum abscissa limit for empirical fitting
	double referenceFitMax_;
	// Difference between reference and calculated data
	Data2D differenceData_;

	public:
	// Return whether reference data exists
	bool hasReferenceData();
	// Load reference data
	bool loadReferenceData(const char* fileName);
	// Return reference data filename (if any)
	Dnchar& referenceDataFileName();
	// Return reference data
	Data2D& referenceData();
	// Set FWHM of Gaussian for Q-dependent instrument broadening function (if required)
	void setQDependentFWHM(double fwhm);
	// Return FWHM of Gaussian for Q-dependent instrument broadening function (if required)
	double qDependentFWHM();
	// Set FWHM of Gaussian for Q-independent instrument broadening function (if required)
	void setQIndependentFWHM(double fwhm);
	// Return FWHM of Gaussian for Q-independent instrument broadening function (if required)
	double qIndependentFWHM();
	// Set reference data normalisation flag
	void setReferenceDataNormalisation(NormalisationType norm);
	// Return whether reference data are normalised
	NormalisationType referenceDataNormalisation();
	// Set reference data self-scattering subtraction flag
	void setReferenceSubtractSelf(bool b);
	// Return whether reference data should have self-scattering subtracted
	bool referenceSubtractSelf();
	// Set minimum abscissa for empirical fitting
	void setReferenceFitMin(double value);
	// Return minimum abscissa value for fitting
	double referenceFitMin();
	// Set maximum abscissa for empirical fitting
	void setReferenceFitMax(double value);
	// Return maximum abscissa for empirical fitting
	double referenceFitMax();
	// Return difference data
	Data2D& differenceData();
	// Finalise reference data
	bool finaliseReferenceData();
	// Calculate RMSE error between calculated and reference data
	double referenceRMSE(double deltaQ);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(const List<Species>& species);
};

#endif
