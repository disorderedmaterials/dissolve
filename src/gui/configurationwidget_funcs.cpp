// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/empiricalformula.h"
#include "gui/configurationwidget.h"
#include "gui/widgets/elementselector.hui"
#include "main/dissolve.h"
#include <QButtonGroup>

ConfigurationWidget::ConfigurationWidget(QWidget *parent) : QWidget(parent)
{
    // Set up our UI
    ui_.setupUi(this);

    // Connect signals / slots
    connect(ui_.ConfigurationView, SIGNAL(dataModified()), this, SLOT(notifyDataModified()));
    connect(ui_.ConfigurationView, SIGNAL(styleModified()), this, SLOT(notifyStyleModified()));
}

ConfigurationWidget::~ConfigurationWidget() {}

/*
 * UI
 */

// Notify that the style of displayed data in the underlying viewer has changed
void ConfigurationWidget::notifyStyleModified() { emit(styleModified()); }

// Notify that the displayed data in the underlying viewer has changed
void ConfigurationWidget::notifyDataModified() { emit(dataModified()); }

// Post redisplay in the underlying view
void ConfigurationWidget::postRedisplay() { ui_.ConfigurationView->postRedisplay(); }

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
void ConfigurationWidget::setConfiguration(Configuration *cfg)
{
    ui_.ConfigurationView->setConfiguration(cfg);

    updateToolbar();
}

// Return contained ConfigurationViewer
ConfigurationViewer *ConfigurationWidget::configurationViewer() { return ui_.ConfigurationView; }

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
    configurationViewer()->setRenderableDrawStyle(checked ? RenderableConfiguration::SpheresStyle
                                                          : RenderableConfiguration::LinesStyle);

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
