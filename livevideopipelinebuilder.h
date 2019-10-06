#ifndef LIVEVIDEOPIPELINEBUILDER_H
#define LIVEVIDEOPIPELINEBUILDER_H

#include "ipipelinebuilder.h"
#include <QDebug>

class LiveVideoPipelineBuilder : public IPipelineBuilder{

    Pipeline mPipeline;
    long long int mWindId;

    void setBin(std::string);
    void setSource(std::string, std::string);
    void setDepayloader(std::string, std::string);
    void setDecoder(std::string, std::string);
    void setVideoSink(std::string, std::string);
    void setState(GstState);
    void setLiveness(bool);
    void setPropertiesOfGstElement(std::string, long long int);
    void addElements();
    void linkElements();
    void setBus();
    void destroyPipeline();

    friend gboolean getMessageFromBusForLiveVideo(GstBus * bus, GstMessage * message, gpointer data);
    friend void onPadAddedForLiveVideo(GstElement *src, GstPad *newPad, gpointer sink);

public:
    LiveVideoPipelineBuilder();
    ~LiveVideoPipelineBuilder();
    void buildPipeline(std::string, long long int);
    Pipeline & getPipeline(){
        return mPipeline;
    }

};

#endif // LIVEVIDEOPIPELINEBUILDER_H
