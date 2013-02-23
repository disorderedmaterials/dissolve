/*
	*** Simplex Minimiser
	*** src/base/Simplex.h
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

#ifndef DUQ_SIMPLEX_H
#define DUQ_SIMPLEX_H

#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
class DUQ;

// Command pointer typedef
typedef double (DUQ::*CostFunction)(Array<double>&);

/*!
 * \brief Simplex Definition
 */
class Simplex
{
	public:
	// Constructor
	Simplex(DUQ* dUQPtr, CostFunction costFunc);
	// Simplex move types
	enum SimplexMove { ReflectionMove, ExpansionMove, OuterContractionMove, InnerContractionMove, ShrinkMove, AllMoves, nSimplexMoves };


	/*
	// Basic Data
	*/
	private:
	// Simplex move parameters
	double rho_, chi_, gamma_, sigma_;
	// Number of alpha values per vertex in Simplex
	int nAlpha_;
	// Number of vertices in Simplex
	int nVertices_;
	// Offset to apply to parameters in fitting process
	double parameterOffset_;
	// Maximum fraction by which to randomise when generating initial vertices
	double initVariation_;
	// dUQ pointer (used to call costFunction_)
	DUQ* dUQPtr_;
	// Cost function pointer
	CostFunction costFunction_;
	// Vertex array
	Array< Array<double> > vertices_;
	// Cost array
	Array<double> costs_;
	// Best alpha encountered during Simplex minimisation
	Array<double> bestAlpha_;
	// Cost of bestAlpha_
	double bestAlphaCost_;
	// Indices of best, worst, and second worst vertices in current Simplex
	int vBest_, vWorst_, vNextWorst_;
	// Maximum number of cycles to perform
	int nCycles_;
	// Number of moves per cycle to perform
	int nMoves_;
	// Convergence criterion for Simplex
	double tolerance_;
	// Counter for Simplex moves
	int moveCount_[nSimplexMoves];
	// Integer count of number of better points found by the Simplex (after minimisation)
	int betterPointsFound_;
	
	private:
	// Return (calculating if necessary) cost of supplied vertex
	double cost(Array<double>& vertex);
	// Return whether to accept  move based on supplied cost value, vertex, and temperature
	bool accept(double trialCost, int vertexId, double temperature);
	// Find extreme cost values in current Simplex
	void findExtremes(double temperature);
	// Reflect worst point in Simplex through centroid
	Array<double> reflect(Array<double>& centroid);
	// Expand Simplex about worst point
	Array<double> expand(Array<double>& centroid);
	// Contract Simplex, giving new vertex outside current polytope
	Array<double> contractOutside(Array<double>& centroid);
	// Contract Simplex, giving new vertex inside current polytope
	Array<double> contractInside(Array<double>& centroid);
	// Shrink Simplex, contracting around best point (and leaving it as-is)
	void shrink();

	public:
	// Initialise starting Simplex
	void initialise(Array<double>& initVertex, double paramOffset = 0.0, double pcntVariation = 0.05);
	// Return MSD of cost values in current Simplex
	double costMSD();
	// Return whether Simplex has converged
	bool converged();
	// Print vertex information
	void printVertexInformation();
	// Print Simplex move information
	void printMoveInformation();
	// Perform standard Simplex minimisation (at temperature specified)
	Array<double> minimise(int nCycles, int maxMoves, double tolerance, double simplexTemperature);
	// Return whether a better point was found by the Simplex
	bool betterPointFound();
};

#endif
