import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import QtCharts 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

//import btmanager 1.0
//import "."

//#define STATE_INIT          0
//#define STATE_UPDATED       1
//#define STATE_REQUESTED     2
//#define STATE_ACK           3
//#define STATE_NACK          4



ApplicationWindow {




    id: root
    visible: true
    width: 1920
    height: 1080
    title: qsTr("Hello World")

    property int state_UNCONNECTED: 0
    property int state_INIT:        1
    property int state_UPDATED:     2
    property int state_REQUESTED:   3
    property int state_FAILED:      4

    onClosing: cppInterface.exitApplication()

    property int fs: 0

   color: "darkgray"

//   Rectangle {
//       id: bg
//       anchors.fill: parent
//       color: "red"
//   }





    // property real offset: 0.1 // y offset


    /* CONNECTION TO CPP */
    Connections {
        id: connectBlock
        target: cppInterface

        //        onMinOfTempChanged: { console.log("MinT",cppInterface.minOfTemp)
        //        }
        //        onMaxOfTempChanged: { console.log("MaxT",cppInterface.maxOfTemp)
        //        }
        //        onMaxOfPresChanged: { console.log("MaxP",cppInterface.maxOfPres)
        //        }
        //        onMinOfPresChanged: { console.log("MinP",cppInterface.minOfPres)
        //        }
        //        onMaxOfAltChanged: { console.log("MaxA",cppInterface.maxOfAlt)
        //        }
        //        onMinOfAltChanged: { console.log("MinA",cppInterface.minOfAlt)
        //        }

        onSomefkingsignal:
        {
            console.log("gecispina")

        }

        onModeStateChanged: {

            if (cppInterface.modeState == state_UPDATED)
            {
                // Modes got updated, give values to sliders
                //console.log("Updating slider values")
                configDialog.prs_mr = cppInterface.prs_mr
                configDialog.prs_osr = cppInterface.prs_osr
                configDialog.temp_mr = cppInterface.temp_mr
                configDialog.temp_osr = cppInterface.temp_osr

                // clear chartview and reset booleans
                viewP.seriesData.clear()
                viewT.seriesData.clear()
                viewA.seriesData.clear()
                viewA.seriesData.clear()


            }
        }

        onDevicesChanged: { console.log("DEVICES CHANGED QML") }


        onSomeBufferHasFreshDataChanged:
        {

            if (cppInterface.someBufferHasFreshData === TRUE)
            {
                //there is stuff -> push them each as signals..
                cppInterface.getAllFromSomeBuffer()

                //give me only the last one
                var tmpString = cppInterface.getLastFromSomeBuffer();

                //put one on log
                //console.log(cppInterface.getLastFromSomeBuffer()); // ***********
            }
        }
        onIsActiveChanged:
        {
            console.log("ActiveChanged")
        }
        // only as test)
        onUpdatePressure:
        {
            if (viewP.seriesData.count == 0) { viewP.minx = pp.x }
            viewP.maxx = pp.x
            viewP.seriesData.append(pp.x,pp.y)
        }
        onUpdateTemperature: {
            if (viewT.seriesData.count == 0) { viewT.minx = pt.x }
            viewT.maxx = pt.x
            viewT.seriesData.append(pt.x,pt.y)
        }
        onUpdateAltitude: {
            if (viewA.seriesData.count == 0) { viewA.minx = pa.x }
            viewA.maxx = pa.x
            viewA.seriesData.append(pa.x,pa.y)
        }
        onUpdateVolume: {
            if (viewV.seriesData.count == 0) { viewV.minx = pv.x }
            viewV.maxx = pv.x
            viewV.seriesData.append(pv.x,pv.y)
        }
        onUpdateFrequency: {
            fs = Math.round(f)
            freq.text = fs.toString()
        }
    }

    Item {
        Timer {
            interval: 500; running: true; repeat: true
            onTriggered: {
                // you can call any function from the interface like:
                //console.log(cppInterface.getLastFromSomeBuffer()); // *******************
                cppInterface.messageFromQml("AA DIESE AUCH");
            }
        }
    }



    DeviceScan {
        id: deviceDialog
        visible: false
    }

    CustomConfig {
        id: configDialog
        visible: false

    }

    Dialog {
        id: dataPointInput
        visible: false
        title: "I am ugly"
        //anchors.centerIn: parent
        font.pixelSize: 20

        height: (dialtxt.height+txtin.height)*4
        width: dialtxt.width*1.05
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            if (txtin.acceptableInput)
            {
                var num = txtin.text.valueOf();
                viewP.maxData = num
                viewT.maxData = num
                viewA.maxData = num
                viewV.maxData = num
                viewP.seriesData.clear()
                viewT.seriesData.clear()
                viewA.seriesData.clear()
                viewA.seriesData.clear()
                cppInterface.resetBorders()
            }
        }
        TextEdit {
            id: dialtxt
            readOnly: true


            //anchors.verticalCenter: parent.top
            //anchors.horizontalCenter: dataPointInput.Center

            font.pixelSize: dataPointInput.font.pixelSize
            text: qsTr("Enter number of points, which should be visible at the same time.")
        }
        TextField {
            id: txtin
            placeholderText: "default: 500"
            font.pixelSize: dataPointInput.font.pixelSize
            anchors.top: dialtxt.bottom
            anchors.verticalCenter: dataPointInput.bottom
            inputMethodHints: Qt.ImhFormattedNumbersOnly

            validator: IntValidator {id: validator; bottom: 10; top: 5000;}
            focus: true

            //anchors.centerIn: dataPointInput

        }
    }




//    menuBar: MenuBar {
//            id: mainBar
//        Menu {
//            title: qsTr("&File")
//            Action { text: qsTr("&New...") }
//            Action { text: qsTr("&Open...") }
//            Action { text: qsTr("&Save") }
//            Action { text: qsTr("Save &As...") }
//            MenuSeparator { }
//            Action { text: qsTr("&Quit")  }

//        }
//        Menu {
//            title: qsTr("&Connection Manager")
//            Action { text: qsTr("&Connect to device")
//                onTriggered: deviceDialog.visible = true
//            }

//        }
//        Menu {
//            //id: confMenu
//            title: qsTr("&Configuration")
//            ActionGroup {
//                id: confActions
//            }

//            Action {
//                id: hdr
//                text: qsTr("High Data &Rate")
//                ActionGroup.group: confActions
//                checkable: true
//                checked: false
//                onTriggered: {
//                    if (!hdr.checked)
//                    {
//                        // ignore, because mode is already set
//                        checked = true
//                    }
//                    else {
//                        if (cppInterface.modeState == state_UPDATED)
//                        {
//                            console.log("High data rate triggered")
//                            // "$mode bg\n" -> never changing

//                            // this updates should be done automatically after state_UPDATED
//                            //                            configDialog.prs_mr = 32
//                            //                            configDialog.prs_osr = 1
//                            //                            configDialog.temp_mr = 32
//                            //                            configDialog.temp_osr = 1
//                            // this is not neccassary any more, because prev values get updated in cpp state machine
//                            //                            cppInterface.prev_prs_mr = cppInterface.prs_mr
//                            //                            cppInterface.prev_prs_osr = cppInterface.prs_osr
//                            //                            cppInterface.prev_temp_mr = cppInterface.temp_mr
//                            //                            cppInterface.prev_temp_osr = cppInterface.temp_osr

//                            if (cppInterface.prs_mr != 32)
//                                cppInterface.prs_mr = 32
//                            if (cppInterface.prs_osr != 1)
//                                cppInterface.prs_osr = 1
//                            if (cppInterface.temp_mr != 32)
//                                cppInterface.temp_mr = 32
//                            if (cppInterface.temp_osr != 1)
//                                cppInterface.temp_osr = 1
//                            cppInterface.modeState = state_REQUESTED

//                        }
//                        else checked = false
//                    }
//                }
//            }
//            Action {
//                id: std
//                text: qsTr("&Standard Precision")
//                ActionGroup.group: confActions
//                checkable: true
//                checked: false
//                onTriggered: {
//                    if (!std.checked)
//                    {
//                        // ignore, because mode is already set
//                        checked = true
//                    }
//                    else {
//                        console.log("Standard precision triggered")
//                        if (cppInterface.prs_mr != 8)
//                            cppInterface.prs_mr = 8
//                        if (cppInterface.prs_osr != 32)
//                            cppInterface.prs_osr = 32
//                        if (cppInterface.temp_mr != 8)
//                            cppInterface.temp_mr = 8
//                        if (cppInterface.temp_osr != 32)
//                            cppInterface.temp_osr = 32
//                        cppInterface.modeState = state_REQUESTED
//                    }
//                }
//            }
//            Action {
//                id: hpr
//                text: qsTr("High &Precision")
//                ActionGroup.group: confActions
//                checkable: true
//                checked: false
//                onTriggered: {
//                    if (!hpr.checked)
//                    {
//                        // ignore, because mode is already set
//                        checked = true
//                    }
//                    else {
//                        console.log("High precision triggered")
//                        if (cppInterface.prs_mr != 1)
//                            cppInterface.prs_mr = 1
//                        if (cppInterface.prs_osr != 128)
//                            cppInterface.prs_osr = 128
//                        if (cppInterface.temp_mr != 1)
//                            cppInterface.temp_mr = 1
//                        if (cppInterface.temp_osr != 128)
//                            cppInterface.temp_osr = 128
//                        cppInterface.modeState = state_REQUESTED
//                    }
//                }
//            }


//            MenuSeparator { }
//            Action { text: qsTr("&Custom...")
//                onTriggered: configDialog.visible = true
//            }
//        }
//        Menu {
//            title: qsTr("&View")

//            Action { text: qsTr("&Reset")
//                onTriggered: {
//                    viewP.seriesData.clear()
//                    viewT.seriesData.clear()
//                    viewA.seriesData.clear()
//                    viewA.seriesData.clear()
//                    cppInterface.resetBorders()
//                }
//            }
//            MenuSeparator {}
//            Action { text: qsTr("&Set data points")
//                onTriggered: {
//                    dataPointInput.visible = true
//                }
//            }
//        }
//        Menu {
//            title: qsTr("&Layout")

//            Action { text: qsTr("&Dark Theme")
//                onTriggered: {
//                    viewP.dataChart.theme = ChartView.ChartThemeDark
//                    viewT.dataChart.theme = ChartView.ChartThemeDark
//                    viewA.dataChart.theme = ChartView.ChartThemeDark
//                    viewV.dataChart.theme = ChartView.ChartThemeDark
//                }
//            }
//            Action { text: qsTr("&Light Theme")
//                onTriggered: {
//                    viewP.dataChart.theme = ChartView.ChartThemeLight
//                    viewT.dataChart.theme = ChartView.ChartThemeLight
//                    viewA.dataChart.theme = ChartView.ChartThemeLight
//                    viewV.dataChart.theme = ChartView.ChartThemeLight
//                }
//            }

//        }
//        //        Menu {
//        //            title: qsTr("&Help")
//        //            Action { text: qsTr("&About") }
//        //        }
//    }





    SwipeView {


        id:swipeview
        anchors.fill: parent
        //anchors.top: mainBar.bottom
        anchors.bottom: tabbar.top
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: tabbar.currentIndex

        DataView {
            id: viewP
            maxy: cppInterface.maxOfPres
            miny: cppInterface.minOfPres
        }

        DataView {
            id: viewT
            maxy: cppInterface.maxOfTemp
            miny: cppInterface.minOfTemp
        }

        DataView {
            id: viewA
            maxy: cppInterface.maxOfAlt
            miny: cppInterface.minOfAlt
        }

        DataView {
            id: viewV
            maxy: cppInterface.maxOfVol
            miny: cppInterface.minOfVol
        }


        Page {
            width: 640
            height: 480
            TextField {
                id: freq
                text: "Frequency"
                placeholderText: qsTr("Sensor data")
                font.pointSize: 24
                anchors.top: parent.top
            }
        }

    }

    footer: TabBar {

        id: tabbar
        currentIndex: swipeview.currentIndex

        TabButton {
            text: qsTr("Pressure")
            font.pointSize: 24
        }
        TabButton {
            text: qsTr("Temperature")
            font.pointSize: 24
        }
        TabButton {
            text: qsTr("Altitude")
            font.pointSize: 24
        }
        TabButton {
            text: qsTr("Volume")
            font.pointSize: 24
        }
        TabButton {
            text: qsTr("Frequency")
            font.pointSize: 24
        }
        //        onCurrentIndexChanged: {
        //            swipeview.currentIndex = tabbar.currentIndex
        //        }
    }


}
