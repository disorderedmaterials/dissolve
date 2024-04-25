// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

Rectangle {
    id: indicator

    property T.AbstractButton control

    implicitWidth: 40
    implicitHeight: 16

    radius: 2
    border.color: Theme.colours.mid

    gradient: Theme.controlBackgroundGradient

    Rectangle {
        x: indicator.control.mirrored ? handle.x : 0
        width: indicator.control.mirrored ? parent.width - handle.x : handle.x + handle.width
        height: parent.height

        opacity: indicator.control.checked ? 1 : 0
        Behavior on opacity {
            enabled: !indicator.control.down
            NumberAnimation { duration: 80 }
        }

        radius: 2
        border.color: Theme.colours.mid
        border.width: indicator.control.enabled ? 1 : 0

        gradient: Theme.controlForegroundGradient
    }

    Rectangle {
        id: handle
        x: Math.max(0, Math.min(parent.width - width, indicator.control.visualPosition * parent.width - (width / 2)))
        y: (parent.height - height) / 2
        width: 20
        height: 16
        radius: 2

        gradient: Theme.getForegroundGradient(control)
        border.width: 1
        border.color: "transparent"

        Rectangle {
            width: parent.width
            height: parent.height
            border.color: Theme.getForegroundColour(control)
            color: "transparent"
            radius: 2

            Rectangle {
                x: 1; y: 1
                width: parent.width - 2
                height: parent.height - 2
                border.color: Theme.colours.mid
                color: "transparent"
                radius: 2
            }
        }

        Behavior on x {
            enabled: !indicator.control.down
            SmoothedAnimation { velocity: 200 }
        }
    }
}
