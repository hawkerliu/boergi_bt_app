import QtQuick 2.9
import QtQuick.Dialogs 1.2

//import btmanager 1.0



Dialog {
    id: scanDialog
    visible: true
    height: 720
    width: 720
    title: "Device discovery"
    standardButtons: Dialog.Close | Dialog.Apply | Dialog.Ok

    onApply: { // (test) start device scan here
        console.log("Apply clicked")
        cppInterface.scanDevices()

    }

    onAccepted:
    {
        devices.update()
    }

    ListView {
        id: devices
//        anchors.left: parent.left
//        anchors.right: parent.right
//        anchors.bottom: parent.bottom
//        anchors.top: title.bottom
        width: 500
        height: 500
        model: cppInterface.devices
        clip: true
        spacing: 16


        delegate: Rectangle {
            id: box
            height: 80
            width: devices.width *0.5
            color: index % 2 === 0 ? "gray" : "steelblue"
            radius: height / 16

            MouseArea {
            anchors.fill: parent
                onClicked: {
                    // -> here pass the address of the device as string to your connect!
                    //deviceFinder.connectToService(modelData.deviceAddress);
                    //app.showPage("Measure.qml")
                    console.log("index: " + index + "NAME: " + modelData.deviceName )
                }
            }

            Text {
                id: device
                font.pixelSize: 12
                text: modelData.deviceName
                anchors.top: parent.top
                anchors.topMargin: parent.height * 0.1
                anchors.leftMargin: parent.height * 0.1
                anchors.left: parent.left
                //color: "black"
            }

            Text {
                id: deviceAddress
                font.pixelSize: 12
                text: modelData.deviceAddress
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.rightMargin: parent.height * 0.1
                anchors.right: parent.right
                color: Qt.darker("black")
            }
        }
    }
}

