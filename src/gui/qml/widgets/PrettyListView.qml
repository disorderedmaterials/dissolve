import QtQuick
import QtQuick.Layouts
import "." as D

ListView {
    id: control
    signal selected
    property string iconPrefix

    delegate: RowLayout {
        property variant fullData: model

        Image {
            height: 16
            width: 16
            cache: false
            source: control.iconPrefix + index
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
