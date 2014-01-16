/*
	*** dUQ - Test Commands
	*** src/lib/main/commands_test.cpp
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
#include "base/comms.h"
#include "base/sysfunc.h"
#include "classes/atom.h"
#include "classes/box.h"
#include <string.h>

/*!
 * \brief Test Routine
 * \details Developers only - Performs some kind of test code
 */
CommandReturnValue DUQ::commandTest(Configuration& cfg)
{
	// Test FT
// 	Data2D test;
// 	int npoints = 1000;
// 	for (int n=0; n<npoints; ++n)
// 	{
// 		double x = (double(n)/double(npoints)) * (2*PI);
// 		test.addPoint(x, cos(x)+cos(3.0*x));
// 	}
// 	test.save("test.txt");
// 	Data2D ft = test, another;
// 	ft.interpolate();
// 	for (int n=0; n<npoints; ++n)
// 	{
// 		double x = n*0.001;
// 		double y = ft.interpolated(x);
// 		another.addPoint(x, y);
// 	}
// 	another.save("interp.txt");
// 	ft.forwardTransformReal();
// 	ft.save("test.sq");

	// Smooth totalRDF_...
// 	Data2D test = totalRDF_;
// 	test.smooth(3);
// 	test.save("smooth3.txt");
// 	test.forwardTransformReal();
// 	test.save("smooth3.sq");
// 	test = totalRDF_;
// 	test.smooth(5);
// 	test.save("smooth5.txt");
// 	test.forwardTransformReal();
// 	test.save("smooth5.sq");

	int typeI, typeJ;
	for (typeI = 0; typeI < typeIndex_.nItems(); ++typeI)
	{
		for (typeJ = typeI;  typeJ < typeIndex_.nItems(); ++typeJ)
		{
			// Grab references to original and modified data
			Data2D& sq = workingSQMatrixA_.ref(typeI, typeJ);

			sq.save(Dnchar::string("mod%s.txt", partialSQMatrix_.ref(typeI, typeJ).name()));
			Data2D gr = sq;
			gr.transformSQ(atomicDensity(), windowFunction_);
			gr.arrayY() += 1.0;
			gr.save(Dnchar::string("mod%sft.txt", partialSQMatrix_.ref(typeI, typeJ).name()));
			
			// Calculate correlation function for S(Q)
			Data2D cr = sq;
			cr.correlateSQ(atomicDensity());
// 			cr.save("cr.txt");
			
			// Calculate PP from Hypernetted chain and Percus-Yevick theories
			Data2D hnc, py, pmf;
			double y;
			for (int n=0; n<cr.nPoints(); ++n)
			{
				// HNC
				y = gr.y(n) - cr.y(n) - 1.0 + log(gr.y(n));
				hnc.addPoint(cr.x(n), y);
				
				// Percus-Yevick
				y = log(1.0 - cr.y(n)/gr.y(n));
				py.addPoint(cr.x(n), y);
				
				// PMF
				y = AVOGADRO * -1.3806488e-23 * temperature_ * log(gr.y(n)) / 1000.0;
				pmf.addPoint(gr.x(n), y);
			}
			hnc.save(Dnchar::string("mod%s-hnc.txt", partialSQMatrix_.ref(typeI, typeJ).name()));
			py.save(Dnchar::string("mod%s-py.txt", partialSQMatrix_.ref(typeI, typeJ).name()));
			pmf.save(Dnchar::string("mod%s-pmf.txt", partialSQMatrix_.ref(typeI, typeJ).name()));
		}
	}
	
// 	// Arbitrarily extend S(Q)
// 	Data2D gr, sq = samples_[0]->referenceFQ();
// 	sq.arrayY() /= 6.2;
// 	for (int n=0; n<sq.nPoints(); ++n) if (sq.x(n) > 11.14) sq.setY(n, 0.0);
// 	for (int n=0; n<1000; ++n) sq.addPoint( (sq.x(1)-sq.x(0)) + sq.arrayX().last(), 0.0);
// 	sq.arrayY() += 1.0;
// 	sq.save("sq.txt");
// 	gr = sq;
// 	gr.arrayY() -= 1.0;
// 	gr.transformSQ(atomicDensity());
// 	gr.arrayY() += 1.0;
// 	for (int n=0; n<gr.nPoints(); ++n) if (gr.x(n) < 3.23) gr.setY(n, 0.0);
// 	gr.save("gr.txt");

// 	// Iterate between S(Q) and g(r)
// 	for (int n=0; n<5; ++n)
// 	{
// 		gr = sq;
// 		gr.transformSQ(atomicDensity());
// 		// Cheat
// 		for (int m=0; m<6; ++m) gr.setY(m, -6.0);
// 		Dnchar s(-1, "iteration%i.rdf", n);
// 		for (int m=gr.nPoints()/2; m < gr.nPoints(); ++m) gr.setY(m, 0.0);
// 		gr.save(s);
// 		
// 		sq = gr;
// 		sq.transformRDF(atomicDensity());
// 		s.sprintf("iteration%i.sq", n);
// 		for (int m=sq.nPoints()/2; m < sq.nPoints(); ++m) sq.setY(m, 0.0);
// 		sq.save(s);
// 	}

// 	// Calculate correlation function for S(Q)
// 	Data2D cr = sq;
// 	cr.correlateSQ(atomicDensity());
// 	cr.save("cr.txt");
// 	
// 	// Calculate PP from Hypernetted chain and Percus-Yevick theories
// 	Data2D hnc, py;
// 	double y;
// 	for (int n=0; n<cr.nPoints(); ++n)
// 	{
// 		// HNC
// 		y = gr.y(n) - cr.y(n) - 1.0 + log(gr.y(n));
// 		hnc.addPoint(cr.x(n), y);
// 		
// 		// Percus-Yevick
// 		y = log(1.0 - cr.y(n)/gr.y(n));
// 		py.addPoint(cr.x(n), y);
// 	}
// 	hnc.save("hnc.txt");
// 	py.save("py.txt");
	return CommandSuccess;
}

/*!
 * \brief Test Energy Calculation Routines
 * \details Calculate the energy of the system using the simplest (and slowest) method possible on each process
 */
CommandReturnValue DUQ::commandTestEnergy(Configuration& cfg)
{
	msg.print("Testing total energy calculation...\n");
	
	msg.print("Test energy is %f\n", totalEnergyTest(cfg));
}
