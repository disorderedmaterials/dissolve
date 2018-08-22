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

#include "classes/site.h"
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
	// Index at which the sites were last calculated for the Configuration
	int configurationIndex_;
	// Target SpeciesSite
	SpeciesSite* speciesSite_;

	public:
	// Create stack for specified Configuration and site
	bool create(Configuration* cfg, SpeciesSite* speciesSite);
	// Return target Configuration
	Configuration* configuration() const;
	// Return target SpeciesSite
	SpeciesSite* speciesSite() const;


	/*
	 * Stack
	 */
	private:
	// Whether the stack contains associate Molecule information
	bool sitesInMolecules_;
	// Whether the current stack contains local axes information
	bool sitesHaveOrientation_;
	// Basic site array (if no local axes are defined)
	Array<Site> sites_;
	// Oriented site array (if local axes are defined)
	Array<OrientedSite> orientedSites_;

	public:
	// Return number of sites in the stack
	int nSites() const;
	// Return whether the stack contains associate Molecule information
	bool sitesInMolecules() const;
	// Return whether the current stack contains local axes information
	bool sitesHaveOrientation() const;
	// Return site with index specified
	const Site& site(int index) const;
};

#endif
