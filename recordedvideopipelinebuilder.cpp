#include "recordedvideopipelinebuilder.h"

RecordedVideoPipelineBuilder::RecordedVideoPipelineBuilder(){
    gst_init(NULL, NULL);
    mPipeline.bin = NULL;
}


RecordedVideoPipelineBuilder::~RecordedVideoPipelineBuilder(){
    destroyPipeline();
}

void RecordedVideoPipelineBuilder::setBin(std::string pipelineName){
    mPipeline.bin = gst_pipeline_new(pipelineName.data());
}

void RecordedVideoPipelineBuilder::setSource(std::string sourceType, std::string sourceName){
    mPipeline.source = gst_element_factory_make(sourceType.data(), sourceName.data());
}

void RecordedVideoPipelineBuilder::setDemuxer(std::string demuxerType, std::string demuxerName){
    mPipeline.demuxer = gst_element_factory_make(demuxerType.data(), demuxerName.data());
}

void RecordedVideoPipelineBuilder::setVideoQueue(std::string queueType, std::string queueName){
    mPipeline.videoqueue = gst_element_factory_make(queueType.data(), queueName.data());
}

void RecordedVideoPipelineBuilder::setVideoDecoder(std::string videodecoderType, std::string videodecoderName){
    mPipeline.videodecoder = gst_element_factory_make(videodecoderType.data(), videodecoderName.data());
}

void RecordedVideoPipelineBuilder::setVideoSink(std::string sinkType, std::string sinkName){
    mPipeline.videosink = gst_element_factory_make(sinkType.data(), sinkName.data());
}

void RecordedVideoPipelineBuilder::setAudioQueue(std::string queueType, std::string queueName){
    mPipeline.audioqueue = gst_element_factory_make(queueType.data(), queueName.data());

}

void RecordedVideoPipelineBuilder::setAudioDecoder(std::string audiodecoderType, std::string audiodecoderName){
    mPipeline.audiodecoder = gst_element_factory_make(audiodecoderType.data(), audiodecoderName.data());
}

void RecordedVideoPipelineBuilder::setAudioSink(std::string audiosinkType, std::string audiosinkName){
    mPipeline.audiosink = gst_element_factory_make(audiosinkType.data(), audiosinkName.data());
}

void RecordedVideoPipelineBuilder:: setState(GstState){
    mPipeline.state = GstState::GST_STATE_PAUSED;
}

void RecordedVideoPipelineBuilder::setLiveness(bool livenessState){
    mPipeline.isLive = livenessState;
}

void RecordedVideoPipelineBuilder::setPropertiesOfGstElement(std::string pFileLocation, long long int pWindId){
    g_object_set(mPipeline.source , "location", pFileLocation.data(), NULL );
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY (mPipeline.videosink), pWindId);
}

void RecordedVideoPipelineBuilder::addElements(){
    if(!mPipeline.bin || !mPipeline.source || !mPipeline.demuxer || !mPipeline.videoqueue || !mPipeline.videodecoder ||
       !mPipeline.videosink || !mPipeline.audioqueue || !mPipeline.audiodecoder || !mPipeline.audiosink){
        g_printerr("All element could not created\n");
    }
    gst_bin_add_many (GST_BIN (mPipeline.bin), mPipeline.source, mPipeline.demuxer, mPipeline.videoqueue,
                      mPipeline.videodecoder , mPipeline.videosink, mPipeline.audioqueue, mPipeline.audiodecoder, mPipeline.audiosink, NULL);
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

    RecordedVideoPipelineBuilder* tempBuilder = (RecordedVideoPipelineBuilder*) data;

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
            g_print ("GST_MESSAGE_STATE_CHANGED");
            break;
        case GST_MESSAGE_EOS:
            g_print ("GST_MESSAGE_EOS");
            gst_element_set_state (tempBuilder->mPipeline.bin, GST_STATE_READY);
            gst_element_seek_simple(tempBuilder->mPipeline.bin, GST_FORMAT_TIME,
                                    GST_SEEK_FLAG_FLUSH, 0);
          break;
        default:
          break;
    }
    return TRUE;
}


void RecordedVideoPipelineBuilder::linkElements(){
    if( !gst_element_link(mPipeline.source, mPipeline.demuxer) ){
        g_printerr ("Source and Demuxer could not be linked.\n");
    }

    if( !gst_element_link(mPipeline.demuxer, mPipeline.videoqueue) ){
        g_printerr ("Demuxer and Videoqueue could not be linked.\n");
    }

    if( !gst_element_link(mPipeline.videoqueue, mPipeline.videodecoder) ){
        g_printerr ("Videoqueue and Videodecoder could not be linked.\n");
    }

    if(!gst_element_link(mPipeline.videodecoder, mPipeline.videosink) ){
        g_printerr ("Videodecoder and Videosink could not be linked.\n");
    }

    if( !gst_element_link(mPipeline.demuxer, mPipeline.audioqueue) ){
        g_printerr ("Demuxer and Audioqueue could not be linked.\n");
    }

    if( !gst_element_link(mPipeline.audioqueue, mPipeline.audiodecoder) ){
        g_printerr ("Audioqueue and Audiodecoder could not be linked.\n");
    }

    if( !gst_element_link(mPipeline.audiodecoder, mPipeline.audiosink) ){
        g_printerr ("Audiodecoder and Audiosink could not be linked.\n");
    }
    g_signal_connect(mPipeline.demuxer, "pad-added", G_CALLBACK (onPadAddedForRecordedVideo), mPipeline.videoqueue);
    g_signal_connect(mPipeline.videodecoder, "pad-added", G_CALLBACK (onPadAddedForRecordedVideo), mPipeline.videosink);
    g_signal_connect(mPipeline.demuxer, "pad-added", G_CALLBACK (onPadAddedForRecordedVideo), mPipeline.audioqueue);

}


void RecordedVideoPipelineBuilder::buildPipeline(std::string pResourcePath, long long int pWindowID){
    this->setBin(std::string("pipeline"));
    this->setSource(std::string("filesrc"), std::string("source"));
    this->setDemuxer(std::string("qtdemux"), std::string("demuxer"));
    this->setVideoQueue(std::string("queue"), std::string("videoqueue"));
    this->setVideoDecoder(std::string("decodebin"), std::string("videodecoder"));
    this->setVideoSink(std::string("glimagesink"), std::string("videosink"));
    this->setAudioQueue(std::string("queue"), std::string("audioqueue"));
    this->setAudioDecoder(std::string("faad"), std::string("audiodecoder"));
    this->setAudioSink(std::string("autoaudiosink"), std::string("audiosink"));

    this->setLiveness(false);
    this->setPropertiesOfGstElement(pResourcePath, pWindowID);
    this->addElements();
    this->linkElements();
    this->setBus();
    this->setState(GstState::GST_STATE_READY);

}

void RecordedVideoPipelineBuilder::setBus(){
    mPipeline.bus = gst_pipeline_get_bus (GST_PIPELINE (mPipeline.bin));
    gst_bus_add_watch (mPipeline.bus, (GstBusFunc) getMessageFromBusForRecordedVideo, this);
}

void RecordedVideoPipelineBuilder::destroyPipeline(){
    g_object_unref(mPipeline.bus);
    gst_element_set_state (mPipeline.bin, GST_STATE_NULL);
    g_object_unref (mPipeline.bin);

}



