import QtQuick
import QtQuick.Layouts
import "." as D

ListView {
    id: control
    property string iconPrefix

    signal selected

    delegate: RowLayout {
        property variant fullData: model

        Image {
            cache: false
            Layout.preferredHeight: 16
            source: control.iconPrefix + index
            Layout.preferredWidth: 16
            sourceSize.width: 16
            sourceSize.height: 16
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
