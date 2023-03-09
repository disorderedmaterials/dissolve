import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ListView {
    id: control;

    signal selected();

    delegate:
    Item {
	height: label.height;

	Image {
	    id: image;
	    source: "qrc" + icon;
	    anchors.top: label.top;
	    anchors.bottom: label.bottom;
	    anchors.left: parent.left;
	    width: height;
	}
	Text {
	    id: label;
	    property variant fullData: model;
	    text: display;
	    color: ListView.isCurrentItem ? palette.active.highlightedText : palette.active.text;
	    anchors.left: image.right;
	    anchors.top: parent.top;
	    MouseArea {
		height: parent.height;
		width: control.width;
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
    highlight:
    Rectangle {
	color: palette.active.highlight;
    }
}
