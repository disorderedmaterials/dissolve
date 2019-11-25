/*
	*** Species Widget - Functions 
	*** src/gui/viewer/specieswidget_funcs.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/specieswidget.h"
#include "gui/widgets/elementselector.hui"
#include "procedure/nodes/addspecies.h"
#include "procedure/nodes/box.h"
#include "classes/coredata.h"
#include "classes/empiricalformula.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include <QButtonGroup>

// Constructor
SpeciesWidget::SpeciesWidget(QWidget* parent) : QWidget(parent)
{
	// Set up our UI
	ui_.setupUi(this);

	// Create a button group for the interaction modes
	QButtonGroup* group = new QButtonGroup;
	group->addButton(ui_.InteractionViewButton);
	group->addButton(ui_.InteractionDrawButton);

	// Connect signals / slots
	connect(ui_.SpeciesView, SIGNAL(dataModified()), this, SLOT(notifyDataModified()));
	connect(ui_.SpeciesView, SIGNAL(styleModified()), this, SLOT(notifyStyleModified()));
	connect(ui_.SpeciesView, SIGNAL(atomSelectionChanged()), this, SLOT(updateStatusBar()));
	connect(ui_.SpeciesView, SIGNAL(interactionModeChanged()), this, SLOT(updateStatusBar()));

	// Make sure our controls are consistent with the underlying viewer / data
	updateToolbar();
	updateStatusBar();
}

// Destructor
SpeciesWidget::~SpeciesWidget()
{
}

// Set main CoreData pointer
void SpeciesWidget::setCoreData(CoreData* coreData)
{
	coreData_ = coreData;
}

/*
 * UI
 */

// Notify that the style of displayed data in the underlying viewer has changed
void SpeciesWidget::notifyStyleModified()
{
	emit(styleModified());
}

// Notify that the displayed data in the underlying viewer has changed
void SpeciesWidget::notifyDataModified()
{
	emit(dataModified());
}

// Post redisplay in the underlying view
void SpeciesWidget::postRedisplay()
{
	ui_.SpeciesView->postRedisplay();
}

// Update toolbar to reflect current viewer state
void SpeciesWidget::updateToolbar()
{
	// Set current interaction mode
	switch (speciesViewer()->interactionMode())
	{
		case (SpeciesViewer::DefaultInteraction):
			ui_.InteractionViewButton->setChecked(true);
			break;
		case (SpeciesViewer::DrawInteraction):
			ui_.InteractionDrawButton->setChecked(true);
			break;
	}

	// Set drawing element symbol
	ui_.InteractionDrawElementButton->setText(speciesViewer()->drawElement()->symbol());

	// Set checkable buttons
	ui_.ViewAxesVisibleButton->setChecked(speciesViewer()->axesVisible());
	ui_.ViewSpheresButton->setChecked(speciesViewer()->renderableDrawStyle() != RenderableSpecies::LinesStyle);
}

// Update status bar
void SpeciesWidget::updateStatusBar()
{
	// Get displayed Species
	const Species* sp = speciesViewer()->species();

	// Set interaction mode text
	ui_.ModeLabel->setText(speciesViewer()->interactionModeText());

	// Set / update empirical formula for the Species and its current atom selection
	ui_.FormulaLabel->setText(sp ? EmpiricalFormula::formula(sp, true) : "--");
	ui_.SelectionLabel->setText(sp && (sp->nSelectedAtoms() > 0) ? EmpiricalFormula::formula(sp->selectedAtoms(), true) : "--");
}

/*
 * Species Viewer
 */

// Return contained SpeciesViewer
SpeciesViewer* SpeciesWidget::speciesViewer()
{
	return ui_.SpeciesView;
}

// Set target Species, updating widget as necessary
void SpeciesWidget::setSpecies(Species* sp)
{
	ui_.SpeciesView->setSpecies(sp);

	updateToolbar();
	updateStatusBar();
}

/*
 * Toolbar
 */

void SpeciesWidget::on_InteractionViewButton_clicked(bool checked)
{
	if (checked) speciesViewer()->setInteractionMode(SpeciesViewer::DefaultInteraction);
}

void SpeciesWidget::on_InteractionDrawButton_clicked(bool checked)
{
	if (checked) speciesViewer()->setInteractionMode(SpeciesViewer::DrawInteraction);
}

void SpeciesWidget::on_InteractionDrawElementButton_clicked(bool checked)
{
	// Select a new element for drawing
	bool ok;
	Element* newElement = ElementSelector::getElement(this, "Choose Element", "Select element to use for drawn atoms", speciesViewer()->drawElement(), &ok);
	if (!ok) return;

	speciesViewer()->setDrawElement(newElement);

	updateToolbar();
}

void SpeciesWidget::on_ViewResetButton_clicked(bool checked)
{
	speciesViewer()->view().showAllData();
	speciesViewer()->view().resetViewMatrix();

	speciesViewer()->postRedisplay();
}

void SpeciesWidget::on_ViewSpheresButton_clicked(bool checked)
{
	speciesViewer()->setRenderableDrawStyle(checked ? RenderableSpecies::SpheresStyle : RenderableSpecies::LinesStyle);

	speciesViewer()->notifyDataModified();

	speciesViewer()->postRedisplay();
}

void SpeciesWidget::on_ViewAxesVisibleButton_clicked(bool checked)
{
	speciesViewer()->setAxesVisible(checked);

	speciesViewer()->postRedisplay();
}

void SpeciesWidget::on_ViewCopyToClipboardButton_clicked(bool checked)
{
	speciesViewer()->copyViewToClipboard(checked);
}

// Tools
void SpeciesWidget::on_ToolsCalculateBondingButton_clicked(bool checked)
{
	// Get displayed Species
	Species* sp = speciesViewer()->species();
	if (!sp) return;

	sp->addMissingBonds();

	// Signal that the data shown has been modified
	speciesViewer()->postRedisplay();
	speciesViewer()->notifyDataModified();
}

void SpeciesWidget::on_ToolsMinimiseButton_clicked(bool checked)
{
	// Get displayed Species
	Species* sp = speciesViewer()->species();
	if (!sp) return;

	// Apply forcefield terms now?
	if (sp->forcefield())
	{
		sp->applyForcefieldTerms(*coreData_);
	}

	// Check that the Species set up is valid
	if (!sp->checkSetUp()) return;

	// Create a temporary CoreData and Dissolve
	CoreData temporaryCoreData;
	Dissolve temporaryDissolve(temporaryCoreData);
	if (!temporaryDissolve.registerMasterModules()) return;

	// Copy our target species to the temporary structure, and create a simple Configuration from it
	Species* temporarySpecies = temporaryDissolve.copySpecies(sp);
	Configuration* temporaryCfg = temporaryDissolve.addConfiguration();
	Procedure& generator = temporaryCfg->generator();
	generator.addRootSequenceNode(new BoxProcedureNode(Vec3<double>(1.0,1.0,1.0), Vec3<double>(90,90,90), true));
	AddSpeciesProcedureNode* addSpeciesNode = new AddSpeciesProcedureNode(temporarySpecies, 1, 0.0001);
	addSpeciesNode->setKeyword<bool>("Rotate", false);
	addSpeciesNode->setEnumeration<AddSpeciesProcedureNode::PositioningType>("Positioning", AddSpeciesProcedureNode::CentralPositioning);
	generator.addRootSequenceNode(addSpeciesNode);
	if (!temporaryCfg->initialiseContent(temporaryDissolve.worldPool(), 15.0)) return;

	// Create a Geometry Optimisation Module in a new processing layer, and set everything up
	if (!temporaryDissolve.createModuleInLayer("GeometryOptimisation", "Processing", temporaryCfg)) return;
	if (!temporaryDissolve.generatePairPotentials()) return;

	// Run the calculation
	if (!temporaryDissolve.prepare()) return;
	temporaryDissolve.iterate(1);

	// Copy the optimised coordinates from the temporary Configuration to the target Species
	ListIterator<SpeciesAtom> atomIterator(sp->atoms());
	int index = 0;
	while (SpeciesAtom* i = atomIterator.iterate()) sp->setAtomCoordinates(i, temporaryCfg->atom(index++)->r());

	// Centre the Species back at the origin
	sp->centreAtOrigin();

	// Need to update the SpeciesViewer, and signal that the data shown has been modified
	speciesViewer()->view().showAllData();
	speciesViewer()->postRedisplay();
	speciesViewer()->notifyDataModified();
}
