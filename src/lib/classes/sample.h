/*
	*** Sample Definition
	*** src/lib/classes/sample.h
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

#ifndef DUQ_SAMPLE_H
#define DUQ_SAMPLE_H

#include "classes/isotopologuemix.h"
#include "classes/atomtypeindex.h"
#include "classes/histogram.h"
#include "base/dnchar.h"
#include "templates/reflist.h"

// Forward Declarations
class Species;
class System;

/*!
 * \brief Sample Definition
 * \details The Sample class describes the component Species in terms of their isotopic
 * substitutions, i.e. through association with an Isotopologue defined for the Species.
 * TODO Experimental Data. Calculation of F(Q) for every defined Sample etc.
 */
class Sample : public ListItem<Sample>
{
	public:
	// Constructor
	Sample();


	/*!
	 * \name Basic Information
	 */
	///@{
	private:
	// Descriptive name
	Dnchar name_;
	
	public:
	// Set name of Sample
	void setName(const char* name);
	// Return name of Sample
	const char* name() const;
	///@}
	
	
	/*!
	 * \name Isotopologue Definition
	 */
	///@{
	private:
	// List of IsotopologueMix-tures for each Species
	List<IsotopologueMix> isotopologueMixtures_;
	
	public:
	// Update IsotopologueMix data
	void updateIsotopologueMixtures(const List<Species>& species);
	// Return whether the Sample contains a mixtures definition for the provided Species
	IsotopologueMix* hasSpeciesIsotopologueMixture(Species* sp) const;
	// Add Isotopologue to mixture
	bool addIsotopologueToMixture(Species* sp, Isotopologue* iso, double relPop);
	// Set Isotopologue data
	bool setIsotopologueInMixture(Species* sp, Isotopologue* iso, double frac = -1.0, Isotopologue* newTope = NULL);
	// Return first IsotopologueMix
	IsotopologueMix* isotopologueMixtures() const;
	// Return nth IsotopologueMix
	IsotopologueMix* isotopologueMixture(int n);
	// Assign default (first) Isotopologues for all Species
	void assignDefaultIsotopes();
	///@}


	/*!
	 * \name RDF / S(Q) Data
	 */
	///@{
	private:
	// Type index for this Sample
	AtomTypeIndex typeIndex_;
	// Average scattering lengths of sample
	double boundCoherentAverageSquared_, boundCoherentSquaredAverage_;
	// Partial S(Q) matrix
	Array2D<Data2D> partialSQMatrix_;
	// Partial weights matrix
	Array2D<double> weightsMatrix_;
	// Total neutron-weighted g(r)
	Data2D totalGR_;
	// Total neutron-weighted F(Q)
	Data2D totalFQ_;

	public:
	// Create type index and RDF / S(Q) storage
	bool createTypeIndex(const List<Species>& species, int multiplier, int nExpectedAtoms, const AtomTypeIndex& masterIndex);
	// Return <b>**2
	double boundCoherentAverageSquared();
	// Return <b**2>
	double boundCoherentSquaredAverage();
	// Setup pair correlations
	bool setupPairCorrelations(double volume, double range, double binWidth, double extensionLimit, Data2D& boxNormalisation, double rho);
	// Calculate weighted pair correlations
	bool calculatePairCorrelations(Array2D<Histogram>& masterRDFs, Array2D<Data2D>& masterSQ);
	// Return reference to array of partials
	Array2D<Data2D>& partialSQMatrix();
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
	///@}


	/*!
	 * \name Reference Data
	 */
	///@{
	public:
	/// Reference data normalisation types
	enum NormalisationType
	{
		NoNormalisation,		/**> Data has not been normalised */
		AverageSquaredNormalisation,	/**> Data has been normalised to < b >**2 */
		SquaredAverageNormalisation	/**> Data has been normalised to < b**2 > */
	};

	private:
	// Whether reference data exists
	bool hasReferenceData_;
	// Filename of reference F(Q) (if present)
	Dnchar referenceDataFileName_;
	// Reference data (fit target)
	Data2D referenceFQ_;
	// Reference data normalisation style
	NormalisationType referenceDataNormalisation_;
	// Whether self-scattering should be subtracted
	bool referenceDataSubtractSelf_;
	// Fourier transform of reference data
	Data2D referenceDataFT_;
	// Minimum Q-limit for empirical fitting
	double referenceFitQMin_;
	// Maximum Q-limit for empirical fitting
	double referenceFitQMax_;
	// Difference between reference and calculated F(Q)
	Data2D differenceFQ_;

	public:
	// Return whether reference data exists
	bool hasReferenceData();
	// Load reference F(Q) data
	bool loadReferenceData(const char* fileName);
	// Return reference data filename (if any)
	Dnchar& referenceDataFileName();
	// Return reference F(Q)
	Data2D& referenceFQ();
	// Set reference data normalisation flag
	void setReferenceDataNormalisation(NormalisationType norm);
	// Return whether reference data are normalised
	NormalisationType referenceDataNormalisation();
	// Set reference data self-scattering subtraction flag
	void setReferenceSubtractSelf(bool b);
	// Return whether reference data should have self-scattering subtracted
	bool referenceSubtractSelf();
	// Set minimum Q-value for empirical fitting
	void setReferenceFitQMin(double q);
	// Return minimum Q-value for empirical fitting
	double referenceFitQMin();
	// Set maximum Q-value for empirical fitting
	void setReferenceFitQMax(double q);
	// Return maximum Q-value for empirical fitting
	double referenceFitQMax();
	// Return difference F(Q)
	Data2D& differenceFQ();
	// Finalise reference data
	bool finaliseReferenceData();
	// Calculate RMSE error between calculated S(Q)s and reference data
	double referenceRMSE(double deltaQ);
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(const List<Species>& species);
	///@}
};

#endif
