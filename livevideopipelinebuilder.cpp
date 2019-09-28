#include "livevideopipelinebuilder.h"

LiveVideoPipelineBuilder::LiveVideoPipelineBuilder(){
    qDebug()<<"LiveVideoPipelineBuilder::LiveVideoPipelineBuilder";
}

LiveVideoPipelineBuilder::~LiveVideoPipelineBuilder(){

}

void LiveVideoPipelineBuilder::setBin(std::string pipelineName){
    mPipeline.bin = gst_pipeline_new(pipelineName.data());
}

void LiveVideoPipelineBuilder::setSource(std::string sourceType, std::string sourceName){
    mPipeline.source = gst_element_factory_make(sourceType.data(), sourceName.data());
}

void LiveVideoPipelineBuilder::setDepayloader(std::string depayloaderType, std::string depayloaderName){
    mPipeline.depayloader = gst_element_factory_make(depayloaderType.data(), depayloaderName.data());
}

void LiveVideoPipelineBuilder::setDecoder(std::string decoderType, std::string decoderName){
    mPipeline.decoder = gst_element_factory_make(decoderType.data(), decoderName.data());
}

void LiveVideoPipelineBuilder::setSink(std::string sinkType, std::string sinkName){
    mPipeline.sink = gst_element_factory_make(sinkType.data(), sinkName.data());
}

void LiveVideoPipelineBuilder::setState(GstState){
    mPipeline.state = GstState::GST_STATE_PAUSED;
}

void LiveVideoPipelineBuilder::setLiveness(bool livenessState){
    mPipeline.isLive = livenessState;
}

void LiveVideoPipelineBuilder::setPropertiesOfGstElement(std::string pVideoSource, long long int pWindId){
    g_object_set(mPipeline.source , "location", pVideoSource.data(), NULL );
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY (mPipeline.sink), pWindId);

}

void LiveVideoPipelineBuilder::addElements(){
    if(!mPipeline.bin || !mPipeline.source || !mPipeline.depayloader || !mPipeline.decoder || !mPipeline.sink){
        g_printerr("All element could not created\n");
    }
    gst_bin_add_many (GST_BIN (mPipeline.bin), mPipeline.source, mPipeline.depayloader, mPipeline.decoder , mPipeline.sink, NULL);
}

static void onPadAdded(GstElement *src, GstPad *newPad, gpointer sink) {
    GstPad *sink_pad = gst_element_get_static_pad ((GstElement *)sink, "sink");
    GstPadLinkReturn ret;

    g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (newPad), GST_ELEMENT_NAME (src));

    if (gst_pad_is_linked (sink_pad)) {
      g_print ("We are already linked. Ignoring.\n");
      return;
    }

    ret = gst_pad_link (newPad, sink_pad);
    if (GST_PAD_LINK_FAILED (ret)){
        g_print ("Link failed.\n");
    }
    else{
        g_print ("Link succeeded.\n");
    }

    gst_object_unref (sink_pad);

}

void LiveVideoPipelineBuilder::linkElements(){
    if( !gst_element_link(mPipeline.source, mPipeline.depayloader) ){
        g_printerr ("Source and Depayloader could not be linked.\n");
    }

    g_signal_connect(mPipeline.source, "pad-added", G_CALLBACK (onPadAdded), mPipeline.depayloader);

    if(!gst_element_link(mPipeline.depayloader, mPipeline.decoder) ){
        g_printerr ("Depayloader and Decoder could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.decoder, mPipeline.sink) ){
        g_printerr ("Decoder and Sink could not be linked.\n");
    }

}

void LiveVideoPipelineBuilder::buildPipeline(long long int pWindowID){
    this->setBin(std::string("pipeline"));
    this->setSource(std::string("rtspsrc"), std::string("source"));
    this->setDepayloader(std::string("rtph264depay"), std::string("depayloader"));
    this->setDecoder(std::string("avdec_h264"), std::string("decoder"));
    this->setSink(std::string("glimagesink"), std::string("sink"));
    this->setLiveness(true);
    this->setPropertiesOfGstElement(std::string("rtsp://semerkandglb.mediatriple.net:1935/semerkandliveedge/semerkand2"), pWindowID);
    this->addElements();
    this->linkElements();
    this->setState(GstState::GST_STATE_NULL);
}


