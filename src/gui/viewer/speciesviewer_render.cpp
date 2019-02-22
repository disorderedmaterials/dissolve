/*
	*** Species Viewer - Render
	*** src/gui/viewer/speciesviewer_render.cpp
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

#include "gui/viewer/speciesviewer.hui"

// Perform post-initialisation operations
void SpeciesViewer::postInitialiseGL()
{
}

// Perform post-resize operations
void SpeciesViewer::postResizeGL()
{
}

// Render content
void SpeciesViewer::render(int xOffset, int yOffset)
{
	Matrix4 A;
	Vec3<double> v;
	float colour[4];
	const float *col;

	// Check pointer is valid
	if (species_ == NULL) return;

	// Draw Atoms
	CharString text(128);
	Isotope* iso;
	GLfloat TEMPORARYCOLOUR[4] = { 0.0, 0.0, 0.0, 1.0 };
	bool atomTypeLabel = false; //mainWindow_->ui.ViewAtomTypeCheck->isChecked();
	bool indexLabel = false; //mainWindow_->ui.ViewIndexCheck->isChecked();
	for (SpeciesAtom* i = species_->atoms().first(); i != NULL; i = i->next)
	{
		A.setTranslation(i->r());

		// The Atom itself
		// TODO Implement ElemnetColour class, based on Elements.
// 		renderPrimitive(&spherePrimitive03_, TEMPORARYCOLOUR, A);

		// Label
		text.clear();
		if (atomTypeLabel) text.strcatf("%s ", i->atomType()->name());
		if (indexLabel) text.strcatf("%i ", i->userIndex());
// 		if (species_->highlightedIsotopologue())
// 		{
// 			iso = species_->highlightedIsotopologue()->atomTypeIsotope(i->atomType());
// 			if (iso == NULL) text.sprintf("%s [???]", i->atomType()->name());
// 			else text.strcatf("[%i-%s]", iso->A(), PeriodicTable::element(i->element()).symbol());
// 		}
// 		renderTextPrimitive(i->r(), text.get(), false, false);
	}

	// Draw Atom Selection
	for (RefListItem<SpeciesAtom,bool>* ri = species_->selectedAtoms().first(); ri != NULL; ri = ri->next)
	{
		A.setTranslation(ri->item->r());
		// TODO - Use proper element colour
// 		col = PeriodicTable::element(ri->item->element()).colour();
// 		colour[0] = col[0];
// 		colour[1] = col[1];
// 		colour[2] = col[2];
// 		colour[3] = 0.5;
		colour[0] = 0.0;
		colour[1] = 0.0;
		colour[2] = 0.0;
		colour[3] = 0.5;
// 		renderPrimitive(&spherePrimitive04_, colour, A);
	}

	// Draw Bonds
	Vec3<double> vij;
	double mag;
	for (SpeciesBond* b = species_->bonds().first(); b != NULL; b = b->next)
	{
		// Get vector between Atoms i->j and move to Bond centre
		vij = b->j()->r() - b->i()->r();
		A.setTranslation(b->i()->r()+vij*0.5);
		mag = vij.magAndNormalise();
		
		// Create rotation matrix for Bond
		A.setColumn(2, vij.x, vij.y, vij.z, 0.0);
		A.setColumn(0, vij.orthogonal(), 0.0);
		A.setColumn(1, vij * A.columnAsVec3(0), 0.0);
		A.columnMultiply(2, 0.5*mag);

		// Render half of Bond in colour of Atom j
		// TODO Use proper element colour
//  		renderPrimitive(&cylinderPrimitive_, PeriodicTable::element(b->j()->element()).colour(), A);
// 		renderPrimitive(&cylinderPrimitive_, colour, A);
		
		// Render half of Bond in colour of Atom i
		A.columnMultiply(2,-1.0);
		// TODO Use proper element colour
// 		renderPrimitive(&cylinderPrimitive_, PeriodicTable::element(b->i()->element()).colour(), A);
// 		renderPrimitive(&cylinderPrimitive_, colour, A);
	}
	
	// If a GrainDefinition is selected, add on highlight to involved atoms
// 	if (species_->highlightedGrain())
// 	{
// 		A.setIdentity();
// 		A.applyScaling(1.1,1.1,1.1);
// 		GLfloat colour[4] = {0.0, 0.0, 0.0, 0.6};
// 		SpeciesAtom* i;
// 		for (RefListItem<SpeciesAtom,int>* ri = species_->highlightedGrain()->atoms(); ri != NULL; ri = ri->next)
// 		{
// 			i = ri->item;
// 			A.setTranslation(i->r());
// // 			colour[0] = PeriodicTable::element(i->element()).colour()[0];
// // 			colour[1] = PeriodicTable::element(i->element()).colour()[1];
// // 			colour[2] = PeriodicTable::element(i->element()).colour()[2];
// 			renderPrimitive(&spherePrimitive03_, colour, A, GL_LINE);
// 		}
// 	}
}

/*
 * Set source (Species)
 */
void SpeciesViewer::setSource(Species *sp)
{
	species_ = sp;
}

/*
 * Set draw style
 */
void SpeciesViewer::setDrawStyle(SpeciesViewer::DrawStyle style)
{
	drawStyle_ = style;
	update();
}

/*
 * Change draw style
 */
void SpeciesViewer::changeDrawStyle(int style)
{
	drawStyle_ = (SpeciesViewer::DrawStyle) style;
	update();
}
