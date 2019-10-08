#ifndef IPIPELINEBUILDER_H
#define IPIPELINEBUILDER_H

#include <memory>
#include <string>
#include "gst/gst.h"
#include "gst/video/videooverlay.h"

struct Pipeline{
    GstElement* bin;
    GstElement* source;
    GstElement* volume;              // for audio
    GstElement* level;               // for audio
    GstElement* demuxer;             // only for recorded video
    GstElement* videoqueue;
    GstElement* videodepayloader;    // only for live video
    GstElement* videodecoder;
    GstElement* videosink;
    GstElement* audioqueue;
    GstElement* audiodepayloader;    // only for live video
    GstElement* audiodecoder;
    GstElement* audioconvertor;      // only for live video
    GstElement* audiosink;
    GstBus* bus;
    GstState state;
    bool isLive;
};


class IPipelineBuilder{

public:
    virtual void buildPipeline(std::string, long long int) = 0;
    virtual ~IPipelineBuilder(){}
    virtual Pipeline & getPipeline() = 0;

private:
    virtual void setBin(std::string) = 0;
    virtual void setSource(std::string, std::string) = 0;
    virtual void setVolume(std::string, std::string) = 0;
    virtual void setLevel(std::string, std::string) = 0;
    virtual void setVideoQueue(std::string, std::string) = 0;
    virtual void setVideoDecoder(std::string, std::string) = 0;
    virtual void setVideoSink(std::string, std::string) = 0;
    virtual void setAudioQueue(std::string, std::string) = 0;
    virtual void setAudioDecoder(std::string, std::string) = 0;
    virtual void setAudioSink(std::string, std::string) = 0;

    virtual void setState(GstState) = 0;
    virtual void setLiveness(bool) = 0;
    virtual void setPropertiesOfGstElement(std::string, long long int) = 0;
    virtual void addElements() = 0;
    virtual void linkElements() = 0;
    virtual void setBus() = 0;
    virtual void destroyPipeline() = 0;

};


#endif // IPIPELINEBUILDER_H
