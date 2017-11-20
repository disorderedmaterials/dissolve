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
	uChromaView_->startNewSession(true);
	ui.PlotWidget->setLayout(layout);

	// Set up a Collection of data for the PlotWidget
	Collection* collection = uChromaView_->addCollection();
	DataSet* set = collection->addDataSet(PairPotentialWidget::FullEnergyData, pairPotential_->uFull());

	refreshing_ = false;
}

/*
 * General Functions
 */

// Add specified XYData to the plot, as the type specified
void PairPotentialWidget::addData(PairPotentialWidget::DisplayData type)
{
	XYData* xyData;
// 	PlotDataStyle style;
	
	switch (type)
	{
		case (PairPotentialWidget::FullEnergyData):
			// Black solid line
			xyData = &pairPotential_->uFull();
// 			style.set(Qt::SolidLine, 0, 0, 0);
			break;
		case (PairPotentialWidget::OriginalEnergyData):
			// Black dashed line
			xyData = &pairPotential_->uOriginal();
// 			style.set(Qt::DashLine, 0, 0, 0);
			break;
		case (PairPotentialWidget::AdditionalEnergyData):
			// Black dotted line
			xyData = &pairPotential_->uAdditional();
// 			style.set(Qt::DotLine, 0, 0, 0);
			break;
		case (PairPotentialWidget::FullForceData):
			// Red solid line
			xyData = &pairPotential_->dUFull();
// 			style.set(Qt::SolidLine, 255, 0, 0);
			break;
	}

// 	ui.PlotWidget->addData(xyData, style);
}

// Remove specified XYData from the plot
void PairPotentialWidget::removeData(PairPotentialWidget::DisplayData type)
{
	XYData* xyData;
	
	switch (type)
	{
		case (PairPotentialWidget::FullEnergyData):
			// Black solid line
			xyData = &pairPotential_->uFull();
			break;
		case (PairPotentialWidget::OriginalEnergyData):
			// Black dashed line
			xyData = &pairPotential_->uOriginal();
			break;
		case (PairPotentialWidget::AdditionalEnergyData):
			// Black dotted line
			xyData = &pairPotential_->uAdditional();
			break;
		case (PairPotentialWidget::FullForceData):
			// Red solid line
			xyData = &pairPotential_->dUFull();
			break;
	}

// 	ui.PlotWidget->removeData(xyData);
}

/*
 * Widget Slots
 */

void PairPotentialWidget::on_FullEnergyCheck_clicked(bool checked)
{
	if (checked) addData(PairPotentialWidget::FullEnergyData);
	else removeData(PairPotentialWidget::FullEnergyData);
}

void PairPotentialWidget::on_OriginalEnergyCheck_clicked(bool checked)
{
	if (checked) addData(PairPotentialWidget::OriginalEnergyData);
	else removeData(PairPotentialWidget::OriginalEnergyData);
}

void PairPotentialWidget::on_AdditionalEnergyCheck_clicked(bool checked)
{
	if (checked) addData(PairPotentialWidget::AdditionalEnergyData);
	else removeData(PairPotentialWidget::AdditionalEnergyData);
}

void PairPotentialWidget::on_FullForceCheck_clicked(bool checked)
{
	if (checked) addData(PairPotentialWidget::FullForceData);
	else removeData(PairPotentialWidget::FullForceData);
}

