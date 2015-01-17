import QtQuick 2.0

Item {
        id: item
        x: 5
        width: parent.width
        height:80

        Rectangle {
            id: background
            anchors.fill: parent

            Row {
                id: row1

                Image {
                    id: imageItem
                    width: 75
                    height: 75
                    asynchronous: true
                    source: album.thumbImage == "" ? "images/album.png" : album.thumbImage
                }

                Column {
                    Row {
                        Text {
                            id: titleItem
                            text: album.title
                            font.bold: true
                        }
                        spacing: 20
                    }

                    Row {
                        Text {
                            id: categoryItem
                            text: album.category
                        }
                        Text {
                            id: subcategoryItem
                            text: album.subcategory
                        }
                        Text {
                            id: imageCountItem
                            text: album.imageCount == -1 ? "?" : album.imageCount
                        }
                        //layoutDirection: Qt.RightToLeft
                        spacing: 20
                    }

                    spacing: 2
                }
                spacing: 10
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.info("Clicked " + album.id)
                selectedAlbumId = album.id
            }
        }

        states: [
            State {
                name: "selected"
                when: (album.id==selectedAlbumId)
                PropertyChanges { target: background; color: "gray" }
            }
        ]

        transitions: [
           Transition {
               from: ""; to: "selected"
               PropertyAnimation { target: background
                                   properties: "color"; duration: 500 }
           },
           Transition {
               from: "selected"; to: ""
               PropertyAnimation { target: background
                                   properties: "color"; duration: 500 }
           } ]
}


