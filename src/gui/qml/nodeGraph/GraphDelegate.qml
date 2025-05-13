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

    ListView {
        id: inputList
        anchors.left: parent.left
        anchors.top: parent.top
        width: 100
        model: inputs
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

    ListView {
        id: outputList
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.left: inputList.right
        height: 20 * outputs.rowCount()
        model: outputs
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

    ListView {
        id: optionList
        anchors.right: parent.right
        // anchors.bottom: parent.bottom
        anchors.top: outputList.bottom
        anchors.left: parent.left
        height: 200
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
