import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import QtCharts 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

//import btmanager 1.0
import "."


ApplicationWindow {
    id: root
    visible: true
    width: 1920
    height: 1080
    title: qsTr("Hello World")

    property real offset: 0.1 // y offset

    //    function addPoint(dataview, point) {
    //        dataview.diff = dataview.maxy - dataview.miny
    //        if (dataview.seriesData.count == 0)
    //        {
    //            dataview.miny = point.y - (dataview.diff*offset)
    //            dataview.maxy = point.y + (dataview.diff*offset)
    //            dataview.minx = point.x
    //        }
    //        dataviewdataview.maxx = point.x
    //        if (dataview.maxy < point.y) dataview.maxy = point.y + (dataview.diff*offset)
    //        if (dataview.miny > point.y) dataview.miny = point.y - (dataview.diff*offset)
    //        dataview.seriesData.append(point.x,point.y)
    //    }

    /* CONNECTION TO CPP */
    Connections {
        id: connectBlock
        target: cppInterface
        onSomeBufferHasFreshDataChanged:
        {
            if (cppInterface.someBufferHasFreshData === TRUE)
            {
                //there is stuff -> push them each as signals..
                cppInterface.getAllFromSomeBuffer()

                //give me only the last one
                var tmpString = cppInterface.getLastFromSomeBuffer();

                //put one on log
                console.log(cppInterface.getLastFromSomeBuffer());
            }
        }
        onIsActiveChanged:
        {
            console.log("ActiveChanged")
        }
        // only as test)
        onUpdatePressure:
        {
            //console.log("Pressure:", pp)
            // erste 1000 werte als x-1000 nehmen

            //            addPoint(viewP,pp)

            viewP.diff = viewP.maxy - viewP.miny
            if (viewP.seriesData.count == 0)
            {
                viewP.miny = pp.y - (viewP.diff*offset)
                viewP.maxy = pp.y + (viewP.diff*offset)
                viewP.minx = pp.x
            }
            viewP.maxx = pp.x
            if (viewP.maxy < pp.y) viewP.maxy = pp.y + (viewP.diff*offset)
            if (viewP.miny > pp.y) viewP.miny = pp.y - (viewP.diff*offset)
            viewP.seriesData.append(pp.x,pp.y)
        }
    }

    Item {
        Timer {
            interval: 500; running: true; repeat: true
            onTriggered: {
                // you can call any function from the interface like:
                console.log(cppInterface.getLastFromSomeBuffer());
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
        objectName: "CustomModeDialog"

    }



    //    BtManager {
    //        id: manager
    //        onUpdatePressure: {
    //            //console.log("Pressure:", pp)
    //            // erste 1000 werte als x-1000 nehmen

    //            //            addPoint(viewP,pp)

    //            viewP.diff = viewP.maxy - viewP.miny
    //            if (viewP.seriesData.count == 0)
    //            {
    //                viewP.miny = pp.y - (viewP.diff*offset)
    //                viewP.maxy = pp.y + (viewP.diff*offset)
    //                viewP.minx = pp.x
    //            }
    //            viewP.maxx = pp.x
    //            if (viewP.maxy < pp.y) viewP.maxy = pp.y + (viewP.diff*offset)
    //            if (viewP.miny > pp.y) viewP.miny = pp.y - (viewP.diff*offset)
    //            viewP.seriesData.append(pp.x,pp.y)
    //        }
    //        onUpdateTemperature: {
    //            //console.log("Temperature:", pt)
    //            viewT.diff = viewT.maxy - viewT.miny
    //            if (viewT.seriesData.count == 0)
    //            {
    //                viewT.miny = pt.y - (viewT.diff*offset)
    //                viewT.maxy = pt.y + (viewT.diff*offset)
    //                viewT.minx = pt.x
    //            }
    //            viewT.maxx = pt.x
    //            if (viewT.maxy < pt.y) viewT.maxy = pt.y + (viewT.diff*offset)
    //            if (viewT.miny > pt.y) viewT.miny = pt.y - (viewT.diff*offset)
    //            viewT.seriesData.append(pt.x,pt.y)
    //        }
    //        onUpdateAltitude: {
    //            //console.log("Altitude:", pa)
    //            viewA.diff = viewA.maxy - viewA.miny
    //            if (viewA.seriesData.count == 0)
    //            {
    //                viewA.miny = pa.y - (viewA.diff*offset)
    //                viewA.maxy = pa.y + (viewA.diff*offset)
    //                viewA.minx = pa.x
    //            }
    //            viewA.maxx = pa.x
    //            if (viewA.maxy < pa.y) viewA.maxy = pa.y + (viewA.diff*offset)
    //            if (viewA.miny > pa.y) viewA.miny = pa.y - (viewA.diff*offset)
    //            viewA.seriesData.append(pa.x,pa.y)
    //        }
    //        onUpdateVolume: {
    //            //console.log("Volume:", pv)
    //            viewV.diff = viewV.maxy - viewV.miny
    //            if (viewV.seriesData.count == 0)
    //            {
    //                viewV.miny = pv.y - (viewV.diff*offset)
    //                viewV.maxy = pv.y + (viewV.diff*offset)
    //                viewV.minx = pv.x
    //            }
    //            viewA.maxx = pv.x
    //            if (viewV.maxy < pv.y) viewV.maxy = pv.y + (viewV.diff*offset)
    //            if (viewV.miny > pv.y) viewV.miny = pv.y - (viewV.diff*offset)
    //            viewV.seriesData.append(pv.x,pv.y)
    //        }
    //        onUpdateFrequency: {
    //            //console.log("Frequency:", f)
    //            freq.text = Math.round(f).toString()
    //        }

    //    }


    //    menuBar: MenuBar {
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
    //                        console.log("High data rate triggered")
    //                    }
    //                }
    //            }
    //            Action {
    //                id: std
    //                text: qsTr("&Standard Precision")
    //                ActionGroup.group: confActions
    //                checkable: true
    //                checked: true
    //                onTriggered: {
    //                    if (!std.checked)
    //                    {
    //                        // ignore, because mode is already set
    //                        checked = true
    //                    }
    //                    else {
    //                        console.log("Standard precision triggered")
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
    //                    }
    //                }
    //            }


    //            MenuSeparator { }
    //            Action { text: qsTr("&Custom...")
    //                onTriggered: configDialog.visible = true
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
        currentIndex: tabbar.currentIndex

        DataView {
            id: viewP
        }

        DataView {
            id: viewT
        }

        DataView {
            id: viewA
        }

        DataView {
            id: viewV
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
