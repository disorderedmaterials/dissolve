import QtQuick
import "widgets" as D

ListView {
    id: control
    signal selected

    /* Border */
    Rectangle {
        anchors.fill: parent
        border.color: palette.active.dark
        border.width: 2
        color: "#00000000"
        z: 1
    }
    /* Background */
    Rectangle {
        anchors.fill: parent
        color: palette.active.base
        z: -11
    }

    delegate: Rectangle {
        id: disp
        property bool picked: ListView.isCurrentItem

        color: disp.picked ? palette.active.highlight : palette.active.base
        height: label.height
        width: control.width

        Image {
            id: image
            anchors.bottom: label.bottom
            anchors.left: parent.left
            anchors.top: label.top
            source: icon ? "qrc:/general/icons/warn.svg" : "qrc:/general/icons/true.svg"
            width: height
        }
        D.Text {
            id: label
            property variant fullData: model

            anchors.left: image.right
            anchors.top: parent.top
            color: picked ? palette.active.highlightedText : palette.active.text
            padding: 2
            text: display

            MouseArea {
                height: parent.height
                width: control.width

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
    }
}
