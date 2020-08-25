/*
    *** Calibration Module Widget - Functions
    *** src/modules/calibration/gui/modulewidget_funcs.cpp
    Copyright T. Youngs 2012-2020

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

#include "classes/atomtype.h"
#include "genericitems/listhelper.h"
#include "gui/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/calibration/calibration.h"
#include "modules/calibration/gui/modulewidget.h"
#include "templates/variantpointer.h"

CalibrationModuleWidget::CalibrationModuleWidget(QWidget *parent, CalibrationModule *module)
    : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Grab our DataViewer widget
    dataView_ = ui_.PlotWidget->dataViewer();

    // Set up the view
    auto &view = dataView_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "g(r) / S(Q)");
    view.axes().setMin(1, -1.0);
    view.axes().setMax(1, 1.0);

    refreshing_ = false;
}

CalibrationModuleWidget::~CalibrationModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void CalibrationModuleWidget::updateControls(int flags) { dataView_->postRedisplay(); }

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool CalibrationModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer session
    if (!dataView_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool CalibrationModuleWidget::readState(LineParser &parser)
{
    // Read DataViewer session
    if (!dataView_->readSession(parser))
        return false;

    return true;
}
