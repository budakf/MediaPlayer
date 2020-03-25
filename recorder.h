#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>

#include <glib.h>
#include "gst/gst.h"
#include "gst/video/videooverlay.h"


struct RecorderPipeline{
    GstElement* bin;
    GstElement* source;
    GstElement* videoQueue;
    GstElement* videoDepayloader;
    GstElement* h264Parser;
    GstElement* audioQueue;
    GstElement* audioDepayloader;
    GstElement* aacParser;
    GstElement* muxer;
    GstElement* sink;
    GstBus* bus;
};


class Recorder : public QObject{
    Q_OBJECT


public:
    Recorder(QObject *parent = nullptr);
    ~Recorder();

    void buildPipeline(std::string, std::string);
    void startRecord();
    void stopRecord();

    friend void onPadAddedForRecorder(GstElement*, GstPad*, gpointer);
    friend gboolean getMessageFromBusForRecorder(GstBus*, GstMessage*, gpointer);
    friend gpointer pushEosThread(gpointer);


private:
    RecorderPipeline* mRecorderPipeline;

    void setPropertiesOfGstElement(std::string, std::string);
    void addElements();
    void linkElements();
    void setBus();
    void destroyPipeline();

};
#endif // RECORDER_H
