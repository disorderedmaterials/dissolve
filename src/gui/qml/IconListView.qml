import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ListView {
    id: control;

    signal selected();

    /* Border */
    Rectangle {
	anchors.fill: parent;
	z: 1;
	color: "#00000000";
	border.color: palette.active.dark;
	border.width: 2;
    }
    /* Background */
    Rectangle {
	anchors.fill: parent;
	z: -11;
	color: palette.active.base;
    }

    delegate:
    Rectangle {

	id: disp;
	property bool picked: ListView.isCurrentItem;
	color: picked ? palette.active.highlight : palette.active.base;
	height: label.height;
	width: control.width;

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
	    padding: 2;
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
