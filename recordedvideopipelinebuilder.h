#ifndef RECORDEDVIDEOPIPELINE_H
#define RECORDEDVIDEOPIPELINE_H

#include "ipipelinebuilder.h"


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

public:
    RecordedVideoPipelineBuilder();
    ~RecordedVideoPipelineBuilder();
    void buildPipeline(long long int);
    Pipeline & getPipeline(){
        return mPipeline;
    }

};


#endif // RECORDEDVIDEOPIPELINE_H
