/*
	*** dUQ - Partials (Simple Method)
	*** src/main/partials_simple.cpp
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

/*!
 * \brief Calculate partials
 * \details Calculate partial RDFs using straight Atom data.
 */
bool DUQ::calculatePartialsSimple(Configuration& cfg)
{
	msg.print("Calculating partial RDFs (simple double-loop)...\n");

	// Variables
	int n, m, nTypes, typeI, typeJ, i, j, nPoints;

	// Construct local arrays of atom type positions
	msg.printVerbose("Constructing local arrays.\n");
	nTypes = typeIndex_.nItems();
	Vec3<double>* r[nTypes];
	int maxr[nTypes], nr[nTypes];
	int* binss[nTypes], *bins;
	n = 0;
	for (AtomTypeData* atd = typeIndex_.first(); atd != NULL; atd = atd->next)
	{
		maxr[n] = atd->population();
		nr[n] = 0;
		r[n] = new Vec3<double>[maxr[n]];
		binss[n] = new int[maxr[n]];
		++n;
	}

	// Loop over Atoms and construct arrays
	Atom* atoms = cfg.atoms();
	for (n=0; n<cfg.nAtoms(); ++n)
	{
		m = atoms[n].atomTypeIndex();
		r[m][nr[m]++] = atoms[n].r();
	}

	msg.printVerbose("Ready..\n");

	// Loop over assigned Atoms
	Vec3<double> centre, *ri, *rj, mim;
	int* histogram;
	double rbin = 1.0 / rdfBinWidth_;
	const Box* box = configuration_.box();

	int start = Comm.interleavedLoopStart(DUQComm::World);
	int stride = Comm.interleavedLoopStride(DUQComm::World);
	
	// Self terms
	for (typeI = 0; typeI<nTypes; ++typeI)
	{
		ri = r[typeI];
		histogram = partialRDFMatrix_.ref(typeI,typeI).histogram();
		bins = binss[typeI];
		nPoints = partialRDFMatrix_.ref(typeI,typeI).nBins();
		for (i=start; i < maxr[typeI]; i += stride)
		{
			centre = ri[i];
			for (j = i+1; j < maxr[typeI]; ++j)
			{
				bins[j] = box->minimumDistance(centre, ri[j]) * rbin;
// 					++count;
			}
			for (j = i+1; j < maxr[typeI]; ++j)
			{
				if (bins[j] < nPoints) ++histogram[bins[j]];
			}
		}
// 		printf("For types %i-%i, count = %i\n", typeI, typeI, count);
	}

	// Cross terms
	for (typeI = 0; typeI<nTypes; ++typeI)
	{
		ri = r[typeI];
		for (typeJ = 0; typeJ<nTypes; ++typeJ)
		{
			// Skip if typeI == typeJ, or if the number of atoms in typeI is greater than typeJ (since it is less efficient)
			if (typeI == typeJ) continue;
			if (nr[typeI] > nr[typeJ]) continue;
			if ((nr[typeI] == nr[typeJ]) && (typeI > typeJ)) continue;

			rj = r[typeJ];
			histogram = partialRDFMatrix_.ref(typeI,typeJ).histogram();
			bins = binss[typeJ];
			nPoints = partialRDFMatrix_.ref(typeI,typeJ).nBins();
			for (i=start; i < maxr[typeI]; i += stride)
			{
				centre = ri[i];
				// TODO Are two separate loops really faster here (and above?)
				for (j = 0; j < maxr[typeJ]; ++j) bins[j] = box->minimumDistance(centre, rj[j]) * rbin;
				for (j = 0; j < maxr[typeJ]; ++j) if (bins[j] < nPoints) ++histogram[bins[j]];
			}
// 			printf("For types %i-%i count = %i, time = %s\n", typeI, typeJ, count, timer.timeString());
		}
	}

	return true;
}
