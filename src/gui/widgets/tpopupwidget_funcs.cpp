/*
	*** TPopupWidget Functions
	*** src/gui/widgets/tpopupwidget_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/widgets/tpopupwidget.hui"
#include "gui/widgets/tmenubutton.hui"
#include "base/messenger.h"
#include <QHideEvent>
#include <QTime>

// Static members
TPopupWidget* TPopupWidget::lastPopup_ = NULL;
QTime TPopupWidget::lastPopupHideTime_;

// Constructor
TPopupWidget::TPopupWidget(TMenuButton* parent) : QWidget(parent, Qt::FramelessWindowHint | Qt::Popup)
{
	parentMenuButton_ = parent;
	widgetDone_ = false;
	mouseEntered_ = false;
}

/*
 * Protected Functions
 */

// Local function called when the widget should be closed after a button has been selceted
void TPopupWidget::done(bool setParentButtonDown)
{
	if (parentMenuButton_) parentMenuButton_->popupDone(setParentButtonDown);
	else Messenger::print("Internal Error: No parent button set in TPopupWidget::done().\n");

	widgetDone_ = true;
	mouseEntered_ = false;

	hide();

	emit(popupDone());
}

// Notify parent button that one of our widgets has changed
void TPopupWidget::popupChanged(int data)
{
	if (parentMenuButton_) parentMenuButton_->popupWidgetChanged(data);
}

/*
 * Public
 */

// Show popup, updating any controls as necessary beforehand
void TPopupWidget::popup()
{
	// Check time of last popup - if it was too short, then we assume this was a click on the same source button
	if ((lastPopup_ == this) && (lastPopupHideTime_.msecsTo(QTime::currentTime()) < 50)) return;

	updateControls();

	show();

	lastPopup_ = this;
}

// Return parent TMenuButton
TMenuButton* TPopupWidget::parentMenuButton()
{
	return parentMenuButton_;
}

/*
 * Virtual Reimplementations
 */

void TPopupWidget::enterEvent(QEvent* event)
{
	mouseEntered_ = true;
}

void TPopupWidget::leaveEvent(QEvent* event)
{
	//if (mouseEntered_) close();
}

void TPopupWidget::hideEvent(QHideEvent* event)
{
	// Call the parent's popupDone() function, unless the widgetDone_ flag is set
	if (parentMenuButton_ && (!widgetDone_)) parentMenuButton_->popupDone(false);

	// Reset the popup's flags
	widgetDone_ = false;
	mouseEntered_ = false;

	// Set the hide time
	lastPopupHideTime_ = QTime::currentTime();

	event->accept();

	// Notify the parent button that we have been hidden
	if (parentMenuButton_) parentMenuButton_->popupHidden();

	emit(popupDone());
}
