#include "recordedvideopipelinebuilder.h"

RecordedVideoPipelineBuilder::RecordedVideoPipelineBuilder(){
    gst_init(NULL, NULL);
}


RecordedVideoPipelineBuilder::~RecordedVideoPipelineBuilder(){

}

void RecordedVideoPipelineBuilder::setBin(std::string pipelineName){
    mPipeline.bin = gst_pipeline_new(pipelineName.data());
}


void RecordedVideoPipelineBuilder::setSource(std::string sourceType, std::string sourceName){
    mPipeline.source = gst_element_factory_make(sourceType.data(), sourceName.data());
}


void RecordedVideoPipelineBuilder::setDecoder(std::string decoderType, std::string decoderName){
    mPipeline.decoder = gst_element_factory_make(decoderType.data(), decoderName.data());
}


void RecordedVideoPipelineBuilder::setSink(std::string sinkType, std::string sinkName){
    mPipeline.sink = gst_element_factory_make(sinkType.data(), sinkName.data());
}

void RecordedVideoPipelineBuilder:: setState(GstState){
    mPipeline.state = GstState::GST_STATE_PAUSED;
}


void RecordedVideoPipelineBuilder::setLiveness(bool livenessState){
    mPipeline.isLive = livenessState;
}

void RecordedVideoPipelineBuilder::setPropertiesOfGstElement(std::string pFileLocation, long long int pWindId){
    g_object_set(mPipeline.source , "location", pFileLocation.data(), NULL );
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY (mPipeline.sink), pWindId);
}

void RecordedVideoPipelineBuilder::addElements(){
    if(!mPipeline.bin || !mPipeline.source || !mPipeline.decoder || !mPipeline.sink){
        g_printerr("All element could not created\n");
    }
    gst_bin_add_many (GST_BIN (mPipeline.bin), mPipeline.source, mPipeline.decoder , mPipeline.sink, NULL);
}

void onPadAddedForRecordedVideo(GstElement *src, GstPad *newPad, gpointer sink) {
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


gboolean getMessageFromBusForRecordedVideo(GstBus * bus, GstMessage * message, gpointer data){
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


void RecordedVideoPipelineBuilder::linkElements(){
    if( !gst_element_link(mPipeline.source, mPipeline.decoder) ){
        g_printerr ("Source and decoder could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.decoder, mPipeline.sink) ){
        g_printerr ("Decoder and sink could not be linked.\n");
    }

    g_signal_connect(mPipeline.decoder, "pad-added", G_CALLBACK (onPadAddedForRecordedVideo), mPipeline.sink);
}


void RecordedVideoPipelineBuilder::buildPipeline(std::string pResourcePath, long long int pWindowID){
    this->setBin(std::string("pipeline"));
    this->setSource(std::string("filesrc"), std::string("source"));
    this->setDecoder(std::string("decodebin"), std::string("decoder"));
    this->setSink(std::string("glimagesink"), std::string("sink"));
    this->setLiveness(false);
    this->setPropertiesOfGstElement(pResourcePath, pWindowID);
    this->addElements();
    this->linkElements();
    this->setState(GstState::GST_STATE_READY);
}

void RecordedVideoPipelineBuilder::setBus(){
    mPipeline.bus = gst_pipeline_get_bus (GST_PIPELINE (mPipeline.bin));
    gst_bus_add_watch (mPipeline.bus, getMessageFromBusForRecordedVideo, NULL);
}

void RecordedVideoPipelineBuilder::destroyPipeline(){
    g_object_unref(mPipeline.bus);
    gst_element_set_state (mPipeline.bin, GST_STATE_NULL);
    g_object_unref (mPipeline.bin);

}



