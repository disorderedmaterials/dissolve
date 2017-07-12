/*
	*** TPlotWidget Functions
	*** src/gui/qcustomplot/tplotwidget_funcs.cpp
	Copyright T. Youngs 2016-2017

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

#include "gui/qcustomplot/tplotwidget.hui"
#include <QVBoxLayout>
#include <QToolButton>
#include "gui/qcustomplot/qcustomplot.hui"

// Constructor
TPlotWidget::TPlotWidget(QWidget* parent) : QWidget(parent)
{
	barsGroup_ = NULL;

	// Create a suitable layout to contain our controls
	layout_ = new QVBoxLayout(this);
	layout_->setMargin(2);
	layout_->setSpacing(2);

	// Create main plot and set some defaults
	plot_ = new QCustomPlot(this);
	QFont labelFont = plot_->xAxis->labelFont();
	labelFont.setPointSizeF(7.5);
	plot_->xAxis->setLabelFont(labelFont);
	plot_->yAxis->setLabelFont(labelFont);
	QFont tickLabelFont = plot_->xAxis->tickLabelFont();
	tickLabelFont.setPointSize(7);
	plot_->xAxis->setTickLabelFont(tickLabelFont);
	plot_->yAxis->setTickLabelFont(tickLabelFont);
	plot_->legend->setVisible(true);
	plot_->legend->setFont(labelFont);

	// Create buttons
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->setMargin(2);
	buttonLayout->setSpacing(2);
	buttonLayout->insertStretch(0, 1);
	clearButton_ = new QToolButton;
	clearButton_->setIcon(QIcon(":/basic/icons/basic_cross.svg"));
	clearButton_->setIconSize(QSize(14, 14));
	clearButton_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	clearButton_->setToolTip("Clear all data from the graph");
	buttonLayout->addWidget(clearButton_);

	// Add widgets to layout
	layout_->addWidget(plot_);
	layout_->addLayout(buttonLayout);

	// Connect signals
	connect(clearButton_, SIGNAL(clicked(bool)), this, SLOT(clearButtonClicked(bool)));
	//connect(filesList_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(filesListItemClicked(QListWidgetItem*)));
}

// Return plot widget
QCustomPlot* TPlotWidget::plot()
{
	return plot_;
}

/*
 * Plot Data
 */

// Add the specified data source to the QCustomPlot
QCPGraph* TPlotWidget::addData(PlotData& source, PlotDataStyle::DataStyle style)
{
	// Copy the source data
	PlotData* data = data_.add();
	(*data) = source;

	// Add a new graph to the plot
	QCPGraph* graph = plot_->addGraph();
	graph->setData(data->x(), data->y());
	graph->setName(data->title());

	// Set the style of the graph
	QPen pen;
	QBrush brush;
	PlotDataStyles::penAndBrush(style, pen, brush);
	graph->setPen(pen);
	graph->setBrush(brush);

	plot_->replot();
}

// Add the specified data source as a bar chart to the QCustomPlot
QCPBars* TPlotWidget::addBarsData(PlotData& source, bool group, PlotDataStyle::DataStyle style)
{
	// Copy the source data
	PlotData* data = data_.add();
	(*data) = source;

	QCPBars* graph = new QCPBars(plot_->xAxis, plot_->yAxis);
	graph->setData(data->x(), data->y());
	graph->setName(data->title());
	graph->setWidthType(QCPBars::wtPlotCoords);
	graph->setWidth(0.1);

	// Add the new graph to the bars group if requested
	if (group)
	{
		if (barsGroup_ == NULL)
		{
			barsGroup_ = new QCPBarsGroup(plot_);
		}
		barsGroup_->append(graph);
	}

	plot_->replot();
}

/*
 * Slots
 */

// Clear button clicked
void TPlotWidget::clearButtonClicked(bool checked)
{
}
