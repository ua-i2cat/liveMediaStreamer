#ifndef _QUEUE_SOURCE_HH
#define _QUEUE_SOURCE_HH

#ifndef _LIVEMEDIA_HH
#include <liveMedia/liveMedia.hh>
#endif

#ifndef _FRAME_QUEUE_HH
#include "../FrameQueue.hh"
#endif

#ifndef _IO_INTERFACE_HH
#include "../IOInterface.hh"
#endif

#ifndef _FRAME_HH
#include "../Frame.hh"
#endif

class QueueSource: public FramedSource {

public:
    static QueueSource* createNew(UsageEnvironment& env, Reader *reader);
    virtual void doGetNextFrame();
    virtual void doStopGettingFrames();
    Reader* getReader() {return fReader;};

protected:
    QueueSource(UsageEnvironment& env, Reader *reader);
        // called only by createNew()
    static void staticDoGetNextFrame(FramedSource* source);

protected:
    Frame* frame;
    Reader *fReader;
};

#endif
