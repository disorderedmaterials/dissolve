/*
	*** PlotWidget PaintEvent
	*** src/gui/plotwidget_paintevent.cpp
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
#include "gui/plotwidget.uih"
#include "base/sysfunc.h"

/*!
 * \brief Widget PaintEvent
 */
void PlotWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QLineF line;
	QRectF rect;
	QString label;
	QPen dataPen;
	QBrush backgroundBrush(backgroundColour_, Qt::SolidPattern);
	painter.setBackground(backgroundBrush);
	painter.setRenderHint(QPainter::TextAntialiasing);

	// Get text line height
	rect = painter.boundingRect(QRectF(), Qt::AlignLeft, "xyz");
	double textHeight = rect.height();

	// Determine maximal label widths for each axis.
	char testText[128];
	QRectF xTextRect, yTextRect, tempRect;
	// --- X
	sprintf(testText, xLabelFormat_.get(), xMin_);
	xTextRect = painter.boundingRect(QRectF(), Qt::AlignHCenter, testText);
	sprintf(testText, xLabelFormat_.get(), xMax_);
	tempRect = painter.boundingRect(QRectF(), Qt::AlignHCenter, testText);
	if (tempRect.width() > xTextRect.width()) { xTextRect.setLeft(tempRect.left()); xTextRect.setRight(tempRect.right()); }
	// --- Y
	sprintf(testText, yLabelFormat_.get(), yMin_);
	yTextRect = painter.boundingRect(QRectF(), Qt::AlignRight | Qt::AlignVCenter, testText);
	sprintf(testText, yLabelFormat_.get(), yMax_);
	tempRect = painter.boundingRect(QRectF(), Qt::AlignRight | Qt::AlignVCenter, testText);
	if (tempRect.width() > yTextRect.width()) { yTextRect.setLeft(tempRect.left()); yTextRect.setRight(tempRect.right()); }

	// Work out areas
	font_.setPointSizeF(8.0);
	xAxisArea_.setRect(spacing_, height() - 2*spacing_ - 2*textHeight, width()-2*spacing_, 2*textHeight + 2*spacing_);
	yAxisArea_.setRect(spacing_, spacing_, yTextRect.width() + 2*spacing_ + textHeight, height()-2*spacing_);
	graphArea_.setRect(yAxisArea_.right(), 2*spacing_+textHeight, width()-2*spacing_-yAxisArea_.width(), height()-2*spacing_-textHeight-xAxisArea_.height());
	
	// Clear background
	painter.setBrush(backgroundBrush);
	painter.drawRect(0, 0, width(), height());

	// Draw simple outline around graph area, and set up clipping on this area
	painter.drawRect(graphArea_);
	painter.setClipRect(graphArea_);

	// Setup transform to give 0,0 at llh corner of graphArea_
	QTransform transform;
	xScale_ = graphArea_.width()/(xMax_-xMin_);
	yScale_ = graphArea_.height()/(yMax_-yMin_);
	transform.translate(graphArea_.left(), graphArea_.bottom());
	transform.scale(xScale_, -yScale_);
	transform.translate(-xMin_, -yMin_);
	painter.setTransform(transform);

	// Draw minor axis gridlines
	QPen grayPen(Qt::gray);
	grayPen.setStyle(Qt::DashLine);
// 	painter.setPen(Qt::gray, Qt::DashLine);
	painter.setPen(grayPen); 
	double xpos, ypos;
	int n;

	// --- X
	xpos = xAxisTickStart_;
	do
	{
		line.setLine(xpos, yMin_, xpos, yMax_);
		painter.drawLine(line);
		xpos += xAxisTickDelta_;
	} while (xpos <= xMax_);
	
	// --- Y
	ypos = yAxisTickStart_;
	do
	{
		line.setLine(xMin_, ypos, xMax_, ypos);
		painter.drawLine(line);
		ypos += yAxisTickDelta_;
	} while (ypos <= yMax_);
	
	// Draw major axis gridlines
	painter.setPen(Qt::black);
	line.setLine(xMin_, 0.0, xMax_, 0.0);
	painter.drawLine(line);
	line.setLine(0.0, yMin_, 0.0, yMax_);
	painter.drawLine(line);
	
	// Draw data
	painter.setBrush(Qt::NoBrush);
	painter.setRenderHint(QPainter::Antialiasing);
	QTransform dataTransform;
	int nDataSetsVisible = 0;
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
	{
		// Is dataset visible?
		if (!pd->visible()) continue;
		
		// Take copy of the current transformation matrix, and modify it as necessary
		dataTransform = transform;
		dataTransform.translate(0.0, verticalSpacing_*pd->verticalOffset());

		// Update Painter transform, pen, and draw the dataset path
		painter.setTransform(dataTransform);
		dataPen.setColor(pd->lineColour());
		dataPen.setStyle(pd->lineStyle());
		painter.setPen(dataPen);
		painter.drawPath(pd->painterPath());
		++nDataSetsVisible;
	}

	// Revert to original transform and pen style/color (drawing the data may have changed it)
	painter.setTransform(transform);
	painter.setPen(foregroundColour_);
	painter.setRenderHint(QPainter::Antialiasing, FALSE);

	// Interaction embellishments?
	if (!clickedWidgetPosition_.isNull())
	{
		if (buttons_&Qt::LeftButton)
		{
			rect.setBottomLeft(clickedGraphPosition_);
			rect.setTopRight(currentGraphPosition_);
			painter.drawRect(rect);
		}
	}

	// Reset transform to cover whole area again
	transform.reset();
	painter.setTransform(transform);
	painter.setClipping(FALSE);

	// -- Draw X tick marks and labels
	double lastPos, deviceXpos, deviceYpos;
	xpos = xAxisTickStart_;
	deviceXpos = (xpos - xMin_)*xScale_ + graphArea_.left();
	deviceYpos = xAxisArea_.top();
	rect = xTextRect;
	rect.translate(deviceXpos, deviceYpos+spacing_);
	lastPos = rect.right() - 2.0*rect.width();
// 	for (n=firstTickX; n<=lastTickX; ++n)
	do
	{
		// Draw tick mark
		line.setLine(deviceXpos, deviceYpos, deviceXpos, deviceYpos+spacing_*0.5);
		painter.drawLine(line);
		
		// Check for overlap between this label and the last one
		if ((rect.left()-5) > lastPos)
		{
			painter.drawText(rect, Qt::AlignHCenter, ftoa(xpos, xLabelFormat_.get()));
			lastPos = rect.right();
		}
		
		// Move text rectangle and tick position
		rect.translate(xAxisTickDelta_*xScale_, 0.0);
		deviceXpos += xAxisTickDelta_*xScale_;
		xpos += xAxisTickDelta_;
	} while (xpos <= xMax_);

	// -- Draw Y tick marks and labels
	ypos = yAxisTickStart_;
	deviceYpos = graphArea_.bottom() - (ypos - yMin_)*yScale_;
	deviceXpos = yAxisArea_.right() - 2.0;
	rect = yTextRect;
	rect.translate(deviceXpos, deviceYpos);
	lastPos = rect.top() + 2.0*rect.height();
// 	for (n=firstTickY; n<=lastTickY; ++n)
	do
	{
		// Draw tick mark
		line.setLine(deviceXpos, deviceYpos, deviceXpos+2.0, deviceYpos);
		painter.drawLine(line);
		
		// Check for overlap between this label and the last one
		if (rect.bottom() < lastPos)
		{
			painter.drawText(rect, Qt::AlignRight, ftoa(ypos, xLabelFormat_.get()));
			lastPos = rect.top();
		}
		
		// Move text rectangle and tick position
		rect.translate(0.0, -yAxisTickDelta_*yScale_);
		deviceYpos -= yAxisTickDelta_*yScale_;
		ypos += yAxisTickDelta_;
	} while (ypos <= yMax_);

	// Draw main graph title
	rect.setRect(graphArea_.left(), spacing_, graphArea_.width(), textHeight);
	painter.drawText(rect, Qt::AlignHCenter, mainTitle_);

	// Draw X-axis title
	xAxisArea_.setLeft( graphArea_.left() );
	xAxisArea_.moveTop( xAxisArea_.top() + spacing_ + textHeight);
	painter.drawText(xAxisArea_, Qt::AlignHCenter | Qt::AlignTop, xAxisTitle_);
	
	// Draw Y-axis title
	transform.reset();
	transform.translate(0, graphArea_.center().y());
	transform.rotate(-90);
	painter.setTransform(transform);
	rect.setRect(-0.5*graphArea_.height(), 0.5*(textHeight+spacing_), graphArea_.height(), textHeight);
	painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, yAxisTitle_);
	transform.reset();
	painter.setTransform(transform);

	// Draw legend (if visible, and there are datasets shown)
	if (legendVisible_ && (nDataSetsVisible > 0))
	{
		// Loop over datasets and determine maximum extent needed for titles
		QRectF textRect;
		int nDataSets = 0;
		for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
		{
			// Is dataset visible?
			if (!pd->visible()) continue;

			tempRect = painter.boundingRect(QRectF(), Qt::AlignLeft, pd->name());
			if (tempRect.width() > textRect.width()) textRect = tempRect;
			++nDataSets;
		}

		// Create rectangle for legend area
		int legendSpacing = 5, legendLineLength = 20;
		QRectF legendRect = textRect;
		legendRect.setWidth( textRect.width() + 3 * legendSpacing + legendLineLength );
		legendRect.moveRight(width()-spacing_);
		legendRect.moveTop(graphArea_.top());
		legendRect.setHeight( textRect.height() * nDataSets + (nDataSets+1) * legendSpacing );
		textRect.moveRight(width() - legendSpacing - spacing_);
		textRect.moveTop(legendRect.top() + legendSpacing);

		// Draw semi-transparent box to underpin legend
		QBrush legendBrush(backgroundColour_, Qt::SolidPattern);
		painter.setBrush(legendBrush);
		painter.drawRect(legendRect);
		QLine legendLine(legendRect.left() + legendSpacing, legendRect.top()+legendSpacing+textRect.height()/2, legendRect.left() + legendSpacing + legendLineLength, legendRect.top()+legendSpacing+textRect.height()/2);

		// Draw on dataSet names and representative lines...
		dataPen.setWidth(2);
		for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
		{
			// Is dataset visible?
			if (!pd->visible()) continue;

			painter.setPen(foregroundColour_);
			painter.drawText(textRect, Qt::AlignLeft, pd->name());
			textRect.moveTop( textRect.top() + textRect.height() + legendSpacing);
			
			dataPen.setColor(pd->lineColour());
			dataPen.setStyle(pd->lineStyle());
			painter.setPen(dataPen);
			painter.drawLine(legendLine);
			legendLine.translate(0, textRect.height()+legendSpacing);
		}
	}

// 	// Finally, draw option hash
// 	QString optionHash, xlims, ylims, temp;
// 	if (autoScale_) optionHash += "a ";
// 	xlims = "x";
// 	if (limitXMin_) xlims = "<x";
// 	if (limitXMax_) xlims += ">";
// 	ylims = "y";
// 	if (limitYMin_) ylims = "<y";
// 	if (limitYMax_) ylims += ">";
// 	temp = "";
// 	if (xlims != "x") temp += xlims;
// 	if (ylims != "y") temp += ylims;
// 	if (!temp.isEmpty()) optionHash += QString("s[") + temp + "]";
// 	
// 	painter.setPen(Qt::gray);
// 	rect.setRect(spacing_, height()-textHeight, 100.0, textHeight);
// 	painter.drawText(rect, Qt::AlignLeft, optionHash);

	// Done.
	painter.end();
}
