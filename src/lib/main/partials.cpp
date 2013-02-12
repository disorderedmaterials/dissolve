/*
	*** dUQ - Partials
	*** src/main/partials.cpp
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
		case (DUQ::NoMethod):
			break;
		case (DUQ::SimpleMethod):
			if (!calculatePartialsSimple(cfg)) return FALSE;
			break;
		default:
			msg.error("Partial calculation style unrecognised!\n");
			return FALSE;
	}

	// Update partials change count...
	partialsChangeCount_ = cfg.changeCount();

	// Collect all processes together
	if (!Comm.wait(dUQComm::World)) return FALSE;

	return TRUE;
}

/*!
 * \brief Setup RDF / F(Q) storage
 */
bool DUQ::setupPartials()
{
	// Construct a matrix based on the typeIndex_ population
	int n, m;
	msg.print("--> Creating matrices (%ix%i)...\n", typeIndex_.nItems(), typeIndex_.nItems());

	partialRDFMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), TRUE);
	intraRDFMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), TRUE);
	partialSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), TRUE);

	Dnchar title;
	AtomTypeData* at1 = typeIndex_.first(), *at2;
	msg.print("--> Creating Lists of partials and linking into matrices...\n");
	for (n=0; n<typeIndex_.nItems(); ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<typeIndex_.nItems(); ++m, at2 = at2->next)
		{
			// Partial g(r)
			title.sprintf("%s-%s", at1->name(), at2->name());
			partialRDFMatrix_.ref(n,m).initialise(0.0, rdfRange_, rdfBinWidth_);
			partialRDFMatrix_.ref(n,m).normalisedData().setName(title.get());
			intraRDFMatrix_.ref(n,m).initialise(0.0, rdfRange_, rdfBinWidth_);
			intraRDFMatrix_.ref(n,m).normalisedData().setName(title.get());

			// -- For normalisation, self-terms must be multiplied by 2.0
			partialRDFMatrix_.ref(n,m).setRadialNumberDensityNormalisation(configuration_.box()->volume(), at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation_);
			intraRDFMatrix_.ref(n,m).setRadialNumberDensityNormalisation(configuration_.box()->volume(), at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation_);

			// Partial S(Q)
			partialSQMatrix_.ref(n,m).setName(title.get());
		}
	}

	// Total g(r)
	int nBins = partialRDFMatrix_.ref(0,0).nBins();
	totalRDF_.initialise(nBins);
	for (n=0; n<nBins; ++n) totalRDF_.setX(n, (n+0.5)*rdfBinWidth_);
	totalRDF_.setName("Unweighted");

	// Total F(Q)
	totalFQ_.setName("Unweighted");

	return TRUE;
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
			intraRDFMatrix_.ref(n,m).reset();
			partialSQMatrix_.ref(n,m).clear();
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
	// that (i,j) == (j,i) as it is stored as a half-matrix in an Array2D.
	int typeI, typeJ;
	Comm.resetAccumulatedTime();
	timer.start();
	double rho = atomicDensity();
	double delta = totalRDF_.x(1) - totalRDF_.x(0), limit = max(rdfExtensionLimit_, rdfRange_);
	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ)
		{
			// Sum histogram data from all processes
			if (!partialRDFMatrix_.ref(typeI,typeJ).allSum()) return CommandFail;
			if (!intraRDFMatrix_.ref(typeI,typeJ).allSum()) return CommandFail;

			// Finalise (normalise) partials
			partialRDFMatrix_.ref(typeI,typeJ).finalise();
			intraRDFMatrix_.ref(typeI,typeJ).finalise();
			if (rdfSmoothing_ > 0) partialRDFMatrix_.ref(typeI,typeJ).normalisedData().smooth(rdfSmoothing_*2+1);

			// Calculate unweighted S(Q)
			partialSQMatrix_.ref(typeI,typeJ) = partialRDFMatrix_.ref(typeI,typeJ).normalisedData();
			partialSQMatrix_.ref(typeI,typeJ).arrayY() -= 1.0;

			// Extend partials?
			double x = partialSQMatrix_.ref(typeI,typeJ).arrayX().last() + delta;
			while (x < limit)
			{
				partialSQMatrix_.ref(typeI,typeJ).addPoint(x, 0.0);
				x += delta;
			}

			// Fourier transform partial RDF into S(Q)
			if (!partialSQMatrix_.ref(typeI,typeJ).transformBroadenedRDF(rho, 0.05, qDependentFWHM_, qIndependentFWHM_, windowFunction_)) return CommandFail;
		}
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
	
	// Update CheckPoint data
	updateCheckPointData2D(DUQ::CheckPointUnweightedSQ);
	updateCheckPointData2D(DUQ::CheckPointFQ);
	updateCheckPointData2D(DUQ::CheckPointUnweightedGR);
	updateCheckPointData2D(DUQ::CheckPointTotalGR);
	sendSignal(DUQ::PairCorrelationsUpdatedSignal);

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
	int n, m, i;

	for (n=0; n<typeIndex_.nItems(); ++n)
	{
		for (m=n; m<typeIndex_.nItems(); ++m)
		{
			Dnchar filename(-1, "%s-unweighted-%s-%s.rdf", baseName, typeIndex_[n]->name(), typeIndex_[m]->name());

			// Open file and check that we're OK to proceed writing to it
			msg.print("Writing RDF file '%s'...\n", filename.get());

			parser.openOutput(filename, TRUE);
			if (!parser.isFileGoodForWriting())
			{
				msg.error("Couldn't open file '%s' for writing.\n", filename.get());
				continue;
			}
			
			Data2D& rdf = partialRDFMatrix_.ref(n,m).normalisedData(), intra = intraRDFMatrix_.ref(n,m).normalisedData();
			parser.writeLineF("# %-14s  %-16s  %-16s\n", "r, Angstroms", "g(r)", "intra(r)"); 
			for (i = 0; i<rdf.nPoints(); ++i) parser.writeLineF("%16.10e  %16.10e  %16.10e\n", rdf.x(i), rdf.y(i), intra.y(i));
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
	if (!Comm.master()) return;
	for (int n=0; n<typeIndex_.nItems(); ++n)
	{
		for (int m=0; m<typeIndex_.nItems(); ++m)
		{
			Dnchar filename(-1, "%s-unweighted-%s-%s.sq", baseName, typeIndex_[n]->name(), typeIndex_[m]->name());
			partialSQMatrix_.ref(n,m).save(filename);
		}
	}

	Dnchar filename(-1, "%s-unweighted-total.fq", baseName);
	totalFQ_.save(filename);
}
