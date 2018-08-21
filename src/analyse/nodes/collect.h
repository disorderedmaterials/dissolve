/*
	*** Analysis Node - Collect
	*** src/analyse/nodes/collect.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_ANALYSISCOLLECT_H
#define DISSOLVE_ANALYSISCOLLECT_H

#include "analyse/nodes/node.h"
#include "base/charstring.h"
#include "templates/reflist.h"
#include "templates/vector3.h"

// Forward Declarations
class LineParser;
class SiteContextStack;

// Analysis Node - Collect
class AnalysisCollectNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisCollectNode();
	// Destructor
	~AnalysisCollectNode();


	/*
	 * Observable Quantities
	 */
	public:
	// Observable Quantities
	enum Observable
	{
		AngleObservable,		/* 'Angle' - Angle formed between three sites */
		DistanceObservable,		/* 'Distance' - Distance between two sites */
		nObservables			/* Number of observables in list */
	};
	// Convert string to Observable
	static Observable observable(const char* s);
	// Convert Observable to string
	static const char* observable(Observable obs);


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum CollectNodeKeyword { EndCollectKeyword, XLabelKeyword, XObservableKeyword, XRangeKeyword, nCollectNodeKeywords };
	// Convert string to control keyword
	static CollectNodeKeyword collectNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* collectNodeKeyword(CollectNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Observables to bin
	Observable xObservable_, yObservable_, zObservable_;
	// Nodes (sites) to use for observable calculation
	RefList<AnalysisNode,int> xSites_, ySites_, zSites_;
	// Axis labels
	CharString xLabel_, yLabel_, zLabel_;
	// Range minima
	Vec3<double> minima_;
	// Range maxima
	Vec3<double> maxima_;
	// Bin widths
	Vec3<double> binWidths_;


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, SiteContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
