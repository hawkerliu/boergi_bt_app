import QtQuick 2.9
import QtQuick.Controls 2.3

Slider {
    id: confSlider
    //id: prsMRslider
    //anchors.top: prsMR.bottom
    snapMode: Slider.SnapAlways
    stepSize: 1
    width: parent.width
    value: 8
    from: 1
    to: 128

    property int iterator: 0
    Keys.onLeftPressed: {
        // dividieren durch 2
        if (confSlider.value == 2) confSlider.value = 1
        if (confSlider.value == 4) confSlider.value = 2
        if (confSlider.value == 8) confSlider.value = 4
        if (confSlider.value == 16) confSlider.value = 8
        if (confSlider.value == 32) confSlider.value = 16
        if (confSlider.value == 64) confSlider.value = 32
        if (confSlider.value == 128) confSlider.value = 64
    }
    onMoved: {
        if (confSlider.value > 2 && confSlider.value < 4)
        {
            increase()
        }
        if (confSlider.value > 4 && confSlider.value < 8)
        {
            for (iterator=confSlider.value;iterator<8;iterator++)
            {
                increase()
            }
        }
        if (confSlider.value > 8 && confSlider.value < 16)
        {
            for (iterator=confSlider.value;iterator<16;iterator++)
            {
                increase()
            }
        }
        if (confSlider.value > 16 && confSlider.value < 32)
        {
            for (iterator=confSlider.value;iterator<32;iterator++)
            {
                increase()
            }
        }
        if (confSlider.value > 32 && confSlider.value < 64)
        {
            for (iterator=confSlider.value;iterator<64;iterator++)
            {
                increase()
            }
        }
        if (confSlider.value > 64 && confSlider.value < 128)
        {
            for (iterator=confSlider.value;iterator<128;iterator++)
            {
                increase()
            }
        }
    }
}
