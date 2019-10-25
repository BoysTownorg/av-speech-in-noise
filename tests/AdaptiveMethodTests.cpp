#include "OutputFileStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
#include "TargetListSetReaderStub.h"
#include "TargetListStub.h"
#include "TrackStub.h"
#include "assert-utility.h"
#include <gtest/gtest.h>
#include <recognition-test/AdaptiveMethod.hpp>

namespace av_speech_in_noise::tests {
namespace {
class TrackSettingsReaderStub : public ITrackSettingsReader {
    const TrackingRule *rule_{};
    std::string filePath_{};

  public:
    auto filePath() const { return filePath_; }

    const TrackingRule *read(std::string s) override {
        filePath_ = std::move(s);
        return rule_;
    }

    void setTrackingRule(const TrackingRule *r) { rule_ = r; }
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(AdaptiveMethod &) = 0;
};

class Initializing : public UseCase {
    AdaptiveTest test_{};

  public:
    void run(AdaptiveMethod &method) override { method.initialize(test_); }
};

class SubmittingCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response_{};

  public:
    void run(AdaptiveMethod &method) override {
        method.submitResponse(response_);
    }
};

class SubmittingCorrectCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response_{};
    ResponseEvaluatorStub &evaluator;

  public:
    explicit SubmittingCorrectCoordinateResponse(
        ResponseEvaluatorStub &evaluator)
        : evaluator{evaluator} {}

    void run(AdaptiveMethod &method) override {
        evaluator.setCorrect();
        method.submitResponse(response_);
    }
};

class SubmittingIncorrectCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response_{};
    ResponseEvaluatorStub &evaluator;

  public:
    explicit SubmittingIncorrectCoordinateResponse(
        ResponseEvaluatorStub &evaluator)
        : evaluator{evaluator} {}

    void run(AdaptiveMethod &method) override {
        evaluator.setIncorrect();
        method.submitResponse(response_);
    }
};

class SubmittingCorrectResponse : public UseCase {
  public:
    void run(AdaptiveMethod &method) override {
        method.submitCorrectResponse();
    }
};

class SubmittingIncorrectResponse : public UseCase {
  public:
    void run(AdaptiveMethod &method) override {
        method.submitIncorrectResponse();
    }
};

class WritingResponseUseCase : public virtual UseCase {
  public:
    virtual int writtenReversals(OutputFileStub &) = 0;
    virtual int writtenSnr(OutputFileStub &) = 0;
};

class WritingTargetUseCase : public virtual UseCase {
  public:
    virtual std::string writtenTarget(OutputFileStub &) = 0;
};

class WritingCoordinateResponse : public WritingResponseUseCase {
    coordinate_response_measure::Response response_{};
    OutputFile &file_;

  public:
    explicit WritingCoordinateResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethod &method) override {
        method.submitResponse(response_);
        method.writeLastCoordinateResponse(&file_);
    }

    int writtenReversals(OutputFileStub &file) override {
        return file.writtenAdaptiveCoordinateResponseTrial().reversals;
    }

    int writtenSnr(OutputFileStub &file) override {
        return file.writtenAdaptiveCoordinateResponseTrial().SNR_dB;
    }
};

class WritingCorrectResponse : public WritingResponseUseCase,
                               public WritingTargetUseCase {
    OutputFile &file_;

  public:
    explicit WritingCorrectResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethod &method) override {
        method.submitCorrectResponse();
        method.writeLastCorrectResponse(&file_);
    }

    int writtenReversals(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().reversals;
    }

    int writtenSnr(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().SNR_dB;
    }

    std::string writtenTarget(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class WritingIncorrectResponse : public WritingResponseUseCase,
                                 public WritingTargetUseCase {
    OutputFile &file_;

  public:
    explicit WritingIncorrectResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethod &method) override {
        method.submitIncorrectResponse();
        method.writeLastIncorrectResponse(&file_);
    }

    int writtenReversals(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().reversals;
    }

    int writtenSnr(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().SNR_dB;
    }

    std::string writtenTarget(OutputFileStub &file) override {
        return file.writtenOpenSetAdaptiveTrial().target;
    }
};

class AdaptiveMethodTests : public ::testing::Test {
  protected:
    TargetListSetReaderStub targetListSetReader;
    TrackSettingsReaderStub trackSettingsReader;
    TrackFactoryStub snrTrackFactory;
    ResponseEvaluatorStub evaluator;
    RandomizerStub randomizer;
    OutputFileStub outputFile;
    AdaptiveMethod method{&targetListSetReader, &trackSettingsReader,
        &snrTrackFactory, &evaluator, &randomizer};
    Initializing initializing;
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingCorrectCoordinateResponse submittingCorrectCoordinateResponse{
        evaluator};
    SubmittingIncorrectCoordinateResponse submittingIncorrectCoordinateResponse{
        evaluator};
    SubmittingCorrectResponse submittingCorrectResponse;
    SubmittingIncorrectResponse submittingIncorrectResponse;
    WritingCoordinateResponse writingCoordinateResponse{outputFile};
    WritingCorrectResponse writingCorrectResponse{outputFile};
    WritingIncorrectResponse writingIncorrectResponse{outputFile};
    AdaptiveTest test;
    coordinate_response_measure::Response coordinateResponse;
    TrackingRule targetLevelRule_;
    std::vector<std::shared_ptr<TargetListStub>> lists;
    std::vector<std::shared_ptr<TrackStub>> tracks;

    AdaptiveMethodTests() {
        trackSettingsReader.setTrackingRule(&targetLevelRule_);
        for (int i = 0; i < 3; ++i) {
            lists.push_back(std::make_shared<TargetListStub>());
            tracks.push_back(std::make_shared<TrackStub>());
        }
        targetListSetReader.setTargetLists({lists.begin(), lists.end()});
        snrTrackFactory.setTracks({tracks.begin(), tracks.end()});
    }

  public:
    auto snrTrackFactoryParameters() const {
        return snrTrackFactory.parameters();
    }

    auto snrTrackFactoryParameters(int x) const {
        return snrTrackFactoryParameters().at(x);
    }

    void initialize() { method.initialize(test); }

    void assertPassedTargetLevelRule(const Track::Settings &s) {
        assertEqual(&std::as_const(targetLevelRule_), s.rule);
    }

    void assertStartingXEqualsOne(const Track::Settings &s) {
        assertEqual(1, s.startingX);
    }

    void assertCeilingEqualsOne(const Track::Settings &s) {
        assertEqual(1, s.ceiling);
    }

    void assertFloorEqualsOne(const Track::Settings &s) {
        assertEqual(1, s.floor);
    }

    void assertBumpLimitEqualsOne(const Track::Settings &s) {
        assertEqual(1, s.bumpLimit);
    }

    void applyToSnrTrackFactoryParameters(
        int n, void (AdaptiveMethodTests::*f)(const Track::Settings &)) {
        for (int i = 0; i < n; ++i)
            (this->*f)(snrTrackFactoryParameters(i));
    }

    void selectList(int n) { randomizer.setRandomInt(n); }

    std::string next() { return method.next(); }

    void assertNextEquals(std::string s) { assertEqual(std::move(s), next()); }

    void setNextForList(int n, std::string s) {
        lists.at(n)->setNext(std::move(s));
    }

    void assertRandomizerPassedIntegerBounds(int a, int b) {
        assertEqual(a, randomizer.lowerIntBound());
        assertEqual(b, randomizer.upperIntBound());
    }

    void submitCoordinateResponse() {
        method.submitResponse(coordinateResponse);
    }

    void submitCorrectResponse() { method.submitCorrectResponse(); }

    void submitIncorrectResponse() { method.submitIncorrectResponse(); }

    auto track(int n) { return tracks.at(n); }

    void setCurrentForTarget(int n, std::string s) {
        lists.at(n)->setCurrent(std::move(s));
    }

    void writeLastCoordinateResponse() {
        method.writeLastCoordinateResponse(&outputFile);
    }

    void writeLastCorrectResponse() {
        method.writeLastCorrectResponse(&outputFile);
    }

    void writeLastIncorrectResponse() {
        method.writeLastIncorrectResponse(&outputFile);
    }

    auto writtenCoordinateResponseTrial() const {
        return outputFile.writtenAdaptiveCoordinateResponseTrial();
    }

    void writeCoordinateResponse() {
        submitCoordinateResponse();
        writeLastCoordinateResponse();
    }

    void writeCorrectResponse() {
        submitCorrectResponse();
        writeLastCorrectResponse();
    }

    void writeIncorrectResponse() {
        submitIncorrectResponse();
        writeLastIncorrectResponse();
    }

    void assertWritesUpdatedReversals(WritingResponseUseCase &useCase) {
        selectList(1);
        initialize();
        track(1)->setReversalsWhenUpdated(3);
        selectList(2);
        run(useCase);
        assertEqual(3, useCase.writtenReversals(outputFile));
    }

    void assertWritesPreUpdatedSnr(WritingResponseUseCase &useCase) {
        selectList(1);
        initialize();
        track(1)->setX(4);
        track(1)->setXWhenUpdated(3);
        selectList(2);
        run(useCase);
        assertEqual(4, useCase.writtenSnr(outputFile));
    }

    auto blueColor() { return coordinate_response_measure::Color::blue; }

    bool writtenCoordinateResponseTrialCorrect() {
        return writtenCoordinateResponseTrial().correct;
    }

    bool snrTrackPushedDown(int n) { return track(n)->pushedDown(); }

    bool snrTrackPushedUp(int n) { return track(n)->pushedUp(); }

    void setCorrectCoordinateResponse() { evaluator.setCorrect(); }

    void setIncorrectCoordinateResponse() { evaluator.setIncorrect(); }

    void setSnrTrackComplete(int n) { track(n)->setComplete(); }

    void assertTestIncompleteAfterCoordinateResponse() {
        submitCoordinateResponse();
        assertTestIncomplete();
    }

    void assertTestCompleteAfterCoordinateResponse() {
        submitCoordinateResponse();
        assertTestComplete();
    }

    void assertTestIncomplete() { assertFalse(testComplete()); }

    bool testComplete() { return method.complete(); }

    void assertTestComplete() { assertTrue(testComplete()); }

    void run(UseCase &useCase) { useCase.run(method); }

    void assertSelectsListInRangeAfterRemovingCompleteTracks(UseCase &useCase) {
        initialize();
        setSnrTrackComplete(2);
        run(useCase);
        assertRandomizerPassedIntegerBounds(0, 1);
    }

    void assertNextReturnsNextFilePathAfter(UseCase &useCase) {
        setNextForList(1, "a");
        selectList(1);
        run(useCase);
        assertNextEquals("a");
    }

    void assertPushesSnrTrackDown(UseCase &useCase) {
        selectList(1);
        initialize();
        selectList(2);
        run(useCase);
        assertTrue(snrTrackPushedDown(1));
        assertFalse(snrTrackPushedUp(1));
    }

    void assertPushesSnrTrackUp(UseCase &useCase) {
        selectList(1);
        initialize();
        selectList(2);
        run(useCase);
        assertFalse(snrTrackPushedDown(1));
        assertTrue(snrTrackPushedUp(1));
    }

    void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
        initialize();
        setNextForList(2, "a");
        setSnrTrackComplete(1);
        selectList(1);
        run(useCase);
        assertNextEquals("a");
    }

    void assertWritesTarget(WritingTargetUseCase &useCase) {
        initialize();
        evaluator.setFileName("a");
        run(useCase);
        assertEqual("a", useCase.writtenTarget(outputFile));
    }

    void assertPassesCurrentTargetToEvaluatorForFileName(UseCase &useCase) {
        selectList(1);
        initialize();
        setCurrentForTarget(1, "a");
        selectList(2);
        run(useCase);
        assertEqual("a", evaluator.filePathForFileName());
    }
};

TEST_F(AdaptiveMethodTests, initializeCreatesSnrTrackForEachList) {
    initialize();
    assertEqual(3UL, snrTrackFactoryParameters().size());
}

TEST_F(AdaptiveMethodTests, initializeCreatesEachSnrTrackWithTargetLevelRule) {
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertPassedTargetLevelRule);
}

TEST_F(AdaptiveMethodTests, initializeCreatesEachSnrTrackWithSnr) {
    test.startingSnr_dB = 1;
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertStartingXEqualsOne);
}

TEST_F(AdaptiveMethodTests, initializeCreatesEachSnrTrackWithCeiling) {
    test.ceilingSnr_dB = 1;
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertCeilingEqualsOne);
}

TEST_F(AdaptiveMethodTests, initializeCreatesEachSnrTrackWithFloor) {
    test.floorSnr_dB = 1;
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertFloorEqualsOne);
}

TEST_F(AdaptiveMethodTests, initializeCreatesEachSnrTrackWithBumpLimit) {
    test.trackBumpLimit = 1;
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertBumpLimitEqualsOne);
}

TEST_F(AdaptiveMethodTests, writeTestParametersPassesToOutputFile) {
    initialize();
    method.writeTestingParameters(&outputFile);
    assertEqual(&std::as_const(test), outputFile.adaptiveTest());
}

TEST_F(AdaptiveMethodTests, initializePassesTargetListDirectory) {
    test.targetListDirectory = "a";
    initialize();
    assertEqual("a", targetListSetReader.directory());
}

TEST_F(AdaptiveMethodTests, initializePassesTrackSettingsFile) {
    test.trackSettingsFile = "a";
    initialize();
    assertEqual("a", trackSettingsReader.filePath());
}

TEST_F(AdaptiveMethodTests, nextReturnsNextFilePathAfterInitialize) {
    assertNextReturnsNextFilePathAfter(initializing);
}

TEST_F(AdaptiveMethodTests, nextReturnsNextFilePathAfterCoordinateResponse) {
    initialize();
    assertNextReturnsNextFilePathAfter(submittingCoordinateResponse);
}

TEST_F(AdaptiveMethodTests, nextReturnsNextFilePathAfterCorrectResponse) {
    initialize();
    assertNextReturnsNextFilePathAfter(submittingCorrectResponse);
}

TEST_F(AdaptiveMethodTests, nextReturnsNextFilePathAfterIncorrectResponse) {
    initialize();
    assertNextReturnsNextFilePathAfter(submittingIncorrectResponse);
}

TEST_F(AdaptiveMethodTests, randomizerPassedIntegerBoundsOfLists) {
    initialize();
    assertRandomizerPassedIntegerBounds(0, 2);
}

TEST_F(AdaptiveMethodTests,
    submitCoordinateResponseSelectsListInRangeAfterRemovingCompleteTracks) {
    assertSelectsListInRangeAfterRemovingCompleteTracks(
        submittingCoordinateResponse);
}

TEST_F(AdaptiveMethodTests,
    submitCorrectResponseSelectsListInRangeAfterRemovingCompleteTracks) {
    assertSelectsListInRangeAfterRemovingCompleteTracks(
        submittingCorrectResponse);
}

TEST_F(AdaptiveMethodTests,
    submitIncorrectResponseSelectsListInRangeAfterRemovingCompleteTracks) {
    assertSelectsListInRangeAfterRemovingCompleteTracks(
        submittingIncorrectResponse);
}

TEST_F(AdaptiveMethodTests, snrReturnsThatOfCurrentTrack) {
    track(0)->setX(1);
    selectList(0);
    initialize();
    assertEqual(1, method.snr_dB());
}

TEST_F(AdaptiveMethodTests, submitCoordinateResponsePassesCurrentToEvaluator) {
    selectList(1);
    initialize();
    setCurrentForTarget(1, "a");
    selectList(2);
    submitCoordinateResponse();
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

TEST_F(AdaptiveMethodTests,
    submitCoordinateResponsePassesCorrectFilePathToEvaluator) {
    selectList(1);
    initialize();
    setCurrentForTarget(1, "a");
    selectList(2);
    submitCoordinateResponse();
    assertEqual("a", evaluator.correctFilePath());
}

TEST_F(AdaptiveMethodTests, submitCoordinateResponsePassesResponseToEvaluator) {
    initialize();
    submitCoordinateResponse();
    assertEqual(&std::as_const(coordinateResponse), evaluator.response());
}

TEST_F(AdaptiveMethodTests, writeCoordinateResponsePassesSubjectColor) {
    initialize();
    coordinateResponse.color = blueColor();
    writeCoordinateResponse();
    assertEqual(blueColor(), writtenCoordinateResponseTrial().subjectColor);
}

TEST_F(AdaptiveMethodTests, writeCoordinateResponsePassesCorrectColor) {
    initialize();
    evaluator.setCorrectColor(blueColor());
    writeCoordinateResponse();
    assertEqual(blueColor(), writtenCoordinateResponseTrial().correctColor);
}

TEST_F(AdaptiveMethodTests, writeCoordinateResponsePassesSubjectNumber) {
    initialize();
    coordinateResponse.number = 1;
    writeCoordinateResponse();
    assertEqual(1, writtenCoordinateResponseTrial().subjectNumber);
}

TEST_F(AdaptiveMethodTests, writeCoordinateResponsePassesCorrectNumber) {
    initialize();
    evaluator.setCorrectNumber(1);
    writeCoordinateResponse();
    assertEqual(1, writtenCoordinateResponseTrial().correctNumber);
}

TEST_F(AdaptiveMethodTests,
    writeCoordinateResponsePassesReversalsAfterUpdatingTrack) {
    assertWritesUpdatedReversals(writingCoordinateResponse);
}

TEST_F(AdaptiveMethodTests,
    writeCorrectResponsePassesReversalsAfterUpdatingTrack) {
    assertWritesUpdatedReversals(writingCorrectResponse);
}

TEST_F(AdaptiveMethodTests,
    writeIncorrectResponsePassesReversalsAfterUpdatingTrack) {
    assertWritesUpdatedReversals(writingIncorrectResponse);
}

TEST_F(
    AdaptiveMethodTests, writeCoordinateResponsePassesSnrBeforeUpdatingTrack) {
    assertWritesPreUpdatedSnr(writingCoordinateResponse);
}

TEST_F(AdaptiveMethodTests, writeCorrectResponsePassesSnrBeforeUpdatingTrack) {
    assertWritesPreUpdatedSnr(writingCorrectResponse);
}

TEST_F(
    AdaptiveMethodTests, writeIncorrectResponsePassesSnrBeforeUpdatingTrack) {
    assertWritesPreUpdatedSnr(writingIncorrectResponse);
}

TEST_F(AdaptiveMethodTests, writeCorrectCoordinateResponseIsCorrect) {
    initialize();
    setCorrectCoordinateResponse();
    writeCoordinateResponse();
    assertTrue(writtenCoordinateResponseTrialCorrect());
}

TEST_F(AdaptiveMethodTests, writeCorrectResponseIsCorrect) {
    initialize();
    writeCorrectResponse();
    assertTrue(outputFile.writtenOpenSetAdaptiveTrial().correct);
}

TEST_F(AdaptiveMethodTests, writeIncorrectCoordinateResponseIsIncorrect) {
    initialize();
    setIncorrectCoordinateResponse();
    writeCoordinateResponse();
    assertFalse(writtenCoordinateResponseTrialCorrect());
}

TEST_F(AdaptiveMethodTests, writeIncorrectResponseIsIncorrect) {
    initialize();
    writeIncorrectResponse();
    assertFalse(outputFile.writtenOpenSetAdaptiveTrial().correct);
}

TEST_F(AdaptiveMethodTests, writeCorrectResponseWritesTarget) {
    assertWritesTarget(writingCorrectResponse);
}

TEST_F(AdaptiveMethodTests, writeIncorrectResponseWritesTarget) {
    assertWritesTarget(writingIncorrectResponse);
}

TEST_F(AdaptiveMethodTests, submitCorrectResponsePassesCurrentToEvaluator) {
    assertPassesCurrentTargetToEvaluatorForFileName(submittingCorrectResponse);
}

TEST_F(AdaptiveMethodTests, submitIncorrectResponsePassesCurrentToEvaluator) {
    assertPassesCurrentTargetToEvaluatorForFileName(
        submittingIncorrectResponse);
}

TEST_F(AdaptiveMethodTests, submitCorrectCoordinateResponsePushesSnrTrackDown) {
    assertPushesSnrTrackDown(submittingCorrectCoordinateResponse);
}

TEST_F(AdaptiveMethodTests, submitCorrectResponsePushesSnrTrackDown) {
    assertPushesSnrTrackDown(submittingCorrectResponse);
}

TEST_F(AdaptiveMethodTests, submitIncorrectCoordinateResponsePushesSnrTrackUp) {
    assertPushesSnrTrackUp(submittingIncorrectCoordinateResponse);
}

TEST_F(AdaptiveMethodTests, submitIncorrectResponsePushesSnrTrackDown) {
    assertPushesSnrTrackUp(submittingIncorrectResponse);
}

TEST_F(AdaptiveMethodTests,
    submitCoordinateResponseSelectsListAmongThoseWithIncompleteTracks) {
    assertSelectsListAmongThoseWithIncompleteTracks(
        submittingCoordinateResponse);
}

TEST_F(AdaptiveMethodTests,
    submitCorrectResponseSelectsListAmongThoseWithIncompleteTracks) {
    assertSelectsListAmongThoseWithIncompleteTracks(submittingCorrectResponse);
}

TEST_F(AdaptiveMethodTests,
    submitIncorrectResponseSelectsListAmongThoseWithIncompleteTracks) {
    assertSelectsListAmongThoseWithIncompleteTracks(
        submittingIncorrectResponse);
}

TEST_F(AdaptiveMethodTests, completeWhenAllTracksComplete) {
    initialize();
    setSnrTrackComplete(0);
    assertTestIncompleteAfterCoordinateResponse();
    setSnrTrackComplete(1);
    assertTestIncompleteAfterCoordinateResponse();
    setSnrTrackComplete(2);
    assertTestCompleteAfterCoordinateResponse();
}
} // namespace
} // namespace av_speech_in_noise::tests
