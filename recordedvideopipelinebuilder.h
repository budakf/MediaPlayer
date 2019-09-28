#ifndef RECORDEDVIDEOPIPELINE_H
#define RECORDEDVIDEOPIPELINE_H

#include "ipipelinebuilder.h"
#include <QDebug>

class RecordedVideoPipelineBuilder: public IPipelineBuilder{

    Pipeline mPipeline;
    long long int mWindId;

    void setBin(std::string);
    void setSource(std::string, std::string);
    void setDecoder(std::string, std::string);
    void setSink(std::string, std::string);
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
    Pipeline & getPipeline(){
        return mPipeline;
    }

};


#endif // RECORDEDVIDEOPIPELINE_H
