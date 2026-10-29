import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

Dialog {
    id: root

    required property GraphModel graphModel
    property string startNode: input.text

    height: implicitHeight
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: implicitWidth
    x: dissolveWindow.width / 2
    y: dissolveWindow.height / 2

    contentItem: Item {
        anchors.fill: parent
        focus: true

        Keys.onEnterPressed: accept()
        Keys.onReturnPressed: accept()

        TextField {
            id: input

            anchors.fill: parent
            font.pixelSize: 14
            placeholderText: "Enter a node name to run graph from..."
        }
    }

    onAccepted: {
        if (graphModel.isValidNode(startNode))
            graphModel.run(startNode);
        root.close();
    }
    onRejected: root.close()
}
