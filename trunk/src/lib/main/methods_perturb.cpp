/*
	*** dUQ Methods - Potential Perturbation
	*** src/lib/main/methods_perturb.cpp
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
#include "classes/sample.h"
#include "classes/atomtype.h"
#include "main/simplex.h"
#include "math/constants.h"

// Public cost function callback
double DUQ::simplexCost(Array<double>& alpha)
{
	// Loop over Samples...
	const double deltaQ = 0.05;
	double Q, sos = 0.0, ushift, uscale, bshift, bscale, newQ, maxQ, FQ, delta;
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
			uscale = alpha[offset+1];
			bshift = alpha[offset+2];
			bscale = alpha[offset+3];

			// Regenerate partial
			modUnboundSQ.clear();
			modBoundSQ.clear();
			Q = deltaQ;
			maxQ = unboundSQ.xMax();
			while (Q < maxQ)
			{
				// Determine scaled/shifted Q
				newQ = (Q+ushift) * uscale;
				if (newQ > maxQ) break;
				modUnboundSQ.addPoint(Q, unboundSQ.interpolated(newQ));

				newQ = (Q+bshift) * bscale;
				if (newQ > maxQ) break;
				modBoundSQ.addPoint(Q, boundSQ.interpolated(newQ));
				
				Q += deltaQ;
			}
			
			offset += 4;
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
	bool haveData = FALSE;
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next) if (sam->hasReferenceData()) haveData = TRUE;
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
	estimatedSQMatrix.initialise(atomTypes_.nItems(), atomTypes_.nItems(), TRUE);
	workingSQMatrix.initialise(atomTypes_.nItems(), atomTypes_.nItems(), TRUE);
	contributions.initialise(atomTypes_.nItems(), atomTypes_.nItems(), TRUE);

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
	Simplex stretchFitSimplex(this, &DUQ::simplexCost);
	Array<double> alpha;
	for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
	{
		for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
		{
			alpha.add(0.0);		// Unbound Shift
			alpha.add(1.0);		// Unbound Scale
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
	double uscale, ushift, bshift, bscale, Q, newQ, maxQ;
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
			ushift = best[offset];
			uscale = best[offset+1];
			bshift = best[offset+2];
			bscale = best[offset+3];

			// Regenerate partials
			modUnboundSQ.clear();
			modBoundSQ.clear();
			Q = deltaQ;
			maxQ = unboundSQ.xMax();
			while (Q < maxQ)
			{
				// Determine scaled/shifted Q
				newQ = (Q+ushift) * uscale;
				if (newQ > maxQ) break;
				modUnboundSQ.addPoint(Q, unboundSQ.interpolated(newQ));
				newQ = (Q+bshift) * bscale;
				if (newQ > maxQ) break;
				modBoundSQ.addPoint(Q, boundSQ.interpolated(newQ));
				Q += deltaQ;
			}
			
			// Write out data...
			if (Comm.master())
			{
				LineParser parser;
				parser.openOutput(Dnchar::string("duq-%s.fit", partialSQMatrix_.ref(typeI, typeJ).name()), TRUE);
				parser.writeLineF("#%-15s  %-16s  %-16s  %-16s\n", "Q, 1/Angstroms", "S(Q)", "bound(Q)", "unbound(Q)");
				for (n=0; n<modUnboundSQ.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", modUnboundSQ.x(n), modUnboundSQ.y(n) + modBoundSQ.interpolated(modUnboundSQ.x(n)),   modBoundSQ.interpolated(modUnboundSQ.x(n)), modUnboundSQ.y(n));
				parser.closeFiles();
			}

			// Increase offset and continue
			offset += 4;
		}
	}

	// Now, loop over Samples with reference data...
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
		modFQ.save(Dnchar::string("duq-%s.fq", sam->name()));
	}

/*	
	// Now, loop over Samples with reference data...
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		if (!sam->hasReferenceData()) continue;

		// Grab references to this Sample's reference, partial and weights data
		Array2D<Data2D>& sampleSQMatrix = sam->partialSQMatrix();
		Array2D<double>& sampleWeightsMatrix = sam->weightsMatrix();

		// Rebin reference data onto a common scale
		sampleRef = zeroArray;
		for (n=0; n<nQPoints; ++n)
		{
			q = zeroArray.x(n);
			sampleRef.setY(n, sam->referenceFQ().interpolated(q));
		}

		// First step - construct contribution weights from weighted simulated S(Q) Sample partials
		int nPartials = 0;
		contributions = zeroArray;
		for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
		{
			for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
			{
				Data2D& sq = sampleSQMatrix.ref(typeI, typeJ);
				Data2D& cntrb = contributions.ref(typeI, typeJ);

				// Loop over points in q
				for (n=0; n<nQPoints; ++n)
				{
					q = zeroArray.x(n);
					y = 0.0;
					for (m=-nQDeltas; m<=nQDeltas; ++m)
					{
						y += sq.interpolated(q);
						q += qWidthDelta;
					}
					cntrb.setY(n, y / (nQDeltas*2 + 1));
				}
				
				++nPartials;
			}
		}
		// -- Normalise all individual weights to 1.0
		for (n=0; n<nQPoints; ++n)
		{
			// Sum values over all partials
			y = 0.0;
			for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
			{
				for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ) y += fabs(contributions.ref(typeI, typeJ).y(n));
			}
			// Now normalise against the sum
			for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
			{
				for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ) contributions.ref(typeI, typeJ).arrayY()[n] /= y;
			}
		}

		// TEST - Write contribution arrays
		for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
		{
			for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
			{
				Dnchar s(-1, "cntrb%i%i.txt", typeI, typeJ);
				contributions.ref(typeI, typeJ).save(s);
			}
		}

		// Now enter the iterative loop
		// -- Zero working arrays to start with
		workingSQMatrix = zeroArray;
		int cycle;
		double fraction;
		for (cycle=0; cycle < 10; ++cycle)
		{
			fraction = (cycle+1) * 0.1;
			// Determine remainder to assign by subtracting current estimatedSQ from Sample reference data
			differenceFQ = sampleRef;
			for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
			{
				for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
				{
					Data2D& workSQ = workingSQMatrix.ref(typeI, typeJ);
					for (n=0; n<nQPoints; ++n) differenceFQ.addY(n, -workSQ.y(n));
				}
			}
			differenceFQ.save(Dnchar::string("diff%02i.txt", cycle));

			// Now, for each partial, look at the relative contributions and add a fraction of the percentage remainder to workingSQ_
			for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
			{
				for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
				{
					Data2D& workSQ = workingSQMatrix.ref(typeI, typeJ);
					Data2D& cntrb = contributions.ref(typeI, typeJ);

					// Loop over points in q
					for (n=0; n<nQPoints; ++n)
					{
						q = zeroArray.x(n);
						if (q < sam->referenceFitQMin()) continue;
						if (q > sam->referenceFitQMax()) break;
						workSQ.addY(n, fraction * cntrb.y(n) * differenceFQ.y(n));
					}
					workSQ.save(Dnchar::string("%s%02i.txt", partialSQMatrix_.ref(typeI, typeJ).name(), cycle));
				}
			}
		}

		// TEST - Write FTs
		for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
		{
			for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
			{
				Dnchar s(-1, "ft%i%i.txt", typeI, typeJ);
				Data2D data = workingSQMatrix.ref(typeI, typeJ);
				data.smooth(3);
				data.transformSQ(atomicDensity(), Data2D::GaussianWindow);
				data.save(s);
			}
		}
	}*/

	// Loop over Samples with reference data, constructing difference functions of the partials with their reference data
// 	Data2D diff;
// 	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
// 	{
// 		if (!sam->hasReferenceData()) continue;
// 		
// 		diff = sam->differenceFQ();
// 		diff.save("lklklk.txt");
// 		diff.transformSQ(rho, windowFunction_);
// 		diff.smooth(3, 3);
// 		diff.arrayY() += 1.0;
// 		diff.save("jjj.txt");
// 		for (int n=0; n<diff.nPoints(); ++n) diff.setY(n, log(diff.y(n)));
// 		diff.save("kkk.txt");
// 	}
// 
	// Loop over PairPotentials and apply corrections
	for (PairPotential* pp = pairPotentials_.first(); pp != NULL; pp = pp->next)
	{
		// Grab master AtomType indices from PairPotential
		typeI = typeIndex_.indexOf(pp->atomTypeI());
		typeJ = typeIndex_.indexOf(pp->atomTypeJ());
		
		// Grab reference to difference data and interpolate it
// 		Data2D& diff = differenceSplines_.ref(typeI,typeJ);
// 		diff.smooth(3);
// 		diff.interpolate();
// 		msg.print("Absolute integral of diff(q) = %f\n", diff.absIntegral());

		// Create windowed copy of the data, and transform into r-space
		Data2D gr = workingSQMatrixA_.ref(typeI, typeJ);
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
			if (gr.x(n) < 3.00) y += pow(3.0/gr.x(n), 12)-1.0;
			pmf.addPoint(gr.x(n), y);
		}
		pmf.save(Dnchar::string("duq-%s.pmf", partialSQMatrix_.ref(typeI, typeJ).name()));
		pp->updatePerturbation(pmf, 2000.0, 2000.0);
		
		// Write pair potential / fit data to file
// 		if (Comm.master())
// 		{
// 			LineParser parser;
// 			Dnchar coeffFile(-1, "duq.%s-%s.fitcoeff", pp->atomTypeNameI(), pp->atomTypeNameJ());
// 			if (parser.openOutput(coeffFile, TRUE))
// 			{
// 				parser.writeLineF("# Potential fit coefficients for %s-%s\n", pp->atomTypeNameI(), pp->atomTypeNameJ());
// 				parser.writeLineF("#   Q, A-1          C(Q)           r, A            C(r)\n");
// 				for (n=0; n<diff.nPoints(); ++n)
// 				{
// // 					parser.writeLineF("%12.4e  %12.4e  %12.4e  %12.4e\n", diff.x(n), diff.y(n), gr.x(n), gr.y(n));
// 				}
// 				parser.closeFiles();
// 			}
// 			else msg.warn("Couldn't open coefficient output file.\n");
// 		}
	}
	
	// Signal that PairPotentials have changed
	updateCheckPointData2D(DUQ::CheckPointOriginalU);
	updateCheckPointData2D(DUQ::CheckPointU);
	updateCheckPointData2D(DUQ::CheckPointDU);
	updateCheckPointData2D(DUQ::CheckPointV);
	sendSignal(DUQ::PairPotentialsUpdatedSignal);

	return CommandSuccess;
}
