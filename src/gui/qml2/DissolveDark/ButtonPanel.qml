// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Controls.impl

Rectangle {
    id: panel

    property Item control
    property bool highlighted: control.highlighted

    visible: !control.flat || control.down || control.checked

    gradient: {
        if (!control.enabled) { Theme.disabledGradient }
        else (control.down || control.checked ? Theme.buttonDownGradient : Theme.buttonUpGradient)
    }

    radius: 2
    border.color: Theme.mid

    Rectangle {
        x: 1; y: 1
        width: parent.width - 2
        height: parent.height - 2
        border.color: Theme.background
        color: "transparent"

        radius: 2
    }
}
