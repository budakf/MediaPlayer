#ifndef RECORDEDVIDEOPIPELINE_H
#define RECORDEDVIDEOPIPELINE_H

#include <QDebug>

#include "ipipelinebuilder.h"

class RecordedVideoPipelineBuilder: public IPipelineBuilder{

    Pipeline mPipeline;
    long long int mWinId;
    std::string mFileLocation;

    void setBin(std::string);
    void setSource(std::string, std::string);
    void setVolume(std::string, std::string);
    void setLevel(std::string, std::string);
    void setDemuxer(std::string, std::string);
    void setVideoQueue(std::string, std::string);
    void setVideoDecoder(std::string, std::string);
    void setVideoSink(std::string, std::string);
    void setAudioQueue(std::string, std::string);
    void setAudioDecoder(std::string, std::string);
    void setAudioSink(std::string, std::string);
    void setState(GstState);
    void setLiveness(bool);

    void setPropertiesOfGstElement(std::string, long long int);
    void addElements();
    void linkElements();
    void setBus();
    void destroyPipeline();

    friend gboolean getMessageFromBusForRecordedVideo(GstBus * bus, GstMessage * message, gpointer data);
    friend void onPadAddedForRecordedVideo(GstElement *src, GstPad *newPad, gpointer sink);

public:
    RecordedVideoPipelineBuilder();
    ~RecordedVideoPipelineBuilder();
    void buildPipeline(std::string, long long int);
    Pipeline & getPipeline(){ return mPipeline; }
    std::string getFileLocation(){ return mFileLocation; }
    long long int getWinId(){ return mWinId; }

};


#endif // RECORDEDVIDEOPIPELINE_H
