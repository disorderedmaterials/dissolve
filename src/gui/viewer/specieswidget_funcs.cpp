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

// Constructor
SpeciesWidget::SpeciesWidget(QWidget* parent) : QWidget(parent)
{
	// Set up our UI
	ui_.setupUi(this);

	// Connect signals / slots
// 	connect(ui_.SpeciesView, SIGNAL(currentCoordinateChanged()), this, SLOT(updateCoordinateInfo()));
}

// Destructor
SpeciesWidget::~SpeciesWidget()
{
}

// Return contained SpeciesViewer
SpeciesViewer* SpeciesWidget::speciesViewer()
{
	return ui_.SpeciesView;
}

/*
 * Signals / Slots
 */

void SpeciesWidget::on_ViewToolButton_clicked(bool checked)
{
	if (checked) speciesViewer()->setInteractionMode(SpeciesViewer::DefaultInteraction);
}

void SpeciesWidget::on_DrawToolButton_clicked(bool checked)
{
}

// Update info label
void SpeciesWidget::updateInfoLabel()
{
// 	ui_.InfoLabel->setText(text);
}
