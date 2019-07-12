#include "RecognitionTestModelTests.hpp"
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests::recognition_test {
    class TestMethodStub : public TestMethod {
        bool complete()  {return {};}
        std::string next()  {return {};}
        std::string current()  {return {};}
        int snr_dB()  {return {};}
        void submitCorrectResponse()  {}
        void submitIncorrectResponse()  {}
        void submitResponse(const FreeResponse &)  {}
        void writeTestingParameters(OutputFile *file) {
            file->writeTest(AdaptiveTest{});
        }
        void writeLastCoordinateResponse(OutputFile *)  {}
        void submitResponse(const coordinate_response_measure::SubjectResponse &)  {}
    };
    
    class InitializingTest : public UseCase {
        TestInformation information{};
        CommonTest common{};
        TestMethod *method;
    public:
        explicit InitializingTest(TestMethod *method) :
            method{method} {}
        
        void run(RecognitionTestModel &) override {
        
        }
        
        void run(RecognitionTestModel_Internal &m) override {
            m.initialize(method, common, information);
        }
    };
    
    class RecognitionTestModelTests3 : public ::testing::Test {
    protected:
        TargetPlayerStub targetPlayer{};
        MaskerPlayerStub maskerPlayer{};
        ResponseEvaluatorStub evaluator{};
        OutputFileStub outputFile{};
        RandomizerStub randomizer{};
        RecognitionTestModel_Internal model{
            &targetPlayer,
            &maskerPlayer,
            &evaluator,
            &outputFile,
            &randomizer
        };
        TestMethodStub testMethod;
        PlayingCalibration playingCalibration{};
        InitializingTest initializingTest{&testMethod};
        
        void run(UseCase &useCase) {
            useCase.run(model);
        }
        
        void assertTargetVideoOnlyHidden() {
            assertTrue(targetPlayerVideoHidden());
            assertTargetVideoNotShown();
        }
        
        bool targetPlayerVideoHidden() {
            return targetPlayer.videoHidden();
        }
        
        void assertTargetVideoNotShown() {
            assertFalse(targetPlayerVideoShown());
        }
        
        bool targetPlayerVideoShown() {
            return targetPlayer.videoShown();
        }
        
        void assertTargetVideoHiddenWhenAuditoryOnly(ConditionUseCase &useCase) {
            useCase.setAuditoryOnly();
            run(useCase);
            assertTargetVideoOnlyHidden();
        }
        
        void assertTargetVideoShownWhenAudioVisual(ConditionUseCase &useCase) {
            useCase.setAudioVisual();
            run(useCase);
            assertTargetVideoOnlyShown();
        }
        
        void assertTargetVideoOnlyShown() {
            assertTargetVideoNotHidden();
            assertTrue(targetPlayerVideoShown());
        }
        
        void assertTargetVideoNotHidden() {
            assertFalse(targetPlayerVideoHidden());
        }
        
        void assertClosesOutputFileOpensAndWritesTestInOrder(UseCase &useCase) {
            run(useCase);
            assertOutputFileLog("close openNewFile writeTest ");
        }
        
        auto &outputFileLog() {
            return outputFile.log();
        }
        
        void assertOutputFileLog(std::string s) {
            assertEqual(std::move(s), outputFileLog());
        }
    };
    
    TEST_F(RecognitionTestModelTests3, subscribesToPlayerEvents) {
        assertEqual(
            static_cast<TargetPlayer::EventListener *>(&model),
            targetPlayer.listener()
        );
        assertEqual(
            static_cast<MaskerPlayer::EventListener *>(&model),
            maskerPlayer.listener()
        );
    }
    
    TEST_F(
        RecognitionTestModelTests3,
        playCalibrationHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(playingCalibration);
    }
    
    TEST_F(
        RecognitionTestModelTests3,
        playCalibrationShowsTargetVideoWhenAudioVisual
    ) {
        assertTargetVideoShownWhenAudioVisual(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests3,
        initializeTestClosesOutputFileOpensAndWritesTestInOrder
    ) {
        assertClosesOutputFileOpensAndWritesTestInOrder(initializingTest);
    }
}
