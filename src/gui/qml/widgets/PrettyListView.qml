import QtQuick
import QtQuick.Layouts
import "." as D

ListView {
    id: control
    signal selected

    delegate: RowLayout {
        property variant fullData: model

        Image {
            Layout.preferredHeight: 15
            Layout.preferredWidth: 15
            source: "image://dissolveModelIcons/moduleLayer/" + index
        }

        D.Text {

            color: ListView.isCurrentItem ? palette.active.highlightedText : palette.active.text
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
    highlight: Rectangle {
        color: palette.active.highlight
    }
}
