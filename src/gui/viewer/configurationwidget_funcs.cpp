/*
	*** Configuration Widget - Functions 
	*** src/gui/viewer/configurationwidget_funcs.cpp
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

#include "gui/viewer/configurationwidget.h"
#include "gui/widgets/elementselector.hui"
#include "classes/coredata.h"
#include "classes/empiricalformula.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include <QButtonGroup>

// Constructor
ConfigurationWidget::ConfigurationWidget(QWidget* parent) : QWidget(parent)
{
	// Set up our UI
	ui_.setupUi(this);

	// Connect signals / slots
	connect(ui_.ConfigurationView, SIGNAL(dataModified()), this, SLOT(notifyDataModified()));
	connect(ui_.ConfigurationView, SIGNAL(styleModified()), this, SLOT(notifyStyleModified()));
}

// Destructor
ConfigurationWidget::~ConfigurationWidget()
{
}

/*
 * UI
 */

// Notify that the style of displayed data in the underlying viewer has changed
void ConfigurationWidget::notifyStyleModified()
{
	emit(styleModified());
}

// Notify that the displayed data in the underlying viewer has changed
void ConfigurationWidget::notifyDataModified()
{
	emit(dataModified());
}

// Post redisplay in the underlying view
void ConfigurationWidget::postRedisplay()
{
	ui_.ConfigurationView->postRedisplay();
}

// Update toolbar to reflect current viewer state
void ConfigurationWidget::updateToolbar()
{
	// Set current interaction mode
	switch (configurationViewer()->interactionMode())
	{
		default:
			break;
	}

	// Set checkable buttons
	ui_.ViewAxesVisibleButton->setChecked(configurationViewer()->axesVisible());
	ui_.ViewSpheresButton->setChecked(configurationViewer()->renderableDrawStyle() != RenderableConfiguration::LinesStyle);
}

/*
 * Configuration Viewer
 */

// Set target Configuration, updating widget as necessary
void ConfigurationWidget::setConfiguration(Configuration* cfg)
{
	ui_.ConfigurationView->setConfiguration(cfg);

	updateToolbar();
}

// Return contained ConfigurationViewer
ConfigurationViewer* ConfigurationWidget::configurationViewer()
{
	return ui_.ConfigurationView;
}

/*
 * Toolbar
 */

void ConfigurationWidget::on_ViewResetButton_clicked(bool checked)
{
	configurationViewer()->view().showAllData();
	configurationViewer()->view().resetViewMatrix();

	configurationViewer()->postRedisplay();
}

void ConfigurationWidget::on_ViewSpheresButton_clicked(bool checked)
{
	configurationViewer()->setRenderableDrawStyle(checked ? RenderableConfiguration::SpheresStyle : RenderableConfiguration::LinesStyle);

	configurationViewer()->notifyStyleModified();

	configurationViewer()->postRedisplay();
}

void ConfigurationWidget::on_ViewAxesVisibleButton_clicked(bool checked)
{
	configurationViewer()->setAxesVisible(checked);

	configurationViewer()->postRedisplay();
}

void ConfigurationWidget::on_ViewCopyToClipboardButton_clicked(bool checked)
{
	configurationViewer()->copyViewToClipboard(checked);
}
