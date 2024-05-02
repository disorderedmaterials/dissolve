// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

/*
 * Code below copied from:
 * https://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
 * Formatting adjustments / comments added by T. Youngs.
 * Constructor modified to take QWidget* as argument, and to set focus policy to Qt::StrongFocus which is also required for the
 * desired effect.
 */

#include <QEvent>
#include <QWidget>

class MouseWheelWidgetAdjustmentGuard : public QObject
{
    public:
    explicit MouseWheelWidgetAdjustmentGuard(QWidget *widget) : QObject(widget) { widget->setFocusPolicy(Qt::StrongFocus); }

    protected:
    // Custom event filter
    bool eventFilter(QObject *o, QEvent *e) override
    {
        /*
         * Ignore the event *if*:
         *
         * 1) The event came from a mouse wheel
         * 2) The widget is valid (obvs)
         * 3) The widget *does not* have focus
         */
        const QWidget *widget = static_cast<QWidget *>(o);
        if (e->type() == QEvent::Wheel && widget && !widget->hasFocus())
        {
            e->ignore();
            return true;
        }

        return QObject::eventFilter(o, e);
    }
};
