import QtQuick 2
import DataManagerSimulationModel 1.0
import DataManagerReferencePointModel 1.0

Rectangle {
    required property DataManagerSimulationModel sim;
    required property DataManagerReferencePointModel ref;
    anchors.fill: parent

    DataManagerDataBox {
	id: databox
	anchors.top: parent.top
	anchors.bottom: parent.bottom
	anchors.left: parent.left
	anchors.right: refbox.left
	model: sim;
    }

    DataManagerReferenceBox {
	id: refbox
	width: parent.width / 2
	anchors.top: parent.top
	anchors.bottom: parent.bottom
	anchors.right: parent.right
    }

}
