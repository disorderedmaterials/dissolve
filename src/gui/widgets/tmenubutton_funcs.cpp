/*
	*** TMenuButton Functions
	*** src/gui/widgets/tmenubutton_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/widgets/tmenubutton.hui"
#include "gui/widgets/tpopupwidget.hui"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStylePainter>
#include <QtWidgets/QStyleOptionToolButton>
#include <QButtonGroup>

// Static Singletons
List<TMenuButtonGroup> TMenuButton::groups_;

// Constructor
TMenuButton::TMenuButton(QWidget* parent) : QToolButton(parent)
{
	// Nullify popup widget to start with
	popupWidget_ = NULL;
	instantPopup_ = false;
	group_ = NULL;

	// Set popup timer delay, style, and connect slot
	popupTimer_.setSingleShot(true);
	popupTimer_.setInterval(QApplication::style()->styleHint(QStyle::SH_ToolButton_PopupDelay));
	connect(&popupTimer_, SIGNAL(timeout()), this, SLOT(popup()));
	popupLocation_ = TMenuButton::PopupOnBottom;

	// Connect signals for pressed / released
	connect(this, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	connect(this, SIGNAL(released()), this, SLOT(buttonReleased()));
}

/*
 * Button
 */

// Return user-assigned index of button
int TMenuButton::index()
{
	return index_;
}

/*
 * Definition
 */

// Set popup widget for button
void TMenuButton::setPopupWidget(TPopupWidget* widget, bool instantPopup)
{
	popupWidget_ = widget;
	instantPopup_ = instantPopup;
}

// Return popup widget set for button
TPopupWidget* TMenuButton::popupWidget()
{
	return popupWidget_;
}

// Set where the popup (if any) should be located relative to the button
void TMenuButton::setPopupLocation(TMenuButton::PopupLocation location)
{
	popupLocation_ = location;
}

// Notify button that popup is done
void TMenuButton::popupDone(bool setButtonDown)
{
	// What we do here exactly depends on the current state of the button
	if (!setButtonDown)
	{
		setDown(false);
		setChecked(false);
	}
	else if (group_ && (!isDown())) group_->setCurrentButton(this);

// 	// Click button if requested
// 	if (clickButton && (!setButtonDown)) emit(click());
}

// Notify button that the popup has been hidden
void TMenuButton::popupWidgetHidden()
{
	emit(popupHidden());
}

// Notify button that a widget on the popup has been changed
void TMenuButton::popupWidgetChanged(int data)
{
	emit(popupChanged(data));
}

/*
 * Widget Functions
 */

void TMenuButton::paintEvent(QPaintEvent* event)
{
	// Initialise a stylepainter and the style option
	QStylePainter painter(this);
	QStyleOptionToolButton opt;
	initStyleOption(&opt);

	// Draw the control
	QRect buttonRect, menuarea;
	buttonRect = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton, this);
	menuarea = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);

	QStyle::State bflags = opt.state & ~QStyle::State_Sunken;

	if (bflags & QStyle::State_AutoRaise) 
	{
		if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) bflags &= ~QStyle::State_Raised;
	}
	QStyle::State mflags = bflags;
	if (opt.state & QStyle::State_Sunken)
	{
		if (opt.activeSubControls & QStyle::SC_ToolButton) bflags |= QStyle::State_Sunken;
		mflags |= QStyle::State_Sunken;
	}

	// Draw button border
	QStyleOption tool(0);
	tool.palette = opt.palette;
	if (opt.subControls & QStyle::SC_ToolButton)
	{
		if (bflags & (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised))
		{
			tool.rect = buttonRect;
			tool.state = bflags;
			painter.drawPrimitive(QStyle::PE_PanelButtonTool, opt);
		}
	}

	// Draw on focus rect if necessary
	if (opt.state & QStyle::State_HasFocus)
	{
		QStyleOptionFocusRect fr;
		fr.QStyleOption::operator=(opt);
		fr.rect.adjust(3, 3, -3, -3);
		if (opt.features & QStyleOptionToolButton::MenuButtonPopup)
		fr.rect.adjust(0, 0, -style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this), 0);
		style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &painter, this);
	}

	// Draw the main label, icon, and arrow on the tool button
	// Based on code from drawControl() in qcommonstyle.cpp (Qt 5.4.1)
	QStyleOptionToolButton inner = opt;
	inner.state = bflags;
	int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt, this);
	inner.rect = buttonRect.adjusted(fw, fw, -fw, -fw);

	// Add in padding around edge of button
	QRect rect = inner.rect;
	rect.adjust(2, 2, -2, -2);

	// Translate rectangle if button state demands it
	if (inner.state & (QStyle::State_Sunken | QStyle::State_On))
	{
		int shiftX = style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &inner, this);
		int shiftY = style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &inner, this);
		rect.translate(shiftX, shiftY);
	}

	// Grab some values for the icon state and mode
	QIcon::State state = inner.state & QStyle::State_On ? QIcon::On : QIcon::Off;
	QIcon::Mode mode;
	if (!(inner.state & QStyle::State_Enabled)) mode = QIcon::Disabled;
	else if ((inner.state & QStyle::State_MouseOver) && (inner.state & QStyle::State_AutoRaise)) mode = QIcon::Active;
	else mode = QIcon::Normal;

	// Get suitable pixmap for icon
	QSize pixmapSize(iconSize().width(), iconSize().height());
	QPixmap pm = inner.icon.pixmap(pixmapSize, mode, state);
	pixmapSize = pm.size() / pm.devicePixelRatio();

	// Arrow type always overrules and is always shown
	bool hasArrow = inner.features & QStyleOptionToolButton::Arrow;
	if (((!hasArrow && inner.icon.isNull()) && !inner.text.isEmpty()) || inner.toolButtonStyle == Qt::ToolButtonTextOnly)
	{
		int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
		if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &inner, this))
		alignment |= Qt::TextHideMnemonic;
		painter.setFont(inner.font);
		style()->drawItemText(&painter, rect, alignment, inner.palette, inner.state & QStyle::State_Enabled, inner.text, QPalette::ButtonText);
	}
	else
	{
		if (inner.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
		{
			// Text Under Icon Style:
			//    ________________
			//   |                |
			//   |                |
			//   |     ICON       |  = Icon (remaining height)
			//   |                |
			//   |________________|
			//   |----------------|  = 2 px spacing
			//   |     TEXT       |  = Text (height determined by font)
			//   |________________|  
			//   |----------------|  = 2 px spacing?????
			//   |________________|  = 6 px for arrow or ellipsis if required 
			
			// Draw arrow (if there is an associated popupwidget)
			if (popupWidget_)
			{
				QRect arrowRect = rect;
				arrowRect.setTop(arrowRect.bottom() - 6);
				QStyleOption arrowOpt;
				arrowOpt.rect = arrowRect;
				arrowOpt.palette = inner.palette;
				arrowOpt.state = inner.state;
				if (!instantPopup_) drawEllipsis(arrowOpt, painter);
				else style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrowOpt, &painter, this);
			}
			rect.setHeight(rect.height() - 8);

			// Draw text
			int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::AlignBottom;
			if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &inner, this)) alignment |= Qt::TextHideMnemonic;
			QRect textRect = rect;
			textRect.setTop(textRect.bottom()-1);
			textRect = style()->itemTextRect(inner.fontMetrics, textRect, alignment, inner.state & QStyle::State_Enabled, inner.text);
			painter.setFont(inner.font);
			style()->drawItemText(&painter, textRect, alignment, inner.palette, inner.state & QStyle::State_Enabled, inner.text, QPalette::ButtonText);
			// -- Adjust available rect now by subtracting text height plus spacing
			rect.setHeight(rect.height() - textRect.height() - 2);

			// Draw icon (centred in remaining space in button)
			style()->drawItemPixmap(&painter, rect, Qt::AlignCenter, pm);
		}
		else if (inner.toolButtonStyle == Qt::ToolButtonTextBesideIcon)
		{
			// Draw arrow (if there is an associated popupwidget)
			if (popupWidget_)
			{
				QRect arrowRect = rect;
				arrowRect.setTop(arrowRect.bottom() - 6);
				QStyleOption arrowOpt;
				arrowOpt.rect = arrowRect;
				arrowOpt.palette = inner.palette;
				arrowOpt.state = inner.state;
				if (!instantPopup_) drawEllipsis(arrowOpt, painter);
				else style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrowOpt, &painter, this);
			}
			rect.setHeight(rect.height() - 8);

			// Draw icon on the left-hand side of the button
			QRect iconRect = rect;
			style()->drawItemPixmap(&painter, iconRect, Qt::AlignLeft | Qt::AlignVCenter, pm);

			// Draw text
			int alignment = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic;
			if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &inner, this)) alignment |= Qt::TextHideMnemonic;
			QRect textRect = rect.adjusted(iconSize().width()+2, 0, 0, 0);
			textRect = style()->itemTextRect(inner.fontMetrics, textRect, alignment, inner.state & QStyle::State_Enabled, inner.text);
			painter.setFont(inner.font);
			style()->drawItemText(&painter, textRect, alignment, inner.palette, inner.state & QStyle::State_Enabled, inner.text, QPalette::ButtonText);
		}
		else if ((inner.toolButtonStyle == Qt::ToolButtonIconOnly) && (!inner.icon.isNull()))
		{
			// Draw arrow / ellipsis (if there is an associated popupwidget)
			// Don't leave free space if for would-be arrow/ellipsis if it is a checkable button
			if (popupWidget_)
			{
				QRect arrowRect = rect;
				arrowRect.setTop(arrowRect.bottom() - 6);
// 				if (popupLocation_ == TMenuButton::PopupOnBottom) arrowRect.setTop(arrowRect.bottom() - 6);
// 				else if (popupLocation_ == TMenuButton::PopupOnTop) arrowRect.setBottom(arrowRect.top() + 6);
// 				else if (popupLocation_ == TMenuButton::PopupOnLeft) arrowRect.setRight(arrowRect.left() + 6);
// 				else if (popupLocation_ == TMenuButton::PopupOnRight) arrowRect.setLeft(arrowRect.right() - 6);
				QStyleOption arrowOpt;
				arrowOpt.rect = arrowRect;
				arrowOpt.palette = inner.palette;
				arrowOpt.state = inner.state;
				if (!instantPopup_) drawEllipsis(arrowOpt, painter);
				else
				{
					if (popupLocation_ == TMenuButton::PopupOnBottom) style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrowOpt, &painter, this);
					else if (popupLocation_ == TMenuButton::PopupOnTop) style()->drawPrimitive(QStyle::PE_IndicatorArrowUp, &arrowOpt, &painter, this);
					else if (popupLocation_ == TMenuButton::PopupOnLeft) style()->drawPrimitive(QStyle::PE_IndicatorArrowLeft, &arrowOpt, &painter, this);
					else if (popupLocation_ == TMenuButton::PopupOnRight) style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &arrowOpt, &painter, this);
				}
			}
			if (!isCheckable()) rect.setHeight(rect.height() - 6);

			// Draw icon
			style()->drawItemPixmap(&painter, rect, Qt::AlignCenter | Qt::AlignVCenter, pm);
		}
	}
}

void TMenuButton::drawEllipsis(QStyleOption& styleOption, QPainter& painter)
{
	// Set up colours and deltas for circles
	QColor colour1, colour2;
	if (!(styleOption.state & QStyle::State_Enabled))
	{
		colour1 = styleOption.palette.light().color();
		colour2 = styleOption.palette.mid().color();
	}
	else if (styleOption.state & QStyle::State_MouseOver)
	{
		colour1 = styleOption.palette.mid().color();
		colour2 = styleOption.palette.buttonText().color();
	}
	else
	{
		colour1 = styleOption.palette.light().color();
		colour2 = styleOption.palette.mid().color();
	}
	// For the deltas, just need to work out in which dir the delta is
	QPoint delta;
	if (styleOption.rect.width() > styleOption.rect.height()) delta = QPoint(6,0);
	else delta = QPoint(0,6);

	// Draw the first circles (offset by 1px down and right)
	painter.translate(1, 1);
	painter.setBrush(colour1);
	painter.setPen(colour1);
	painter.drawEllipse(styleOption.rect.center(), 2, 2);
	painter.drawEllipse(styleOption.rect.center()-delta, 2, 2);
	painter.drawEllipse(styleOption.rect.center()+delta, 2, 2);

	// Draw second set of circles (without offset)
	painter.translate(-1, -1);
	painter.setBrush(colour2);
	painter.setPen(colour2);
	painter.drawEllipse(styleOption.rect.center(), 2, 2);
	painter.drawEllipse(styleOption.rect.center()-delta, 2, 2);
	painter.drawEllipse(styleOption.rect.center()+delta, 2, 2);
}

QSize TMenuButton::sizeHint() const
{
	// Framewidth and padding around button (all styles)
	QStyleOptionToolButton opt;
	int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt, this);
	int padding = 2;
	int w = padding*2 + fw*2;
	int h = padding*2 + fw*2;

	if (toolButtonStyle() == Qt::ToolButtonTextUnderIcon)
	{
		// Arrow / ellipsis footer
		h += 8;

		// Get text rect
		QRect textRect = style()->itemTextRect(fontMetrics(), textRect, 0, true, text());

		// Set width from larger of icon size and text width
		w += iconSize().width() > textRect.width() ? iconSize().width() : textRect.width();

		// Add on height from icon and text
		h += textRect.height() + 2 + iconSize().height();
	}
	else if (toolButtonStyle() == Qt::ToolButtonTextBesideIcon)
	{
		// Arrow / ellipsis footer
		h += 8;

		// Get text rect
		QRect textRect = style()->itemTextRect(fontMetrics(), textRect, 0, true, text());

		// Add on larger of iconSize().height() and textRect.height()...
		h += iconSize().height() > textRect.height() ? iconSize().height() : textRect.height();

		// Set final width
		w += iconSize().width() + 2 + textRect.width();
	}
	else if ((toolButtonStyle() == Qt::ToolButtonIconOnly) && (!icon().isNull()))
	{
		// Arrow / ellipsis footer
		h += 8;

		// Adjust for size of icon
		w += iconSize().width();
		h += iconSize().height();
	}

	return QSize(w,h);
}

// Mouse button was pressed on the button
void TMenuButton::buttonPressed()
{
// 	printf("PRESSED [%s] (isDown=%i, isChecked=%i)\n", qPrintable(text()), isDown(), isChecked());

	// Store current checked state of button
	checkedBeforePressed_ = isChecked();

	// Start popup timer (if not an instantPopup_)
	if (popupWidget_)
	{
		if (instantPopup_) emit(popup());
		else popupTimer_.start();
	}
}

// Mouse button was released
void TMenuButton::buttonReleased()
{
// 	printf("RELEASED [%s]\n", qPrintable(text()));
	// Make sure timer is stopped - it will have either already popped, or this was a single-click and we don't want the popup
	popupTimer_.stop();

	// Check the mouse coordinates at the point of release
	QPoint point = mapFromGlobal(QCursor::pos());
	if ((point.x() >= width()) || (point.y() >= height()) || (point.x() < 0) || (point.y() < 0))
	{
		return;
	}
	else
	{
		// Signal the group of this button (if there is one) that the button has been properly clicked
		if (group_) group_->setCurrentButton(this);
	}
}

// Call the popup widget
void TMenuButton::popup()
{
	if (!popupWidget_) return;

// 	printf("POPUP signalled (wasChecked = %i, down = %i).\n", checkedBeforePressed_, popupWidget_->shown());

	// Show the popup widget - call the widget's popup() method, so controls are set correctly
	popupWidget_->popup();

	// 	printf("Cursor = %i %i\n", QCursor::pos().x(), QCursor::pos().y());
	if (!parentWidget())
	{
		printf("No parent widget in TMenuButton - can't position popupWidget_.\n");
		return;
	}

	// Reposition popup widget
	QPoint popupPos;
	switch (popupLocation_)
	{
		case (TMenuButton::PopupOnBottom):
			// Flush left with the tool button, and immediately underneath it
			popupPos = parentWidget()->mapToGlobal(pos()+QPoint(0, height()));
			break;
		case (TMenuButton::PopupOnTop):
			// Flush left with the tool button, and immediately on top of it
			popupPos = parentWidget()->mapToGlobal(pos()+QPoint(0, -popupWidget_->height()));
			break;
		case (TMenuButton::PopupOnLeft):
			// Flush top with the tool button, and immediately to its left
			popupPos = parentWidget()->mapToGlobal(pos()+QPoint(-popupWidget_->width(), 0));
			break;
		case (TMenuButton::PopupOnRight):
			// Flush top with the tool button, and immediately to its right
			popupPos = parentWidget()->mapToGlobal(pos()+QPoint(width(), 0));
			break;
	}
	popupWidget_->move(popupPos);
}

// Return whether popup (if there is one) is visible
bool TMenuButton::popupVisible()
{
	if (popupWidget_) return popupWidget_->isVisible();
	else return false;
}

/*
 * Group
 */

// Add this button to the named group
void TMenuButton::setGroup(QString groupName, int index)
{
	// Search for this group...
	TMenuButtonGroup* group;
	for (group = groups_.first(); group != NULL; group = group->next()) if (group->name() == groupName) break;
	if (group == NULL)
	{
		group = groups_.add();
		group->setName(groupName);
	}

	group_ = group;
	group_->addButton(this);
	index_ = index;
}

// Check specified button in specified group
bool TMenuButton::setGroupButtonChecked(QString groupName, int buttonIndex)
{
	// First, find named group
	TMenuButtonGroup* group;
	for (group = groups_.first(); group != NULL; group = group->next()) if (group->name() == groupName) break;
	if (!group)
	{
		Messenger::print("Internal error: No TMenuButton group named '%s'\n", qPrintable(groupName));
		return false;
	}

	return group->setCurrentButton(buttonIndex);
}
