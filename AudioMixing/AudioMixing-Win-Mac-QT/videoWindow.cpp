#include <QDebug>
#include "videoWindow.h"
#include "nrtc_engine.h"

VideoWindow::VideoWindow(QWidget *parent /*= Q_NULLPTR*/)
    :QMainWindow(parent)
{
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    ui.setupUi(this);
   
    this->setWindowTitle("NERtcSample-AudioMixer");

    setVisible(false);

    m_videoWindowMap[1] = ui.video_1;
    m_videoWindowMap[2] = ui.video_2;
    m_videoWindowMap[3] = ui.video_3;
    m_videoWindowMap[4] = ui.video_4;
    m_audioSetting = new AudioMixSetting;


}

VideoWindow::~VideoWindow()
{

}

void VideoWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    on_disconnectBtn_clicked(false);
}



void VideoWindow::setNeRtcEngine(std::shared_ptr<NRTCEngine> ptr)
{
    m_engine = ptr;
    m_audioSetting->setEngine(ptr);

    connect(m_engine.get(), &NRTCEngine::userJoined, this, &VideoWindow::onUserJoined);
    connect(m_engine.get(), &NRTCEngine::userLeft, this, &VideoWindow::onUserLeft);
}

void VideoWindow::onJoinChannel(QString& roomid, QString& usrId)
{
    this->show();
    
    void* hwnd = ui.video_1->getVideoHwnd();
    ui.video_1->setUsrID(usrId);
    m_engine->setupLocalVideo(hwnd);
    m_engine->joinChannel("", roomid, usrId, true, true, 2);
    //开启camera
    m_engine->enableVideo(true);
    m_engine->enableAudio(true);

}

void VideoWindow::on_disconnectBtn_clicked(bool checked)
{
    this->hide();
    m_videoCount = 1;
    for ( auto item : m_videoWindowMap )
    {   
        item.second->closeRender();
    }
    if(m_audioSetting->isVisible())
        m_audioSetting->hide();
    m_engine->leaveChannel();
    emit closeVideoWindowSignal();
    
    
}

void VideoWindow::on_audiomixBtn_clicked(bool checked)
{
    m_audioSetting->isVisible() ? m_audioSetting->hide() : m_audioSetting->show();
}

void VideoWindow::onUserJoined(quint64 uid)
{
    //暂定最大4人，可以自己开放房间最大人数
    m_videoCount++;
    if (m_videoCount >= 4) {
        return;
    }
   

    auto hwnd = m_videoWindowMap[m_videoCount]->getVideoHwnd();
    m_videoWindowMap[m_videoCount]->setUsrID(QString::number(uid));
    m_engine->setupRemoteVideo(uid, hwnd);
   
}

void VideoWindow::onUserLeft(quint64 uid)
{  
    m_engine->stopRemoteVideo(uid);
    m_videoWindowMap[m_videoCount]->closeRender();
    m_videoCount--;
}

