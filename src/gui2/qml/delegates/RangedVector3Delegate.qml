import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.qmlmodels
import Dissolve
import DissolveIconsModule
import "../DissolveIconsModule"
import "../Dissolve"

DelegateChoice {
    id: root

    function createRangedVector(min, value, max, applyRange) {
        var unRangedValue = [[], [Number(value[0]), Number(value[1]), Number(value[2])], []];
        if (applyRange) {
            var hasMin = !(min[0] == "" && min[1] == "" && min[2] == "");
            var hasMax = !(max[0] == "" && max[1] == "" && max[2] == "");
            if (!(hasMin || hasMax))
                return unRangedValue;
            if (hasMax && !hasMin)
                return [[], [Number(value[0]), Number(value[1]), Number(value[2])], [Number(max[0]), Number(max[1]), Number(max[2])]];
            if (hasMin && !hasMax)
                return [[Number(min[0]), Number(min[1]), Number(min[2])], [Number(value[0]), Number(value[1]), Number(value[2])], []];
            return [[Number(min[0]), Number(min[1]), Number(min[2])], [Number(value[0]), Number(value[1]), Number(value[2])], [Number(max[0]), Number(max[1]), Number(max[2])]];
        }
        return unRangedValue;
    }

    Row {
        Layout.alignment: Qt.AlignRight
        Layout.column: 2
        Layout.fillWidth: true
        Layout.row: index
        spacing: 2

        TextField {
            id: xInput

            ToolTip.text: "X value"
            ToolTip.visible: hovered
            text: Number(param[1][0]).toFixed(5)
            width: 30

            validator: DoubleValidator {
                bottom: -10e9
                decimals: 5
                top: 10e9
            }

            onAccepted: param = root.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], applyMinMaxCheckBox.checked)
            onActiveFocusChanged: if (activeFocus)
                selectAll()
        }
        TextField {
            id: yInput

            ToolTip.text: "Y value"
            ToolTip.visible: hovered
            text: Number(param[1][1]).toFixed(5)
            width: 30

            validator: DoubleValidator {
                bottom: -10e9
                decimals: 5
                top: 10e9
            }

            onAccepted: param = root.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], applyMinMaxCheckBox.checked)
            onActiveFocusChanged: if (activeFocus)
                selectAll()
        }
        TextField {
            id: zInput

            ToolTip.text: "Z value"
            ToolTip.visible: hovered
            text: Number(param[1][2]).toFixed(5)
            width: 30

            validator: DoubleValidator {
                bottom: -10e9
                decimals: 5
                top: 10e9
            }

            onAccepted: param = root.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], applyMinMaxCheckBox.checked)
            onActiveFocusChanged: if (activeFocus)
                selectAll()
        }
        CheckBox {
            id: applyMinMaxCheckBox

            Layout.alignment: Qt.AlignRight
            ToolTip.text: "Apply min/max"
            ToolTip.visible: hovered
            checked: false
            enabled: root.enabled
        }
        Button {
            id: rangeEditButton

            property Popup editor: Popup {
                id: popup

                property var lastMax: ["", "", ""]
                property var lastMin: ["", "", ""]
                property var maxRejectAnimation: null
                property var minRejectAnimation: null

                function verifyMaxRange(max) {
                    var hasMaxVector3 = !(max[0] == "" || max[1] == "" || max[2] == "");
                    var isEmpty = max[0] == "" && max[1] == "" && max[2] == "";
                    if (hasMaxVector3 || isEmpty)
                        return true;
                    maxRejectAnimation.start();
                    return false;
                }
                function verifyMinRange(min) {
                    var hasMinVector3 = !(min[0] == "" || min[1] == "" || min[2] == "");
                    var isEmpty = min[0] == "" && min[1] == "" && min[2] == "";
                    if (hasMinVector3 || isEmpty)
                        return true;
                    minRejectAnimation.start();
                    return false;
                }

                closePolicy: Popup.NoAutoClose
                height: 180
                padding: 2
                parent: Overlay.overlay
                width: 120

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 2

                    Rectangle {
                        id: header

                        border.color: "lightgrey"
                        border.width: 1
                        color: "white"
                        height: 20
                        radius: 3
                        width: parent.width

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 3
                            anchors.rightMargin: 6
                            spacing: 2

                            Label {
                                Layout.fillWidth: true
                                color: "grey"
                                font.bold: true
                                text: qsTr("Range editor")
                            }
                        }
                    }
                    ColumnLayout {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        spacing: 4

                        GroupBox {
                            id: minGroupBox

                            property color inputColor: applyMinMaxCheckBox.checked ? "transparent" : "lightblue"

                            Layout.fillWidth: true
                            title: qsTr("Lower bound")

                            SequentialAnimation {
                                Component.onCompleted: popup.minRejectAnimation = this

                                PropertyAnimation {
                                    duration: 150
                                    property: "inputColor"
                                    target: minGroupBox
                                    to: "red"
                                }
                                PropertyAnimation {
                                    duration: 500
                                    property: "inputColor"
                                    target: minGroupBox
                                    to: "transparent"
                                }
                            }
                            ColumnLayout {
                                anchors.fill: parent

                                Row {
                                    Layout.fillWidth: true
                                    spacing: 2

                                    TextField {
                                        id: minXInput

                                        ToolTip.text: "Minimum x"
                                        ToolTip.visible: hovered
                                        enabled: applyMinMaxCheckBox.checked
                                        text: ""
                                        width: 30

                                        background: Rectangle {
                                            border.width: 1
                                            color: minGroupBox.inputColor
                                        }
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            decimals: 5
                                            top: 10e9
                                        }

                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                    }
                                    TextField {
                                        id: minYInput

                                        ToolTip.text: "Minimum y"
                                        ToolTip.visible: hovered
                                        enabled: applyMinMaxCheckBox.checked
                                        text: ""
                                        width: 30

                                        background: Rectangle {
                                            border.width: 1
                                            color: minGroupBox.inputColor
                                        }
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            decimals: 5
                                            top: 10e9
                                        }

                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                    }
                                    TextField {
                                        id: minZInput

                                        ToolTip.text: "Minimum z"
                                        ToolTip.visible: hovered
                                        enabled: applyMinMaxCheckBox.checked
                                        text: ""
                                        width: 30

                                        background: Rectangle {
                                            border.width: 1
                                            color: minGroupBox.inputColor
                                        }
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            decimals: 5
                                            top: 10e9
                                        }

                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                    }
                                }
                            }
                        }
                        GroupBox {
                            id: maxGroupBox

                            property color inputColor: applyMinMaxCheckBox.checked ? "transparent" : "lightblue"

                            Layout.fillWidth: true
                            title: qsTr("Upper bound")

                            SequentialAnimation {
                                Component.onCompleted: popup.maxRejectAnimation = this

                                PropertyAnimation {
                                    duration: 150
                                    property: "inputColor"
                                    target: maxGroupBox
                                    to: "red"
                                }
                                PropertyAnimation {
                                    duration: 500
                                    property: "inputColor"
                                    target: maxGroupBox
                                    to: "transparent"
                                }
                            }
                            ColumnLayout {
                                anchors.fill: parent

                                Row {
                                    Layout.fillWidth: true
                                    spacing: 2

                                    TextField {
                                        id: maxXInput

                                        ToolTip.text: "Maximum x"
                                        ToolTip.visible: hovered
                                        enabled: applyMinMaxCheckBox.checked
                                        text: ""
                                        width: 30

                                        background: Rectangle {
                                            border.width: 1
                                            color: maxGroupBox.inputColor
                                        }
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            decimals: 5
                                            top: 10e9
                                        }

                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                    }
                                    TextField {
                                        id: maxYInput

                                        ToolTip.text: "Maximum y"
                                        ToolTip.visible: hovered
                                        enabled: applyMinMaxCheckBox.checked
                                        text: ""
                                        width: 30

                                        background: Rectangle {
                                            border.width: 1
                                            color: maxGroupBox.inputColor
                                        }
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            decimals: 5
                                            top: 10e9
                                        }

                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                    }
                                    TextField {
                                        id: maxZInput

                                        ToolTip.text: "Maximum z"
                                        ToolTip.visible: hovered
                                        enabled: applyMinMaxCheckBox.checked
                                        text: ""
                                        width: 30

                                        background: Rectangle {
                                            border.width: 1
                                            color: maxGroupBox.inputColor
                                        }
                                        validator: DoubleValidator {
                                            bottom: -10e9
                                            decimals: 5
                                            top: 10e9
                                        }

                                        onActiveFocusChanged: if (activeFocus)
                                            selectAll()
                                    }
                                }
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        anchors.rightMargin: 8
                        spacing: 8

                        Item {
                            Layout.fillWidth: true
                        }
                        Button {
                            enabled: applyMinMaxCheckBox.checked
                            text: qsTr("Apply")

                            onClicked: {
                                const min = [minXInput.text, minYInput.text, minZInput.text];
                                const max = [maxXInput.text, maxYInput.text, maxZInput.text];
                                if (popup.verifyMinRange(min) && popup.verifyMaxRange(max)) {
                                    param = root.createRangedVector(min, [xInput.text, yInput.text, zInput.text], max, applyMinMaxCheckBox.checked);
                                    popup.lastMin = min;
                                    popup.lastMax = max;
                                    popup.close();
                                }
                            }
                        }
                        Button {
                            text: qsTr("Cancel")

                            onClicked: {
                                if (!popup.verifyMinRange([minXInput.text, minYInput.text, minZInput.text])) {
                                    minXInput.text = popup.lastMin[0];
                                    minYInput.text = popup.lastMin[1];
                                    minZInput.text = popup.lastMin[2];
                                }
                                if (!popup.verifyMaxRange([maxXInput.text, maxYInput.text, maxZInput.text])) {
                                    maxXInput.text = popup.lastMax[0];
                                    maxYInput.text = popup.lastMax[1];
                                    maxZInput.text = popup.lastMax[2];
                                }
                                popup.close();
                            }
                        }
                    }
                }
            }

            ToolTip.text: "Edit x, y, and z ranges"
            ToolTip.visible: hovered
            height: applyMinMaxCheckBox.height * 1.2
            icon.source: "qrc:/DissolveIconsModule/wizard.svg"
            width: height

            onClicked: {
                const p = rangeEditButton.mapToItem(Overlay.overlay, rangeEditButton.width, 0);
                editor.x = p.x;
                editor.y = p.y;
                editor.open();
            }
        }
    }
}
