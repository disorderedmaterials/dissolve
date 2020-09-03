/*
    *** Simplex Minimiser
    *** src/base/simplex.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "templates/array.h"
#include "templates/list.h"

// Simplex Algorithm
template <class T> class Simplex
{
    public:
    // Simplex move types
    enum SimplexMove
    {
        ReflectionMove,
        ExpansionMove,
        OuterContractionMove,
        InnerContractionMove,
        ShrinkMove,
        AllMoves,
        nSimplexMoves
    };
    // Command pointer typedef
    typedef double (T::*SimplexCostFunction)(const Array<double> &alpha);

    /*
     * Basic Data
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
    // Object (used to call costFunction_)
    T &object_;
    // Cost function pointer
    SimplexCostFunction costFunction_;
    // Vertex array
    Array<Array<double>> vertices_;
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
    double cost(Array<double> &vertex)
    {
        // Compute cold cost of vertex first, to compare against bestAlpha
        vertex -= parameterOffset_;
        double coldCost = (object_.*(costFunction_))(vertex);
        vertex += parameterOffset_;

        // Check against best point found so far
        if (coldCost < bestAlphaCost_)
        {
            bestAlpha_ = vertex;
            bestAlphaCost_ = coldCost;
            ++betterPointsFound_;
        }

        return coldCost;
    }

    // Return whether to accept move based on supplied cost value, vertex, and temperature
    bool accept(double trialCost, int vertexId, double temperature)
    {
        // First, get cost of comparison vertex
        double comparisonCost = costs_[vertexId];

        // If supplied cost is lower, then accept it regardless
        if (trialCost < comparisonCost)
            return true;
        else
        {
            // Accept with some probability...
            double deltaCost = trialCost - comparisonCost;
            if (DissolveMath::random() < exp(-deltaCost / temperature))
                return true;
        }
        return false;
    }

    // Find extreme cost values in current Simplex at specified vertex temperature
    void findExtremes(double temperature)
    {
        // Set initial values
        vBest_ = 0;
        vWorst_ = 1;
        vNextWorst_ = nVertices_ - 1;
        int n;

        // Make sure costs are up to date...
        for (n = 0; n < nVertices_; ++n)
            costs_[n] = cost(vertices_[n]);

        // Find best, worst, and nextWorst vertices as normal...
        for (n = 0; n < nVertices_; ++n)
        {
            if (costs_[n] < costs_[vBest_])
                vBest_ = n;
            else if (costs_[n] > costs_[vWorst_])
            {
                vNextWorst_ = vWorst_;
                vWorst_ = n;
            }
            else if ((costs_[n] > costs_[vNextWorst_]) && n != vBest_)
                vNextWorst_ = n;
        }

        // ...and then randomly select points in the Simplex which we will trial as new best, worst, and nextworst
        // points
        do
        {
            n = int(DissolveMath::random() * nVertices_);
        } while (n == vWorst_);
        if (DissolveMath::random() < exp(-fabs(costs_[vBest_] - costs_[n]) / temperature))
        {
            // Swap points if necessary
            if (vNextWorst_ == n)
                vNextWorst_ = vBest_;
            // else if (vWorst_ == n) vWorst_ = vBest_;
            vBest_ = n;
        }
        n = int(DissolveMath::random() * nVertices_);
        if (DissolveMath::random() < exp(-fabs(costs_[vWorst_] - costs_[n]) / temperature))
        {
            // Swap points if necessary
            if (vNextWorst_ == n)
                vNextWorst_ = vWorst_;
            else if (vBest_ == n)
                vBest_ = vWorst_;
            vWorst_ = n;
        }
        do
        {
            n = int(DissolveMath::random() * nVertices_);
        } while (n == vWorst_);
        if (DissolveMath::random() < exp(-fabs(costs_[vNextWorst_] - costs_[n]) / temperature))
        {
            // Swap points if necessary
            if (vBest_ == n)
                vBest_ = vNextWorst_;
            // else if (vWorst_ == n) vWorst_ = vNextWorst_;
            vNextWorst_ = n;
        }
    }

    // Reflect specified vertex through specified centroid
    Array<double> reflect(Array<double> &centroid)
    {
        // Reflect specified point : x(new) = (1 + rho) * y - rho * x(worst)    where y = sum x(i) / n, i != worst
        Array<double> newVertex(centroid);
        newVertex = centroid * (1.0 + rho_) - vertices_[vWorst_] * rho_;
        return newVertex;
    }

    // Expand Simplex about worst point
    Array<double> expand(Array<double> &centroid)
    {
        // Expand around highest point : x(new) = (1 + rho * chi) * xbar - rho * chi * x(n+1)    where xbar = sum(i=1,n)
        // x(i) / n
        Array<double> newVertex(centroid);
        newVertex = centroid * (1.0 + rho_ * chi_) - vertices_[vWorst_] * rho_ * chi_;
        return newVertex;
    }

    // Contract Simplex, giving new vertex outside current polytope
    Array<double> contractOutside(Array<double> &centroid)
    {
        // Contract simplex (new point outside current polytope) : x(new) = (1 + rho * gamma) * xbar - rho * gamma *
        // vertex(nalpha+1)
        Array<double> newVertex(centroid);
        newVertex = centroid * (1.0 + rho_ * gamma_) - vertices_[vWorst_] * (rho_ * gamma_);
        return newVertex;
    }

    // Contract Simplex, giving new vertex inside current polytope
    Array<double> contractInside(Array<double> &centroid)
    {
        // Contract simplex (new point inside current polytope) : x(new) = (1 - gamma) * xbar + gamma * vertex(nalpha+1)
        Array<double> newVertex(centroid);
        newVertex = centroid * (1.0 - gamma_) + vertices_[vWorst_] * gamma_;
        return newVertex;
    }

    // Shrink Simplex, contracting around best point (and leaving it as-is)
    void shrink()
    {
        // Shrink vertices of current simplex, leaving only the first (x(1)) as-is: x(n) = x(1) + sigma(x(n) - x(1)),
        // n=2,nalpha+1
        for (int n = 0; n < nVertices_; ++n)
        {
            if (n == vBest_)
                continue;
            vertices_[n] = (vertices_[n] - vertices_[vBest_]) * sigma_ + vertices_[vBest_];
        }
    }

    public:
    Simplex(T &object, SimplexCostFunction costFunc) : object_(object)
    {
        // Private variables
        nAlpha_ = 0;
        nVertices_ = 0;
        betterPointsFound_ = 0;
        costFunction_ = costFunc;

        // Set move parameters
        rho_ = 1.0;
        chi_ = 2.0;
        gamma_ = 0.5;
        sigma_ = 0.5;
    }

    // Initialise starting Simplex
    void initialise(Array<double> &initVertex, double paramOffset, double fracVariation = 0.1)
    {
        nAlpha_ = initVertex.nItems();
        nVertices_ = nAlpha_ + 1;
        bestAlpha_ = initVertex;
        betterPointsFound_ = 0;
        initVariation_ = fracVariation;
        parameterOffset_ = paramOffset;

        vertices_.initialise(nVertices_);
        costs_.initialise(nVertices_);

        // Set initial vertex and starting best cost
        vertices_[0] = initVertex;
        vertices_[0] += parameterOffset_;
        costs_[0] = cost(initVertex);
        bestAlphaCost_ = costs_[0];
    }

    // MSD of cost values in current Simplex
    double costMSD()
    {
        // Get average cost of all vertices
        double average = 0.0;
        for (int n = 0; n < nVertices_; ++n)
            average += costs_[n];
        average /= nVertices_;

        // Now calculate SD of individual costs with mean value
        double serror = 0.0;
        for (int n = 0; n < nVertices_; ++n)
            serror += (costs_[n] - average) * (costs_[n] - average);
        return sqrt(serror / nVertices_);
    }

    // Return whether Simplex has converged
    bool converged() { return (costMSD() < tolerance_); }

    // Print vertex information
    void printVertexInformation()
    {
        for (int n = 0; n < nVertices_; ++n)
            Messenger::print("[SMPLX]\t\tVertex {} has cold cost value = {:12.6e}\n", n, costs_[n]);
    }

    // Print Simplex move information
    void printMoveInformation()
    {
        Messenger::print("[SMPLX]\tIn {} moves {} reflections, {} expansions, {} contractions ({} inner, {} outer) and {} "
                         "shrinks were performed.\n",
                         moveCount_[Simplex::AllMoves], moveCount_[Simplex::ReflectionMove], moveCount_[Simplex::ExpansionMove],
                         moveCount_[Simplex::OuterContractionMove] + moveCount_[Simplex::InnerContractionMove],
                         moveCount_[Simplex::InnerContractionMove], moveCount_[Simplex::OuterContractionMove],
                         moveCount_[Simplex::ShrinkMove]);
    }

    // Perform standard Simplex minimisation (at temperature specified)
    Array<double> minimise(int nCycles, int maxMoves, double tolerance, double simplexTemperature)
    {
        // Simplex Simulated Annealing following original Nelder-Mead simplex algorithm
        // and incoporating Simulated Annealing. In all the following a positive temperature
        // implies a subtraction from the total cost function of the vertex (i.e. reducing costs)
        // while a negative temperature implies an addition to the total cost function (i.e. increasing costs)

        int n, move;
        double reflectedCost, newCost;
        Array<double> centroid(nAlpha_), reflectedVertex(nAlpha_), newVertex(nAlpha_);

        // Check nAlpha
        if (nAlpha_ == 0)
        {
            Messenger::error("Simplex appears not to have been initialised properly.\n");
            return newVertex;
        }

        nCycles_ = 0;
        nMoves_ = maxMoves;
        tolerance_ = tolerance;
        betterPointsFound_ = 0;

        // Reset move counters
        for (n = 0; n < Simplex::nSimplexMoves; ++n)
            moveCount_[n] = 0;

        // Begin Simplex Minimisation
        int cycle = 0;
        while (cycle < nCycles)
        {
            // Generate new starting vertices
            vertices_[0] = bestAlpha_;
            costs_[0] = bestAlphaCost_;

            double r;
            for (n = 1; n < nVertices_; ++n)
            {
                vertices_[n] = vertices_[0];
                r = (2.0 * DissolveMath::random()) - 1.0;
                vertices_[n][n - 1] = (vertices_[n][n - 1] - parameterOffset_) * 1.0 + initVariation_ * r;
                costs_[n] = cost(vertices_[n]);
            }

            for (move = 1; move <= nMoves_; ++move)
            {
                ++moveCount_[Simplex::AllMoves];

                // Find best, worst, and next-worst points of Simplex
                findExtremes(simplexTemperature);

                // Calculate centroid for use in simplex move routines
                centroid = 0.0;
                for (n = 0; n < nVertices_; ++n)
                    if (n != vWorst_)
                        centroid += vertices_[n];
                centroid /= nAlpha_;

                // First move attempt - Calculate reflection vertex
                reflectedVertex = reflect(centroid);
                reflectedCost = cost(reflectedVertex);

                // ... If this point is lower in cost than the next-highest vertex, but higher than the best
                // point, accept it and terminate iteration
                if (accept(reflectedCost, vNextWorst_, simplexTemperature) &&
                    (!accept(reflectedCost, vBest_, simplexTemperature)))
                {
                    vertices_[vWorst_] = reflectedVertex;
                    ++moveCount_[Simplex::ReflectionMove];
                    if (converged())
                        break;
                    continue;
                }

                // Stage 3a) - Calculate expansion point (if reflected point is lower than the best point
                // already found) Stage 3b) - Calculate contraction point (if reflected point is worse than the
                // next worst (n-1'th) point)
                if (accept(reflectedCost, vBest_, simplexTemperature))
                {
                    // Calculate expansion point
                    newVertex = expand(centroid);
                    newCost = cost(newVertex);

                    // ... If expanded point is better than reflected point, accept expanded point and
                    // terminate iteration.
                    // ... If reflected point is better (or equal to) expanded point, accept reflected point
                    // and terminate iteration
                    if (newCost < reflectedCost)
                    {
                        vertices_[vWorst_] = newVertex;
                        costs_[vWorst_] = newCost;
                        ++moveCount_[Simplex::ExpansionMove];
                        if (converged())
                            break;
                        continue;
                    }
                    else
                    {
                        vertices_[vWorst_] = reflectedVertex;
                        costs_[vWorst_] = reflectedCost;
                        ++moveCount_[Simplex::ReflectionMove];
                        if (converged())
                            break;
                        continue;
                    }
                }
                else
                {
                    // Attempt contractions... if either fails, perform a shrink and continue with next
                    // iteration
                    // ... If reflected point is bettern than worst point, but worse than next-best (n-1)
                    // point, do 'outside' contraction
                    if (!accept(reflectedCost, vNextWorst_, simplexTemperature) &&
                        accept(reflectedCost, vWorst_, simplexTemperature))
                    {
                        newVertex = contractOutside(centroid);
                        newCost = cost(newVertex);
                        if (newCost <= reflectedCost)
                        {
                            vertices_[vWorst_] = newVertex;
                            costs_[vWorst_] = newCost;
                            ++moveCount_[Simplex::OuterContractionMove];
                            if (converged())
                                break;
                            continue;
                        }
                        else
                        {
                            // Perform shrink and recalculate costs
                            shrink();
                            for (n = 0; n < nVertices_; ++n)
                                if (n != vBest_)
                                    costs_[n] = cost(vertices_[n]);
                            ++moveCount_[Simplex::ShrinkMove];
                            if (converged())
                                break;
                            continue;
                        }
                    }
                    else
                    {
                        // ...otherwise do an inside contraction
                        newVertex = contractInside(centroid);
                        newCost = cost(newVertex);
                        if (accept(newCost, vWorst_, simplexTemperature))
                        {
                            vertices_[vWorst_] = newVertex;
                            costs_[vWorst_] = newCost;
                            ++moveCount_[Simplex::InnerContractionMove];
                            if (converged())
                                break;
                            continue;
                        }
                        else
                        {
                            // Perform shrink and recalculate costs
                            shrink();
                            for (n = 0; n < nVertices_; ++n)
                                if (n != vBest_)
                                    costs_[n] = cost(vertices_[n]);
                            ++moveCount_[Simplex::ShrinkMove];
                            if (converged())
                                break;
                            continue;
                        }
                    }
                }
            }

            // Check for convergence in this cycle
            if (converged())
                break;
            ++cycle;
        }

        return bestAlpha_;
    }
    // Return whether a better point was found by the Simplex
    bool betterPointFound() { return (betterPointsFound_ > 0); }
};
