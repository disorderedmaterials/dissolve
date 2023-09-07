import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../widgets" as D

ListView {
    id: control
    signal selected

    delegate: RowLayout {
        property variant fullData: model
        Image {
            Layout.preferredHeight: 15
            Layout.preferredWidth: 15
            source: "qrc:/general/icons/species.svg"
        }
        D.Text {
            text: display
            color: ListView.isCurrentItem ? palette.active.highlightedText : palette.active.text
            padding: 2
        }
        MouseArea {
            anchors.fill: parent

            onClicked: {
                control.currentIndex = index;
                control.focus = true;
            }
            onDoubleClicked: {
                control.currentIndex = index;
                selected();
            }
        }
    }

    /*delegate: D.Text {
        text: display
        color: ListView.isCurrentItem ? palette.active.highlightedText : palette.active.text
        padding: 2
        MouseArea {
            anchors.fill: parent

            onClicked: {
                control.currentIndex = index;
                control.focus = true;
            }
            onDoubleClicked: {
                control.currentIndex = index;
                selected();
            }
        }
    }*/

    highlight: Rectangle {
        color: palette.active.highlight
    }
}
