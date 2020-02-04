/*
	*** Data Widget - Functions
	*** src/gui/datawidget_funcs.cpp
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

#include "gui/datawidget.h"
#include "gui/render/view.h"
#include "gui/helpers/treewidgetupdater.h"
#include "gui/graphgizmo.h"
#include <QButtonGroup>
#include <QInputDialog>

// Constructor
DataWidget::DataWidget(QWidget* parent) : QWidget(parent)
{
	// Set up our UI
	ui_.setupUi(this);

	// Create button group for interaction tools
	QButtonGroup* interactionToolsGroup = new QButtonGroup;
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

// Destructor
DataWidget::~DataWidget()
{
}

// Return contained DataViewer
DataViewer* DataWidget::dataViewer()
{
	return ui_.DataView;
}

/*
 * Tools
 */

// Interaction
void DataWidget::on_InteractionViewButton_clicked(bool checked)
{
	dataViewer()->setInteractionMode(DataViewer::DefaultInteraction);
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
	if (refreshLock_.isLocked()) return;

	if (checked)
	{
		dataViewer()->view().setAutoFollowType(View::AllAutoFollow);
		if (ui_.GraphFollowXButton->isChecked()) ui_.GraphFollowXButton->setChecked(false);
	}
	else dataViewer()->view().setAutoFollowType(View::NoAutoFollow);

	dataViewer()->postRedisplay();
}

void DataWidget::on_GraphFollowXButton_clicked(bool checked)
{
	if (refreshLock_.isLocked()) return;

	if (checked)
	{
		dataViewer()->view().setAutoFollowType(View::XAutoFollow);
		if (ui_.GraphFollowAllButton->isChecked()) ui_.GraphFollowAllButton->setChecked(false);
	}
	else dataViewer()->view().setAutoFollowType(View::NoAutoFollow);

	dataViewer()->postRedisplay();
}

void DataWidget::on_GraphFollowXLengthSpin_valueChanged(double value)
{
	if (refreshLock_.isLocked()) return;

	dataViewer()->view().setAutoFollowXLength(value);

	dataViewer()->postRedisplay();
}

// View
void DataWidget::on_ViewTypeCombo_currentIndexChanged(int index)
{
	if (refreshLock_.isLocked()) return;

	dataViewer()->view().setViewType( (View::ViewType) index );

	dataViewer()->postRedisplay();
}

void DataWidget::on_ViewLinkedViewButton_clicked(bool checked)
{
	if (refreshLock_.isLocked()) return;

	// If the button has just been checked, request the target view
	if (checked)
	{
		// Get possible target GraphGizmos, and construct list of associated DataViewers
		RefList<GraphGizmo> graphGizmos = Gizmo::findAll<GraphGizmo>("Graph");
		RefDataList<DataViewer,GraphGizmo*> targets;
		for (GraphGizmo* gizmo : graphGizmos) targets.append(gizmo->dataViewer(), gizmo);
		targets.remove(dataViewer());
		if (targets.nItems() == 0)
		{
			ui_.ViewLinkedViewButton->setChecked(false);
			return;
		}

		// Construct a list of targets as a QStringList
		QStringList destinations;
		int currentItem = -1, count = 0;
		RefDataListIterator<DataViewer,GraphGizmo*> targetIterator(targets);
		while (DataViewer* viewer = targetIterator.iterate())
		{
			destinations << targetIterator.currentData()->uniqueName();
			if (&viewer->view() == dataViewer()->view().linkedView()) currentItem = count;
			++count;
		}

		bool ok;
		QString viewName = QInputDialog::getItem(this, "Set View Link", "Select the view to link to...", destinations, currentItem, false, &ok);
		if (!ok)
		{
			ui_.ViewLinkedViewButton->setChecked(false);
			return;
		}

		// The destination view from the
		int viewIndex = destinations.indexOf(viewName);
		DataViewer* viewParent = targets.item(viewIndex);
		if (!viewParent) return;
		
		dataViewer()->linkView(viewParent);
	}
	else dataViewer()->unlinkView();
}

void DataWidget::on_ViewToggleDataButton_clicked(bool checked)
{
	if (refreshLock_.isLocked()) return;

	ui_.DataGroup->setVisible(checked);
}

void DataWidget::on_ViewAxesVisibleButton_clicked(bool checked)
{
	if (refreshLock_.isLocked()) return;

	dataViewer()->setAxesVisible(checked);

	dataViewer()->postRedisplay();
}

void DataWidget::on_ViewCopyToClipboardButton_clicked(bool checked)
{
	dataViewer()->copyViewToClipboard(checked);
}

/*
 * Update Functions
 */

// Data tree top-level item update function
void DataWidget::dataTreeTopLevelUpdateFunction(QTreeWidget* treeWidget, int topLevelItemIndex, RenderableGroup* data, bool createItem)
{
	QTreeWidgetItem* item;
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, VariantPointer<RenderableGroup>(data));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		treeWidget->insertTopLevelItem(topLevelItemIndex, item);
	}
	else item = treeWidget->topLevelItem(topLevelItemIndex);

	// Set item data
	item->setText(0, data->name());
// 	item->setIcon(0, QIcon(":/general/icons/general_true.svg"));
	item->setCheckState(0, data->isVisible() ? Qt::Checked : Qt::Unchecked);

	// Update child items
	TreeWidgetRefListUpdater<DataWidget,Renderable> renderableUpdater(item, data->renderables(), this, &DataWidget::dataTreeItemUpdateFunction);
}

// Data tree item update function
void DataWidget::dataTreeItemUpdateFunction(QTreeWidgetItem* parentItem, int childIndex, Renderable* data, bool createItem)
{
	QTreeWidgetItem* item;
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, VariantPointer<Renderable>(data));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		parentItem->insertChild(childIndex, item);
	}
	else item = parentItem->child(childIndex);

	// Set item data
	item->setText(0, data->name());
// 	item->setIcon(0, QIcon(":/general/icons/general_true.svg"));
	item->setCheckState(0, data->isVisible() ? Qt::Checked : Qt::Unchecked);
}

// Data tree item changed
void DataWidget::on_DataTree_itemChanged(QTreeWidgetItem* item, int column)
{
	if (refreshLock_.isLocked()) return;

	// If this is a top-level item (parent() == NULL) then retrieve the Renderable Group. If not, get the associated Renderable.
	if (item->parent())
	{
		Renderable* renderable= VariantPointer<Renderable>(item->data(0, Qt::UserRole));
		if (!renderable) return;
		renderable->setVisible(item->checkState(0) == Qt::Checked);
	}
	else
	{
		RenderableGroup* group = VariantPointer<RenderableGroup>(item->data(0, Qt::UserRole));
		if (!group) return;
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
		case (DataViewer::DefaultInteraction):
			ui_.InteractionViewButton->setChecked(true);
			break;
// 		case (DataViewer::ZoomInteraction):
// 			ui_.InteractionZoomutton->setChecked(true);
// 			break;
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
	View& view = dataViewer()->view();
	Vec3<double> rLocal = dataViewer()->current2DAxesCoordinates();
	QString text;
	switch (view.viewType())
	{
		case (View::FlatXYView):
			text.sprintf("x = %e, y = %e", rLocal.x, rLocal.y);
			break;
		case (View::FlatXZView):
			text.sprintf("x = %e, z = %e", rLocal.x, rLocal.z);
			break;
		case (View::FlatZYView):
			text.sprintf("z = %e, y = %e", rLocal.z, rLocal.y);
			break;
		default:
			text.sprintf("x = %e, y = %e, z = %e", rLocal.x, rLocal.y, rLocal.z);
			break;
	}

	ui_.CoordinateLabel->setText(text);
}

// Update data tree
void DataWidget::updateDataTree()
{
	Locker refreshLock(refreshLock_);

	TreeWidgetUpdater<DataWidget, RenderableGroup> dataTreeUpdater(ui_.DataTree, dataViewer()->groupManager().groups(), this, &DataWidget::dataTreeTopLevelUpdateFunction);
}
