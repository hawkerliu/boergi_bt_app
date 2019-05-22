import QtQuick 2.9
import QtQuick.Dialogs 1.2

//import btmanager 1.0

Dialog {
    id: scanDialog
    visible: true
    title: "Device discovery"
    standardButtons: Dialog.Close | Dialog.Apply

    onApply: { // (test) start device scan here
        console.log("Apply clicked")
        //title: BtManager.sayHello()

        //BtManager.sayHelloSlot()

    }

    height: deviceCol.height


    ListView {
        id: deviceList
        Column {
            id:deviceCol
            Text {
                text: qsTr("device1")
                // text: BtManager.sayHello()
            }
            Text {
                text: qsTr("device2")
            }
            Text {
                text: qsTr("device3")
            }
            Text {
                text: qsTr("device4")
            }
            Text {
                text: qsTr("device5")
            }
        }
    }


}
