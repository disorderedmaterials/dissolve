import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

TextInput {
    id: root
    required property GraphModel graphModel
    required property string parentNodeName
    required property Text parentLabel
    required property int paramType
    property color renameStatusColor: "transparent"
    selectByMouse: true
    font: parentLabel.font
    color: parentLabel.color
    text: parentLabel.text
    horizontalAlignment: parentLabel.horizontalAlignment
    verticalAlignment: parentLabel.verticalAlignment
    onAccepted: finaliseEditing()

    signal renameAccepted()

    function rename(parentNodeName, currentParameterName, newParameterName) {
        if (root.paramType == 1)
            return root.graphModel.renameInput(parentNodeName, currentParameterName, newParameterName);
        if (root.paramType == 0)
            return root.graphModel.renameOutput(parentNodeName, currentParameterName, newParameterName);
    }

    function finaliseEditing() {
        const currentParameterName = parentLabel.text
        const newParameterName = root.text
        if (!rename(parentNodeName, currentParameterName, newParameterName)) {
            rejectAnimation.start();
            return;
        }
        renameAccepted();
    }

    Rectangle {
        anchors.fill: parent
        color: "white"
        border.width: 2
        border.color: parent.renameStatusColor
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
            target: root
            property: "renameStatusColor"
            to: "red"
            duration: 150
        }
        PropertyAnimation {
            target: root
            property: "renameStatusColor"
            to: "transparent"
            duration: 500
        }
    }
}