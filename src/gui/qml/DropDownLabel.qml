import QtQuick 6.0
import QtQuick.Layouts 6.0

RowLayout {
    property bool active;
    property string label;
    signal activated();
    Text {
	text: "▶"
	rotation: active ? 90 : 0;
    }
    Text {
	text: label;
	TapHandler {
	    onTapped: activated();
	}
    }
}
