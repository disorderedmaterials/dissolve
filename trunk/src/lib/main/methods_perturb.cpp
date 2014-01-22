/*
	*** dUQ Methods - Potential Perturbation
	*** src/lib/main/methods_perturb.cpp
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
#include "classes/sample.h"
#include "classes/atomtype.h"
#include "templates/simplex.h"
#include "math/constants.h"

// Public cost function callback
double DUQ::simplexCost(Array<double>& alpha)
{
	// Loop over Samples...
	const double deltaQ = 0.05;
	double Q, sos = 0.0, ushift, uscalex, uscaley, bshift, bscale, newQ, maxQ, FQ, delta;
	int typeI, typeJ, offset;

	// Generate modified, unweighted S(Q)
	offset = 0;
	for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
	{
		for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
		{
			// Grab references to original and modified data
			Data2D& unboundSQ = unboundSQMatrix_.ref(typeI, typeJ);
			Data2D& boundSQ = boundSQMatrix_.ref(typeI, typeJ);
			Data2D& modUnboundSQ = workingSQMatrixA_.ref(typeI, typeJ);
			Data2D& modBoundSQ = workingSQMatrixB_.ref(typeI, typeJ);

			// Grab shift and scale values
			ushift = alpha[offset];
			uscalex = alpha[offset+1];
			uscaley = alpha[offset+2];
			bshift = alpha[offset+3];
			bscale = alpha[offset+4];

			// Regenerate partial
			modUnboundSQ.clear();
			modBoundSQ.clear();
			Q = deltaQ;
			maxQ = unboundSQ.xMax();
			while (Q < maxQ)
			{
				// Determine scaled/shifted Q
				newQ = (Q+ushift) * uscalex;
				if (newQ > maxQ) break;
				modUnboundSQ.addPoint(Q, unboundSQ.interpolated(newQ)*uscaley);

				newQ = (Q+bshift) * bscale;
				if (newQ > maxQ) break;
				modBoundSQ.addPoint(Q, boundSQ.interpolated(newQ));
				
				Q += deltaQ;
			}
			
			offset += 5;
		}
	}
	
	// Now, loop over Samples with reference data...
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		if (!sam->hasReferenceData()) continue;
		Data2D& sampleRef = sam->referenceFQ();

		// Scan over fit Q range for the Sample
		Q = sam->referenceFitQMin();
		while (Q < sam->referenceFitQMax())
		{
			// Calculate F(Q) at this Q value
			FQ = 0.0;
			for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
			{
				for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
				{
					FQ += workingSQMatrixA_.ref(typeI, typeJ).interpolated(Q) * sam->weightsMatrix().ref(typeI, typeJ);
					FQ += workingSQMatrixB_.ref(typeI, typeJ).interpolated(Q) * sam->weightsMatrix().ref(typeI, typeJ);
				}
			}
			delta = FQ - sampleRef.interpolated(Q);
			sos += delta*delta;

			Q += deltaQ;
		}
	}

	return sos;
}

/*!
 * \brief Generate/update perturbation to potential
 */
CommandReturnValue DUQ::perturb(Configuration& cfg)
{
	int n, m, typeI, typeJ;
	static double oldVectorMag = 0.0;
	double q, diffSpacing = 0.1, rho = atomicDensity();

	// Check that we have some reference data to fit against...
	bool haveData = false;
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next) if (sam->hasReferenceData()) haveData = true;
	if (!haveData)
	{
		msg.error("Potential perturbation requested, but no reference data exists in any Sample.\n");
		return CommandFail;
	}

	// First thing - calculate partials if they are out of date...
	if (cfg.changeCount() != partialsChangeCount_)
	{
		msg.print("--> Configuration has changed - recalculating partials...\n");
		CommandReturnValue rv = calculatePairCorrelations(cfg);
		if (rv != CommandSuccess) return rv;
	}

	// Get current minimum / maximum Q values to fit over, spanning all reference datasets
	double fitQMax = totalFQ_.arrayX().first();
	double fitQMin = totalFQ_.arrayX().last() / 2.0;
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		if (!sam->hasReferenceData()) continue;
		if (sam->referenceFitQMin() < fitQMin) fitQMin = sam->referenceFitQMin();
		if (sam->referenceFitQMax() > fitQMax) fitQMax = sam->referenceFitQMax();
	}
	msg.print("--> Fit range is %f to %f Angstroms-1 over all reference datasets.\n", fitQMin, fitQMax);

	// Initialiase arrays for storage of estimated partials and simulated contribution weights
	Array2D<Data2D> workingSQMatrix, estimatedSQMatrix, contributions;
	estimatedSQMatrix.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	workingSQMatrix.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
	contributions.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);

	// Construct arrays for contribution weights
	const double qWidth = 0.1;
	const int nQDeltas = 5;
	double qWidthDelta = qWidth / (nQDeltas*2);
	double halfQWidth = qWidth * 0.5;
	int nQPoints = 0;
	Data2D zeroArray, differenceFQ, sampleRef;
	q = fitQMin + halfQWidth;
	double y;
	while (q < fitQMax)
	{
		zeroArray.addPoint(q, 0.0);
		q += qWidth;
		++nQPoints;
	}
	contributions = zeroArray;
	estimatedSQMatrix = zeroArray;
	workingSQMatrix = zeroArray;

	// TEST
	Simplex<DUQ> stretchFitSimplex(this, &DUQ::simplexCost);
	Array<double> alpha;
	for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
	{
		for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
		{
			alpha.add(0.0);		// Unbound Shift
			alpha.add(1.0);		// Unbound XScale
			alpha.add(1.0);		// Unbound YScale
			alpha.add(0.0);		// Bound Shift
			alpha.add(1.0);		// Bound Scale
		}
	}

	stretchFitSimplex.initialise(alpha, 0.01);
	Array<double> best = stretchFitSimplex.minimise(simplexNCycles_, simplexNMoves_, simplexTolerance_, simplexTemperature_);
	for (n=0; n< best.nItems(); ++n) printf("%i  %f\n", n, best[n]);
	simplexCost(best);

	// Generate modified, unweighted S(Q)
	int offset = 0;
	const double deltaQ = 0.05;
	double uscalex, uscaley, ushift, bshift, bscale, Q, newQ, maxQ;
	AtomTypeData* at1 = typeIndex_.first();
	for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI, at1 = at1->next)
	{
		AtomTypeData* at2 = at1;
		for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ, at2 = at2->next)
		{
			// Grab references to original and modified data
			Data2D& unboundSQ = unboundSQMatrix_.ref(typeI, typeJ);
			Data2D& boundSQ = boundSQMatrix_.ref(typeI, typeJ);
			Data2D& modUnboundSQ = workingSQMatrixA_.ref(typeI, typeJ);
			Data2D& modBoundSQ = workingSQMatrixB_.ref(typeI, typeJ);

			// Grab shift and scale values
			ushift = best[offset];
			uscalex = best[offset+1];
			uscaley = best[offset+2];
			bshift = best[offset+3];
			bscale = best[offset+4];

			// Regenerate partials
			modUnboundSQ.clear();
			modBoundSQ.clear();
			Q = deltaQ;
			maxQ = unboundSQ.xMax();
			Data2D crap;

			while (Q < maxQ)
			{
				// Determine scaled/shifted Q
				newQ = (Q+ushift) * uscalex;
				if (newQ > maxQ) break;
				modUnboundSQ.addPoint(Q, unboundSQ.interpolated(newQ)*uscaley);
				newQ = (Q+bshift) * bscale;
				if (newQ > maxQ) break;
				modBoundSQ.addPoint(Q, boundSQ.interpolated(newQ));
				Q += deltaQ;
				crap.addPoint(Q, unboundSQ.interpolated(newQ)*uscaley - unboundSQ.interpolated(Q));
			}
			crap.save("crap.txt");
			// Write out data...
			if (Comm.master())
			{
				LineParser parser;
				parser.openOutput(Dnchar::string("duq-%s.fit", partialSQMatrix_.ref(typeI, typeJ).name()), true);
				parser.writeLineF("#%-15s  %-16s  %-16s  %-16s\n", "Q, 1/Angstroms", "S(Q)", "bound(Q)", "unbound(Q)");
				for (n=0; n<modUnboundSQ.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", modUnboundSQ.x(n), modUnboundSQ.y(n) + modBoundSQ.interpolated(modUnboundSQ.x(n)),   modBoundSQ.interpolated(modUnboundSQ.x(n)), modUnboundSQ.y(n));
				parser.closeFiles();
			}

			// Update PairPotential
			PairPotential* pp = hasPairPotential(at1->atomType(), at2->atomType());

			// Create windowed copy of the data, and transform into r-space
// 			Data2D gr = modUnboundSQ.ref(typeI, typeJ);
			Data2D gr = crap;
			gr.save("xxx.txt");
	// 		gr.subtractInterpolated(unboundSQMatrix_.ref(typeI, typeJ));
			gr.trim(0.0, gr.xMax()*0.5);
			gr.transformSQ(atomicDensity(), windowFunction_);
	// 		gr.medianFilter(5);
			gr.smooth(5);
			gr += 1.0;
			gr.save(Dnchar::string("duq-%s.gr", partialSQMatrix_.ref(typeI, typeJ).name()));

			// Construct potential of mean force
			Data2D pmf;
			double const mingr = 0.0, mingrmag = 1000.0;
			double logy;
			for (n=0; n<gr.nPoints(); ++n)
			{
				if (gr.y(n) < 0.0) y = mingrmag;
				else if (gr.y(n) <= mingr) y = (1.0-exp(-10*(1.0-gr.y(n)/mingr))) * mingrmag;
				else y = AVOGADRO * -1.3806488e-23 * temperature_ * log(gr.y(n)) / 1000.0;
// 				if (gr.x(n) < 3.00) y += pow(3.0/gr.x(n), 12)-1.0;
				pmf.addPoint(gr.x(n), y);
			}
			pmf.save(Dnchar::string("duq-%s.pmf", partialSQMatrix_.ref(typeI, typeJ).name()));
			pp->updatePerturbation(pmf, uscaley, 0.5);
			
			// Increase offset and continue
			offset += 5;
		}
	}

	// Construct and save new estimated F(Q)
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		if (!sam->hasReferenceData()) continue;
		Data2D& sampleRef = sam->referenceFQ();
		Data2D modFQ;

		// Scan over fit Q range for the Sample
		Q = sam->referenceFitQMin();
		while (Q < sam->referenceFitQMax())
		{
			// Calculate F(Q) at this Q value
			double FQ = 0.0;
			for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
			{
				for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
				{
					FQ += workingSQMatrixA_.ref(typeI, typeJ).interpolated(Q) * sam->weightsMatrix().ref(typeI, typeJ);
					FQ += workingSQMatrixB_.ref(typeI, typeJ).interpolated(Q) * sam->weightsMatrix().ref(typeI, typeJ);
				}
			}
			modFQ.addPoint(Q, FQ);
			Q += deltaQ;
		}
		
		// Save estimated F(Q)
		modFQ.save(Dnchar::string("duq-%s-estimated.fq", sam->name()));
	}

	return CommandSuccess;
}
