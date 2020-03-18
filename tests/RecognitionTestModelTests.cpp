#include "LogString.hpp"
#include "MaskerPlayerStub.hpp"
#include "ModelEventListenerStub.hpp"
#include "OutputFileStub.hpp"
#include "RandomizerStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "TargetPlayerStub.hpp"
#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace av_speech_in_noise {
namespace {
void insert(LogString &log, const std::string &s) { log.insert(s); }

class TestMethodStub : public TestMethod {
  public:
    auto submittedCorrectResponse() const -> bool {
        return submittedCorrectResponse_;
    }

    auto submittedIncorrectResponse() const -> bool {
        return submittedIncorrectResponse_;
    }

    auto submittedFreeResponse() const -> bool {
        return submittedFreeResponse_;
    }

    auto submittedCorrectKeywords() const -> bool {
        return submittedCorrectKeywords_;
    }

    void setSnr_dB(int x) { snr_dB_ = x; }

    auto snr_dB() -> int override { return snr_dB_; }

    void setNextTarget(std::string s) { nextTarget_ = std::move(s); }

    auto nextTarget() -> std::string override {
        insert(log_, "next ");
        currentTarget_ = currentTargetWhenNext_;
        return nextTarget_;
    }

    void setComplete() { complete_ = true; }

    auto complete() -> bool override { return complete_; }

    auto currentTarget() -> std::string override { return currentTarget_; }

    void setCurrentTarget(std::string s) { currentTarget_ = std::move(s); }

    void setCurrentTargetWhenNextTarget(std::string s) {
        currentTargetWhenNext_ = std::move(s);
    }

    void submitCorrectResponse() override {
        insert(log_, "submitCorrectResponse ");
        submittedCorrectResponse_ = true;
    }

    void submitIncorrectResponse() override {
        insert(log_, "submitIncorrectResponse ");
        submittedIncorrectResponse_ = true;
    }

    void submit(const open_set::FreeResponse &) override {
        insert(log_, "submitFreeResponse ");
        submittedFreeResponse_ = true;
    }

    void submit(const open_set::CorrectKeywords &) override {
        insert(log_, "submitCorrectKeywords ");
        submittedCorrectKeywords_ = true;
    }

    void submit(const coordinate_response_measure::Response &) override {
        insert(log_, "submitCoordinateResponse ");
    }

    void writeTestingParameters(OutputFile *file) override {
        insert(log_, "writeTestingParameters ");
        file->writeTest(AdaptiveTest{});
    }

    void writeLastCoordinateResponse(OutputFile *) override {
        insert(log_, "writeLastCoordinateResponse ");
    }

    void writeLastCorrectResponse(OutputFile *) override {
        insert(log_, "writeLastCorrectResponse ");
    }

    void writeLastIncorrectResponse(OutputFile *) override {
        insert(log_, "writeLastIncorrectResponse ");
    }

    void writeLastCorrectKeywords(OutputFile *) override {
        insert(log_, "writeLastCorrectKeywords ");
    }

    void writeTestResult(OutputFile *) override {
        insert(log_, "writeTestResult ");
    }

    auto log() const -> const LogString & { return log_; }

  private:
    LogString log_{};
    std::string currentTarget_{};
    std::string currentTargetWhenNext_{};
    std::string nextTarget_{};
    int snr_dB_{};
    bool complete_{};
    bool submittedCorrectResponse_{};
    bool submittedIncorrectResponse_{};
    bool submittedFreeResponse_{};
    bool submittedCorrectKeywords_{};
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(RecognitionTestModelImpl &) = 0;
};

class TargetWritingUseCase : public virtual UseCase {
  public:
    virtual auto target(OutputFileStub &) -> std::string = 0;
};

class SubmittingResponse : public virtual UseCase {};

class InitializingTest : public UseCase {
  public:
    explicit InitializingTest(TestMethod *method, const Test &test)
        : test{test}, method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initialize(method, test);
    }

  private:
    const Test &test{};
    TestMethod *method;
};

class InitializingTestWithSingleSpeaker : public UseCase {
  public:
    explicit InitializingTestWithSingleSpeaker(TestMethod *method)
        : method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initializeWithSingleSpeaker(method, {});
    }

  private:
    TestMethod *method;
};

class InitializingTestWithDelayedMasker : public UseCase {
  public:
    explicit InitializingTestWithDelayedMasker(TestMethod *method)
        : method{method} {}

    void run(RecognitionTestModelImpl &m) override {
        m.initializeWithDelayedMasker(method, {});
    }

  private:
    TestMethod *method;
};

class AudioDeviceUseCase : public virtual UseCase {
  public:
    virtual void setAudioDevice(std::string) = 0;
};

class ConditionUseCase : public virtual UseCase {
  public:
    virtual void setAuditoryOnly() = 0;
    virtual void setAudioVisual() = 0;
};

class PlayingCalibration : public AudioDeviceUseCase {
  public:
    explicit PlayingCalibration(Calibration &calibration)
        : calibration{calibration} {}

    void setAudioDevice(std::string s) override {
        calibration.audioSettings.audioDevice = std::move(s);
    }

    void run(RecognitionTestModelImpl &m) override {
        m.playCalibration(calibration);
    }

  private:
    Calibration &calibration;
};

class PlayingTrial : public AudioDeviceUseCase {
  public:
    void setAudioDevice(std::string s) override {
        trial.audioDevice = std::move(s);
    }

    void run(RecognitionTestModelImpl &m) override { m.playTrial(trial); }

  private:
    AudioSettings trial;
};

class SubmittingFreeResponse : public SubmittingResponse,
                               public TargetWritingUseCase {
  public:
    explicit SubmittingFreeResponse(const open_set::FreeResponse &response = {})
        : response{response} {}

    void run(RecognitionTestModelImpl &m) override { m.submit(response); }

    auto target(OutputFileStub &file) -> std::string override {
        return file.freeResponseTrial().target;
    }

  private:
    const open_set::FreeResponse &response;
};

class SubmittingCorrectKeywords : public SubmittingResponse,
                                  public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submit(open_set::CorrectKeywords{});
    }

    auto target(OutputFileStub &file) -> std::string override {
        return file.correctKeywordsTrial().target;
    }
};

class SubmittingCoordinateResponse : public SubmittingResponse {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submit(coordinate_response_measure::Response{});
    }
};

auto openSetAdaptiveTarget(OutputFileStub &file) -> std::string {
    return file.openSetAdaptiveTrial().target;
}

class SubmittingCorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitCorrectResponse();
    }

    auto target(OutputFileStub &file) -> std::string override {
        return openSetAdaptiveTarget(file);
    }
};

class SubmittingIncorrectResponse : public TargetWritingUseCase {
  public:
    void run(RecognitionTestModelImpl &m) override {
        m.submitIncorrectResponse();
    }

    auto target(OutputFileStub &file) -> std::string override {
        return openSetAdaptiveTarget(file);
    }
};

auto dB(double x) -> double { return 20 * std::log10(x); }

void setCurrentTarget(TestMethodStub &m, std::string s) {
    m.setCurrentTarget(std::move(s));
}

auto targetFileName(RecognitionTestModelImpl &m) -> std::string {
    return m.targetFileName();
}

auto filePathForFileName(ResponseEvaluatorStub &r) -> std::string {
    return r.filePathForFileName();
}

class RecognitionTestModelTests : public ::testing::Test {
  protected:
    ModelEventListenerStub listener;
    TargetPlayerStub targetPlayer{};
    MaskerPlayerStub maskerPlayer{};
    ResponseEvaluatorStub evaluator{};
    OutputFileStub outputFile{};
    RandomizerStub randomizer{};
    RecognitionTestModelImpl model{
        &targetPlayer, &maskerPlayer, &evaluator, &outputFile, &randomizer};
    TestMethodStub testMethod;
    Calibration calibration;
    PlayingCalibration playingCalibration{calibration};
    av_speech_in_noise::Test test{};
    InitializingTest initializingTest{&testMethod, test};
    InitializingTestWithSingleSpeaker initializingTestWithSingleSpeaker{
        &testMethod};
    InitializingTestWithDelayedMasker initializingTestWithDelayedMasker{
        &testMethod};
    PlayingTrial playingTrial;
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingCorrectResponse submittingCorrectResponse;
    SubmittingIncorrectResponse submittingIncorrectResponse;
    open_set::FreeResponse freeResponse{};
    SubmittingFreeResponse submittingFreeResponse{freeResponse};
    SubmittingCorrectKeywords submittingCorrectKeywords;

    RecognitionTestModelTests() { model.subscribe(&listener); }

    void run(UseCase &useCase) { useCase.run(model); }

    void assertTargetVideoOnlyHidden() {
        assertTrue(targetPlayerVideoHidden());
        assertTargetVideoNotShown();
    }

    auto targetPlayerVideoHidden() -> bool {
        return targetPlayer.videoHidden();
    }

    void assertTargetVideoNotShown() { assertFalse(targetPlayerVideoShown()); }

    auto targetPlayerVideoShown() -> bool { return targetPlayer.videoShown(); }

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

    auto outputFileLog() -> auto & { return outputFile.log(); }

    void assertOutputFileLog(const std::string &s) {
        assertEqual(s, outputFileLog());
    }

    template <typename T>
    void assertDevicePassedToPlayer(
        const T &player, AudioDeviceUseCase &useCase) {
        useCase.setAudioDevice("a");
        run(useCase);
        assertEqual("a", player.device());
    }

    void assertDevicePassedToTargetPlayer(AudioDeviceUseCase &useCase) {
        assertDevicePassedToPlayer(targetPlayer, useCase);
    }

    void assertDevicePassedToMaskerPlayer(AudioDeviceUseCase &useCase) {
        assertDevicePassedToPlayer(maskerPlayer, useCase);
    }

    auto maskerPlayerFadedIn() -> bool { return maskerPlayer.fadeInCalled(); }

    void assertTargetPlayerPlayed() { assertTrue(targetPlayerPlayed()); }

    auto targetPlayerPlayed() -> bool { return targetPlayer.played(); }

    auto targetFilePath() { return targetPlayer.filePath(); }

    void assertTargetFilePathEquals(const std::string &what) {
        assertEqual(what, targetFilePath());
    }

    void assertPassesNextTargetToPlayer(UseCase &useCase) {
        testMethod.setNextTarget("a");
        run(useCase);
        assertTargetFilePathEquals("a");
    }

    void assertTargetPlayerPlaybackCompletionSubscribed(UseCase &useCase) {
        run(useCase);
        assertTrue(targetPlayerPlaybackCompletionSubscribed());
    }

    auto targetPlayerPlaybackCompletionSubscribed() -> bool {
        return targetPlayer.playbackCompletionSubscribedTo();
    }

    void assertSeeksToRandomMaskerPositionWithinTrialDuration(
        UseCase &useCase) {
        targetPlayer.setDurationSeconds(1);
        maskerPlayer.setFadeTimeSeconds(2);
        maskerPlayer.setDurationSeconds(3);
        run(useCase);
        assertEqual(0., randomizer.lowerFloatBound());
        assertEqual(3. - 2 - 1 - 2, randomizer.upperFloatBound());
    }

    auto maskerPlayerSecondsSeeked() { return maskerPlayer.secondsSeeked(); }

    void assertMaskerPlayerSeekedToRandomTime(UseCase &useCase) {
        randomizer.setRandomFloat(1);
        run(useCase);
        assertEqual(1., maskerPlayerSecondsSeeked());
    }

    auto targetPlayerLevel_dB() { return targetPlayer.level_dB(); }

    void assertTargetPlayerLevelEquals_dB(double x) {
        assertEqual(x, targetPlayerLevel_dB());
    }

    void setMaskerLevel_dB_SPL(int x) { test.maskerLevel_dB_SPL = x; }

    void setTestingFullScaleLevel_dB_SPL(int x) {
        test.fullScaleLevel_dB_SPL = x;
    }

    void setMaskerRms(double x) { maskerPlayer.setRms(x); }

    void setSnr_dB(int x) { testMethod.setSnr_dB(x); }

    void maskerFadeOutComplete() { maskerPlayer.fadeOutComplete(); }

    void assertSavesOutputFileAfterWritingTrial(UseCase &useCase) {
        run(useCase);
        assertTrue(outputFileLog().endsWith("save "));
    }

    void assertCallThrowsRequestFailure(
        UseCase &useCase, const std::string &what) {
        try {
            run(useCase);
            FAIL() << "Expected recognition_test::"
                      "ModelImpl::"
                      "RequestFailure";
        } catch (const ModelImpl::RequestFailure &e) {
            assertEqual(what, e.what());
        }
    }

    void setMaskerFilePath(std::string s) {
        test.maskerFilePath = std::move(s);
    }

    void assertThrowsRequestFailureWhenInvalidAudioDevice(
        AudioDeviceUseCase &useCase) {
        throwInvalidAudioDeviceWhenSet();
        useCase.setAudioDevice("a");
        assertCallThrowsRequestFailure(
            useCase, "'a' is not a valid audio device.");
    }

    void throwInvalidAudioDeviceWhenSet() {
        maskerPlayer.throwInvalidAudioDeviceWhenDeviceSet();
        targetPlayer.throwInvalidAudioDeviceWhenDeviceSet();
    }

    void runIgnoringFailureWithTrialInProgress(UseCase &useCase) {
        setTrialInProgress();
        runIgnoringFailure(useCase);
    }

    void setTrialInProgress() { maskerPlayer.setPlaying(); }

    void runIgnoringFailure(UseCase &useCase) {
        try {
            run(useCase);
        } catch (const ModelImpl::RequestFailure &) {
        }
    }

    void assertMaskerPlayerNotPlayed() { assertFalse(maskerPlayerFadedIn()); }

    void assertTargetPlayerNotPlayed() { assertFalse(targetPlayerPlayed()); }

    void assertThrowsRequestFailureWhenTrialInProgress(UseCase &useCase) {
        setTrialInProgress();
        assertCallThrowsRequestFailure(useCase, "Trial in progress.");
    }

    void assertTestIncomplete() { assertFalse(testComplete()); }

    auto testComplete() -> bool { return model.testComplete(); }

    void assertTestComplete() { assertTrue(testComplete()); }

    void assertSetsTargetLevel(UseCase &useCase) {
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        run(initializingTest);
        setMaskerRms(5);
        setSnr_dB(2);
        run(useCase);
        assertTargetPlayerLevelEquals_dB(2 + 3 - 4 - dB(5));
    }

    auto writtenFreeResponseTrial() { return outputFile.freeResponseTrial(); }

    void assertResponseDoesNotLoadNextTargetWhenComplete(UseCase &useCase) {
        testMethod.setNextTarget("a");
        run(initializingTest);
        testMethod.setComplete();
        testMethod.setNextTarget("b");
        run(useCase);
        assertTargetFilePathEquals("a");
    }

    void assertWritesTarget(TargetWritingUseCase &useCase) {
        evaluator.setFileName("a");
        run(useCase);
        assertEqual("a", useCase.target(outputFile));
    }

    void assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(
        UseCase &useCase) {
        run(initializingTest);
        setCurrentTarget(testMethod, "a");
        testMethod.setCurrentTargetWhenNextTarget("b");
        run(useCase);
        assertEqual("a", filePathForFileName(evaluator));
    }

    void assertTestMethodLogContains(
        UseCase &useCase, const std::string &what) {
        run(initializingTest);
        run(useCase);
        assertTrue(testMethod.log().contains(what));
    }

    void assertTrialNumber(int n) { assertEqual(n, model.trialNumber()); }

    void assertYieldsTrialNumber(UseCase &useCase, int n) {
        run(useCase);
        assertTrialNumber(n);
    }

    void assertOnlyUsingFirstChannelOfTargetPlayer() {
        assertTrue(targetPlayer.usingFirstChannelOnly());
    }

    void assertUsingAllMaskerPlayerChannels() {
        assertTrue(maskerPlayer.usingAllChannels());
    }

    void assertMaskerPlayerChannelDelaysCleared() {
        assertTrue(maskerPlayer.channelDelaysCleared());
    }
};

#define RECOGNITION_TEST_MODEL_TEST(a) TEST_F(RecognitionTestModelTests, a)

RECOGNITION_TEST_MODEL_TEST(subscribesToPlayerEvents) {
    assertEqual(static_cast<TargetPlayer::EventListener *>(&model),
        targetPlayer.listener());
    assertEqual(static_cast<MaskerPlayer::EventListener *>(&model),
        maskerPlayer.listener());
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationShowsTargetVideo) {
    run(playingCalibration);
    assertTargetVideoOnlyShown();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestClosesOutputFileOpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerClosesOutputFileOpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(
        initializingTestWithSingleSpeaker);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerClosesOutputFileOpensAndWritesTestInOrder) {
    assertClosesOutputFileOpensAndWritesTestInOrder(
        initializingTestWithDelayedMasker);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestUsesAllTargetPlayerChannels) {
    run(initializingTest);
    assertTrue(targetPlayer.usingAllChannels());
}

RECOGNITION_TEST_MODEL_TEST(initializeTestUsesAllMaskerPlayerChannels) {
    run(initializingTest);
    assertUsingAllMaskerPlayerChannels();
}

RECOGNITION_TEST_MODEL_TEST(initializeTestClearsAllMaskerPlayerChannelDelays) {
    run(initializingTest);
    assertMaskerPlayerChannelDelaysCleared();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerUsesFirstChannelOnlyOfTargetPlayer) {
    run(initializingTestWithSingleSpeaker);
    assertOnlyUsingFirstChannelOfTargetPlayer();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerClearsAllMaskerPlayerChannelDelays) {
    run(initializingTestWithSingleSpeaker);
    assertMaskerPlayerChannelDelaysCleared();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithSingleSpeakerUsesFirstChannelOnlyOfMaskerPlayer) {
    run(initializingTestWithSingleSpeaker);
    assertTrue(maskerPlayer.usingFirstChannelOnly());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerUsesFirstChannelOnlyOfTargetPlayer) {
    run(initializingTestWithDelayedMasker);
    assertOnlyUsingFirstChannelOfTargetPlayer();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerUsesAllMaskerPlayerChannels) {
    run(initializingTestWithDelayedMasker);
    assertUsingAllMaskerPlayerChannels();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestWithDelayedMaskerSetsFirstChannelMaskerDelay) {
    run(initializingTestWithDelayedMasker);
    assertEqual(gsl::index{0}, maskerPlayer.channelDelayed());
    assertEqual(RecognitionTestModelImpl::maskerChannelDelaySeconds,
        maskerPlayer.channelDelaySeconds());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestOpensNewOutputFilePassingTestInformation) {
    run(initializingTest);
    assertEqual(
        outputFile.openNewFileParameters(), &std::as_const(test.identity));
}

RECOGNITION_TEST_MODEL_TEST(playTrialPassesAudioDeviceToTargetPlayer) {
    assertDevicePassedToTargetPlayer(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPassesAudioDeviceToTargetPlayer) {
    assertDevicePassedToTargetPlayer(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(playTrialPassesAudioDeviceToMaskerPlayer) {
    assertDevicePassedToMaskerPlayer(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(playTrialFadesInMasker) {
    run(playingTrial);
    assertTrue(maskerPlayerFadedIn());
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPlaysTarget) {
    run(playingCalibration);
    assertTargetPlayerPlayed();
}

RECOGNITION_TEST_MODEL_TEST(fadeInCompletePlaysTarget) {
    maskerPlayer.fadeInComplete();
    assertTargetPlayerPlayed();
}

RECOGNITION_TEST_MODEL_TEST(initializeTestPassesNextTargetToTargetPlayer) {
    assertPassesNextTargetToPlayer(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(returnsTargetFileName) {
    evaluator.setFileName("a");
    assertEqual("a", targetFileName(model));
}

RECOGNITION_TEST_MODEL_TEST(
    passesCurrentToEvaluatorWhenReturningTargetFileName) {
    run(initializingTest);
    setCurrentTarget(testMethod, "a");
    targetFileName(model);
    assertEqual("a", filePathForFileName(evaluator));
}

RECOGNITION_TEST_MODEL_TEST(initializingTestResetsTrialNumber) {
    assertYieldsTrialNumber(initializingTest, 1);
}

RECOGNITION_TEST_MODEL_TEST(submittingCoordinateResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingCoordinateResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingCorrectResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingCorrectResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingIncorrectResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingIncorrectResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingFreeResponseIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingFreeResponse, 2);
}

RECOGNITION_TEST_MODEL_TEST(submittingCorrectKeywordsIncrementsTrialNumber) {
    run(initializingTest);
    assertYieldsTrialNumber(submittingCorrectKeywords, 2);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCorrectKeywordsWritesTestResultWhenComplete) {
    run(initializingTest);
    testMethod.setComplete();
    run(submittingCorrectKeywords);
    assertTrue(testMethod.log().endsWith("writeTestResult "));
    assertTrue(outputFileLog().endsWith("save "));
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCoordinateResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingIncorrectResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingFreeResponsePassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submittingCorrectKeywordsPassesNextTargetToTargetPlayer) {
    run(initializingTest);
    assertPassesNextTargetToPlayer(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationPassesAudioFileToTargetPlayer) {
    calibration.filePath = "a";
    run(playingCalibration);
    assertTargetFilePathEquals("a");
}

RECOGNITION_TEST_MODEL_TEST(initializeTestPassesMaskerFilePathToMaskerPlayer) {
    setMaskerFilePath("a");
    run(initializingTest);
    assertEqual("a", maskerPlayer.filePath());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(
        submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectKeywordsSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseSubscribesToTargetPlaybackCompletionNotification) {
    assertTargetPlayerPlaybackCompletionSubscribed(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponseSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectKeywordsSeeksToRandomMaskerPositionWithinTrialDuration) {
    assertSeeksToRandomMaskerPositionWithinTrialDuration(
        submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectKeywordsSeeksToRandomMaskerPosition) {
    assertMaskerPlayerSeekedToRandomTime(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestSetsInitialMaskerPlayerLevel) {
    setMaskerLevel_dB_SPL(1);
    setTestingFullScaleLevel_dB_SPL(2);
    setMaskerRms(3);
    run(initializingTest);
    assertEqual(1 - 2 - dB(3), maskerPlayer.level_dB());
}

RECOGNITION_TEST_MODEL_TEST(initializeTestSetsTargetPlayerLevel) {
    setSnr_dB(2);
    setMaskerLevel_dB_SPL(3);
    setTestingFullScaleLevel_dB_SPL(4);
    setMaskerRms(5);
    run(initializingTest);
    assertTargetPlayerLevelEquals_dB(2 + 3 - 4 - dB(5));
}

RECOGNITION_TEST_MODEL_TEST(submitCoordinateResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectKeywordsSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(submitIncorrectResponseSetsTargetPlayerLevel) {
    assertSetsTargetLevel(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationSetsTargetPlayerLevel) {
    calibration.level_dB_SPL = 1;
    calibration.fullScaleLevel_dB_SPL = 2;
    targetPlayer.setRms(3);
    run(playingCalibration);
    assertTargetPlayerLevelEquals_dB(1 - 2 - dB(3));
}

RECOGNITION_TEST_MODEL_TEST(startTrialShowsTargetPlayerWhenAudioVisual) {
    test.condition = Condition::audioVisual;
    run(initializingTest);
    run(playingTrial);
    assertTrue(targetPlayerVideoShown());
}

RECOGNITION_TEST_MODEL_TEST(maskerFadeOutCompleteHidesTargetPlayer) {
    maskerFadeOutComplete();
    assertTargetVideoOnlyHidden();
}

RECOGNITION_TEST_MODEL_TEST(startTrialDoesNotShowTargetPlayerWhenAuditoryOnly) {
    test.condition = Condition::auditoryOnly;
    run(initializingTest);
    run(playingTrial);
    assertTargetVideoNotShown();
}

RECOGNITION_TEST_MODEL_TEST(initializeTestHidesTargetPlayer) {
    run(initializingTest);
    assertTargetVideoOnlyHidden();
}

RECOGNITION_TEST_MODEL_TEST(targetPlaybackCompleteFadesOutMasker) {
    targetPlayer.playbackComplete();
    assertTrue(maskerPlayer.fadeOutCalled());
}

RECOGNITION_TEST_MODEL_TEST(fadeOutCompleteNotifiesTrialComplete) {
    maskerFadeOutComplete();
    assertTrue(listener.notified());
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectKeywordsSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseSavesOutputFileAfterWritingTrial) {
    assertSavesOutputFileAfterWritingTrial(submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestThrowsRequestFailureIfFileFailsToOpen) {
    outputFile.throwOnOpen();
    assertCallThrowsRequestFailure(
        initializingTest, "Unable to open output file.");
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile) {
    calibration.filePath = "a";
    targetPlayer.throwInvalidAudioFileOnRms();
    assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestThrowsRequestFailureWhenMaskerPlayerThrowsInvalidAudioFile) {
    setMaskerFilePath("a");
    maskerPlayer.throwInvalidAudioFileOnLoad();
    assertCallThrowsRequestFailure(initializingTest, "unable to read a");
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialWithInvalidAudioDeviceThrowsRequestFailure) {
    assertThrowsRequestFailureWhenInvalidAudioDevice(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationWithInvalidAudioDeviceThrowsRequestFailure) {
    assertThrowsRequestFailureWhenInvalidAudioDevice(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(
    playTrialDoesNotChangeAudioDeviceWhenTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    assertFalse(maskerPlayer.setDeviceCalled());
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationDoesNotChangeAudioDeviceWhenTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertFalse(targetPlayer.setDeviceCalled());
}

RECOGNITION_TEST_MODEL_TEST(playTrialDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingTrial);
    assertMaskerPlayerNotPlayed();
}

RECOGNITION_TEST_MODEL_TEST(playCalibrationDoesNotPlayIfTrialInProgress) {
    runIgnoringFailureWithTrialInProgress(playingCalibration);
    assertTargetPlayerNotPlayed();
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(initializingTest);
}

RECOGNITION_TEST_MODEL_TEST(playTrialThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(playingTrial);
}

RECOGNITION_TEST_MODEL_TEST(
    playCalibrationThrowsRequestFailureIfTrialInProgress) {
    assertThrowsRequestFailureWhenTrialInProgress(playingCalibration);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestDoesNotLoadMaskerIfTrialInProgress) {
    setMaskerFilePath("a");
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertEqual("", maskerPlayer.filePath());
}

RECOGNITION_TEST_MODEL_TEST(
    initializeTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress) {
    test.condition = Condition::auditoryOnly;
    runIgnoringFailureWithTrialInProgress(initializingTest);
    assertTargetVideoNotHidden();
}

RECOGNITION_TEST_MODEL_TEST(audioDevicesReturnsOutputAudioDeviceDescriptions) {
    maskerPlayer.setOutputAudioDeviceDescriptions({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

RECOGNITION_TEST_MODEL_TEST(testCompleteWhenComplete) {
    run(initializingTest);
    assertTestIncomplete();
    testMethod.setComplete();
    assertTestComplete();
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(
        submittingCoordinateResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectKeywordsDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingCorrectKeywords);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(submittingCorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseDoesNotLoadNextTargetWhenComplete) {
    assertResponseDoesNotLoadNextTargetWhenComplete(
        submittingIncorrectResponse);
}

RECOGNITION_TEST_MODEL_TEST(initializeTestDoesNotLoadNextTargetWhenComplete) {
    testMethod.setNextTarget("a");
    testMethod.setComplete();
    run(initializingTest);
    assertTargetFilePathEquals("");
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesResponse) {
    freeResponse.response = "a";
    run(submittingFreeResponse);
    assertEqual("a", writtenFreeResponseTrial().response);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesFlagged) {
    freeResponse.flagged = true;
    run(submittingFreeResponse);
    assertTrue(writtenFreeResponseTrial().flagged);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesWithoutFlag) {
    run(submittingFreeResponse);
    assertFalse(writtenFreeResponseTrial().flagged);
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseWritesTarget) {
    assertWritesTarget(submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitFreeResponsePassesCurrentTargetToEvaluatorBeforeAdvancingTarget) {
    assertPassesCurrentTargetToEvaluatorBeforeAdvancingTarget(
        submittingFreeResponse);
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCoordinateResponse,
        "submitCoordinateResponse writeLastCoordinateResponse ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCorrectResponse,
        "submitCorrectResponse writeLastCorrectResponse ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingIncorrectResponse,
        "submitIncorrectResponse writeLastIncorrectResponse ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectKeywordsWritesTrialAfterSubmittingResponse) {
    assertTestMethodLogContains(submittingCorrectKeywords,
        "submitCorrectKeywords writeLastCorrectKeywords ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCoordinateResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingCoordinateResponse, "writeLastCoordinateResponse next ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitIncorrectResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingIncorrectResponse, "writeLastIncorrectResponse next ");
}

RECOGNITION_TEST_MODEL_TEST(
    submitCorrectResponseQueriesNextTargetAfterWritingResponse) {
    assertTestMethodLogContains(
        submittingCorrectResponse, "writeLastCorrectResponse next ");
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectResponseSubmitsCorrectResponse) {
    run(initializingTest);
    run(submittingCorrectResponse);
    assertTrue(testMethod.submittedCorrectResponse());
}

RECOGNITION_TEST_MODEL_TEST(submitIncorrectResponseSubmitsIncorrectResponse) {
    run(initializingTest);
    run(submittingIncorrectResponse);
    assertTrue(testMethod.submittedIncorrectResponse());
}

RECOGNITION_TEST_MODEL_TEST(submitFreeResponseSubmitsResponse) {
    run(initializingTest);
    run(submittingFreeResponse);
    assertTrue(testMethod.submittedFreeResponse());
}

RECOGNITION_TEST_MODEL_TEST(submitCorrectKeywordsSubmitsResponse) {
    run(initializingTest);
    run(submittingCorrectKeywords);
    assertTrue(testMethod.submittedCorrectKeywords());
}
}
}
