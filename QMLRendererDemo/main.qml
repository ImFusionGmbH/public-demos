import QtQuick 2.4
import QtQuick.Layouts 1.0
import ImFusion 1.0

Rectangle {
	id: rootItem
	anchors.fill: parent;
	color: "#343434"

	Rectangle {
		id: loadBut
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.margins: 5
		height: load.contentHeight + 10
		width: load.contentWidth + 10
		color: 'gray'
		border.color: 'black'

		Text {
			id: load
			anchors.fill: parent
			anchors.margins: 5
			text: "Load Data"
			font.pointSize: 20

			MouseArea {
				id: loadMouseArea
				anchors.fill: parent
				onClicked:  {
					imfusionview.onLoadClicked()
				}
			}
		}
	}

	Rectangle {
		id: tfBut
		anchors.top: parent.top
		anchors.left: loadBut.right
		anchors.margins: 5
		height: tf.contentHeight + 10
		width: tf.contentWidth + 10
		color: 'gray'
		border.color: 'black'

		Text {
			id: tf
			anchors.fill: parent
			anchors.margins: 5
			font.pointSize: 20
			text: "Toggle Transfer Function"

			MouseArea {
				id: tfMouseArea
				anchors.fill: parent
				onClicked: imfusionview.onTFToggled()

			}
		}
	}

	Rectangle {
		id: imageBut
		anchors.top: parent.top
		anchors.left: tfBut.right
		anchors.margins: 5
		height: image.contentHeight + 10
		width: image.contentWidth + 10
		color: 'gray'
		border.color: 'black'

		Text {
			id: image
			anchors.fill: parent
			anchors.margins: 5
			font.pointSize: 20
			text: "Toggle MPR Views"

			MouseArea {
				id: viewMouseArea
				anchors.fill: parent
				onClicked: imfusionview.onViewsToggled()
			}
		}
	}

	Image {
		id: logo
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.margins: 5
		height: imageBut.height
		fillMode: Image.PreserveAspectFit
		source: "img/logo.png"
	}

	ImFusionFboView {
		id: imfusionview
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: loadBut.bottom
		anchors.margins: 5
		objectName: "TheImFusionView"
		visible: true
	}
}
