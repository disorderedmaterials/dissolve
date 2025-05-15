import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

NodeBox {
    property double endX: x
    property variant rootModel
    property double startX: x + width

    image: icon
    nodeType: name
    px: posX
    py: posY
    x: posX
    y: posY

    onDeleted: rootModel.deleteNode(index)

    Column {
        Row {
            Column {
                Repeater {
                    model: inputs

                    delegate: Text {
                        ToolTip.text: description
                        ToolTip.visible: hovered
                        anchors.margins: 4
                        font.pointSize: 10
                        text: name
                        wrapMode: Text.Wrap
                    }
                }
            }
            Rectangle {
                color: palette.active.mid
                height: parent.height
                width: (inputs.rowCount() > 0 && outputs.rowCount() > 0) ? 2 : 0
            }
            Column {
                Repeater {
                    model: outputs

                    delegate: Text {
                        ToolTip.text: description
                        ToolTip.visible: hovered
                        anchors.margins: 4
                        font.pointSize: 10
                        text: name
                        wrapMode: Text.Wrap
                    }
                }
            }
        }
        Rectangle {
            color: palette.active.mid
            height: options.rowCount() > 0 ? 2 : 0
            width: parent.width
        }
        Column {
            Repeater {
                model: options

                delegate: ParameterDelegate {
                }
            }
        }
    }
}
