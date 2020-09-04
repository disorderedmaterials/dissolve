/*
    *** Data Viewer - Functions
    *** src/gui/dataviewer_funcs.cpp
    Copyright T. Youngs 2013-2020

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

#include "base/messenger.h"
#include "gui/dataviewer.hui"

DataViewer::DataViewer(QWidget *parent) : BaseViewer(parent)
{
    // Interaction
    setInteractionMode(DataViewer::DefaultInteraction);

    // Set up the view
    view_.setViewType(View::FlatXYView);

    // Set options
    dissolve_ = nullptr;
    controlFlags_ = NoFlags;
}

DataViewer::~DataViewer() {}

/*
 * Options
 */

// Set pointer to Dissolve
void DataViewer::setDissolve(Dissolve *dissolve) { dissolve_ = dissolve; }

// Set control flags
void DataViewer::setFlags(int flags) { controlFlags_ = flags; }

// Return whether the specified flag is set
bool DataViewer::hasFlag(ControlFlag flag) const { return (controlFlags_ & flag); }
