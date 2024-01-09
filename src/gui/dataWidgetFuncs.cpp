// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataWidget.h"
#include "gui/render/view.h"
#include <QButtonGroup>
#include <QInputDialog>

DataWidget::DataWidget(QWidget *parent) : QWidget(parent)
{
    // Set up our UI
    ui_.setupUi(this);

    // Set data for group manager model
    renderableGroupManagerModel_.setSourceData(ui_.DataView->groupManager());
    ui_.DataTree->setModel(&renderableGroupManagerModel_);

    // Hide data group by default
    ui_.DataGroup->setVisible(false);

    // Connect signals / slots
    connect(ui_.DataView, SIGNAL(currentCoordinateChanged()), this, SLOT(updateStatusBar()));
    connect(ui_.DataView, SIGNAL(interactionModeChanged()), this, SLOT(updateStatusBar()));
    connect(ui_.DataView, SIGNAL(controlAspectChanged()), this, SLOT(updateToolbar()));
    connect(ui_.DataView, SIGNAL(renderableAdded()), this, SLOT(updateDataTree()));
    connect(ui_.DataView, SIGNAL(renderableRemoved()), this, SLOT(updateDataTree()));
    connect(ui_.DataView, SIGNAL(renderableChanged()), this, SLOT(updateDataTree()));
    connect(&renderableGroupManagerModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)),
            this, SLOT(renderableGroupManagerDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    // Make sure that our controls reflect the state of the underlying DataViewer
    updateToolbar();
    updateStatusBar();
    updateDataTree();
}

// Return contained DataViewer
DataViewer *DataWidget::dataViewer() { return ui_.DataView; }

/*
 * Tools
 */

// Graph
void DataWidget::on_GraphResetButton_clicked(bool checked)
{
    dataViewer()->view().resetViewMatrix();
    dataViewer()->view().showAllData();

    dataViewer()->postRedisplay();
}

void DataWidget::on_GraphFollowAllButton_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    if (checked)
    {
        dataViewer()->view().setAutoFollowType(View::AllAutoFollow);
        if (ui_.GraphFollowXButton->isChecked())
            ui_.GraphFollowXButton->setChecked(false);
    }
    else
        dataViewer()->view().setAutoFollowType(View::NoAutoFollow);

    dataViewer()->postRedisplay();
}

void DataWidget::on_GraphFollowXButton_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    if (checked)
    {
        dataViewer()->view().setAutoFollowType(View::XAutoFollow);
        if (ui_.GraphFollowAllButton->isChecked())
            ui_.GraphFollowAllButton->setChecked(false);
    }
    else
        dataViewer()->view().setAutoFollowType(View::NoAutoFollow);

    dataViewer()->postRedisplay();
}

void DataWidget::on_GraphFollowXLengthSpin_valueChanged(double value)
{
    if (refreshLock_.isLocked())
        return;

    dataViewer()->view().setAutoFollowXLength(value);

    dataViewer()->postRedisplay();
}

// View
void DataWidget::on_ViewTypeCombo_currentIndexChanged(int index)
{
    if (refreshLock_.isLocked())
        return;

    dataViewer()->view().setViewType((View::ViewType)index);

    dataViewer()->postRedisplay();
}

void DataWidget::on_ViewToggleDataButton_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    ui_.DataGroup->setVisible(checked);
}

void DataWidget::on_ViewAxesVisibleButton_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    dataViewer()->setAxesVisible(checked);

    dataViewer()->postRedisplay();
}

void DataWidget::on_ViewCopyToClipboardButton_clicked(bool checked) { dataViewer()->copyViewToClipboard(checked); }

/*
 * Update Functions
 */

// Data tree item changed
void DataWidget::renderableGroupManagerDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)
{
    dataViewer()->postRedisplay();
}

// Clear renderable data
void DataWidget::clearRenderableData()
{
    dataViewer()->clearRenderables();

    updateDataTree();
}

// Update toolbar
void DataWidget::updateToolbar()
{
    Locker refreshLock(refreshLock_);

    // Controls reflecting the state of options in the underlying DataViewer
    // -- Graph
    View::ViewType vt = dataViewer()->view().viewType();
    View::AutoFollowType aft = dataViewer()->view().autoFollowType();
    ui_.GraphFollowAllButton->setChecked(aft == View::AllAutoFollow);
    ui_.GraphFollowXButton->setChecked(aft == View::XAutoFollow);
    ui_.GraphFollowXLengthSpin->setValue(dataViewer()->view().autoFollowXLength());
    ui_.GraphFollowXButton->setEnabled(vt == View::FlatXYView);
    ui_.GraphFollowXLengthSpin->setEnabled(vt == View::FlatXYView);
    // View
    ui_.ViewTypeCombo->setCurrentIndex(dataViewer()->view().viewType());
    ui_.ViewAxesVisibleButton->setChecked(dataViewer()->axesVisible());
}

// Update status bar
void DataWidget::updateStatusBar()
{
    Locker refreshLock(refreshLock_);

    // Update mode text
    ui_.ModeLabel->setText(dataViewer()->interactionModeText());

    // Update coordinate information
    auto &view = dataViewer()->view();
    const auto rLocal = dataViewer()->current2DAxesCoordinates();
    QString text;
    switch (view.viewType())
    {
        case (View::FlatXYView):
            text = QString("x = %1, y = %2").arg(rLocal.x).arg(rLocal.y);
            break;
        case (View::FlatXZView):
            text = QString("x = %1, z = %2").arg(rLocal.x).arg(rLocal.z);
            break;
        case (View::FlatZYView):
            text = QString("z = %1, y = %2").arg(rLocal.z).arg(rLocal.y);
            break;
        default:
            text = QString("x = %1, y = %2, z = %3").arg(rLocal.x).arg(rLocal.y).arg(rLocal.z);
            break;
    }

    ui_.CoordinateLabel->setText(text);
}

// Update data tree
void DataWidget::updateDataTree()
{
    Locker refreshLock(refreshLock_);
    renderableGroupManagerModel_.setSourceData(ui_.DataView->groupManager());
}
