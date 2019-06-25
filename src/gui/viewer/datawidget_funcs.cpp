/*
	*** Graph Widget - Functions 
	*** src/gui/viewer/datawidget_funcs.cpp
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

#include "gui/viewer/datawidget.h"
#include "gui/viewer/render/view.h"
#include "gui/helpers/treewidgetupdater.h"
#include <QButtonGroup>

// Constructor
DataWidget::DataWidget(QWidget* parent) : QWidget(parent)
{
	// Set up our UI
	ui_.setupUi(this);

	// Create button group for interaction tools
	QButtonGroup* interactionToolsGroup = new QButtonGroup;
	interactionToolsGroup->addButton(ui_.InteractionViewButton);

	// Hide data tree by default
	ui_.DataTree->setVisible(false);

	// Connect signals / slots
	connect(ui_.DataView, SIGNAL(currentCoordinateChanged()), this, SLOT(updateStatusBar()));
	connect(ui_.DataView, SIGNAL(interactionModeChanged()), this, SLOT(updateStatusBar()));
	connect(ui_.DataView, SIGNAL(controlAspectChanged()), this, SLOT(updateToolbar()));
	connect(ui_.DataView, SIGNAL(renderableAdded()), this, SLOT(updateDataTree()));
	connect(ui_.DataView, SIGNAL(renderableRemoved()), this, SLOT(updateDataTree()));

	refreshing_ = false;

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

// View
void DataWidget::on_ViewResetButton_clicked(bool checked)
{
	dataViewer()->view().showAllData();

	dataViewer()->postRedisplay();
}

void DataWidget::on_ViewAxesVisibleButton_clicked(bool checked)
{
	dataViewer()->setAxesVisible(checked);

	dataViewer()->postRedisplay();
}

void DataWidget::on_ViewCopyToClipboardButton_clicked(bool checked)
{
	dataViewer()->copyViewToClipboard(checked);
}

void DataWidget::on_ViewFollowAllButton_clicked(bool checked)
{
	if (checked)
	{
		dataViewer()->view().setAutoFollowType(View::AllAutoFollow);
		if (ui_.ViewFollowXButton->isChecked()) ui_.ViewFollowXButton->setChecked(false);
	}
	else dataViewer()->view().setAutoFollowType(View::NoAutoFollow);

	dataViewer()->postRedisplay();
}

void DataWidget::on_ViewFollowXButton_clicked(bool checked)
{
	if (checked)
	{
		dataViewer()->view().setAutoFollowType(View::XAutoFollow);
		if (ui_.ViewFollowAllButton->isChecked()) ui_.ViewFollowAllButton->setChecked(false);
	}
	else dataViewer()->view().setAutoFollowType(View::NoAutoFollow);

	dataViewer()->postRedisplay();
}

void DataWidget::on_ViewFollowXLengthSpin_valueChanged(double value)
{
	dataViewer()->view().setAutoFollowXLength(value);

	dataViewer()->postRedisplay();
}

// Data
void DataWidget::on_DataToggleTreeButton_clicked(bool checked)
{
	ui_.DataTree->setVisible(checked);
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
	TreeWidgetRefListUpdater<DataWidget,Renderable,int> renderableUpdater(item, data->renderables(), this, &DataWidget::dataTreeItemUpdateFunction);
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

// Update toolbar
void DataWidget::updateToolbar()
{
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
	ui_.ViewAxesVisibleButton->setChecked(dataViewer()->axesVisible());
	View::ViewType vt = dataViewer()->view().viewType();
	View::AutoFollowType aft = dataViewer()->view().autoFollowType();
	ui_.ViewFollowAllButton->setChecked(aft == View::AllAutoFollow);
	ui_.ViewFollowXButton->setChecked(aft == View::XAutoFollow);
	ui_.ViewFollowXLengthSpin->setValue(dataViewer()->view().autoFollowXLength());
	ui_.ViewFollowXButton->setEnabled(vt == View::FlatXYView);
	ui_.ViewFollowXLengthSpin->setEnabled(vt == View::FlatXYView);
}

// Update status bar
void DataWidget::updateStatusBar()
{
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
	RenderableGroupManager& groupManager = dataViewer()->groupManager();
	TreeWidgetUpdater<DataWidget, RenderableGroup> dataTreeUpdater(ui_.DataTree, groupManager.groups(), this, &DataWidget::dataTreeTopLevelUpdateFunction);
}
