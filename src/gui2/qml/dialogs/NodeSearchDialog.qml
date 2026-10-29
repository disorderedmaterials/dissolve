import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

Dialog {
    id: root

    required property var initialLandingArea
    property var selection: ""

    function exitSearchDialog() {
        root.resetSearch();
        root.close();
    }
    function finaliseSelection(nodeName: string, reset: bool) {
        root.selection = nodeName;
        if (reset == true)
            root.resetSearch();
    }
    function resetSearch() {
        nodeSearchField.text = "";
    }

    closePolicy: Popup.NoAutoClose
    height: 400
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    title: "Add Node"
    width: 500

    contentItem: ColumnLayout {
        id: columnLayout

        Rectangle {
            id: nodeSearchFieldArea

            Layout.fillWidth: true
            Layout.preferredHeight: nodeSearchField.implicitHeight
            border.color: nodeSearchField.activeFocus && nodeSearchField.text.length > 0 ? palette.highlight : palette.mid
            border.width: 1
            clip: true
            radius: 5

            TextField {
                id: nodeSearchField

                anchors.fill: parent
                height: contentHeight
                placeholderText: qsTr("Node name...")
                placeholderTextColor: "grey"

                background: Item {
                }
            }
        }
        ScrollView {
            id: scrollArea

            Layout.fillHeight: true
            Layout.fillWidth: true

            background: Rectangle {
                border.color: palette.mid
                color: palette.base
                radius: 4
            }

            ListView {
                id: searchResultsListView

                readonly property var parentDialog: root

                anchors.fill: parent
                model: searchFilterModel
                opacity: 10

                delegate: NodeDisplayDelegate {
                    parentListView: searchResultsListView

                    Component.onCompleted: {
                        if (!name || name.length === 0)
                            console.error("Node has no name - this should not happen, please contact the Dissolve developer team.");
                        if (!description || description.length === 0)
                            console.error("Node '%1' has no description - this should not happen, please contact the Dissolve developer team.".arg(name));
                    }
                }
            }
        }
    }

    onAccepted: {
        if (nodeSearchField.text.length > 0)
            nodeRegistry.instantiateNode(initialLandingArea.x, initialLandingArea.y, root.selection);
        root.exitSearchDialog();
    }
    onRejected: root.exitSearchDialog()

    NodeRegistrySortFilterModel {
        id: searchFilterModel

        searchText: nodeSearchField.text
        sourceModel: nodeRegistry
    }
}
