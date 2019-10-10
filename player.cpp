#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::Player){
    ui->setupUi(this);
    setMenus();
    mState = PlayerState::NULLSTATE;
    mRunning = false;
    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, &Player::improveSlider);
}

Player::~Player(){
    delete ui;
}


void Player::setMenus(){
    setMediaPlayerMenu();
    setFileMenu();
    setAudioMenu();
    setVideoMenu();
}


void Player::setMediaPlayerMenu(){
    connect(ui->actionAboutMediaPlayer, &QAction::triggered, this,
            &Player::openAboutMediaPlayerDialog);
    connect(ui->actionQuitMediaPlayer, &QAction::triggered, [](){
        std::cout<<"Quit App"<<std::endl;
        QApplication::quit();
    });
}


void Player::setFileMenu(){
    connect(ui->actionOpenNewFile, &QAction::triggered, this, &Player::openNewFileDialog);
    connect(ui->actionOpenNetwork, &QAction::triggered, this, &Player::openNetworkDialog);
}


void Player::setAudioMenu(){
    connect(ui->actionVolumeUp, &QAction::triggered, this, &Player::increaseVolume);
    connect(ui->actionVolumeDown, &QAction::triggered, this, &Player::decreaseVolume);
}


void Player::setVideoMenu(){
    connect(ui->actionVolumeUp, &QAction::triggered, this, &Player::convertRGB);
    connect(ui->actionVolumeDown, &QAction::triggered, this, &Player::convertGrayScale);
}


void Player::openAboutMediaPlayerDialog(){
    QMessageBox* aboutMessageBox = new QMessageBox(this);
    aboutMessageBox->setWindowTitle("About MediaPlayer");
    aboutMessageBox->setText("MediaPlayer1.0");
    aboutMessageBox->setInformativeText("Used Techonologies\nQt C++ Gstreamer\nDeveloped by Fatih Budak");
    aboutMessageBox->setStandardButtons(QMessageBox::Ok);
    aboutMessageBox->show();
}


void Player::openNewFileDialog(){
    mResourcePath.clear();
    mResourcePath = QFileDialog::getOpenFileName(this, tr("Open Video"),
                             "/home", tr("Video Files (*.mp4 *.flv *.webm *.3gp)")).toStdString();

    if(!mResourcePath.empty()){
        mPipelineBuilder.reset(nullptr);
        mPipelineBuilder = std::make_unique<RecordedVideoPipelineBuilder>();
        mPipelineBuilder->buildPipeline(mResourcePath, ui->mVideoPlayerArea->winId());
        mState = PlayerState::RECORDEDSTATE;
        ui->mRewindBtn->show();
        ui->mForwardBtn->show();
        ui->mSlider->setValue(0);
        play();
    }
}


void Player::openNetworkDialog(){
    mResourcePath.clear();
    bool ok;
    mResourcePath = QInputDialog::getText(this, tr(""), tr("Live Video Resource"),
                                 QLineEdit::Normal, "rtsp://", &ok).toStdString();

    if(!mResourcePath.empty()){
        mPipelineBuilder.reset(nullptr);
        mPipelineBuilder = std::make_unique<LiveVideoPipelineBuilder>();
        mPipelineBuilder->buildPipeline(mResourcePath, ui->mVideoPlayerArea->winId());
        mState = PlayerState::LIVESTATE;
        ui->mRewindBtn->hide();
        ui->mForwardBtn->hide();
        play();
    }
}


void Player::increaseVolume(){
    ui->mVolumeSlider->setValue(ui->mVolumeSlider->value() + 10);
    g_object_set(mPipelineBuilder->getPipeline().volume, "volume", (ui->mVolumeSlider->value() + 10) * 0.01, NULL);
}

void Player::decreaseVolume(){
    ui->mVolumeSlider->setValue(ui->mVolumeSlider->value() - 10);
    g_object_set(mPipelineBuilder->getPipeline().volume, "volume", (ui->mVolumeSlider->value() - 10) * 0.01, NULL);
}

void Player::convertRGB(){

}

void Player::convertGrayScale(){

}

void Player::play(){
    gst_element_set_state(mPipelineBuilder->getPipeline().bin, GST_STATE_PLAYING);
    enablePlayingButtons(true);
    ui->mPlayBtn->setText("Pause");
    mRunning = true;
    if(mState == PlayerState::RECORDEDSTATE){
        mTimer->start(1000);
    }
    else if(mState == PlayerState::LIVESTATE){
        ui->mSlider->setValue(ui->mSlider->maximum());
        ui->mSlider->setDisabled(true);
         ui->mVideoTime->setText( QDateTime::fromTime_t(0).toUTC().toString("hh:mm:ss") );
    }
}

void Player::stop(){
    gst_element_set_state (mPipelineBuilder->getPipeline().bin, GST_STATE_NULL);
    mRunning = false;
}

void Player::pause(){
    gst_element_set_state(mPipelineBuilder->getPipeline().bin, GST_STATE_PAUSED);
    mRunning = false;
    mTimer->stop();
}

void Player::enablePlayingButtons(bool pEnabled){
    ui->mPlayBtn->setEnabled(pEnabled);
    ui->mRewindBtn->setEnabled(pEnabled);
    ui->mForwardBtn->setEnabled(pEnabled);
    ui->mSlider->setEnabled(pEnabled);
    ui->mVolumeSlider->setEnabled(pEnabled);
}

void Player::on_mPlayBtn_clicked(){
    if(!mRunning){
        play();
        ui->mPlayBtn->setText("Pause");
    }
    else{
        pause();
        ui->mPlayBtn->setText("Play");
    }
}

void Player::on_mRewindBtn_clicked(){
    gint64 position;
    gst_element_query_position(mPipelineBuilder->getPipeline().bin ,GST_FORMAT_TIME, &position);
    gint64 newPosition = (position - 10 * GST_SECOND) < 0  ?  0 : (position - 10 * GST_SECOND);
    gst_element_seek_simple(mPipelineBuilder->getPipeline().bin, GST_FORMAT_TIME,
                            GST_SEEK_FLAG_FLUSH, newPosition);
    ui->mSlider->setValue(newPosition/GST_SECOND);
    setVideoTime();
}

void Player::on_mForwardBtn_clicked(){
    gint64 length, position;
    gst_element_query_position(mPipelineBuilder->getPipeline().bin ,GST_FORMAT_TIME, &position);
    gst_element_query_duration(mPipelineBuilder->getPipeline().bin ,GST_FORMAT_TIME, &length);
    gint64 newPosition = (position + 10 * GST_SECOND) > length ? length : (position + 10 * GST_SECOND);
    gst_element_seek_simple(mPipelineBuilder->getPipeline().bin, GST_FORMAT_TIME,
                            GST_SEEK_FLAG_FLUSH, newPosition);
    ui->mSlider->setValue(newPosition/GST_SECOND);
    setVideoTime();
}

void Player::on_mSlider_sliderReleased(){
    qint64 length, newPosition;
    gst_element_query_duration(mPipelineBuilder->getPipeline().bin, GST_FORMAT_TIME, &length);
    newPosition = ui->mSlider->value() * GST_SECOND;
    qDebug()<<ui->mSlider->value() << newPosition/GST_SECOND << length/GST_SECOND;
    gst_element_seek_simple(mPipelineBuilder->getPipeline().bin, GST_FORMAT_TIME,
                            GST_SEEK_FLAG_FLUSH, newPosition);
    setVideoTime();
}


void Player::improveSlider(){
    if(mState == PlayerState::RECORDEDSTATE){
        qint64 length;
        gst_element_query_duration(mPipelineBuilder->getPipeline().bin ,GST_FORMAT_TIME, &length);
        int max = length/GST_SECOND;
        ui->mSlider->setMaximum(max);

        int older = ui->mSlider->value();
        ui->mSlider->setValue(++older);
        setVideoTime();
    }
}

void Player::setVideoTime(){
    int time = ui->mSlider->value();
    ui->mVideoTime->setText( QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss") );
}

void Player::on_mVolumeSlider_sliderReleased(){
    g_object_set(mPipelineBuilder->getPipeline().volume, "volume", ui->mVolumeSlider->value() * 0.01, NULL);
}
