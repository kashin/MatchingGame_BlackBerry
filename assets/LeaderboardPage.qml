import bb.cascades 1.4
import don.matching 1.0

Page {
    Container {
        Container {
            id: leadingContainer
            Container {
                Label {
                    id: listViewHeaderLabel
                    text: qsTr("High Scores")
                    textStyle {
                        base: SystemDefaults.TextStyles.BigText
                        fontWeight: FontWeight.Bold
                    }
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Label {
                        text: qsTr("Player Name")
                        textStyle {
                            base: SystemDefaults.TextStyles.TitleText
                            textAlign: TextAlign.Center
                        }
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1
                        }
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        text: qsTr("Score")
                        textStyle {
                            base: SystemDefaults.TextStyles.TitleText
                            textAlign: TextAlign.Center
                        }
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1
                        }
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                } // Label's container
                Divider {
                }
            }
        } // leadingContainer
        ListView {
            id: leaderBoardList
            dataModel: LeaderBoardModel {
                id: leaderBoardModel
            }
            listItemComponents: [
                ListItemComponent {
                    id: listItem
                    type: "item"
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        Label {
                            text: ListItemData.playerName
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                textAlign: TextAlign.Center
                            }
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 1
                            }
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            text: ListItemData.score
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                textAlign: TextAlign.Center
                            }
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 1
                            }
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                    } // Container
                } // ListItemComponent
            ]
        }
    }
    onCreationCompleted: {
        leaderBoardModel.init();
    }
}
