/*
	*** Module Widget
	*** src/gui/modulewidget.h
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

#ifndef DISSOLVE_MODULEWIDGET_H
#define DISSOLVE_MODULEWIDGET_H

#include <QWidget>

// Forward Declarations
class LineParser;

// ModuleWidget, base class for any Module-specific control widget
class ModuleWidget : public QWidget
{
	public:
	// Constructor / Destructor
	ModuleWidget(QWidget* parent);
	virtual ~ModuleWidget();


	/*
	 * UI
	 */
	protected:
	// Whether widget is currently refreshing
	bool refreshing_;

	public:
	// Update flags
	enum UpdateFlags
	{
		DefaultUpdateFlag = 0,			/* Standard update */
		ResetGraphDataTargetsFlag = 1		/* Any renderables should be cleared and regenerated in viewers */
	};
	// Update controls within widget
	virtual void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);
	// Disable sensitive controls within widget
	virtual void disableSensitiveControls();
	// Enable sensitive controls within widget
	virtual void enableSensitiveControls();


	/*
	 * State I/O
	 */
	public:
	// Read widget state through specified LineParser
	virtual bool readState(LineParser& parser);
	// Write widget state through specified LineParser
	virtual bool writeState(LineParser& parser) const;
};

#endif
