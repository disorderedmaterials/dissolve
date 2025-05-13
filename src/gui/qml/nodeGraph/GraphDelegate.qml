import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

NodeBox {
    property variant rootModel
    property double endX: x
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
                        text: name
                        font.pointSize: 10
                        anchors.margins: 4
                        wrapMode: Text.Wrap
                        ToolTip.visible: hovered
                        ToolTip.text: description
                    }
                }
            }

            Rectangle {
                height: parent.height
                width: (inputs.rowCount() > 0 && outputs.rowCount() > 0) ? 2 : 0
                color: palette.active.mid
            }

            Column {
                Repeater {
                    model: outputs
                    delegate: Text {
                        text: name
                        font.pointSize: 10
                        anchors.margins: 4
                        wrapMode: Text.Wrap
                        ToolTip.visible: hovered
                        ToolTip.text: description
                    }
                }
            }
        }

        Rectangle {
            width: parent.width
            height: options.rowCount() > 0 ? 2 : 0
            color: palette.active.mid
        }

        Column {
            Repeater {
                model: options
                delegate: RowLayout {
                    Text {
                        text: name
                        font.pointSize: 10
                        anchors.margins: 4
                        wrapMode: Text.Wrap
                        ToolTip.visible: hovered
                        ToolTip.text: description
                    }
                }
            }
        }
    }
}
