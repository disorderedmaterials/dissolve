/*
	*** Renderable - Species
	*** src/gui/viewer/render/renderablespecies.cpp
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

#include "gui/viewer/render/renderablespecies.h"
#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/view.h"
#include "data/elementcolours.h"

// Constructor
RenderableSpecies::RenderableSpecies(const Species* source, const char* objectTag) : Renderable(Renderable::SpeciesRenderable, objectTag), source_(source)
{
	// Create basic primitives
	atomPrimitive_ = createBasicPrimitive(GL_TRIANGLES, false);
	atomPrimitive_->sphere(1.0, 8, 10);
	selectedAtomPrimitive_ = createBasicPrimitive(GL_LINES, false);
	selectedAtomPrimitive_->sphere(1.1, 8, 10);
	bondPrimitive_ = createBasicPrimitive(GL_TRIANGLES, false);
	bondPrimitive_->cylinder(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1, 8);
	unitCellPrimitive_ = createBasicPrimitive(GL_LINES, false);
// 	unitCellPrimitive_->wireCube(1.0, 4, 0, 0, 0);

	// Create main primitive assemblies
	speciesPrimitive_ = createPrimitiveAssembly();
	selectionPrimitive_ = createPrimitiveAssembly();

	// Set versions
	selectionPrimitiveVersion_ = -1;
}

// Destructor
RenderableSpecies::~RenderableSpecies()
{
}

/*
 * Data
 */

// Return whether a valid data source is available (attempting to set it if not)
bool RenderableSpecies::validateDataSource()
{
	// If there is no valid source set, attempt to set it now...
	if (!source_) source_ = Species::findObject(objectTag_);

	return source_;
}

// Return version of data
const int RenderableSpecies::version() const
{
	return (source_ ? source_->version() : -99);
}

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableSpecies::transformData()
{
	if (!source_) return;

	// If the transformed data are already up-to-date, no need to do anything
	if (transformDataVersion_ == version()) return;

	// Loop over Atoms, seeking extreme x, y, and z values
	ListIterator<SpeciesAtom> atomIterator(source_->atoms());
	while (SpeciesAtom* i = atomIterator.iterate())
	{
		if (atomIterator.isFirst())
		{
			transformMin_ = i->r();
			transformMax_ = i->r();
		}
		else
		{
			if (i->r().x < transformMin_.x) transformMin_.x = i->r().x;
			else if (i->r().x > transformMax_.x) transformMax_.x = i->r().x;
			if (i->r().y < transformMin_.y) transformMin_.y = i->r().y;
			else if (i->r().y > transformMax_.y) transformMax_.y = i->r().y;
			if (i->r().z < transformMin_.z) transformMin_.z = i->r().z;
			else if (i->r().z > transformMax_.z) transformMax_.z = i->r().z;
		}
	}

	// Need to add on a little extra to the limits since the atoms have a radius
	transformMin_ -= 1.0;
	transformMax_ += 1.0;

	transformMinPositive_.x = transformMin_.x < 0.0 ? 0.01 : transformMin_.x;
	transformMinPositive_.y = transformMin_.y < 0.0 ? 0.01 : transformMin_.y;
	transformMinPositive_.z = transformMin_.z < 0.0 ? 0.01 : transformMin_.z;
	transformMaxPositive_.x = transformMax_.x < 0.0 ? 1.0 : transformMax_.x;
	transformMaxPositive_.y = transformMax_.y < 0.0 ? 1.0 : transformMax_.y;
	transformMaxPositive_.z = transformMax_.z < 0.0 ? 1.0 : transformMax_.z;

	// Update the transformed data 'version'
	transformDataVersion_ = version();
}

// Calculate min/max y value over specified x range (if possible in the underlying data)
bool RenderableSpecies::yRangeOverX(double xMin, double xMax, double& yMin, double& yMax)
{
	yMin = 0.0;
	yMax = 1.0;

	return true;
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableSpecies::recreatePrimitives(const View& view, const ColourDefinition& colourDefinition)
{
	Matrix4 A;
	const float* colour;
	const float colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };

	// Clear existing data
	speciesPrimitive_->clear();
	selectionPrimitive_->clear();

	// Set basic styling for assemblies
	speciesPrimitive_->add(true, GL_FILL);
	selectionPrimitive_->add(true, GL_LINE);

	/*
	 * Draw Atoms
	 */
	ListIterator<SpeciesAtom> atomIterator(source_->atoms());
	while (SpeciesAtom* i = atomIterator.iterate())
	{
		A.setIdentity();
		A.setTranslation(i->r());
		A.applyScaling(0.3);

		// The atom itself
		colour = ElementColours::colour(i->element());
		speciesPrimitive_->add(atomPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);

		// Label
// 		text.clear();
// 		if (atomTypeLabel) text.strcatf("%s ", i->atomType()->name());
// 		if (indexLabel) text.strcatf("%i ", i->userIndex());
// 		if (species_->highlightedIsotopologue())
// 		{
// 			iso = species_->highlightedIsotopologue()->atomTypeIsotope(i->atomType());
// 			if (iso == NULL) text.sprintf("%s [???]", i->atomType()->name());
// 			else text.strcatf("[%i-%s]", iso->A(), PeriodicTable::element(i->element()).symbol());
// 		}
// 		renderTextPrimitive(i->r(), text.get(), false, false);

		// Is the atom selected?
		if (i->isSelected())
		{
			selectionPrimitive_->add(selectedAtomPrimitive_, A, colourBlack[0], colourBlack[1], colourBlack[2], colourBlack[3]);
		}
	}

	// Draw Bonds
	Vec3<double> vij;
	double mag;
	for (SpeciesBond* b = source_->bonds().first(); b != NULL; b = b->next)
	{
		A.setIdentity();

		// Get vector between Atoms i->j and move to Bond centre
		vij = b->j()->r() - b->i()->r();
		A.setTranslation(b->i()->r()+vij*0.5);
		mag = vij.magAndNormalise();
		
		// Create rotation matrix for Bond
		A.setColumn(2, vij.x, vij.y, vij.z, 0.0);
		A.setColumn(0, vij.orthogonal(), 0.0);
		A.setColumn(1, vij * A.columnAsVec3(0), 0.0);
		A.columnMultiply(2, 0.5*mag);
		A.applyScaling(0.1, 0.1, 1.0);

		// Render half of Bond in colour of Atom j
		colour = ElementColours::colour(b->j()->element());
		speciesPrimitive_->add(bondPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);
		
		// Render half of Bond in colour of Atom i
		A.columnMultiply(2,-1.0);
		colour = ElementColours::colour(b->i()->element());
		speciesPrimitive_->add(bondPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);
	}
}

// Recreate selection Primitive
void RenderableSpecies::recreateSelectionPrimitive()
{
	if (selectionPrimitiveVersion_ == source_->atomSelectionVersion()) return;

	// Clear existing data
	selectionPrimitive_->clear();

	// Set basic styling
	selectionPrimitive_->add(true, GL_LINE);

	Matrix4 A;

	ListIterator<SpeciesAtom> atomIterator(source_->atoms());
	while (SpeciesAtom* i = atomIterator.iterate())
	{
		if (!i->isSelected()) continue;

		A.setIdentity();
		A.setTranslation(i->r());
		A.applyScaling(0.3);

		selectionPrimitive_->add(selectedAtomPrimitive_, A, 0.5, 0.5, 0.5, 1.0);
	}

	selectionPrimitiveVersion_ = source_->atomSelectionVersion();
}
