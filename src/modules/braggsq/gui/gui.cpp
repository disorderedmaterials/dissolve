/*
	*** BraggSQ Module - GUI
	*** src/modules/braggsq/gui/gui.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/braggsq/braggsq.h"
#include "modules/braggsq/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget* BraggSQModule::createWidget(QWidget* parent, Dissolve& dissolve)
{
	return new BraggSQModuleWidget(parent, this, dissolve);
}

