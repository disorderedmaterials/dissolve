/*
	*** PairPotential Widget - Functions
	*** src/gui/pairpotentialwidget_funcs.cpp
	Copyright T. Youngs 2007-2017

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

#include "gui/pairpotentialwidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "classes/pairpotential.h"
#include "classes/configuration.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
PairPotentialWidget::PairPotentialWidget(QWidget* parent, PairPotential* pp) : SubWidget(parent), pairPotential_(pp)
{
	// Set up user interface
	ui.setupUi(this);

	// Set information panel contents
	CharString topText("%s-%s / %s", pairPotential_->atomTypeNameI(), pairPotential_->atomTypeNameJ(), PairPotential::shortRangeType(pairPotential_->shortRangeType()));
	ui.TopLabel->setText(topText.get());
// 	CharString bottomText("%s", module_->uniqueName());
// 	ui.BottomLabel->setText(bottomText.get());

	// Add a UChromaWidget to the PlotWidget
	QVBoxLayout* layout = new QVBoxLayout;
	uChromaView_ = new UChromaViewWidget;
	layout->addWidget(uChromaView_);
	ui.PlotWidget->setLayout(layout);

	// Start a new, empty session
	uChromaView_->startNewSession(true);

	// Set up Collections for the pair potential data, adding them to the default pane
	Collection* collection;

	collection = uChromaView_->addCollectionToCurrentViewPane("Full");
	collection->addDataSet(pairPotential_->uFull());
	collection->colour().setColourSource(ColourDefinition::SingleColourSource);
	collection->colour().setColourScalePoint(ColourDefinition::SingleColourSource, Qt::black);

	collection = uChromaView_->addCollectionToCurrentViewPane("Original");
	collection->addDataSet(pairPotential_->uOriginal());
	collection->colour().setColourSource(ColourDefinition::SingleColourSource);
	collection->colour().setColourScalePoint(ColourDefinition::SingleColourSource, Qt::red);
	collection->displayLineStyle().setStipple(LineStipple::HalfDashStipple);

	collection = uChromaView_->addCollectionToCurrentViewPane("Additional");
	collection->addDataSet(pairPotential_->uAdditional());
	collection->colour().setColourSource(ColourDefinition::SingleColourSource);
	collection->colour().setColourScalePoint(ColourDefinition::SingleColourSource, Qt::blue);
	collection->displayLineStyle().setStipple(LineStipple::DotStipple);

	collection = uChromaView_->addCollectionToCurrentViewPane("Force");
	collection->addDataSet(pairPotential_->dUFull());
	collection->colour().setColourSource(ColourDefinition::SingleColourSource);
	collection->colour().setColourScalePoint(ColourDefinition::SingleColourSource, Qt::green);
	collection->setVisible(false);

	// Set up the view pane
	ViewPane* viewPane = uChromaView_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "r, Angstroms");
	viewPane->axes().setMax(0, pairPotential_->range());
	viewPane->axes().setTitle(1, "U(r), kJ mol\\sup{-1}");
	viewPane->axes().setMin(1, -5.0);
	viewPane->axes().setMax(1, 10.0);

	refreshing_ = false;
}

PairPotentialWidget::~PairPotentialWidget()
{
}

// Update controls within widget
void PairPotentialWidget::updateControls()
{
	refreshing_ = true;

	refreshing_ = false;
}

/*
 * Reimplementations
 */

void PairPotentialWidget::closeEvent(QCloseEvent* event)
{
	emit (windowClosed(pairPotential_));
}

/*
 * Widget Slots
 */

void PairPotentialWidget::on_FullEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible("Full", checked);
}

void PairPotentialWidget::on_OriginalEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible("Original", checked);
}

void PairPotentialWidget::on_AdditionalEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible("Additional", checked);
}

void PairPotentialWidget::on_FullForceCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible("Force", checked);
}
