// DATAVIEW

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import QtCharts 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3



Page {

    id: pageDV
    //anchors.fill: parent
    width: 640
    height: 480
    property int maxData: 500
    property real miny
    property real maxy
    property real minx
    property real maxx
    property real diff

    property alias seriesData: series
    ChartView {
        id: viewDV
        legend.visible: false
        anchors.fill: parent
        //antialiasing: true
        ValueAxis {
            id: axisY
            min: miny
            max: maxy
        }
        ValueAxis {
            id: axisX
            min: minx
            max: maxx
        }
        LineSeries {
            id: series
            name: "External"
            axisX: axisX
            axisY: axisY
            onPointAdded: {
                if (series.count > maxData)
                {
                    minx = series.at(0).x
                    series.remove(0)
                }
            }
        }
    }
}
