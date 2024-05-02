import QtQuick
import QtQuick.Controls
import "widgets" as D

Item {
    id: control
    property variant dialogModel

    signal selected

    Component.onCompleted: ffList.forceActiveFocus()

    Binding {
        property: "ff"
        target: control.dialogModel
        value: ffList.currentItem == null ? null : ffList.currentItem.fullData.raw
    }
    D.GroupBox {
        anchors.top: parent.top
        height: parent.height / 2
        title: "Select Forcefield"
        width: parent.width

        TextField {
            id: ffFilter
            anchors.right: parent.right
            anchors.top: parent.top
            placeholderText: qsTr("Filter")

            onTextEdited: control.dialogModel.forcefields.setFilterFixedString(text)
        }
        Image {
            anchors.bottom: ffFilter.bottom
            anchors.right: ffFilter.left
            anchors.top: ffFilter.top
            fillMode: Image.PreserveAspectFit
            source: "qrc:/general/icons/filter.svg"
        }
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.top: ffFilter.bottom
            color: palette.active.base
            width: parent.width

            D.PrettyListView {
                id: ffList
                anchors.fill: parent
                clip: true
                currentIndex: -1
                focus: true
                model: control.dialogModel.forcefields

                onSelected: control.selected()
            }
        }
    }
    D.GroupBox {
        anchors.bottom: parent.bottom
        height: parent.height / 2 - 5
        title: "Forcefield Description"
        width: parent.width

        Rectangle {
            anchors.fill: parent
            color: palette.active.base

            TextArea {
                anchors.fill: parent
                readOnly: true
                text: ffList.currentItem == null ? "" : ffList.currentItem.fullData.description
                wrapMode: TextEdit.Wrap
            }
        }
    }
}
