/*
	*** dUQ - CheckPoint
	*** src/lib/main/checkpoint.cpp
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

/*!
 * \brief Setup CheckPoint lists for Data2D types
 * \details XXX
 */
void DUQ::setupCheckPointData()
{
	// Master only
	if (Comm.slave()) return;

	// Clear existing data
	for (int n=0; n<DUQ::nCheckPointData2DTypes; ++n) checkPointData2D_[n].clear();

	// PairPotentials
	msg.print("--> PairPotentials...\n");
	for (PairPotential* pp = pairPotentials_.first(); pp != NULL; pp = pp->next)
	{
		checkPointData2D_[DUQ::CheckPointOriginalU].add()->setSource(pp->originalU(), "Energy (Original)", true);
		checkPointData2D_[DUQ::CheckPointDU].add()->setSource(pp->dU(), "Force (Full)");
		checkPointData2D_[DUQ::CheckPointU].add()->setSource(pp->u(), "Energy (Full)");
		checkPointData2D_[DUQ::CheckPointV].add()->setSource(pp->v(), "Energy (Perturbation)");
	}

	// Unweighted S(Q) data
	msg.print("--> Unweighted S(Q)...\n");
	for (int n=0; n<typeIndex_.nItems(); ++n)
	{
		for (int m=n; m<typeIndex_.nItems(); ++m)
		{
			checkPointData2D_[DUQ::CheckPointUnweightedSQ].add()->setSource(partialSQMatrix_.ptr(n,m), "Simulated");
		}
	}

	// Sample F(Q) data
	msg.print("--> Total F(Q)...\n");
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		// Add simulated data
		checkPointData2D_[DUQ::CheckPointFQ].add()->setSource(sam->totalFQ(), "Simulated");

		// Add static reference data
		checkPointData2D_[DUQ::CheckPointFQ].add()->setSource(sam->referenceFQ(), "Reference", true);
		
		// Add difference data
		checkPointData2D_[DUQ::CheckPointFQ].add()->setSource(sam->differenceFQ(), "Difference");
	}

	// Unweighted partial g(r) data
	msg.print("--> Partial g(r)...\n");
	for (int n=0; n<typeIndex_.nItems(); ++n)
	{
		for (int m=n; m<typeIndex_.nItems(); ++m)
		{
			checkPointData2D_[DUQ::CheckPointUnweightedGR].add()->setSource(partialRDFMatrix_.ref(n,m).normalisedData(), "Simulated");
		}
	}

	// Neutron-weighted total g(r)
	msg.print("--> Total F(Q)...\n");
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		checkPointData2D_[DUQ::CheckPointTotalGR].add()->setSource(sam->totalGR(), "Simulated");
	}

	// Configuration data
	checkPointConfiguration_.setSource(configuration_);
	
	// Energy
	checkPointData2D_[DUQ::CheckPointEnergy].add()->setSource(energyData_, "Total");
}

/*!
 * \brief Update CheckPoint data for specified type
 * \details XXX
 */
void DUQ::updateCheckPointData2D(DUQ::CheckPointData2D type)
{
	// Master only
	if (Comm.slave()) return;

	checkPointMutex_.lock();
	
	// Loop over items in specified list...
	for (CheckPoint<Data2D>* cp = checkPointData2D_[type].first(); cp != NULL; cp = cp->next) cp->update();

	checkPointMutex_.unlock();
}

/*!
 * \brief Return number of Data2D items in specified list
 */
int DUQ::nCheckPointData2D(DUQ::CheckPointData2D type)
{
	// Master only
	if (Comm.slave()) return 0;

	checkPointMutex_.lock();
	int result = checkPointData2D_[type].nItems();
	checkPointMutex_.unlock();
	return result;
}

/*!
 * \brief Return pointer to nth Data2D of specified type
 */
CheckPoint<Data2D>* DUQ::checkPointData2D(DUQ::CheckPointData2D type, int n)
{
	// Master only
	if (Comm.slave()) return NULL;

	checkPointMutex_.lock();
	CheckPoint<Data2D>* cp = checkPointData2D_[type].item(n);
	checkPointMutex_.unlock();
	return cp;
}

/*!
 * \brief Return pointer to Configuration CheckPoint data
 */
CheckPoint<Configuration>* DUQ::checkPointConfiguration()
{
	// Master only
	if (Comm.slave()) return NULL;

	return &checkPointConfiguration_;
}
