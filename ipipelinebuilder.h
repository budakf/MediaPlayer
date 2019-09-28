#ifndef IPIPELINEBUILDER_H
#define IPIPELINEBUILDER_H

#include <memory>
#include <string>
#include "gst/gst.h"
#include "gst/video/videooverlay.h"

struct Pipeline{
    GstElement* bin;
    GstElement* source;
    GstElement* decoder;
    GstElement* depayloader;    //
    GstElement* sink;
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
    virtual void setDecoder(std::string, std::string) = 0;
    virtual void setSink(std::string, std::string) = 0;
    virtual void setState(GstState) = 0;
    virtual void setLiveness(bool) = 0;
    virtual void setPropertiesOfGstElement(std::string, long long int) = 0;
    virtual void addElements() = 0;
    virtual void linkElements() = 0;
    virtual void setBus() = 0;
    virtual void destroyPipeline() = 0;

};


#endif // IPIPELINEBUILDER_H
