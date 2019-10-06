#include "livevideopipelinebuilder.h"

LiveVideoPipelineBuilder::LiveVideoPipelineBuilder(){
    qDebug()<<"LiveVideoPipelineBuilder::LiveVideoPipelineBuilder";
    gst_init(NULL, NULL);
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
    mPipeline.videodepayloader = gst_element_factory_make(depayloaderType.data(), depayloaderName.data());
}

void LiveVideoPipelineBuilder::setDecoder(std::string decoderType, std::string decoderName){
    mPipeline.videodecoder = gst_element_factory_make(decoderType.data(), decoderName.data());
}

void LiveVideoPipelineBuilder::setVideoSink(std::string sinkType, std::string sinkName){
    mPipeline.videosink = gst_element_factory_make(sinkType.data(), sinkName.data());
}

void LiveVideoPipelineBuilder::setState(GstState){
    mPipeline.state = GstState::GST_STATE_PAUSED;
}

void LiveVideoPipelineBuilder::setLiveness(bool livenessState){
    mPipeline.isLive = livenessState;
}

void LiveVideoPipelineBuilder::setPropertiesOfGstElement(std::string pVideoSource, long long int pWindId){
    g_object_set(mPipeline.source , "location", pVideoSource.data(), NULL );
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY (mPipeline.videosink), pWindId);

}

void LiveVideoPipelineBuilder::addElements(){
    if(!mPipeline.bin || !mPipeline.source || !mPipeline.videodepayloader || !mPipeline.videodecoder || !mPipeline.videosink){
        g_printerr("All element could not created\n");
    }
    gst_bin_add_many (GST_BIN (mPipeline.bin), mPipeline.source, mPipeline.videodepayloader, mPipeline.videodecoder , mPipeline.videosink, NULL);
}


void onPadAddedForLiveVideo(GstElement *src, GstPad *newPad, gpointer sink) {
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


gboolean getMessageFromBusForLiveVideo(GstBus * bus, GstMessage * message, gpointer data){
    g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message) );

    switch (GST_MESSAGE_TYPE(message) ) {
        case GST_MESSAGE_ERROR:
        {
            GError *err;
            gchar *debug;

            gst_message_parse_error (message, &err, &debug);
            g_print ("Error: %s\n", err->message);
            g_error_free (err);
            g_free (debug);
            break;
        }
        case GST_MESSAGE_STATE_CHANGED:
            //
            break;
        case GST_MESSAGE_EOS:
            //
          break;
        default:
          break;
    }


    return TRUE;

}


void LiveVideoPipelineBuilder::linkElements(){
    if( !gst_element_link(mPipeline.source, mPipeline.videodepayloader) ){
        g_printerr ("Source and Depayloader could not be linked.\n");
    }

    g_signal_connect(mPipeline.source, "pad-added", G_CALLBACK (onPadAddedForLiveVideo), mPipeline.videodepayloader);

    if(!gst_element_link(mPipeline.videodepayloader, mPipeline.videodecoder) ){
        g_printerr ("Depayloader and Decoder could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.videodecoder, mPipeline.videosink) ){
        g_printerr ("Decoder and Sink could not be linked.\n");
    }

}

void LiveVideoPipelineBuilder::buildPipeline(std::string pResourcePath, long long int pWindowID){
    this->setBin(std::string("pipeline"));
    this->setSource(std::string("rtspsrc"), std::string("source"));
    this->setDepayloader(std::string("rtph264depay"), std::string("depayloader"));
    this->setDecoder(std::string("avdec_h264"), std::string("decoder"));
    this->setVideoSink(std::string("glimagesink"), std::string("sink"));
    this->setLiveness(true);
    this->setPropertiesOfGstElement(pResourcePath, pWindowID);
    this->addElements();
    this->linkElements();
    this->setState(GstState::GST_STATE_READY);
}


void LiveVideoPipelineBuilder::setBus(){
    mPipeline.bus = gst_pipeline_get_bus (GST_PIPELINE (mPipeline.bin));
    gst_bus_add_watch (mPipeline.bus, getMessageFromBusForLiveVideo, NULL);
}

void LiveVideoPipelineBuilder::destroyPipeline(){
    g_object_unref(mPipeline.bus);
    gst_element_set_state(mPipeline.bin, GST_STATE_NULL);
    g_object_unref(mPipeline.bin);
}



