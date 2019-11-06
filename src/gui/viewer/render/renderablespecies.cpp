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
	// Set defaults
	displayStyle_ = SpheresStyle;
	linesAtomRadius_ = 0.05;
	spheresAtomRadius_ = 0.3;
	spheresBondRadius_ = 0.1;

	// Create basic primitives
	atomPrimitive_ = createPrimitive(GL_TRIANGLES, false);
	atomPrimitive_->sphere(1.0, 8, 10);
	selectedAtomPrimitive_ = createPrimitive(GL_LINES, false);
	selectedAtomPrimitive_->sphere(1.1, 8, 10);
	bondPrimitive_ = createPrimitive(GL_TRIANGLES, false);
	bondPrimitive_->cylinder(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1, 8);
	unitCellPrimitive_ = createPrimitive(GL_LINES, false);
// 	unitCellPrimitive_->wireCube(1.0, 4, 0, 0, 0);
	lineSpeciesPrimitive_ = createPrimitive(GL_LINES, true);
	lineSelectionPrimitive_ = createPrimitive(GL_LINES, true);
	lineSelectionPrimitive_->setNoInstances();
	lineInteractionPrimitive_ = createPrimitive(GL_LINES, true);
	lineInteractionPrimitive_->setNoInstances();

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
int RenderableSpecies::dataVersion() const
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
	if (transformDataVersion_ == dataVersion()) return;

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
	transformDataVersion_ = dataVersion();
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

// Create cylinder bond between supplied atoms in specified assembly
void RenderableSpecies::createCylinderBond(PrimitiveAssembly& assembly, const SpeciesAtom* i, const SpeciesAtom* j, double radialScaling)
{
	Matrix4 A;

	// Get vector between Atoms i->j and move to Bond centre
	Vec3<double> vij = j->r() - i->r();
	A.setTranslation(i->r()+vij*0.5);
	const double mag = vij.magAndNormalise();

	// Create rotation matrix for Bond
	A.setColumn(2, vij.x, vij.y, vij.z, 0.0);
	A.setColumn(0, vij.orthogonal(), 0.0);
	A.setColumn(1, vij * A.columnAsVec3(0), 0.0);
	A.columnMultiply(2, 0.5*mag);
	A.applyScaling(radialScaling, radialScaling, 1.0);

	// Render half of Bond in colour of Atom j
	const float* colour = ElementColours::colour(j->element());
	assembly.add(bondPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);

	// Render half of Bond in colour of Atom i
	A.columnMultiply(2,-1.0);
	colour = ElementColours::colour(i->element());
	assembly.add(bondPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);
}

// Recreate necessary primitives / primitive assemblies for the data
void RenderableSpecies::recreatePrimitives(const View& view, const ColourDefinition& colourDefinition)
{
	Matrix4 A;
	const GLfloat* colour;
	const GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };

	// Clear existing data
	lineSpeciesPrimitive_->forgetAll();
	lineSelectionPrimitive_->forgetAll();
	speciesAssembly_.clear();
	selectionAssembly_.clear();

	// Render according to the current displayStyle
	if (displayStyle_ == LinesStyle)
	{
		// Set basic styling and content for assemblies
		speciesAssembly_.add(false, GL_LINE);
		speciesAssembly_.add(lineSpeciesPrimitive_, A);

		// Draw Atoms
		ListIterator<SpeciesAtom> atomIterator(source_->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			// Only draw the atom if it has no bonds, in which case draw it as a 'cross'
			if (i->nBonds() != 0) continue;

			const Vec3<double> r = i->r();
			colour = ElementColours::colour(i->element());

			lineSpeciesPrimitive_->line(r.x - linesAtomRadius_, r.y, r.z, r.x + linesAtomRadius_, r.y, r.z, colour);
			lineSpeciesPrimitive_->line(r.x, r.y - linesAtomRadius_, r.z, r.x, r.y + linesAtomRadius_, r.z, colour);
			lineSpeciesPrimitive_->line(r.x, r.y, r.z - linesAtomRadius_, r.x, r.y, r.z + linesAtomRadius_, colour);
		}

		// Draw bonds
		for (SpeciesBond* b = source_->bonds().first(); b != NULL; b = b->next())
		{
			// Determine half delta i-j for bond
			const Vec3<double> ri = b->i()->r();
			const Vec3<double> rj = b->j()->r();
			const Vec3<double> dij = (rj - ri) * 0.5;

			// Draw bond halves
			lineSpeciesPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z, ElementColours::colour(b->i()->element()));
			lineSpeciesPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z, ElementColours::colour(b->j()->element()));
		}
	}
	else if (displayStyle_ == SpheresStyle)
	{
		// Set basic styling for assemblies
		speciesAssembly_.add(true, GL_FILL);
		selectionAssembly_.add(true, GL_LINE);

		// Draw Atoms
		ListIterator<SpeciesAtom> atomIterator(source_->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			A.setIdentity();
			A.setTranslation(i->r());
			A.applyScaling(spheresAtomRadius_);

			// The atom itself
			colour = ElementColours::colour(i->element());
			speciesAssembly_.add(atomPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);

			// Is the atom selected?
			if (i->isSelected())
			{
				selectionAssembly_.add(selectedAtomPrimitive_, A, colourBlack[0], colourBlack[1], colourBlack[2], colourBlack[3]);
			}
		}

		// Draw bonds
		for (SpeciesBond* b = source_->bonds().first(); b != NULL; b = b->next()) createCylinderBond(speciesAssembly_, b->i(), b->j(), spheresBondRadius_);
	}
}

// Send primitives for rendering
const void RenderableSpecies::sendToGL(const double pixelScaling)
{
	// Set appropriate lighting for the species and interaction assemblies
	if (displayStyle_ == LinesStyle) glDisable(GL_LIGHTING);
	else glEnable(GL_LIGHTING);
	speciesAssembly_.sendToGL(pixelScaling);
	interactionAssembly_.sendToGL(pixelScaling);

	// Selection assembly is always drawn with no lighting as it comprises only of lines
	glDisable(GL_LIGHTING);
	selectionAssembly_.sendToGL(pixelScaling);
}

// Recreate selection Primitive
void RenderableSpecies::recreateSelectionPrimitive()
{
	if (selectionPrimitiveVersion_ == source_->atomSelectionVersion()) return;

	// Clear existing data
	selectionAssembly_.clear();
	lineSelectionPrimitive_->forgetAll();

	const GLfloat* colour;
	Matrix4 A;
	const Atom* i;
	const SpeciesBond* b;

	if (displayStyle_ == LinesStyle)
	{
		// Set basic styling for assemblies
		selectionAssembly_.add(false, GL_LINE);
		selectionAssembly_.add(LineStyle(3.0));
		selectionAssembly_.add(lineSelectionPrimitive_, A);

		// Draw selection
		ListIterator<SpeciesAtom> atomIterator(source_->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			// If not selected, continue
			if (!i->isSelected()) continue;

			// Get element colour
			colour = ElementColours::colour(i->element());

			// If the atom has no bonds, draw it as a 'cross', otherwise render all bond halves
			if (i->nBonds() == 0)
			{
				const Vec3<double> r = i->r();

				lineSelectionPrimitive_->line(r.x - linesAtomRadius_, r.y, r.z, r.x + linesAtomRadius_, r.y, r.z, colour);
				lineSelectionPrimitive_->line(r.x, r.y - linesAtomRadius_, r.z, r.x, r.y + linesAtomRadius_, r.z, colour);
				lineSelectionPrimitive_->line(r.x, r.y, r.z - linesAtomRadius_, r.x, r.y, r.z + linesAtomRadius_, colour);
			}
			else
			{
				// Draw all bonds from this atom
				const PointerArray<SpeciesBond>& bonds = i->bonds();
				for (int n=0; n<bonds.nItems(); ++n, b = bonds.at(n))
				{
					const Vec3<double> ri = i->r();
					const Vec3<double> dij = (b->partner(i)->r() - ri) * 0.5;

					// Draw bond halves
					lineSelectionPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z, colour);
				}
			}
		}
	}
	else if (displayStyle_ == SpheresStyle)
	{
		// Set basic styling
		selectionAssembly_.add(true, GL_LINE);

		ListIterator<SpeciesAtom> atomIterator(source_->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			if (!i->isSelected()) continue;

			A.setIdentity();
			A.setTranslation(i->r());
			A.applyScaling(spheresAtomRadius_);

			selectionAssembly_.add(selectedAtomPrimitive_, A, 0.5, 0.5, 0.5, 1.0);
		}
	}

	selectionPrimitiveVersion_ = source_->atomSelectionVersion();
}

// Clear interaction Primitive
void RenderableSpecies::clearInteractionPrimitive()
{
	interactionAssembly_.clear();
	lineInteractionPrimitive_->forgetAll();
}

// Recreate interaction Primitive to display drawing interaction (from existing atom to existing atom)
void RenderableSpecies::recreateDrawInteractionPrimitive(SpeciesAtom* fromAtom, SpeciesAtom* toAtom)
{
	// Clear existing data
	clearInteractionPrimitive();

	Matrix4 A;

	// Render based on the current drawing style
	if (displayStyle_ == LinesStyle)
	{
		// Set basic styling for assembly
		interactionAssembly_.add(false, GL_LINE);
		interactionAssembly_.add(lineInteractionPrimitive_, A);

		// Determine half delta i-j for bond
		const Vec3<double> ri = fromAtom->r();
		const Vec3<double> rj = toAtom->r();
		const Vec3<double> dij = (rj - ri) * 0.5;

		// Draw bond halves
		lineInteractionPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z, ElementColours::colour(fromAtom->element()));
		lineInteractionPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z, ElementColours::colour(toAtom->element()));
	}
	else if (displayStyle_ == SpheresStyle)
	{
		// Set basic styling for assembly
		interactionAssembly_.add(true, GL_FILL);

		// Draw the temporary bond between the atoms
		createCylinderBond(interactionAssembly_, fromAtom, toAtom, spheresBondRadius_);
	}
}

// Recreate interaction Primitive to display drawing interaction (from existing atom to point)
void RenderableSpecies::recreateDrawInteractionPrimitive(SpeciesAtom* fromAtom, Vec3<double> toPoint, Element* toElement)
{
	// Clear existing data
	clearInteractionPrimitive();

	Matrix4 A;

	// Temporary SpeciesAtom
	static SpeciesAtom j;
	j.setElement(toElement);
	j.setCoordinates(toPoint);

	// Render based on the current drawing style
	if (displayStyle_ == LinesStyle)
	{
		// Set basic styling for assembly
		interactionAssembly_.add(false, GL_LINE);
		interactionAssembly_.add(lineInteractionPrimitive_, A);

		// Determine half delta i-j for bond
		const Vec3<double> ri = fromAtom->r();
		const Vec3<double> rj = j.r();
		const Vec3<double> dij = (rj - ri) * 0.5;

		// Draw bond halves
		lineInteractionPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z, ElementColours::colour(fromAtom->element()));
		lineInteractionPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z, ElementColours::colour(j.element()));
	}
	else if (displayStyle_ == SpheresStyle)
	{
		// Set basic styling for assembly
		interactionAssembly_.add(true, GL_FILL);

		// Draw the temporary atom
		A.setTranslation(j.r());
		A.applyScaling(spheresAtomRadius_);
		const float* colour = ElementColours::colour(j.element());
		interactionAssembly_.add(atomPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);

		// Draw the temporary bond between the atoms
		createCylinderBond(interactionAssembly_, fromAtom, &j, spheresBondRadius_);
	}
}

// Recreate interaction Primitive to display drawing interaction (from point to point)
void RenderableSpecies::recreateDrawInteractionPrimitive(Vec3<double> fromPoint, Element* fromElement, Vec3<double> toPoint, Element* toElement)
{
	// Clear existing data
	clearInteractionPrimitive();

	Matrix4 A;

	// Temporary SpeciesAtoms
	static SpeciesAtom i, j;
	i.setElement(fromElement);
	i.setCoordinates(fromPoint);
	j.setElement(toElement);
	j.setCoordinates(toPoint);

	// Render based on the current drawing style
	if (displayStyle_ == LinesStyle)
	{
		// Set basic styling for assembly
		interactionAssembly_.add(false, GL_LINE);
		interactionAssembly_.add(lineInteractionPrimitive_, A);

		// Determine half delta i-j for bond
		const Vec3<double> ri = i.r();
		const Vec3<double> rj = j.r();
		const Vec3<double> dij = (rj - ri) * 0.5;

		// Draw bond halves
		lineInteractionPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z, ElementColours::colour(i.element()));
		lineInteractionPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z, ElementColours::colour(j.element()));
	}
	else if (displayStyle_ == SpheresStyle)
	{
		// Set basic styling for assembly
		interactionAssembly_.add(true, GL_FILL);

		// Draw the temporary atoms
		A.setTranslation(i.r());
		A.applyScaling(spheresAtomRadius_);
		const float* colour = ElementColours::colour(i.element());
		interactionAssembly_.add(atomPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);

		A.setIdentity();
		A.setTranslation(j.r());
		A.applyScaling(spheresAtomRadius_);
		colour = ElementColours::colour(j.element());
		interactionAssembly_.add(atomPrimitive_, A, colour[0], colour[1], colour[2], colour[3]);

		// Draw the temporary bond between the atoms
		createCylinderBond(interactionAssembly_, &i, &j, spheresBondRadius_);
	}
}

/*
 * Style
 */

// Display Style Keywords
const char* SpeciesDisplayStyleKeywords[] = { "Lines", "Spheres" };

// Convert display style index to text string
const char* RenderableSpecies::displayStyle(int id)
{
	if ((id < 0) || (id >= RenderableSpecies::nDisplayStyles)) return "INVALID_STYLE";

	return SpeciesDisplayStyleKeywords[id];
}

// Convert text string to display style index
int RenderableSpecies::displayStyle(const char* s)
{
	for (int n=0; n<nDisplayStyles; ++n) if (DissolveSys::sameString(s, SpeciesDisplayStyleKeywords[n])) return (RenderableSpecies::DisplayStyle) n;

	return -1;
}
