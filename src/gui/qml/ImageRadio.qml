import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

RadioButton {
    id: control
    property url source;
    contentItem: Item {
	Rectangle {
	    id: padding;
	    anchors.left: parent.left;
	    width: 2 * control.spacing;
	}
	Image {
	    id: icon
	    source: control.source;
	    anchors.left: padding.right;
	    fillMode: Image.PreserveAspectFit;
	    height: instructions.height;
	    opacity: control.enabled ? 1.0 : 0.3;
	}
	Text {
	    anchors.left: icon.right;
	    id: instructions;
	    text: control.text;
	    opacity: control.enabled ? 1.0 : 0.3;
	    verticalAlignment: Text.AlignVCenter;
	}
    }
}
