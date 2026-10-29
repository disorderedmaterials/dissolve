import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

TextInput {
    id: root

    required property GraphModel graphModel
    required property int paramType
    required property Text parentLabel
    required property string parentNodeName
    property color renameStatusColor: "transparent"

    signal renameAccepted

    function finaliseEditing() {
        const currentParameterName = parentLabel.text;
        const newParameterName = root.text;
        if (!rename(parentNodeName, currentParameterName, newParameterName)) {
            rejectAnimation.start();
            return;
        }
        renameAccepted();
    }
    function rename(parentNodeName, currentParameterName, newParameterName) {
        if (root.paramType == 1)
            return root.graphModel.renameInput(parentNodeName, currentParameterName, newParameterName);
        if (root.paramType == 0)
            return root.graphModel.renameOutput(parentNodeName, currentParameterName, newParameterName);
    }

    color: parentLabel.color
    font: parentLabel.font
    horizontalAlignment: parentLabel.horizontalAlignment
    selectByMouse: true
    text: parentLabel.text
    verticalAlignment: parentLabel.verticalAlignment

    onAccepted: finaliseEditing()

    Rectangle {
        anchors.fill: parent
        border.color: parent.renameStatusColor
        border.width: 2
        color: "white"
        z: -1
    }
    /*
    onActiveFocusChanged: {
        if (!activeFocus)
            finaliseEditing();
    }
    */
    SequentialAnimation {
        id: rejectAnimation

        PropertyAnimation {
            duration: 150
            property: "renameStatusColor"
            target: root
            to: "red"
        }
        PropertyAnimation {
            duration: 500
            property: "renameStatusColor"
            target: root
            to: "transparent"
        }
    }
}
