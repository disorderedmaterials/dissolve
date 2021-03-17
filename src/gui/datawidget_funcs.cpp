// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/datawidget.h"
#include "gui/graphgizmo.h"
#include "gui/helpers/treewidgetupdater.h"
#include "gui/render/view.h"
#include <QButtonGroup>
#include <QInputDialog>

Q_DECLARE_METATYPE(std::shared_ptr<Renderable>)
Q_DECLARE_METATYPE(RenderableGroup *)

DataWidget::DataWidget(QWidget *parent) : QWidget(parent)
{
    // Set up our UI
    ui_.setupUi(this);

    // Create button group for interaction tools
    QButtonGroup *interactionToolsGroup = new QButtonGroup;
    interactionToolsGroup->addButton(ui_.InteractionViewButton);

    // Hide data group by default
    ui_.DataGroup->setVisible(false);

    // Connect signals / slots
    connect(ui_.DataView, SIGNAL(currentCoordinateChanged()), this, SLOT(updateStatusBar()));
    connect(ui_.DataView, SIGNAL(interactionModeChanged()), this, SLOT(updateStatusBar()));
    connect(ui_.DataView, SIGNAL(controlAspectChanged()), this, SLOT(updateToolbar()));
    connect(ui_.DataView, SIGNAL(renderableAdded()), this, SLOT(updateDataTree()));
    connect(ui_.DataView, SIGNAL(renderableRemoved()), this, SLOT(updateDataTree()));
    connect(ui_.DataView, SIGNAL(renderableChanged()), this, SLOT(updateDataTree()));

    // Make sure that our controls reflect the state of the underlying DataViewer
    updateToolbar();
    updateStatusBar();
    updateDataTree();
}

DataWidget::~DataWidget() {}

// Return contained DataViewer
DataViewer *DataWidget::dataViewer() { return ui_.DataView; }

/*
 * Tools
 */

// Interaction
void DataWidget::on_InteractionViewButton_clicked(bool checked)
{
    dataViewer()->setInteractionMode(DataViewer::InteractionMode::Default);
}

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

void DataWidget::on_ViewLinkedViewButton_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    // If the button has just been checked, request the target view
    if (checked)
    {
        // Get possible target GraphGizmos, and construct list of associated DataViewers
        RefList<GraphGizmo> graphGizmos = Gizmo::findAll<GraphGizmo>("Graph");
        RefDataList<DataViewer, GraphGizmo *> targets;
        for (GraphGizmo *gizmo : graphGizmos)
            targets.append(gizmo->dataViewer(), gizmo);
        targets.remove(dataViewer());
        if (targets.nItems() == 0)
        {
            ui_.ViewLinkedViewButton->setChecked(false);
            return;
        }

        // Construct a list of targets as a QStringList
        QStringList destinations;
        auto currentItem = -1, count = 0;
        RefDataListIterator<DataViewer, GraphGizmo *> targetIterator(targets);
        while (DataViewer *viewer = targetIterator.iterate())
        {
            destinations << targetIterator.currentData()->uniqueName();
            if (&viewer->view() == dataViewer()->view().linkedView())
                currentItem = count;
            ++count;
        }

        bool ok;
        QString viewName = QInputDialog::getItem(this, "Set View Link", "Select the view to link to...", destinations,
                                                 currentItem, false, &ok);
        if (!ok)
        {
            ui_.ViewLinkedViewButton->setChecked(false);
            return;
        }

        // The destination view from the
        auto viewIndex = destinations.indexOf(viewName);
        DataViewer *viewParent = targets.item(viewIndex);
        if (!viewParent)
            return;

        dataViewer()->linkView(viewParent);
    }
    else
        dataViewer()->unlinkView();
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

// Data tree top-level item update function
void DataWidget::dataTreeTopLevelUpdateFunction(QTreeWidget *treeWidget, int topLevelItemIndex, RenderableGroup &data,
                                                bool createItem)
{
    QTreeWidgetItem *item;
    if (createItem)
    {
        item = new QTreeWidgetItem;
        item->setData(0, Qt::UserRole, QVariant::fromValue(&data));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        treeWidget->insertTopLevelItem(topLevelItemIndex, item);
    }
    else
        item = treeWidget->topLevelItem(topLevelItemIndex);

    // Set item data
    item->setText(0, QString::fromStdString(std::string(data.name())));
    item->setCheckState(0, data.isVisible() ? Qt::Checked : Qt::Unchecked);

    // Update child item
    TreeWidgetUpdater<DataWidget, Renderable> renderableUpdater(item, data.renderables(), this,
                                                                &DataWidget::dataTreeItemUpdateFunction);
}

// Data tree item update function
void DataWidget::dataTreeItemUpdateFunction(QTreeWidgetItem *parentItem, int childIndex, std::shared_ptr<Renderable> &data,
                                            bool createItem)
{
    QTreeWidgetItem *item;
    if (createItem)
    {
        item = new QTreeWidgetItem;
        item->setData(0, Qt::UserRole, QVariant::fromValue(data));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        parentItem->insertChild(childIndex, item);
    }
    else
        item = parentItem->child(childIndex);

    // Set item data
    item->setText(0, QString::fromStdString(std::string(data->name())));
    item->setCheckState(0, data->isVisible() ? Qt::Checked : Qt::Unchecked);
}

// Data tree item changed
void DataWidget::on_DataTree_itemChanged(QTreeWidgetItem *item, int column)
{
    if (refreshLock_.isLocked())
        return;

    // If this is a top-level item (parent() == nullptr) then retrieve the Renderable Group. If not, get the associated
    // Renderable.
    if (item->parent())
    {
        auto renderable = item->data(0, Qt::UserRole).value<std::shared_ptr<Renderable>>();
        if (!renderable)
            return;
        renderable->setVisible(item->checkState(0) == Qt::Checked);
    }
    else
    {
        auto *group = item->data(0, Qt::UserRole).value<RenderableGroup *>();
        if (!group)
            return;
        group->setVisible(item->checkState(0) == Qt::Checked);
    }

    // Refresh the data display
    dataViewer()->postRedisplay();
}

// Clear renderable data
void DataWidget::clearRenderableData()
{
    // Clear our data tree first
    ui_.DataTree->clear();

    // Now clear the renderables
    dataViewer()->clearRenderables();
}

// Update toolbar
void DataWidget::updateToolbar()
{
    Locker refreshLock(refreshLock_);

    // Set current interaction mode
    switch (dataViewer()->interactionMode())
    {
        case (DataViewer::InteractionMode::Default):
            ui_.InteractionViewButton->setChecked(true);
            break;
        default:
            break;
    }

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
    ui_.ViewLinkedViewButton->setChecked(dataViewer()->view().linkedView());
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
            text = QString("x = {%1}, y = {%2}").arg(rLocal.x).arg(rLocal.y);
            break;
        case (View::FlatXZView):
            text = QString("x = {%1}, z = {%2}").arg(rLocal.x).arg(rLocal.z);
            break;
        case (View::FlatZYView):
            text = QString("z = {%1}, y = {%2}").arg(rLocal.z).arg(rLocal.y);
            break;
        default:
            text = QString("x = {%1}, y = {%2}, z = {%3}").arg(rLocal.x, rLocal.y, rLocal.z);
            break;
    }

    ui_.CoordinateLabel->setText(text);
}

// Update data tree
void DataWidget::updateDataTree()
{
    Locker refreshLock(refreshLock_);

    TreeWidgetUpdater<DataWidget, RenderableGroup> dataTreeUpdater(ui_.DataTree, dataViewer()->groupManager().groups(), this,
                                                                   &DataWidget::dataTreeTopLevelUpdateFunction);
}
