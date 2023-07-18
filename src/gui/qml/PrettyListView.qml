import QtQuick

ListView {
    id: control
    signal selected

    delegate: Text {
        property variant fullData: this.model

        color: ListView.isCurrentItem ? palette.active.highlightedText : palette.active.text
        padding: 2
        text: this.display

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
    highlight: Rectangle {
        color: palette.active.highlight
    }
}
