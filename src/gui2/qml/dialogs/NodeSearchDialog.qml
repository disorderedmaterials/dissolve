import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

Dialog {
    id: root

    height: 400
    width: 500

    modal: true
    closePolicy: Popup.NoAutoClose
    title: "Add Node"

    standardButtons: Dialog.Ok | Dialog.Cancel

    required property var initialLandingArea
    property var selection: ""
    function finaliseSelection(nodeName: string, reset: bool) {
        root.selection = nodeName;
        if (reset == true)
            root.resetSearch();
    }
    function resetSearch() {
        nodeSearchField.text = "";
    }
    function exitSearchDialog() {
        root.resetSearch();
        root.close();
    }
    onAccepted: {
        if (nodeSearchField.text.length > 0)
            nodeRegistry.instantiateNode(initialLandingArea.x, initialLandingArea.y, root.selection);

        root.exitSearchDialog();
    }
    onRejected: root.exitSearchDialog();

    NodeRegistrySortFilterModel {
        id: searchFilterModel
        sourceModel: nodeRegistry
        searchText: nodeSearchField.text
    }
    contentItem: ColumnLayout {
        id: columnLayout
    
        Rectangle {
            id: nodeSearchFieldArea

            Layout.fillWidth: true
            Layout.preferredHeight: nodeSearchField.implicitHeight

            border.color: nodeSearchField.activeFocus && nodeSearchField.text.length > 0
                            ? palette.highlight
                            : palette.mid

            radius: 5
            border.width: 1
            clip: true

            TextField {
                id: nodeSearchField
                anchors.fill: parent
                height: contentHeight
                placeholderText: qsTr("Node name...")
                placeholderTextColor: "grey"

                background: Item {}
            }
        }        

        ScrollView {
            id: scrollArea

            Layout.fillWidth: true
            Layout.fillHeight: true

            background: Rectangle {
                color: palette.base
                border.color: palette.mid
                radius: 4
            }

            ListView {
                id: searchResultsListView

                readonly property var parentDialog: root

                anchors.fill: parent
                opacity: 10

                model: searchFilterModel

                delegate: NodeDisplayDelegate {
                    parentListView: searchResultsListView

                    Component.onCompleted: {
                        if (!name || name.length === 0)
                            console.error("Node has no name - this should not happen, please contact the Dissolve developer team.")

                        if (!description || description.length === 0)
                            console.error("Node '%1' has no description - this should not happen, please contact the Dissolve developer team.".arg(name))
                    }
                }
            }   
        }
    }
}