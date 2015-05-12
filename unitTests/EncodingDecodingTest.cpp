/*
 *  EncodingDecodingTest.cpp - EncodingDecodingFunctionalTest class test
 *  Copyright (C) 2015  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors:  David Cassany <david.cassany@i2cat.net>
 *
 */

#include <string>
#include <iostream>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/XmlOutputter.h>

#include "FilterFunctionalMockup.hh"
#include "modules/videoEncoder/VideoEncoderX264.hh"
#include "modules/videoEncoder/VideoEncoderX265.hh"
#include "modules/videoDecoder/VideoDecoderLibav.hh"

class VideoEncoderDecoderFunctionalTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(VideoEncoderDecoderFunctionalTest);
    CPPUNIT_TEST(h264Test);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void h264Test();

    OneToOneVideoScenarioMockup *encodingSce, *decodingSce;
    VideoEncoderX264or5* encoder;
    VideoDecoderLibav* decoder;
    AVFramesReader* reader;
    InterleavedFramesWriter* writer;
    InterleavedFramesWriter* rawWriter;
};

void VideoEncoderDecoderFunctionalTest::setUp()
{
    encoder = new VideoEncoderX264();   
    decoder = new VideoDecoderLibav();
    encodingSce = new OneToOneVideoScenarioMockup(encoder, RAW, YUV420P);
    decodingSce = new OneToOneVideoScenarioMockup(decoder, H264);
    CPPUNIT_ASSERT(encodingSce->connectFilter());
    CPPUNIT_ASSERT(decodingSce->connectFilter());
    reader = new AVFramesReader();
    writer = new InterleavedFramesWriter();
    rawWriter = new InterleavedFramesWriter();
}

void VideoEncoderDecoderFunctionalTest::tearDown()
{
    /*encodingSce->disconnectFilter();
    delete encodingSce;
    delete encoder;
    delete reader;
    delete writer;*/
}

void VideoEncoderDecoderFunctionalTest::h264Test()
{
    InterleavedVideoFrame *frame = NULL;
    InterleavedVideoFrame *midFrame = NULL;
    InterleavedVideoFrame *filteredFrame = NULL;
    bool milestone = false;
    
    CPPUNIT_ASSERT(reader->openFile("testsData/videoVectorTest.h264", H264));
    CPPUNIT_ASSERT(writer->openFile("testsData/videoVectorTest_out.h264"));
    CPPUNIT_ASSERT(rawWriter->openFile("testsData/videoVectorTest_out.raw"));
    
    while((frame = reader->getFrame())!=NULL){
        decodingSce->processFrame(frame, midFrame);
        if (midFrame){
            CPPUNIT_ASSERT(rawWriter->writeInterleavedFrame(midFrame));
            encodingSce->processFrame(midFrame, filteredFrame);
            if (filteredFrame){
                CPPUNIT_ASSERT(filteredFrame->getWidth() == midFrame->getWidth() 
                    && filteredFrame->getHeight() == midFrame->getHeight());
                CPPUNIT_ASSERT(writer->writeInterleavedFrame(filteredFrame));
                milestone = true;
            }
        }
    }
    
    //TODO: totally dump encoder
    
    writer->closeFile();
    rawWriter->closeFile();
    reader->close();
    
    CPPUNIT_ASSERT(milestone);
}

CPPUNIT_TEST_SUITE_REGISTRATION(VideoEncoderDecoderFunctionalTest);

int main(int argc, char* argv[])
{
    std::ofstream xmlout("VideoEncoderDecoderFunctionalTest.xml");
    CPPUNIT_NS::TextTestRunner runner;
    CPPUNIT_NS::XmlOutputter *outputter = new CPPUNIT_NS::XmlOutputter(&runner.result(), xmlout);

    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( "", false );
    outputter->write();

    utils::printMood(runner.result().wasSuccessful());

    return runner.result().wasSuccessful() ? 0 : 1;
}