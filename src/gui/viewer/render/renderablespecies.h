/*
	*** Renderable - Species
	*** src/gui/viewer/render/renderablespecies.h
	Copyright T. Youngs 2019

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

#ifndef DISSOLVE_RENDER_RENDERABLESPECIES_H
#define DISSOLVE_RENDER_RENDERABLESPECIES_H

#include "gui/viewer/render/renderable.h"
#include "classes/species.h"

// Forward Declarations
class Axes;

// Renderable for Species
class RenderableSpecies : public Renderable
{
	public:
	// Constructor / Destructor
	RenderableSpecies(const Species* source, const char* objectTag);
	~RenderableSpecies();


	/*
	 * Data
	 */
	private:
	// Source data
	const Species* source_;

	private:
	// Return whether a valid data source is available (attempting to set it if not)
	bool validateDataSource();

	public:
	// Return version of data
	int dataVersion() const;


	/*
	 * Transform / Limits
	 */
	protected:
	// Transform data according to current settings
	void transformData();

	public:
	// Calculate min/max y value over specified x range (if possible in the underlying data)
	bool yRangeOverX(double xMin, double xMax, double& yMin, double& yMax);


	/*
	 * Rendering Primitives
	 */
	private:
	// Basic primitives
	Primitive* atomPrimitive_, *selectedAtomPrimitive_, *unitCellPrimitive_, *bondPrimitive_;
	// Main primitives
	Primitive* lineSpeciesPrimitive_, *lineSelectionPrimitive_, *lineInteractionPrimitive_;
	// Main assemblies
	PrimitiveAssembly speciesAssembly_, selectionAssembly_, interactionAssembly_;
	// Version at which selection primitive was created, relative to selection version
	int selectionPrimitiveVersion_;

	private:
	// Create cylinder bond between supplied atoms in specified assembly
	void createCylinderBond(PrimitiveAssembly& assembly, const SpeciesAtom* i, const SpeciesAtom* j, double radialScaling);

	protected:
	// Recreate necessary primitives / primitive assemblies for the data
	void recreatePrimitives(const View& view, const ColourDefinition& colourDefinition);
	// Send primitives for rendering
	const void sendToGL(const double pixelScaling);

	public:
	// Recreate selection Primitive
	void recreateSelectionPrimitive();
	// Clear interaction Primitive
	void clearInteractionPrimitive();
	// Recreate interaction Primitive to display drawing interaction (from existing atom to existing atom)
	void recreateDrawInteractionPrimitive(SpeciesAtom* fromAtom, SpeciesAtom* toAtom);
	// Recreate interaction Primitive to display drawing interaction (from existing atom to point)
	void recreateDrawInteractionPrimitive(SpeciesAtom* fromAtom, Vec3<double> toPoint, Element* toElement);
	// Recreate interaction Primitive to display drawing interaction (from point to point)
	void recreateDrawInteractionPrimitive(Vec3<double> fromPoint, Element* fromElement, Vec3<double> toPoint, Element* toElement);


	/*
	 * Style
	 */
	public:
	// Display Styles enum
	enum DisplayStyle { LinesStyle, SpheresStyle, nDisplayStyles };

	private:
	// Radius of free (unbound) atoms when drawing with lines
	double linesAtomRadius_;
	// Radius of atoms when drawing with spheres
	double spheresAtomRadius_;
	// Radius of bonds when drawing with spheres
	double spheresBondRadius_;

	public:
	// Return keyword for display style index
	const char* displayStyle(int id);
	// Return display style index from string
	int displayStyle(const char* s);

};

#endif
