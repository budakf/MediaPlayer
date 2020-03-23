#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTimer>
#include <QDateTime>
#include <QProcess>

#include <iostream>
#include <memory>

#include "recordedvideopipelinebuilder.h"
#include "livevideopipelinebuilder.h"
#include "multipleinputdialog.h"
#include "recorder.h"

namespace Ui{
class Player;
}

class Player : public QMainWindow{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();

    enum class PlayerState{
        NULLSTATE,
        LIVESTATE,
        RECORDEDSTATE
    };

private:
    Ui::Player *ui;
    std::unique_ptr<IPipelineBuilder> mPipelineBuilder;
    std::unique_ptr<Recorder> mRecorder;

    std::string mResourcePath;
    QStringList mRecordPaths;

    PlayerState mState;
    bool mRunning;
    QTimer* mTimer;

    void setMenus();
    void setMediaPlayerMenu();
    void setFileMenu();
    void setAudioMenu();
    void setVideoMenu();
    void setVideoRecordMenu();

    void play();
    void stop();
    void pause();

    void startRecord(QStringList);
    void stopRecord();

    void enablePlayingButtons(bool);

private slots:
    void openAboutMediaPlayerDialog();
    void openNewFileDialog();
    void openNetworkDialog();
    void increaseVolume();
    void decreaseVolume();
    void convertRGB();
    void convertGrayScale();
    void openNewRecordDialog();

    void on_mPlayBtn_clicked();
    void on_mRewindBtn_clicked();
    void on_mForwardBtn_clicked();
    void on_mSlider_sliderReleased();
    void improveSlider();
    void setVideoTime();
    void on_mVolumeSlider_sliderReleased();
    void resetVideo();

    void enableActionStopRecord();
    void disableActionStopRecord();

};

#endif // PLAYER_H
