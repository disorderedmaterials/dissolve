// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/empiricalFormula.h"
#include "gui/configurationViewerWidget.h"
#include "gui/widgets/elementSelector.hui"
#include "main/dissolve.h"
#include <QButtonGroup>

ConfigurationViewerWidget::ConfigurationViewerWidget(QWidget *parent) : QWidget(parent)
{
    // Set up our UI
    ui_.setupUi(this);

    // Connect signals / slots
    connect(ui_.ConfigurationView, SIGNAL(dataModified()), this, SLOT(notifyDataModified()));
    connect(ui_.ConfigurationView, SIGNAL(styleModified()), this, SLOT(notifyStyleModified()));
}

ConfigurationViewerWidget::~ConfigurationViewerWidget() {}

/*
 * UI
 */

// Notify that the style of displayed data in the underlying viewer has changed
void ConfigurationViewerWidget::notifyStyleModified() { Q_EMIT(styleModified()); }

// Notify that the displayed data in the underlying viewer has changed
void ConfigurationViewerWidget::notifyDataModified() { Q_EMIT(dataModified()); }

// Post redisplay in the underlying view
void ConfigurationViewerWidget::postRedisplay() { ui_.ConfigurationView->postRedisplay(); }

// Update toolbar to reflect current viewer state
void ConfigurationViewerWidget::updateToolbar()
{
    // Set current interaction mode
    switch (configurationViewer()->interactionMode())
    {
        default:
            break;
    }

    // Set checkable buttons
    if (configurationViewer()->configuration())
    {
        ui_.ViewAxesVisibleButton->setChecked(configurationViewer()->axesVisible());
        ui_.ViewSpheresButton->setChecked(configurationViewer()->renderableDrawStyle() != RenderableConfiguration::LinesStyle);
    }
}

/*
 * Configuration Viewer
 */

// Set target Configuration, updating widget as necessary
void ConfigurationViewerWidget::setConfiguration(Configuration *cfg)
{
    ui_.ConfigurationView->setConfiguration(cfg);

    // Persist existing style
    if (cfg)
    {
        configurationViewer()->setRenderableDrawStyle(ui_.ViewSpheresButton->isChecked() ? RenderableConfiguration::SpheresStyle
                                                                                         : RenderableConfiguration::LinesStyle);
    }

    updateToolbar();
}

// Return contained ConfigurationViewer
ConfigurationViewer *ConfigurationViewerWidget::configurationViewer() { return ui_.ConfigurationView; }

/*
 * Toolbar
 */

void ConfigurationViewerWidget::on_ViewResetButton_clicked(bool checked)
{
    configurationViewer()->view().showAllData();
    configurationViewer()->view().resetViewMatrix();

    configurationViewer()->postRedisplay();
}

void ConfigurationViewerWidget::on_ViewSpheresButton_clicked(bool checked)
{
    configurationViewer()->setRenderableDrawStyle(checked ? RenderableConfiguration::SpheresStyle
                                                          : RenderableConfiguration::LinesStyle);

    configurationViewer()->notifyStyleModified();

    configurationViewer()->postRedisplay();
}

void ConfigurationViewerWidget::on_ViewAxesVisibleButton_clicked(bool checked)
{
    configurationViewer()->setAxesVisible(checked);

    configurationViewer()->postRedisplay();
}

void ConfigurationViewerWidget::on_ViewCopyToClipboardButton_clicked(bool checked)
{
    configurationViewer()->copyViewToClipboard(checked);
}
