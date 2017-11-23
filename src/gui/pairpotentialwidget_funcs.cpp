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
PairPotentialWidget::PairPotentialWidget(QWidget* parent, PairPotential* pp) : QWidget(parent), pairPotential_(pp)
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

	collection = uChromaView_->addCollectionToCurrentViewPane("Full", PairPotentialWidget::FullEnergyData);
	collection->addDataSet(pairPotential_->uFull());
	collection->setColourSource(Collection::SingleColourSource);
	collection->setColourScalePoint(Collection::SingleColourSource, Qt::black);

	collection = uChromaView_->addCollectionToCurrentViewPane("Original", PairPotentialWidget::OriginalEnergyData);
	collection->addDataSet(pairPotential_->uOriginal());
	collection->setColourSource(Collection::SingleColourSource);
	collection->setColourScalePoint(Collection::SingleColourSource, Qt::red);
	collection->displayLineStyle().setStipple(LineStipple::HalfDashStipple);

	collection = uChromaView_->addCollectionToCurrentViewPane("Additional", PairPotentialWidget::AdditionalEnergyData);
	collection->addDataSet(pairPotential_->uAdditional());
	collection->setColourSource(Collection::SingleColourSource);
	collection->setColourScalePoint(Collection::SingleColourSource, Qt::blue);
	collection->displayLineStyle().setStipple(LineStipple::DotStipple);

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

/*
 * Widget Slots
 */

void PairPotentialWidget::on_FullEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible(PairPotentialWidget::FullEnergyData, checked);
}

void PairPotentialWidget::on_OriginalEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible(PairPotentialWidget::OriginalEnergyData, checked);
}

void PairPotentialWidget::on_AdditionalEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible(PairPotentialWidget::AdditionalEnergyData, checked);
}

void PairPotentialWidget::on_FullForceCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible(PairPotentialWidget::FullForceData, checked);
}
