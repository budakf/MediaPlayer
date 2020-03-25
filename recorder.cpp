#include "recorder.h"

//GstElement* bin;
//GstElement* source;
//GstElement* videoQueue;
//GstElement* videoDepayloader;
//GstElement* h264Parser;
//GstElement* audioQueue;
//GstElement* audioDepayloader;
//GstElement* aacParser;
//GstElement* muxer;
//GstElement* sink;

Recorder::Recorder(QObject *parent) : QObject(parent){
    gst_init(NULL, NULL);
    mRecorderPipeline = new RecorderPipeline;
}

void Recorder::setPropertiesOfGstElement(std::string pUrl, std::string pFileLocation){
    g_object_set(mRecorderPipeline->source , "location", pUrl.data(), NULL );
    g_object_set(mRecorderPipeline->sink , "location", pFileLocation.data(), NULL );
}

void Recorder::addElements(){
    if(!mRecorderPipeline->bin || !mRecorderPipeline->source || !mRecorderPipeline->videoQueue || !mRecorderPipeline->videoDepayloader || !mRecorderPipeline->h264Parser ||
       !mRecorderPipeline->audioQueue || !mRecorderPipeline->audioDepayloader || !mRecorderPipeline->aacParser || !mRecorderPipeline->muxer || !mRecorderPipeline->sink ){
        g_printerr("All element could not be created");
    }
    gst_bin_add_many( GST_BIN (mRecorderPipeline->bin), mRecorderPipeline->source, mRecorderPipeline->videoQueue, mRecorderPipeline->videoDepayloader, mRecorderPipeline->h264Parser,
                      mRecorderPipeline->audioQueue, mRecorderPipeline->audioDepayloader, mRecorderPipeline->aacParser, mRecorderPipeline->muxer, mRecorderPipeline->sink, NULL);
}

void onPadAddedForRecorder(GstElement *src, GstPad *newPad, gpointer sink){
    GstPad *sinkPad = gst_element_get_static_pad ( (GstElement *)sink, "sink");
    GstPadLinkReturn ret;

    g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (newPad), GST_ELEMENT_NAME (src));

    if (gst_pad_is_linked (sinkPad)) {
      g_print ("We are already linked. Ignoring.\n");
      return;
    }

    ret = gst_pad_link (newPad, sinkPad);
    if ( GST_PAD_LINK_FAILED (ret) ){
        g_print ("Link failed.\n");
    }
    else{
        g_print ("Link succeeded.\n");
    }

    gst_object_unref (sinkPad);

}

void Recorder::linkElements(){
    g_signal_connect(mRecorderPipeline->source, "pad-added", G_CALLBACK (onPadAddedForRecorder), mRecorderPipeline->videoQueue);

    if(!gst_element_link(mRecorderPipeline->videoQueue, mRecorderPipeline->videoDepayloader)){
        g_printerr("videoQueue and videoDepayloader could not linked");
    }
    if(!gst_element_link(mRecorderPipeline->videoDepayloader, mRecorderPipeline->h264Parser)){
        g_printerr("videoDepayloader and h264parser could not linked");
    }
    if(!gst_element_link(mRecorderPipeline->h264Parser, mRecorderPipeline->muxer)){
        g_printerr("videoDepayloader and muxer could not linked");
    }


    g_signal_connect(mRecorderPipeline->source, "pad-added", G_CALLBACK (onPadAddedForRecorder), mRecorderPipeline->audioQueue);

    if(!gst_element_link(mRecorderPipeline->audioQueue, mRecorderPipeline->audioDepayloader)){
        g_printerr("audioQueue and audioDepayloader could not linked");
    }
    if(!gst_element_link(mRecorderPipeline->audioDepayloader, mRecorderPipeline->aacParser)){
        g_printerr("audioDepayloader and aacParser could not linked");
    }
    if(!gst_element_link(mRecorderPipeline->aacParser, mRecorderPipeline->muxer)){
        g_printerr("aacParser and muxer could not linked");
    }


    if(!gst_element_link(mRecorderPipeline->muxer, mRecorderPipeline->sink)){
        g_printerr("videoDepayloader and sink could not linked");
    }
}

gboolean getMessageFromBusForRecorder(GstBus * bus, GstMessage * message, gpointer data){
    //g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message) );

//    Recorder* tempBuilder = static_cast<Recorder*>(data);

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
//            g_print ("GST_MESSAGE_STATE_CHANGED\n");
            break;
        case GST_MESSAGE_EOS:
            g_print ("GST_MESSAGE_EOS\n");
          break;
        default:
          break;
    }

    return TRUE;
}


void Recorder::setBus(){
    mRecorderPipeline->bus = gst_pipeline_get_bus (GST_PIPELINE (mRecorderPipeline->bin));
    //gst_bus_add_watch (mPipeline.bus, getMessageFromBusForRecordedVideo, this);
    gst_bus_set_sync_handler(mRecorderPipeline->bus, (GstBusSyncHandler)getMessageFromBusForRecorder, this, NULL);

}


void Recorder::destroyPipeline(){
    g_object_unref(mRecorderPipeline->bus);
    gst_element_set_state(mRecorderPipeline->bin, GST_STATE_NULL);
    g_object_unref(mRecorderPipeline->bin);
}


Recorder::~Recorder(){
    destroyPipeline();
}


void Recorder::buildPipeline(std::string pUrl, std::string pFileLocation){
    mRecorderPipeline->bin = gst_pipeline_new("pipeline");
    mRecorderPipeline->source = gst_element_factory_make("rtspsrc","source");
    mRecorderPipeline->videoQueue = gst_element_factory_make("queue","videoQueue");
    mRecorderPipeline->videoDepayloader = gst_element_factory_make("rtph264depay", "videoDepayloader");
    mRecorderPipeline->h264Parser = gst_element_factory_make("h264parse","h264Parser");
    mRecorderPipeline->audioQueue = gst_element_factory_make("queue","audioQueue");
    mRecorderPipeline->audioDepayloader = gst_element_factory_make("rtpmp4gdepay", "audioDepayloader");
    mRecorderPipeline->aacParser = gst_element_factory_make("aacparse","aacParser");
    mRecorderPipeline->muxer = gst_element_factory_make("mp4mux","muxer");
    mRecorderPipeline->sink = gst_element_factory_make("filesink", "sink");

    this->addElements();
    this->linkElements();
    this->setPropertiesOfGstElement(pUrl, pFileLocation);
    this->setBus();
}


void Recorder::startRecord(){
    gst_element_set_state(mRecorderPipeline->bin, GST_STATE_PLAYING);
}


gpointer pushEosThread(gpointer pPipeline){

    RecorderPipeline* pipeline = static_cast<RecorderPipeline*>(pPipeline);

//    g_object_set (pipeline->bin, "message-forward", TRUE, NULL);
    GstPad* audioPad = gst_element_get_static_pad(pipeline->muxer,"audio_0");
    gst_pad_send_event(audioPad , gst_event_new_eos ());

    GstPad* videoPad = gst_element_get_static_pad(pipeline->muxer,"video_0");
    gst_pad_send_event(videoPad , gst_event_new_eos ());

    gst_element_set_state(pipeline->bin, GST_STATE_NULL);

    gst_object_unref (audioPad);
    gst_object_unref (videoPad);
    return NULL;
}

void Recorder::stopRecord(){
    g_thread_new("push-eos-thread", pushEosThread, this->mRecorderPipeline);
}
