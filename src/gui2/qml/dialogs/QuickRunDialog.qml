import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

Dialog {
    id: root

    required property GraphModel graphModel
    property string startNode: input.text

    x: dissolveWindow.width / 2
    y: dissolveWindow.height / 2

    height: implicitHeight
    width: implicitWidth

    standardButtons: Dialog.Ok | Dialog.Cancel

    contentItem: Item {
        anchors.fill: parent
        focus: true

        Keys.onReturnPressed: accept()
        Keys.onEnterPressed: accept()

        TextField {
            id: input
            anchors.fill: parent
            font.pixelSize: 14
            placeholderText: "Enter a node name to run graph from..."
        }    
    }

    onAccepted: {
        if (graphModel.isValidNode(startNode))
            graphModel.run(startNode)

        root.close()
    }
    onRejected: root.close()
}