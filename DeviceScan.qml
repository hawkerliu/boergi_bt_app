import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtGraphicalEffects 1.13
import QtQuick.Layouts 1.13






ApplicationWindow {

    id: scanDialog
    visible: true
    height: root.height/1.5//720
    width: root.width/4//500
    title: "Device discovery"
    modality: Qt.ApplicationModal//Qt.ApplicationModal // Qt.WindowModal
    flags: Qt.Dialog




    Material.theme: globalTheme
    Material.accent: globalAccent

    property int fontSize: 20
    property int borderMargin: 5
    property int timeoutCounter: 0

    onVisibleChanged: {
        if (visible == true)
        {
            cppInterface.scanDevices()
        }
    }

    Rectangle {
        id: scanButton
        anchors.horizontalCenter: parent.horizontalCenter
        //anchors.top: scanDialog.title.bottom
        anchors.top: parent.top
        anchors.topMargin: borderMargin

        height: parent.height*0.1
        width: parent.width*0.6
        enabled: !cppInterface.scanning
        radius: 30
        opacity: cppInterface.scanning === true ? 0.7 : 1
        color: Qt.darker("steelblue")

        MouseArea {
            anchors.fill: parent
            onClicked: {
                cppInterface.scanDevices()
            }
        }

        Text {
            anchors.centerIn: parent
            font.pixelSize: fontSize
            text: cppInterface.scanning === true ? "Scanning..." : "Start Device Scan"
            color: "white"
        }
    }

    Popup {
        id: popup
        visible: false
        anchors.centerIn: parent
        closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
        dim: false // true
        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
        }
        exit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 }
        }
        Text {
            id: popupText
            color: Material.theme == Material.Light ? "black" : "white"
            anchors.centerIn: parent
            //text: "Pairing error."
            font.pixelSize: fontSize
        }
    }

    ListView {
        id: devices
        anchors.top: scanButton.bottom
        anchors.topMargin: borderMargin*2
        anchors.horizontalCenter: scanButton.horizontalCenter
        width: parent.width*0.95//scanButton.width//parent.width
        height: parent.height*0.79
        model: cppInterface.devices
        clip: true
        spacing: anchors.topMargin/2

        property int selectedIndex: -1
        property string activeAddr: ""
        property string activeName: ""
        property int prevIdx: -1
        property Item prevBox
        property bool selected: false

        signal connect

        onConnect: {
            if (!selected) {
                console.log("Nothing selected.")
                //cppInterface.connectDeviceQML("")
            }
            else {
                console.log("Try to connect to",activeAddr)
                cppInterface.connectDeviceQML(activeAddr,activeName)
            }
        }

        delegate: Rectangle {
            id: box
            height: 80
            width: devices.width
            color: stdColor
            radius: scanButton.radius/2
            property color stdColor: "lightgray"
            property color selectedColor: "gray"
            ColorAnimation {
                from: "white"
                to: "black"
                duration: 200
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    cppInterface.stopScanDevices()
                    devices.activeAddr = modelData.deviceAddress
                    devices.activeName = modelData.deviceName
                    box.color = box.color == selectedColor ? stdColor : selectedColor
                    devices.currentIndex = index
                    if (devices.prevBox && devices.prevBox != box)
                    {
                        devices.prevBox.color = stdColor
                    }
                    devices.selected = box.color == selectedColor ? true : false
                    devices.prevBox = box
                }
            }

            Text {
                id: device
                font.pixelSize: fontSize
                text: modelData.deviceName
                anchors.top: parent.top
                anchors.topMargin: parent.height * 0.1
                anchors.leftMargin: parent.height * 0.1
                anchors.left: parent.left
            }

            Text {
                id: deviceAddress
                font.pixelSize: fontSize
                text: modelData.deviceAddress
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.rightMargin: parent.height * 0.1
                anchors.right: parent.right
                color: Qt.darker("black")
            }
        }
    }

    Rectangle {
        id: connectButton
        anchors.top: devices.bottom
        anchors.topMargin: devices.topMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: borderMargin
        anchors.left: parent.left
        anchors.leftMargin: borderMargin
        //anchors.right: disconnectButton.left
        //anchors.rightMargin: 5

        height: parent.height*0.1
        width: parent.width/2

        radius: 30
        opacity: connectButton.enabled == false ? 0.7 : 1
        color: scanButton.color
        enabled: devices.selected && (cppInterface.socketState === 0)// && devices.activeAddr

        Connections {
            target: cppInterface
            onErrorToQml:
            {
                switch (error) {
                case 0: console.log("No error")
                    break;
                case 1: console.log("Pairing error")
                    popupText.text = "Pairing error."
                    popup.open()
                    connectText.text = "Connect"
                    connectButton.enabled = true
                    break;
                case 2: console.log("Timeout")
                    timeoutCounter++
                    popupText.text = "Timeout"+"("+timeoutCounter+")"+" while initialization. Retrying..."
                    popup.open()
                    break;
                case 100: console.log("Unknown device error")
                    //                    popupText.text = "Unknown error."
                    //                    popup.open()
                    // connectButton.enabled = true
                    break;
                }
            }

            onInitComplete: {
                popupText.text = "Initialization complete."
                popup.close()
                scanDialog.visible = false
            }

            onSocketStateChanged:
            {
                switch (cppInterface.socketState)
                {
                case 0: // UnconnectedState
                    console.log("UnconnectedState")
                    connectText.text = "Connect"
                    connectButton.enabled = true
                    root.bgVisible = true
                    dragGrid.graphsVisible = false
                    mainBar.cntState = "Disconnected"
                    break;
                case 2: // ConnectingState
                    console.log("ConnectingState")
                    connectText.text = "Connecting..."
                    mainBar.cntState = "Connecting..."
                    break;
                case 3: // ConnectedState
                    console.log("ConnectedState")
                    connectText.text = "Connected"
                    popupText.text = "Connected! Initializing..."
                    timeoutCounter = 0
                    mainBar.cntState = qsTr("Connected")
                    break;
                case 6: // ClosingState
                    console.log("ClosingState")
                    connectText.text = "Disconnecting..."
                    mainBar.cntState = "Disconnecting..."
                    break;
                }
            }
        }


        MouseArea {
            anchors.fill: parent
            onClicked: {
                devices.connect()
                connectButton.enabled = false
                connectText.text = "Connecting..."
                popupText.text = "Connecting to " + devices.activeName + " ..."
                popup.open()
            }
        }

        Text {
            id: connectText
            anchors.centerIn: parent
            font.pixelSize: fontSize
            text: "Connect"
            color: "white"
        }
    }

    Rectangle {
        id: disconnectButton
        anchors.top: devices.bottom
        anchors.topMargin: devices.topMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: borderMargin
        anchors.right: parent.right
        anchors.rightMargin: borderMargin
        anchors.left: connectButton.right
        height: parent.height*0.1
        width: parent.width/2
        radius: 30
        opacity: disconnectButton.enabled == false ? 0.7 : 1
        color: scanButton.color
        enabled: cppInterface.socketState == 2 || cppInterface.socketState == 3

        MouseArea {
            anchors.fill: parent
            onClicked: {
                cppInterface.disconnectDeviceQML();

                //                for (var i=0;i<grid.count;i++)
                //                {
                //                    //grid.remove(i)
                //                    //grid.itemAt(i).visible = false
                //                    //grid.children[i].visible = false
                //                }
            }
        }

        Text {
            id: disconnectText
            anchors.centerIn: parent
            font.pixelSize: fontSize
            text: "Disconnect"
            color: "white"
        }
    }
}

