/*
	*** dUQ - Partials
	*** src/main/partials.cpp
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
#include "classes/box.h"
#include "classes/atom.h"
#include "classes/cell.h"
#include "classes/atomtype.h"
#include "base/comms.h"
#include <string.h>

/*
// Partials
*/

// RDFMethod keywords
const char* RDFMethodKeywords[] = { "None", "Simple" };

/*!
 * \brief Convert text string to RDFMethod
 */
DUQ::RDFMethod DUQ::rdfMethod(const char* s)
{
	for (int n=0; n<DUQ::nRDFMethods; ++n) if (strcmp(s,RDFMethodKeywords[n]) == 0) return (DUQ::RDFMethod) n;
	return DUQ::nRDFMethods;
}

/*!
 * \brief Convert RDFMethod to text string
 */
const char* DUQ::rdfMethod(DUQ::RDFMethod rm)
{
	return RDFMethodKeywords[rm];
}

/*!
 * \brief Calculate partials
 * \details Calculate partial RDFs using straight Atom data.
 */
bool DUQ::calculatePartialRDFs(Configuration& cfg)
{
	switch (rdfMethod_)
	{
		case (DUQ::SimpleMethod):
			if (!calculatePartialsSimple(cfg)) return false;
			break;
		default:
			msg.error("Partial calculation style unrecognised!\n");
			return false;
	}

	// Update partials change count...
	partialsChangeCount_ = cfg.changeCount();

	// Collect all processes together
	if (!Comm.wait(DUQComm::World)) return false;

	return true;
}

/*!
 * \brief Setup RDF / F(Q) storage
 */
bool DUQ::setupPartials()
{
	// Construct a matrix based on the typeIndex_ population
	int n, m;
	msg.print("--> Creating matrices (%ix%i)...\n", typeIndex_.nItems(), typeIndex_.nItems());

	partialRDFMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	boundRDFMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	unboundRDFMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	partialSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	boundSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	braggSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	unboundSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	workingSQMatrixA_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
	workingSQMatrixB_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);

	Dnchar title;
	AtomTypeData* at1 = typeIndex_.first(), *at2;
	msg.print("--> Creating Lists of partials and linking into matrices...\n");
	msg.printVerbose("Range/binWidth/Volume = %f/%f/%f\n", rdfRange_, rdfBinWidth_, configuration_.box()->volume());
	for (n=0; n<typeIndex_.nItems(); ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<typeIndex_.nItems(); ++m, at2 = at2->next)
		{
			// Partial g(r)
			title.sprintf("%s-%s", at1->name(), at2->name());
			partialRDFMatrix_.ref(n,m).initialise(0.0, rdfRange_, rdfBinWidth_);
			partialRDFMatrix_.ref(n,m).normalisedData().setName(title.get());
			boundRDFMatrix_.ref(n,m).initialise(0.0, rdfRange_, rdfBinWidth_);
			boundRDFMatrix_.ref(n,m).normalisedData().setName(title.get());
			unboundRDFMatrix_.ref(n,m).initialise(0.0, rdfRange_, rdfBinWidth_);
			unboundRDFMatrix_.ref(n,m).normalisedData().setName(title.get());

			// -- For normalisation, self-terms must be multiplied by 2.0
			partialRDFMatrix_.ref(n,m).setRadialNumberDensityNormalisation(configuration_.box()->volume(), at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation_);
			boundRDFMatrix_.ref(n,m).setRadialNumberDensityNormalisation(configuration_.box()->volume(), at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation_);
			unboundRDFMatrix_.ref(n,m).setRadialNumberDensityNormalisation(configuration_.box()->volume(), at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation_);

			// Partial S(Q)
			partialSQMatrix_.ref(n,m).setName(title.get());
			boundSQMatrix_.ref(n,m).setName(title.get());
			unboundSQMatrix_.ref(n,m).setName(title.get());
			braggSQMatrix_.ref(n,m).setName(title.get());
		}
	}

	// Total g(r)
	int nBins = partialRDFMatrix_.ref(0,0).nBins();
	totalRDF_.initialise(nBins);
	for (n=0; n<nBins; ++n) totalRDF_.setX(n, (n+0.5)*rdfBinWidth_);
	totalRDF_.setName("Unweighted");

	// Total F(Q)
	totalFQ_.setName("Unweighted");

	return true;
}

/*!
 * \brief Reset pair correlations
 */
void DUQ::resetPairCorrelations()
{
	for (int n=0; n<typeIndex_.nItems(); ++n)
	{
		for (int m=n; m<typeIndex_.nItems(); ++m)
		{
			partialRDFMatrix_.ref(n,m).reset();
			boundRDFMatrix_.ref(n,m).reset();
			unboundRDFMatrix_.ref(n,m).reset();
			partialSQMatrix_.ref(n,m).clear();
			boundSQMatrix_.ref(n,m).clear();
			unboundSQMatrix_.ref(n,m).clear();
			boundSQMatrix_.ref(n,m).clear();
		}
	}
	totalRDF_.arrayY() = 0.0;
	totalFQ_.arrayY() = 0.0;
}

/*!
 * \brief Calculate pair correlations
 */
CommandReturnValue DUQ::calculatePairCorrelations(Configuration& cfg)
{
	// Check that we actually need to calculate new partials
	if (cfg.changeCount() == partialsChangeCount_)
	{
		msg.print("Refused to calculate partials for configuration - nothing has changed since the last calculation.\n");
		return CommandSuccess;
	}

	// Clear old RDF/S(Q) data
	resetPairCorrelations();
	
	// Calculate full partials
	Timer timer;
	if (!calculatePartialRDFs(cfg)) return CommandFail;
	timer.stop();
	msg.print("--> Finished calculation of partials (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Calculate intramolecular partials
	timer.start();
	if (!calculateIntramolecularRDFs(cfg)) return CommandFail;
	timer.stop();
	msg.print("--> Finished calculation of intramolecular partials (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Perform summation of partial data
	// Note that merging/summation of cross-term data (i.e. [n][m] with [m][n]) is not necessary since the partials matrix knows
	// that (i,j) == (j,i) as it is stored as a half-matrix in an Array2D object.
	int typeI, typeJ;
	Comm.resetAccumulatedTime();
	timer.start();
	double rho = atomicDensity();
	double delta = totalRDF_.x(1) - totalRDF_.x(0);
	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ)
		{
			// Sum histogram data from all processes
			if (!partialRDFMatrix_.ref(typeI,typeJ).allSum()) return CommandFail;
			if (!boundRDFMatrix_.ref(typeI,typeJ).allSum()) return CommandFail;

			// Create unbound histogram from total and bound data
			unboundRDFMatrix_.ref(typeI, typeJ) = partialRDFMatrix_.ref(typeI,typeJ);
			unboundRDFMatrix_.ref(typeI, typeJ).addHistogramData(boundRDFMatrix_.ref(typeI,typeJ), -1.0);

			// Finalise (normalise) partials
			partialRDFMatrix_.ref(typeI,typeJ).finalise();
			boundRDFMatrix_.ref(typeI,typeJ).finalise();
			unboundRDFMatrix_.ref(typeI,typeJ).finalise();

			// Smooth partials if requested
			if (rdfSmoothing_ > 0)
			{
				partialRDFMatrix_.ref(typeI,typeJ).normalisedData().smooth(rdfSmoothing_*2+1);
				boundRDFMatrix_.ref(typeI,typeJ).normalisedData().smooth(rdfSmoothing_*2+1);
				unboundRDFMatrix_.ref(typeI,typeJ).normalisedData().smooth(rdfSmoothing_*2+1);
			}

			// Copy RDF data ready for Fourier transform
			// -- Copy RDF data
			partialSQMatrix_.ref(typeI,typeJ) = partialRDFMatrix_.ref(typeI,typeJ).normalisedData();
			partialSQMatrix_.ref(typeI,typeJ).arrayY() -= 1.0;
			boundSQMatrix_.ref(typeI,typeJ) = boundRDFMatrix_.ref(typeI,typeJ).normalisedData();
// 			boundSQMatrix_.ref(typeI,typeJ).arrayY() -= 1.0;
			unboundSQMatrix_.ref(typeI,typeJ) = unboundRDFMatrix_.ref(typeI,typeJ).normalisedData();
			unboundSQMatrix_.ref(typeI,typeJ).arrayY() -= 1.0;
		}
	}
	timer.stop();
	msg.print("--> Finished summation and normalisation of partial RDF data (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Perform FT of partial g(r) into S(Q)
	// TODO Parallelise this
	Comm.resetAccumulatedTime();
	timer.start();
	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ)
		{
			if (!partialSQMatrix_.ref(typeI,typeJ).transformBroadenedRDF(rho, 0.05, qDependentFWHM_, qIndependentFWHM_, windowFunction_)) return CommandFail;
			if (!boundSQMatrix_.ref(typeI,typeJ).transformBroadenedRDF(rho, 0.05, qDependentFWHM_, qIndependentFWHM_, windowFunction_)) return CommandFail;
			if (!unboundSQMatrix_.ref(typeI,typeJ).transformBroadenedRDF(rho, 0.05, qDependentFWHM_, qIndependentFWHM_, windowFunction_)) return CommandFail;
		}
	}
	timer.stop();
	msg.print("--> Finished Fourier transform of partial g(r) into partial S(Q) (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Calculate Bragg partials (if requested)
	if (braggCalculationOn_)
	{
		Comm.resetAccumulatedTime();
		timer.start();
		if (!calculateBraggSQ(cfg)) return CommandFail;
		timer.stop();
		msg.print("--> Finished calculation of partial Bragg S(Q) (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());
	}

	// Calculate total g(r) and F(Q)
	totalFQ_ = partialSQMatrix_.ref(0,0);
	totalFQ_.arrayY() = 0.0;
	double factor;
	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ)
		{
			// Total RDF
			factor = typeIndex_[typeI]->fraction() * typeIndex_[typeJ]->fraction() * (typeI == typeJ ? 1.0 : 2.0);
			totalRDF_.addY(partialRDFMatrix_.ref(typeI,typeJ).normalisedData().arrayY(), factor);
			
			// F(Q)
			totalFQ_.addY(partialSQMatrix_.ref(typeI,typeJ).arrayY(), factor);
		}
	}
	timer.stop();
	msg.print("--> Finished summation and FT of partials, and generation of total unweighted RDF/F(Q) (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Calculate weighted partials for each Sample, along with current RMSE
	Comm.resetAccumulatedTime();
	timer.start();
	totalRMSE_ = 0.0;
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		if (!sam->calculatePairCorrelations(partialRDFMatrix_, partialSQMatrix_)) return CommandFail;
		if (sam->hasReferenceData()) totalRMSE_ += sam->referenceRMSE(rmseDeltaQ_);
	}
	timer.stop();
	msg.print("--> Finished generation of Sample partials (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	msg.print("--> Current RMSE over all Samples = %f barns/sr/atom\n", totalRMSE_);

	return CommandSuccess;
}

/*!
 * \brief Save all unweighted RDFs
 */
void DUQ::saveRDFs(const char* baseName)
{
	// Only the Master process can do this
	if (!Comm.master()) return;
	LineParser parser;
	int typeI, typeJ, n;

	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ)
		{
			Dnchar filename(-1, "%s-unweighted-%s-%s.rdf", baseName, typeIndex_[typeI]->name(), typeIndex_[typeJ]->name());

			// Open file and check that we're OK to proceed writing to it
			msg.print("Writing RDF file '%s'...\n", filename.get());

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				msg.error("Couldn't open file '%s' for writing.\n", filename.get());
				continue;
			}
			
			Data2D& rdf = partialRDFMatrix_.ref(typeI,typeJ).normalisedData();
			Data2D& bound = boundRDFMatrix_.ref(typeI,typeJ).normalisedData();
			Data2D& unbound = unboundRDFMatrix_.ref(typeI,typeJ).normalisedData();
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s\n", "r, Angstroms", "g(r)", "bound(r)", "unbound(r)"); 
			for (n = 0; n<rdf.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", rdf.x(n), rdf.y(n), bound.y(n), unbound.y(n));
			parser.closeFiles();
		}
	}

	Dnchar filename(-1, "%s-unweighted-total.rdf", baseName);
	totalRDF_.save(filename);
}

/*!
 * \brief Save all partial S(Q)
 */
void DUQ::saveSQ(const char* baseName)
{
	// Only the Master process can do this
	if (!Comm.master()) return;
	LineParser parser;
	int typeI, typeJ, n;

	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ)
		{
			Dnchar filename(-1, "%s-unweighted-%s-%s.sq", baseName, typeIndex_[typeI]->name(), typeIndex_[typeJ]->name());

			// Open file and check that we're OK to proceed writing to it
			msg.print("Writing S(Q) file '%s'...\n", filename.get());

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				msg.error("Couldn't open file '%s' for writing.\n", filename.get());
				continue;
			}
			
			Data2D& sq = partialSQMatrix_.ref(typeI,typeJ);
			Data2D& bound = boundSQMatrix_.ref(typeI,typeJ);
			Data2D& unbound = unboundSQMatrix_.ref(typeI,typeJ);
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s\n", "Q, 1/Angstroms", "S(Q)", "bound(Q)", "unbound(Q)"); 
			for (n = 0; n<sq.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", sq.x(n), sq.y(n), bound.y(n), unbound.y(n));
			parser.closeFiles();
		}
	}

	Dnchar filename(-1, "%s-unweighted-total.fq", baseName);
	totalFQ_.save(filename);
}
