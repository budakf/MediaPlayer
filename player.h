#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTimer>
#include <QDateTime>

#include <iostream>
#include <memory>

#include "recordedvideopipelinebuilder.h"
#include "livevideopipelinebuilder.h"

namespace Ui {
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
    std::string mResourcePath;
    PlayerState mState;
    bool mRunning;
    QTimer* mTimer;

    void setMenus();
    void setMediaPlayerMenu();
    void setFileMenu();
    void setAudioMenu();
    void setVideoMenu();
    void play();
    void stop();
    void pause();
    void enablePlayingButtons(bool);

private slots:
    void openAboutMediaPlayerDialog();
    void openNewFileDialog();
    void openNetworkDialog();
    void increaseVolume();
    void decreaseVolume();
    void convertRGB();
    void convertGrayScale();
    void on_mPlayBtn_clicked();
    void on_mRewindBtn_clicked();
    void on_mForwardBtn_clicked();
    void on_mSlider_sliderReleased();
    void improveSlider();
    void setVideoTime();
};

#endif // PLAYER_H
