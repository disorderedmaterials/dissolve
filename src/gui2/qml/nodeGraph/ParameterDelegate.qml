import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.qmlmodels
import Dissolve
import DissolveIconsModule
import "../DissolveIconsModule"
import "../Dissolve"

DelegateChooser {
    id: root

    role: "type"

    property bool enabled: true

    DelegateChoice {
        roleValue: "bool"

        CheckBox {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            checked: param
            enabled: root.enabled

            onClicked: param = !param
        }
    }
    DelegateChoice {
        roleValue: "number"

        TextField {
            text: Number(param).toFixed(5)
            width: 30
            validator: DoubleValidator {
                bottom: -10e9
                top: 10e9
                decimals: 5
            }
            onActiveFocusChanged: { if (activeFocus) { selectAll(); } else {param = Number(text); }}
        }
    }
    DelegateChoice {
        roleValue: "optional number"

        Row {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index

            CheckBox {
                checked: param != null
                enabled: root.enabled

                onClicked: {
                    if (param == null) {
                        param = 0;
                    } else {
                        param = null;
                    }
                }
            }
            SpinBox {
                enabled: (param != null) && root.enabled
                value: param

                onValueModified: param = value
            }
        }
    }
    DelegateChoice {
        roleValue: "string"

        TextField {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            text: param
            enabled: root.enabled

            onTextChanged: param = text
        }
    }
    DelegateChoice {
        roleValue: "file path"

        Row {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            Layout.fillWidth: true
            spacing: 0

            TextField {
                id: filePathField

                text: param
                enabled: root.enabled
            }

            ToolButton {
                id: filePickerButton

                icon.source: "qrc:/DissolveIconsModule/documents.svg"
                display: AbstractButton.iconOnly
                onClicked: fileDialog.open()
                enabled: root.enabled

                ToolTip.text: "Select a file"
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            FileDialog {
                id: fileDialog

                title: "Choose a file..."
                fileMode: FileDialog.OpenFile
                onAccepted: param = Utility.urlToLocalFile(selectedFile)
            }
        }
    }
    DelegateChoice {
        id: vec3Delegate
        roleValue: "vector3"

        function createVector(xInput, yInput, zInput) {
            var x = Number(xInput);
            var y = Number(yInput);
            var z = Number(zInput);
            return [x, y, z]
        }

        Row {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            Layout.fillWidth: true
            spacing: 2

            TextField {
                id: xInput
                text: Number(param[0]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = vec3Delegate.createVector(xInput.text, yInput.text, zInput.text)
                onActiveFocusChanged: if (activeFocus) selectAll()
            }

            TextField {
                id: yInput
                text: Number(param[1]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = vec3Delegate.createVector(xInput.text, yInput.text, zInput.text)
                onActiveFocusChanged: if (activeFocus) selectAll()
            }

            TextField {
                id: zInput
                text: Number(param[2]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = vec3Delegate.createVector(xInput.text, yInput.text, zInput.text)
                onActiveFocusChanged: if (activeFocus) selectAll()
            }
        }

    }
    DelegateChoice {
        id: rangedVec3Delegate
        roleValue: "ranged vector3"

        function createRangedVector(min, value, max, hasRange) {
            var x = Number(value[0]);
            var y = Number(value[1]);
            var z = Number(value[2]);
            if (hasRange)
            {
                if (min[0] == "" || min[1] == "" || min[2] == "" || max[0] == "" || max[1] == "" || max[2] == "") {
                    rangedVec3Delegate.resetRanges();
                    return [value]
                }
                var minX = Number(min[0]);
                var minY = Number(min[1]);
                var minZ = Number(min[2]);
                var maxX = Number(max[0]);
                var maxY = Number(max[1]);
                var maxZ = Number(max[2]);
                return [[minX, minY, minZ], [x, y, z], [maxX, maxY, maxZ]]
            }
            return [value]
        }
        function resetRanges() {
            minXInput.text = ""
            minYInput.text = ""
            minZInput.text = ""
            maxXInput.text = ""
            maxYInput.text = ""
            maxZInput.text = ""
        }

        property bool applyMinMax: false

        Row {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            Layout.fillWidth: true
            spacing: 2

            TextField {
                id: minXInput
                text: ""
                width: 30
                enabled: rangedVec3Delegate.applyMinMax
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], value, min, rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Optional minimum x"
            }
            TextField {
                id: minYInput
                text: ""
                width: 30
                enabled: rangedVec3Delegate.applyMinMax
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], value, min, rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Optional minimum y"
            }
            TextField {
                id: minZInput
                text: ""
                width: 30
                enabled: rangedVec3Delegate.applyMinMax
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], value, min, rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Optional minimum z"
            }

            Text { text: '|' }

            TextField {
                id: xInput
                text: Number(param[1][0]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Value"
            }
            TextField {
                id: yInput
                text: Number(param[1][1]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Value"
            }
            TextField {
                id: zInput
                text: Number(param[1][2]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Value"
            }

            Text { text: '|' }

            TextField {
                id: maXInput
                text: ""
                width: 30
                enabled: rangedVec3Delegate.applyMinMax
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Optional maximum x"
            }
            TextField {
                id: maxYInput
                text: ""
                width: 30
                enabled: rangedVec3Delegate.applyMinMax
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Optional maximum y"
            }
            TextField {
                id: maxZInput
                text: ""
                width: 30
                enabled: rangedVec3Delegate.applyMinMax
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = rangedVec3Delegate.createRangedVector([minXInput.text, minYInput.text, minZInput.text], [xInput.text, yInput.text, zInput.text], [maxXInput.text, maxYInput.text, maxZInput.text], rangedVec3Delegate.applyMinMax)
                onActiveFocusChanged: if (activeFocus) selectAll()
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Optional maximum z"
            }

            CheckBox {
                Layout.alignment: Qt.AlignRight
                checked: rangedVec3Delegate.applyMinMax
                enabled: root.enabled

                onClicked: rangedVec3Delegate.applyMinMax = checked
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: "Apply min/max"
            }
        }
    }
    DelegateChoice {
        roleValue: "enum"

        ComboBox {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            currentIndex: param
            model: innerModel
            textRole: "display"
            valueRole: "display"
            enabled: root.enabled

            onActivated: idx => param = idx
        }
    }
    DelegateChoice {
        Text {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            text: param
            enabled: root.enabled
        }
    }
}
