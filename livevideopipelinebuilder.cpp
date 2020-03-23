#include "livevideopipelinebuilder.h"

LiveVideoPipelineBuilder::LiveVideoPipelineBuilder(){
    qDebug()<<"LiveVideoPipelineBuilder::LiveVideoPipelineBuilder";
    gst_init(NULL, NULL);
    mPipeline.bin = NULL;
}

LiveVideoPipelineBuilder::~LiveVideoPipelineBuilder(){
    destroyPipeline();
}

void LiveVideoPipelineBuilder::setBin(std::string pipelineName){
    mPipeline.bin = gst_pipeline_new(pipelineName.data());
}

void LiveVideoPipelineBuilder::setSource(std::string sourceType, std::string sourceName){
    mPipeline.source = gst_element_factory_make(sourceType.data(), sourceName.data());
}

void LiveVideoPipelineBuilder::setVolume(std::string volumeType, std::string volumeName){
    mPipeline.volume = gst_element_factory_make(volumeType.data(), volumeName.data());
}

void LiveVideoPipelineBuilder::setLevel(std::string levelType, std::string levelName){
    mPipeline.level = gst_element_factory_make(levelType.data(), levelName.data());
}

void LiveVideoPipelineBuilder::setVideoQueue(std::string videoqueueType, std::string videoqueueName){
    mPipeline.videoqueue = gst_element_factory_make(videoqueueType.data(), videoqueueName.data());
}

void LiveVideoPipelineBuilder::setVideoDepayloader(std::string depayloaderType, std::string depayloaderName){
    mPipeline.videodepayloader = gst_element_factory_make(depayloaderType.data(), depayloaderName.data());
}

void LiveVideoPipelineBuilder::setVideoDecoder(std::string decoderType, std::string decoderName){
    mPipeline.videodecoder = gst_element_factory_make(decoderType.data(), decoderName.data());
}

void LiveVideoPipelineBuilder::setVideoSink(std::string sinkType, std::string sinkName){
    mPipeline.videosink = gst_element_factory_make(sinkType.data(), sinkName.data());
}

void LiveVideoPipelineBuilder::setAudioQueue(std::string audioqueueType, std::string audioqueueName){
    mPipeline.audioqueue = gst_element_factory_make(audioqueueType.data(), audioqueueName.data());
}

void LiveVideoPipelineBuilder::setAudioDepayloader(std::string audiodepayloaderType, std::string audiodepayloaderName){
    mPipeline.audiodepayloader = gst_element_factory_make(audiodepayloaderType.data(), audiodepayloaderName.data());
}

void LiveVideoPipelineBuilder::setAudioDecoder(std::string audiodecoderType, std::string audiodecoderName){
    mPipeline.audiodecoder = gst_element_factory_make(audiodecoderType.data(), audiodecoderName.data());
}

void LiveVideoPipelineBuilder::setAudioConvertor(std::string audioconvertorType, std::string audioconvertorName){
    mPipeline.audioconvertor = gst_element_factory_make(audioconvertorType.data(), audioconvertorName.data());
}

void LiveVideoPipelineBuilder::setAudioSink(std::string audiosinkType, std::string audiosinkName){
    mPipeline.audiosink = gst_element_factory_make(audiosinkType.data(), audiosinkName.data());
}

void LiveVideoPipelineBuilder::setState(GstState){
    mPipeline.state = GstState::GST_STATE_PAUSED;
}

void LiveVideoPipelineBuilder::setLiveness(bool livenessState){
    mPipeline.isLive = livenessState;
}

void LiveVideoPipelineBuilder::setPropertiesOfGstElement(std::string pVideoSource, long long int pWindId){
    g_object_set(mPipeline.source , "location", pVideoSource.data(), NULL );
    g_object_set(mPipeline.volume , "volume", 0.5, NULL );
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY (mPipeline.videosink), pWindId);
}

void LiveVideoPipelineBuilder::addElements(){
    if(!mPipeline.bin || !mPipeline.source || !mPipeline.videoqueue || !mPipeline.videodepayloader || !mPipeline.videodecoder || !mPipeline.videosink ||
       !mPipeline.audioqueue || !mPipeline.audiodepayloader || !mPipeline.audiodecoder || !mPipeline.audioconvertor || !mPipeline.audiosink){
        g_printerr("All element could not created\n");
    }

    gst_bin_add_many (GST_BIN (mPipeline.bin), mPipeline.source, mPipeline.videoqueue, mPipeline.videodepayloader, mPipeline.videodecoder, mPipeline.videosink, mPipeline.audioqueue,
                      mPipeline.audiodepayloader, mPipeline.audiodecoder, mPipeline.audioconvertor, mPipeline.volume, mPipeline.level, mPipeline.audiosink, NULL);
}

void onPadAddedForLiveVideo(GstElement *src, GstPad *newPad, gpointer sink){
    GstPad *sink_pad = gst_element_get_static_pad ((GstElement *)sink, "sink");
    GstPadLinkReturn ret;

    g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (newPad), GST_ELEMENT_NAME (src));

    if (gst_pad_is_linked (sink_pad)){
      g_print ("We are already linked. Ignoring.\n");
      return;
    }

    ret = gst_pad_link(newPad, sink_pad);
    if (GST_PAD_LINK_FAILED (ret)){
        g_print ("Link failed.\n");
    }
    else{
        g_print ("Link succeeded.\n");
    }

    gst_object_unref (sink_pad);

}


gboolean getMessageFromBusForLiveVideo(GstBus * bus, GstMessage * message, gpointer data){
    //g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message) );

    switch (GST_MESSAGE_TYPE(message) ){
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
    if( !gst_element_link(mPipeline.source, mPipeline.videoqueue) ){
        g_printerr ("Source and Videoqueue could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.videoqueue, mPipeline.videodepayloader) ){
        g_printerr ("Videoqueue and VideoDepayloader could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.videodepayloader, mPipeline.videodecoder) ){
        g_printerr ("VideoDepayloader and VideoDecoder could not be linked.\n");
    }

    if( !gst_element_link(mPipeline.videodecoder, mPipeline.videosink) ){
        g_printerr ("VideoDecoder and VideoSink could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.source, mPipeline.audioqueue) ){
        g_printerr ("Source and AudioQueue could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.audioqueue, mPipeline.audiodepayloader) ){
        g_printerr ("AudioQueue and AudioDepayloader could not be linked.\n");
    }

    if( !gst_element_link(mPipeline.audiodepayloader, mPipeline.audiodecoder) ){
        g_printerr ("AudioDepayloader and AudioDecoder could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.audiodecoder, mPipeline.audioconvertor) ){
        g_printerr ("AudioDecoder and AudioConvertor could not be linked.\n");
    }

    if( !gst_element_link(mPipeline.audioconvertor, mPipeline.volume) ){
        g_printerr ("AudioConvertor and Volume could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.volume, mPipeline.level) ){
        g_printerr ("Volume and Level could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.level, mPipeline.audiosink) ){
        g_printerr ("Level and AudioSink could not be linked.\n");
    }

    g_signal_connect(mPipeline.source, "pad-added", G_CALLBACK (onPadAddedForLiveVideo), mPipeline.videoqueue);
    g_signal_connect(mPipeline.source, "pad-added", G_CALLBACK (onPadAddedForLiveVideo), mPipeline.audioqueue);

}

void LiveVideoPipelineBuilder::buildPipeline(std::string pResourcePath, long long int pWindowID){
    this->setBin(std::string("pipeline"));
    this->setSource(std::string("rtspsrc"), std::string("source"));
    this->setVideoQueue(std::string("queue"), std::string("videoqueue"));
    this->setVideoDepayloader(std::string("rtph264depay"), std::string("videodepayloader"));
    this->setVideoDecoder(std::string("avdec_h264"), std::string("videodecoder"));
    this->setVideoSink(std::string("glimagesink"), std::string("videosink"));
    this->setAudioQueue(std::string("queue"), std::string("audioqueue"));
    this->setAudioDepayloader(std::string("rtpmp4gdepay"), std::string("audiodepayloader"));
    this->setAudioDecoder(std::string("avdec_aac"), std::string("audiodecoder"));
    this->setAudioConvertor(std::string("audioconvert"), std::string("audioconvertor"));
    this->setVolume(std::string("volume"), std::string("volume"));
    this->setLevel(std::string("level"), std::string("level"));
    this->setAudioSink(std::string("autoaudiosink"), std::string("audiosink"));

    this->setLiveness(true);
    this->setPropertiesOfGstElement(pResourcePath, pWindowID);
    this->addElements();
    this->linkElements();
    this->setBus();
    this->setState(GstState::GST_STATE_READY);
}


void LiveVideoPipelineBuilder::setBus(){
    mPipeline.bus = gst_pipeline_get_bus (GST_PIPELINE (mPipeline.bin));
    //gst_bus_add_watch (mPipeline.bus, getMessageFromBusForLiveVideo, this);
    gst_bus_set_sync_handler(mPipeline.bus, (GstBusSyncHandler)getMessageFromBusForLiveVideo, this, NULL);

}

void LiveVideoPipelineBuilder::destroyPipeline(){
    g_object_unref(mPipeline.bus);
    gst_element_set_state(mPipeline.bin, GST_STATE_NULL);
    g_object_unref(mPipeline.bin);
}



