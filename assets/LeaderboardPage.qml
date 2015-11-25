import bb.cascades 1.4
import don.matching 1.0

Page {
    Container {
        ListView {
            id: leaderBoardList
            dataModel: LeaderBoardModel {
                id: leaderBoardModel
            }
        }
    }
    onCreationCompleted: {
        leaderBoardModel.init();
    }
}
