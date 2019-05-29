/*
	*** Keyword Widget - FileAndFormat
	*** src/gui/keywordwidgets/fileandformat_funcs.cpp
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

/*
 * Code below copied from: https://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
 * Formatting adjustments / comments added by T. Youngs.
 * Constructor modified to take QWidget* as argument, and to set focus policy to Qt::StrongFocus which is also required for the desired effect.
 */

#include <QEvent>
#include <QWidget>

class MouseWheelWidgetAdjustmentGuard : public QObject
{
	public:
	// Constructor
	explicit MouseWheelWidgetAdjustmentGuard(QWidget* widget) : QObject(widget)
	{
		widget->setFocusPolicy(Qt::StrongFocus);
	}

	protected:
	// Custom event filter
	bool eventFilter(QObject* o, QEvent* e) override
	{
		/*
		 * Ignore the event *if*:
		 * 
		 * 1) The event came from a mouse wheel
		 * 2) The widget is valid (obvs)
		 * 3) The widget *does not* have focus
		 */
		const QWidget* widget = static_cast<QWidget*>(o);
		if (e->type() == QEvent::Wheel && widget && !widget->hasFocus())
		{
			e->ignore();
			return true;
		}

		return QObject::eventFilter(o, e);
	}
};
