/*
	*** PlotWidget Functions
	*** src/gui/plotwidget_funcs.cpp
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtOpenGL/QGLWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include "gui/plotwidget.uih"
#include "base/sysfunc.h"

// Static Singletons
QColor PlotWidget::lineColours_[PlotWidget::nLineColours];

/*!
 * \brief Constructor
 */
PlotWidget::PlotWidget(QWidget* parent) : QWidget(parent)
{
	// Context Menu
	contextMenu_ = new QMenu(this);
	contextMenuValid_ = false;

	// Toolbar
	toolBarWidget_ = NULL;
	coordinatesLabel_ = NULL;
	for (int n=0; n<nToolBarButtons; ++n) toolBarButtons_[n] = NULL;

	// Set plot defaults
	spacing_ = 4;
	backgroundColour_ = Qt::white;
	foregroundColour_ = Qt::black;
	font_.setPointSize(8);
	xMin_ = -10.0;
	xMax_ = 10.0;
	yMin_ = -10.0;
	yMax_ = 10.0;
	xAxisTickStart_ = -10.0;
	yAxisTickStart_ = -10.0;
	xAxisTickDelta_ = 2.0;
	yAxisTickDelta_ = 2.0;
	xLabelFormat_ = "%4.1f";
	yLabelFormat_ = "%4.1f";
	legendVisible_ = true;
	verticalSpacing_ = 1.0;
	xMinLimit_ = 0.0;
	xMaxLimit_ = 0.0;
	yMinLimit_ = 0.0;
	yMaxLimit_ = 0.0;
	limitXMin_ = false;
	limitXMax_ = false;
	limitYMin_ = false;
	limitYMax_ = false;
	autoScale_ = true;
	
	// Pre-defined colours
	lineColours_[PlotWidget::BlackColour] = Qt::black;
	lineColours_[PlotWidget::RedColour] = Qt::red;
	lineColours_[PlotWidget::BlueColour] = Qt::blue;
	lineColours_[PlotWidget::GreenColour] = Qt::green;

	// Titles
	mainTitle_ = "NewGraph";
	xAxisTitle_ = "XAxis";
	yAxisTitle_ = "YAxis";
}

/*!
 * \brief Destructor
 */
PlotWidget::~PlotWidget()
{
}

/*
// Widgets / Slots / Reimplementations
*/

/*!
 * \brief Recreate context menu
 */
void PlotWidget::recreateContextMenu()
{
	// Clear all old context menu items
	contextMenu_->clear();
	int count = 0;
	QAction* action;

	// Create Groups submenu
	QMenu* subMenu = contextMenu_->addMenu("Groups");
	if (dataSetGroups_.nItems() == 0)
	{
		QAction* action = subMenu->addAction("<No Groups Defined>");
		action->setEnabled(false);
	}
	else
	{
		QMenu* groupMenu;
		count = 0;
		for (Dnchar* group = dataSetGroups_.first(); group != NULL; group = group->next)
		{
			groupMenu = subMenu->addMenu(group->get());
			action = groupMenu->addAction("Show All");
			action->setData(QVariant(count));
			QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(contextMenuGroupShowAllClicked(bool)));
			action = groupMenu->addAction("Hide All");
			action->setData(QVariant(count));
			QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(contextMenuGroupHideAllClicked(bool)));
			++count;
		}
	}

	// Add DataSets to QMenu
	contextMenu_->addSeparator();
	count = 0;
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
	{
		QAction* action = contextMenu_->addAction(pd->name());
		action->setCheckable(true);
		action->setChecked(pd->visible());
		action->setData(QVariant(count));
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(contextMenuDataSetClicked(bool)));
		++count;
	}
	
	contextMenuValid_ = true;
}

/*!
 * \brief Mouse press event
 */
void PlotWidget::mousePressEvent(QMouseEvent* event)
{
	buttons_ = event->buttons();
	clickedWidgetPosition_ = event->pos();

	if (buttons_&Qt::LeftButton)
	{
		// Where is mouse cursor (use original click position to check)?
		if (graphArea_.contains(clickedWidgetPosition_)) clickedGraphPosition_ = widgetToGraph(event->pos());
		else clickedWidgetPosition_ = QPoint();
	}
	else if (buttons_&Qt::RightButton)
	{
		// Raise context menu...
		recreateContextMenu();
		contextMenu_->exec(event->globalPos());
	}
	else if (buttons_&Qt::MiddleButton)
	{
		// Where is mouse cursor (use original click position to check)?
		if (graphArea_.contains(clickedWidgetPosition_)) clickedGraphPosition_ = widgetToGraph(event->pos());
		else clickedWidgetPosition_ = QPoint();
	}
	
// 	printf("Clicked graph coords = %f,%f\n", clickedGraphPosition_.x(), clickedGraphPosition_.y());
}

/*!
 * \brief Mouse release event
 */
void PlotWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (buttons_&Qt::LeftButton)
	{
		// Set current mouse position (in graph coordinates)
		currentGraphPosition_ = widgetToGraph(event->pos());
// 		printf("Unclicked graph coords = %f,%f\n", currentGraphPosition_.x(), currentGraphPosition_.y());

		// Where is mouse cursor (use original click position to check)?
		if (graphArea_.contains(clickedWidgetPosition_))
		{
			// If the box is too small in either direction, assume that we are translating to the point instead...
			bool small = false;
			if (fabs(currentGraphPosition_.x()-clickedGraphPosition_.x()) < (xMax_-xMin_)*0.01) small = true;
			if (fabs(currentGraphPosition_.y()-clickedGraphPosition_.y()) < (yMax_-yMin_)*0.01) small = true;
			if (small)
			{
				double dx = currentGraphPosition_.x() - (xMin_+xMax_)*0.5;
				double dy = currentGraphPosition_.y() - (yMin_+yMax_)*0.5;
				xMin_ += dx;
				xMax_ += dx;
				yMin_ += dy;
				yMax_ += dy;
				calculateTickDeltas(5);
				repaint();
			}
			else zoomToGraph(clickedGraphPosition_.x(), clickedGraphPosition_.y(), currentGraphPosition_.x(), currentGraphPosition_.y());
		}
	}
	else if (buttons_&Qt::MiddleButton)
	{
	}
	
	// Reset values
	buttons_ = 0;
	clickedWidgetPosition_ = QPoint();
}

/*!
 * \brief Mouse move event
 */
void PlotWidget::mouseMoveEvent(QMouseEvent* event)
{
	currentGraphPosition_ = widgetToGraph(event->pos());
	setFocus();
	if (buttons_&Qt::MiddleButton)
	{
		if (!clickedWidgetPosition_.isNull())
		{
			double dx = currentGraphPosition_.x() - clickedGraphPosition_.x();
			double dy = currentGraphPosition_.y() - clickedGraphPosition_.y();
			xMin_ -= dx;
			xMax_ -= dx;
			yMin_ -= dy;
			yMax_ -= dy;
			calculateTickDeltas(5);

			clickedGraphPosition_ = widgetToGraph(event->pos());
			repaint();
		}
	}
	
	// Set coordinates label (if it exists)
	if (coordinatesLabel_)
	{
		Dnchar label(-1, "X=%6.2e,Y=%6.2e", currentGraphPosition_.x(), currentGraphPosition_.y());
		coordinatesLabel_->setText(label.get());
	}
}

/*!
 * \brief Mouse wheel event
 */
void PlotWidget::wheelEvent(QWheelEvent* event)
{
	// Get a fraction of the current y range
	double yBit = (yMax_ - yMin_) * 0.1;
	if (event->delta() < 0) yBit *= -1.0;

	// Shift y range of graph....
	yMin_ += yBit;
	yMax_ += yBit;

	// Recalculate tick deltas
	calculateTickDeltas(5);

	repaint();
}

/*!
 * \brief Key press event
 */
void PlotWidget::keyPressEvent(QKeyEvent* event)
{
	bool accept = true;
	switch (event->key())
	{
		// Toggle autoscaling
		case (Qt::Key_A):
			autoScale_ = !autoScale_;
			updateToolBarWidget();
			update();
			break;
		// Fit graph to data (optionally obeying soft limits)
		case (Qt::Key_F):
			if (event->modifiers().testFlag(Qt::ShiftModifier)) fitData(false);
			else fitData(true);
			update();
			break;
		// Toggle X axis soft limits
		case (Qt::Key_X):
			if (event->modifiers().testFlag(Qt::ShiftModifier)) limitXMax_ = !limitXMax_;
			else limitXMin_ = !limitXMin_;
			updateToolBarWidget();
			fitData(true);
			update();
			break;
		// Toggle Y axis soft limits
		case (Qt::Key_Y):
			if (event->modifiers().testFlag(Qt::ShiftModifier)) limitYMax_ = !limitYMax_;
			else limitYMin_ = !limitYMin_;
			updateToolBarWidget();
			fitData(true);
			update();
			break;
		default:
			accept = false;
			break;
	}
	if (accept) event->accept();
	else event->ignore();
}

/*!
 * \brief Key release event
 */
void PlotWidget::keyReleaseEvent(QKeyEvent* event)
{
	event->ignore();
}

/*!
 * \brief Context Menu DataSet entry clicked
 */
void PlotWidget::contextMenuDataSetClicked(bool checked)
{
	// Get sender QAction and its QVariant value
	QAction* action = static_cast<QAction*>(sender());
	if (!action) return;
	QVariant data = action->data();
	if (!data.isValid())
	{
		printf("GUI_ERROR - QVariant contains no valid data in PlotWidget::contextMenuDataSetClicked().\n");
		return;
	}
	PlotData* pd = dataSets_[data.toInt()];
	if (!pd) return;
	
	printf("Selected dataset = %p, name = %s\n", pd, qPrintable(pd->name()));
	pd->setVisible(checked);
	update();
}

/*!
 * \brief Context Menu Group ShowAll entry clicked
 */
void PlotWidget::contextMenuGroupShowAllClicked(bool checked)
{
	// Get sender QAction and its QVariant value
	QAction* action = static_cast<QAction*>(sender());
	if (!action) return;
	QVariant data = action->data();
	if (!data.isValid())
	{
		printf("GUI_ERROR - QVariant contains no valid data in PlotWidget::contextMenuGroupShowAllClicked().\n");
		return;
	}

	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next) if (pd->groupIndex() == data.toInt()) pd->setVisible(true);
	
	update();
}

/*!
 * \brief Context Menu Group HideAll entry clicked
 */
void PlotWidget::contextMenuGroupHideAllClicked(bool checked)
{
	// Get sender QAction and its QVariant value
	QAction* action = static_cast<QAction*>(sender());
	if (!action) return;
	QVariant data = action->data();
	if (!data.isValid())
	{
		printf("GUI_ERROR - QVariant contains no valid data in PlotWidget::contextMenuGroupHideAllClicked().\n");
		return;
	}

	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next) if (pd->groupIndex() == data.toInt()) pd->setVisible(false);

	update();
}

/*
 * Toolbar (if specified)
 */

/*!
 * \brief Update toolbar to reflect internal state
 */
void PlotWidget::updateToolBarWidget()
{
	if (!toolBarWidget_) return;
	toolBarButtons_[PlotWidget::AutoScaleButton]->setDown(autoScale_);
	toolBarButtons_[PlotWidget::LimitXMinButton]->setDown(limitXMin_);
	toolBarButtons_[PlotWidget::LimitXMaxButton]->setDown(limitXMax_);
	toolBarButtons_[PlotWidget::LimitYMinButton]->setDown(limitYMin_);
	toolBarButtons_[PlotWidget::LimitYMaxButton]->setDown(limitYMax_);
}

/*!
 * \brief Set toolbar for PlotWidget
 */
void PlotWidget::setToolBarWidget(QWidget* widget)
{
	const bool checkable[PlotWidget::nToolBarButtons] = { true, true, false, true, true, false, true } ;

	// Set widget target
	toolBarWidget_ = widget;
	
	// Create layout within widget
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);

	// Create toolbuttons	
	toolBarButtons_[PlotWidget::AutoScaleButton] = new QPushButton("A");
	toolBarButtons_[PlotWidget::LimitXMinButton] = new QPushButton("[");
	toolBarButtons_[PlotWidget::LimitXButton] = new QPushButton("X");
	toolBarButtons_[PlotWidget::LimitXMaxButton] = new QPushButton("]");
	toolBarButtons_[PlotWidget::LimitYMinButton] = new QPushButton("[");
	toolBarButtons_[PlotWidget::LimitYButton] = new QPushButton("Y");
	toolBarButtons_[PlotWidget::LimitYMaxButton] = new QPushButton("]");

	// Set sizes of tool buttons, font, and visibility
	QFont font;
	font.setPointSize(8);
	for (int n=0; n<PlotWidget::nToolBarButtons; ++n)
	{
		QPushButton* btn = toolBarButtons_[n];
		if (btn == NULL) continue;

		// Add widget to layout
		layout->addWidget(toolBarButtons_[n]);

		// Set button propertieas
		btn->setFont(font);
		if (checkable[n]) btn->setCheckable(true);
		btn->setMaximumSize( QSize(20,20) );
		btn->setVisible(true);
	}

	// Add horizontal spacer between buttons and label
	QSpacerItem* spacer = new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Ignored);
	layout->addSpacerItem(spacer);

	// Create label
	coordinatesLabel_ = new QLabel("X=0.0, Y=0.0");
	coordinatesLabel_->setFont(font);
	layout->addWidget(coordinatesLabel_);

	// Connect button signals to slots
	this->connect(toolBarButtons_[PlotWidget::AutoScaleButton], SIGNAL(clicked(bool)), SLOT(setAutoScale(bool)));
	this->connect(toolBarButtons_[PlotWidget::LimitXMinButton], SIGNAL(clicked(bool)), SLOT(setXMinLimit(bool)));
	this->connect(toolBarButtons_[PlotWidget::LimitXMaxButton], SIGNAL(clicked(bool)), SLOT(setXMaxLimit(bool)));
	this->connect(toolBarButtons_[PlotWidget::LimitYMinButton], SIGNAL(clicked(bool)), SLOT(setYMinLimit(bool)));
	this->connect(toolBarButtons_[PlotWidget::LimitYMaxButton], SIGNAL(clicked(bool)), SLOT(setYMaxLimit(bool)));

	// Finally, place layout onto toolBarWidget_ and update controls
	toolBarWidget_->setLayout(layout);
	toolBarWidget_->show();
	updateToolBarWidget();
}

/*
 * Style
 */

/*!
 * \brief Return nth pre-defined colour
 */
QColor PlotWidget::lineColour(int n)
{
	return lineColours_[n%PlotWidget::nLineColours];
}

/*
 * Plot Area
 */

/*!
 * \brief Determine suitable tick deltas based on current graph limits
 * \details Given the current min and max values of the x and y axes, determine a 'best-fit' tick delta, given the constraints that it must
 * be exactly one of several basic values, multiplied by some power of 10.
 */
void PlotWidget::calculateTickDeltas(int maxTicks)
{
	double range, axisMin, axisMax, firstTick, tickDelta;
	const int nBaseValues = 5, maxIterations = 10;
	int axis, power = 1, baseValues[nBaseValues] = { 1, 2, 3, 4, 5 }, baseValueIndex, nTicks, iteration, minTicks = maxTicks/2;
	for (axis=0; axis<2; ++axis)
	{
		// Calculate axis range and check it
		range = (axis == 0 ? xMax_-xMin_ : yMax_-yMin_);

		axisMin = (axis == 0 ? xMin_ : yMin_);
		axisMax = (axis == 0 ? xMax_ : yMax_);
		baseValueIndex = 0;
		power = log10(range / maxTicks) - 1;
		iteration = 0;
		
		if (range > 1.0e-10)
		{
			do
			{
				// Calculate current tickDelta
				tickDelta = baseValues[baseValueIndex]*pow10(power);

				// Get first tickmark value
				firstTick = int(axisMin / tickDelta) * tickDelta;
				if (firstTick < axisMin) firstTick += tickDelta;
				
				// How many ticks now fit between the firstTick and max value?
				// Add 1 to get total ticks for this delta (i.e. including firstTick
				nTicks = (axisMax-firstTick) / tickDelta;
				++nTicks;
				
				// Check n...
				if (nTicks > maxTicks)
				{
					++baseValueIndex;
					if (baseValueIndex == nBaseValues) ++power;
					baseValueIndex = baseValueIndex%nBaseValues;
				}
				else if (nTicks < minTicks)
				{
					--baseValueIndex;
					if (baseValueIndex == -1)
					{
						--power;
						baseValueIndex += nBaseValues;
					}
				}
				
				++iteration;
				if (iteration == maxIterations) break;
				
			} while ((nTicks > maxTicks) || (nTicks < minTicks));
		}
		else
		{
			firstTick = axisMin;
			tickDelta = 1.0;
		}
		
		// Set tick values for current axis
		axis == 0 ? xAxisTickDelta_ = tickDelta : yAxisTickDelta_ = tickDelta;
		axis == 0 ? xAxisTickStart_ = firstTick : yAxisTickStart_ = firstTick;
	}
}

/*!
 * \brief Set main title
 */
void PlotWidget::setMainTitle(QString title)
{
	mainTitle_ = title;
	repaint();
}

/*!
 * \brief Set x-axis title
 */
void PlotWidget::setXAxisTitle(QString title)
{
	xAxisTitle_ = title;
	repaint();
}

/*!
 * \brief Set y-axis title
 */
void PlotWidget::setYAxisTitle(QString title)
{
	yAxisTitle_ = title;
	repaint();
}

/*!
 * \brief Set x and y axis titles
 */
void PlotWidget::setTitles(QString mainTitle, QString xTitle, QString yTitle)
{
	mainTitle_ = mainTitle;
	xAxisTitle_ = xTitle;
	yAxisTitle_ = yTitle;
	update();
}

/*!
 * \brief Set soft X limits for plot area
 */
void PlotWidget::setXLimits(bool setMinLimit, bool applyMinLimit, double minLimit, bool setMaxLimit, bool applyMaxLimit, double maxLimit)
{
	if (setMinLimit)
	{
		limitXMin_ = applyMinLimit;
		xMinLimit_ = minLimit;
	}
	if (setMaxLimit)
	{
		limitXMax_ = applyMaxLimit;
		xMaxLimit_ = maxLimit;
	}
	fitData(true);
}

/*!
 * \brief Set soft Y limits for plot area
 */
void PlotWidget::setYLimits(bool setMinLimit, bool applyMinLimit, double minLimit, bool setMaxLimit, bool applyMaxLimit, double maxLimit)
{
	if (setMinLimit)
	{
		limitYMin_ = applyMinLimit;
		yMinLimit_ = minLimit;
	}
	if (setMaxLimit)
	{
		limitYMax_ = applyMaxLimit;
		yMaxLimit_ = maxLimit;
	}
	fitData(true);
}

/*!
 * \brief Enable/disable autoscaling
 */
void PlotWidget::setAutoScale(bool enabled)
{
	autoScale_ = enabled;
	update();
}

/*!
 * \brief Enable/disable soft x minimum limit
 */
void PlotWidget::setXMinLimit(bool enabled)
{
	limitXMin_ = enabled;
	fitData(true);
}

/*!
 * \brief Enable/disable soft x maxiimum limit
 */
void PlotWidget::setXMaxLimit(bool enabled)
{
	limitXMax_ = enabled;
	fitData(true);
}

/*!
 * \brief Enable/disable soft y minimum limit
 */
void PlotWidget::setYMinLimit(bool enabled)
{
	limitYMin_ = enabled;
	fitData(true);
}

/*!
 * \brief Enable/disable soft y maxiimum limit
 */
void PlotWidget::setYMaxLimit(bool enabled)
{
	limitYMax_ = enabled;
	fitData(true);
}

/*
// Source Data
*/

/*!
 * \brief Add data to Plot (CheckPoint<Data2D>)
 */
PlotData* PlotWidget::addDataSet(CheckPoint<Data2D>* data, bool visible, int yOffset, QColor color, Qt::PenStyle style, PlotData::DataModifier xModifier, PlotData::DataModifier yModifier)
{
	PlotData* pd = dataSets_.add();
	pd->setSourceData(data, xModifier, yModifier);
	pd->setVisible(visible);
	pd->setVerticalOffset(yOffset);
	pd->setLineColour(color);
	pd->setLineStyle(style);
	pd->generate();
	
	// Set group index - does the group exist already?
	Dnchar* group;
	for (group = dataSetGroups_.first(); group != NULL; group = group->next) if (strcmp(data->groupName(), group->get()) == 0) break;
	if (group) pd->setGroupIndex(dataSetGroups_.indexOf(group));
	else if (data->groupName() != NULL)
	{
		group = dataSetGroups_.add();
		group->set(data->groupName());
		pd->setGroupIndex(dataSetGroups_.nItems()-1);
	}

	// Flag recreation of contextMenu_, and repaint widget
	contextMenuValid_ = false;
	repaint();

	return pd;
}

/*!
 * \brief Add data to Plot (local Data2D)
 */
PlotData* PlotWidget::addDataSet(Data2D& data, const char* groupName, bool visible, int yOffset, QColor color, Qt::PenStyle style, PlotData::DataModifier xModifier, PlotData::DataModifier yModifier)
{
	PlotData* pd = dataSets_.add();
	pd->setSourceData(data, xModifier, yModifier);
	pd->setVisible(visible);
	pd->setVerticalOffset(yOffset);
	pd->setLineColour(color);
	pd->setLineStyle(style);
	pd->generate();
	
	// Set group index - does the group exist already?
	Dnchar* group;
	for (group = dataSetGroups_.first(); group != NULL; group = group->next) if (strcmp(groupName, group->get()) == 0) break;
	if (group) pd->setGroupIndex(dataSetGroups_.indexOf(group));
	else if (groupName != NULL)
	{
		group = dataSetGroups_.add();
		group->set(groupName);
		pd->setGroupIndex(dataSetGroups_.nItems()-1);
	}

	// Flag recreation of contextMenu_, and repaint widget
	contextMenuValid_ = false;
	repaint();

	return pd;
}

/*!
 * \brief Remove specified data from Plot
 */
void PlotWidget::removeDataSet(CheckPoint<Data2D>* data)
{
	// Find specified CheckPoint data in the list
	PlotData* pd;
	for (pd = dataSets_.first(); pd != NULL; pd = pd->next) if (pd->externalSourceData() == data) break;
	if (pd == NULL)
	{
		msg.print("Tried to remove CheckPoint data from PlotWidget, but it is not being displayed.\n");
		return;
	}

	dataSets_.remove(pd);

	// Flag recreation of contextMenu_, and repaint widget
	contextMenuValid_ = false;
	repaint();
}

/*!
 * \brief Remove all data from plot
 */
void PlotWidget::removeAllDataSets()
{
	dataSets_.clear();

	// Flag recreation of contextMenu_, and repaint widget
	contextMenuValid_ = false;

	repaint();
}

/*!
 * \brief Refresh displayData_, recreating lists etc.
 */
void PlotWidget::refreshData()
{
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next) pd->generate();
	if (autoScale_) fitData(true);
	repaint();
}

/*
// Functions
*/

/*!
 * \brief Convert widget coordinates to graph coordinates
 */
QPointF PlotWidget::widgetToGraph(QPoint pos)
{
	return QPointF((pos.x() - graphArea_.left()) / xScale_ + xMin_, (graphArea_.bottom() - pos.y()) / yScale_ + yMin_ );
}

/*!
 * \brief Zoom to specified graph coordinates
 */
void PlotWidget::zoomToGraph(double x1, double y1, double x2, double y2)
{
	// Set range
	xMin_ = x1;
	xMax_ = x2;
	yMin_ = y1;
	yMax_ = y2;

	// Swap values if necessary
	double temp;
	if (xMin_ > xMax_)
	{
		temp = xMin_;
		xMin_ = xMax_;
		xMax_ = temp;
	}
	if (yMin_ > yMax_)
	{
		temp = yMin_;
		yMin_ = yMax_;
		yMax_ = temp;
	}

	// New limits, so recalculate tick values
	calculateTickDeltas(5);
// 	printf("Zoom To  %f/%f and %f/%f\n", xMin_, xMax_, yMin_, yMax_);
	
	repaint();
}

/*!
 * \brief Rescale axes to fit all current data
 */
void PlotWidget::fitData(bool obeySoftLimits)
{
	// Set initial values
	int nVisible = 0;
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
	{
		// If dataset isn't currently shown, move on
		if (!pd->visible()) continue;
		
		// If this is the first visible dataset encountered, set initial limits - otherwise, compare
		double off = verticalSpacing_ * pd->verticalOffset();
		if (nVisible == 0)
		{
			xMin_ = pd->xMin();
			xMax_ = pd->xMax();
			yMin_ = pd->yMin() + verticalSpacing_*pd->verticalOffset();
			yMax_ = pd->yMax() + verticalSpacing_*pd->verticalOffset();
		}
		else
		{
			if (pd->xMin() < xMin_) xMin_ = pd->xMin();
			if (pd->xMax() > xMax_) xMax_ = pd->xMax();
			if ((pd->yMin() + off) < yMin_) yMin_ = pd->yMin() + off;
			if ((pd->yMax() + off) > yMax_) yMax_ = pd->yMax() + off;
		}
		++nVisible;
	}
	
	if (nVisible > 0)
	{
		// Increase y limits by 5%
		yMin_ *= (yMin_ < 0 ? 1.05 : 0.95);
		yMax_ *= (yMax_ > 0 ? 1.05 : 0.95);
	}

	// Obey soft limits?
	if (obeySoftLimits)
	{
		if (limitXMin_ && (xMin_ < xMinLimit_)) xMin_ = xMinLimit_;
		if (limitXMax_ && (xMax_ > xMaxLimit_)) xMax_ = xMaxLimit_;
		if (limitYMin_ && (yMin_ < yMinLimit_)) yMin_ = yMinLimit_;
		if (limitYMax_ && (yMax_ > yMaxLimit_)) yMax_ = yMaxLimit_;
	}

	// New limits, so recalculate tick values
	calculateTickDeltas(5);

// 	printf("New axis limits are %f/%f and %f/%f\n", xMin_, xMax_, yMin_, yMax_);

	repaint();
}
