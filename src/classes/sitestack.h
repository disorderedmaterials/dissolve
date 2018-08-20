/*
	*** Site Stack
	*** src/classes/sitestack.h
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

#ifndef DISSOLVE_SITESTACK_H
#define DISSOLVE_SITESTACK_H

#include "math/matrix3.h"
#include "base/charstring.h"
#include "templates/array.h"
#include "templates/listitem.h"

// Forward Declarations
class Configuration;
class SpeciesSite;
class Molecule;

// Site Stack Definition
class SiteStack : public ListItem<SiteStack>
{
	public:
	// Constructor
	SiteStack();
	// Destructor
	~SiteStack();


	/*
	 * Target
	 */
	private:
	// Target Configuration from which we will determine sites
	Configuration* configuration_;
	// Target SpeciesSite
	SpeciesSite* site_;

	public:
	// Initialise for specified Configuration and site
	bool initialise(Configuration* cfg, SpeciesSite* site);


	/*
	 * Stack
	 */
	private:
	// Number of sites in stack
	int nSites_;
	// Whether the stack contains associate Molecule information
	bool sitesInMolecules_;
	// Whether the current stack contains local axes information
	bool sitesHaveAxes_;
	// Site origins
	Array< Vec3<double> > origins_;
	// Molecules to which sites are related (if relevant)
	Array<Molecule*> molecules_;
	// Local axes (if available)
	Array<Matrix3> axes_;

	public:
	// Return number of sites in the stack
	int nSites() const;
	// Return whether the stack contains associate Molecule information
	bool sitesInMolecules() const;
	// Return whether the current stack contains local axes information
	bool sitesHaveAxes() const;
	// Return site origins
	const Array< Vec3<double>& >& origins() const;
	// Molecules to which sites are related (if relevant)
	const Array<Molecule*>& molecules() const;
	// Local axes (if available)
	const Array<Matrix3>& axes() const;
};

#endif
