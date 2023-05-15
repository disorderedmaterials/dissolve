import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ListView {
    id: control;

    signal selected();

    delegate:
    Rectangle {

	id: disp;
	property bool picked: ListView.isCurrentItem;
	color: picked ? palette.active.highlight : palette.active.base;
	height: label.height;
	width: label.width + label.height;

	Image {
	    id: image;
	    source: icon ? "qrc:/general/icons/general_warn.svg" : "qrc:/general/icons/general_true.svg";
	    anchors.top: label.top;
	    anchors.bottom: label.bottom;
	    anchors.left: parent.left;
	    width: height;
	}
	Text {
	    id: label;
	    property variant fullData: model;
	    text: display;
	    color: picked ? palette.active.highlightedText : palette.active.text;
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
}
