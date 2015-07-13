import QtQuick 2.0

Rectangle {
    id: simplebutton
    color: "grey"
    width: 150; height: 75

    Text{
        id: buttonLabel
        anchors.centerIn: parent
        text: "button label"
    }

    Rectangle {
        id: rectangle1
        x: -99
        y: -117
        width: 350
        height: 309
        color: "#ffffff"
        z: -1
    }
}
