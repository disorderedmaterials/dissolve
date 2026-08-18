import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

Rectangle {
    id: root

    readonly property bool isCurrentSelection: parentListView.currentIndex === index
    property var parentListView
    readonly property var rootDialog: parentListView.parentDialog

    width: parentListView.width
    height: 50

    readonly property color highlightColor: Qt.rgba(
             palette.highlight.r,
             palette.highlight.g,
             palette.highlight.b,
             0.2
         )

    color: (isCurrentSelection || nodeDisplayMouseArea.containsMouse) ? highlightColor : "transparent"
    border.width: isCurrentSelection ? 2 : 0
    border.color: palette.highlight

    property bool hovered: false
    MouseArea {
        id: nodeDisplayMouseArea

        anchors.fill: parent
        hoverEnabled: true

        cursorShape: Qt.PointingHandCursor

        onDoubleClicked: {
            rootDialog.finaliseSelection(name, true);
            nodeRegistry.instantiateNode(root.rootDialog.initialLandingArea.x, root.rootDialog.initialLandingArea.y, root.rootDialog.selection);
            rootDialog.close();
        }

        onClicked: {
            parentListView.currentIndex = index;
            rootDialog.finaliseSelection(name, false);
        }
        
    }
    ColumnLayout {
        spacing: 2
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: 10
        }
        Text {
            id: nodeNameField
            text: name
            font.pixelSize: 16
            font.bold: true
        }
        Text {
            id: nodeDescriptionField
            text: description
            font.pixelSize: 11
            color: (root.isCurrentSelection || nodeDisplayMouseArea.containsMouse)
                ? palette.text
                : "grey"
        }
    }   
}