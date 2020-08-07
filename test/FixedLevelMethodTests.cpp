#include "LogString.hpp"
#include "OutputFileStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "assert-utility.hpp"
#include <recognition-test/FixedLevelMethod.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(FixedLevelMethodImpl &) = 0;
};

class InitializingMethod : public UseCase {
    TargetPlaylist &list;
    const FixedLevelFixedTrialsTest &test;

  public:
    InitializingMethod(
        TargetPlaylist &list, const FixedLevelFixedTrialsTest &test)
        : list{list}, test{test} {}

    void run(FixedLevelMethodImpl &m) override { m.initialize(test, &list); }
};

class InitializingMethodWithFiniteTargetPlaylist : public UseCase {
  public:
    InitializingMethodWithFiniteTargetPlaylist(
        FiniteTargetPlaylist &list, const FixedLevelTest &test)
        : list{list}, test{test} {}

    void run(FixedLevelMethodImpl &m) override { m.initialize(test, &list); }

  private:
    FiniteTargetPlaylist &list;
    const FixedLevelTest &test;
};

class SubmittingCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response_{};

  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response_); }

    void setColor(coordinate_response_measure::Color c) { response_.color = c; }

    void setNumber(int n) { response_.number = n; }

    [[nodiscard]] auto response() const -> auto & { return response_; }
};

class SubmittingFreeResponse : public UseCase {
    FreeResponse response{};

  public:
    void run(FixedLevelMethodImpl &m) override { m.submit(response); }
    void setFlagged() { response.flagged = true; }
};

auto blueColor() { return coordinate_response_measure::Color::blue; }

void run(UseCase &useCase, FixedLevelMethodImpl &method) {
    useCase.run(method);
}

auto nextTarget(FixedLevelMethodImpl &method) -> std::string {
    return method.nextTarget().path;
}

void assertNextTargetEquals(
    FixedLevelMethodImpl &method, const std::string &s) {
    assertEqual(s, nextTarget(method));
}

void setNext(TargetPlaylistStub &list, std::string s) {
    list.setNext(std::move(s));
}

class FixedLevelMethodTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    TargetPlaylistStub targetList;
    OutputFileStub outputFile;
    FixedLevelMethodImpl method{evaluator};
    FixedLevelFixedTrialsTest test{};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;
    InitializingMethod initializingMethod{targetList, test};

    FixedLevelMethodTests() { run(initializingMethod, method); }

    void writeLastCoordinateResponse() {
        method.writeLastCoordinateResponse(outputFile);
    }

    auto writtenFixedLevelTrial() {
        return outputFile.writtenFixedLevelTrial();
    }

    auto writtenFixedLevelTrialCorrect() -> bool {
        return writtenFixedLevelTrial().correct;
    }

    void setCurrentTarget(std::string s) {
        targetList.setCurrent(std::move(s));
    }
};

#define FIXED_LEVEL_METHOD_TEST(a) TEST_F(FixedLevelMethodTests, a)

FIXED_LEVEL_METHOD_TEST(nextReturnsNextTarget) {
    setNext(targetList, "a");
    assertNextTargetEquals(method, "a");
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectColor) {
    submittingCoordinateResponse.setColor(blueColor());
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().subjectColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    submittingCoordinateResponse.setNumber(1);
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().subjectNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    evaluator.setCorrectColor(blueColor());
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual(blueColor(), writtenFixedLevelTrial().correctColor);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    evaluator.setCorrectNumber(1);
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual(1, writtenFixedLevelTrial().correctNumber);
}

FIXED_LEVEL_METHOD_TEST(writeCoordinateResponsePassesStimulus) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertEqual("a", writtenFixedLevelTrial().target);
}

FIXED_LEVEL_METHOD_TEST(writeCorrectCoordinateResponse) {
    evaluator.setCorrect();
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertTrue(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeIncorrectCoordinateResponse) {
    evaluator.setIncorrect();
    run(submittingCoordinateResponse, method);
    writeLastCoordinateResponse();
    assertFalse(writtenFixedLevelTrialCorrect());
}

FIXED_LEVEL_METHOD_TEST(writeTestPassesSettings) {
    method.writeTestingParameters(outputFile);
    assertEqual(&static_cast<const FixedLevelTest &>(std::as_const(test)),
        outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesResponse) {
    run(submittingCoordinateResponse, method);
    assertEqual(&submittingCoordinateResponse.response(), evaluator.response());
}

FIXED_LEVEL_METHOD_TEST(completeWhenTrialsExhausted) {
    test.trials = 3;
    run(initializingMethod, method);
    run(submittingCoordinateResponse, method);
    assertFalse(method.complete());
    run(submittingFreeResponse, method);
    assertFalse(method.complete());
    run(submittingCoordinateResponse, method);
    assertTrue(method.complete());
}

FIXED_LEVEL_METHOD_TEST(submitCoordinateResponsePassesCurrentToEvaluator) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

FIXED_LEVEL_METHOD_TEST(
    submitCoordinateResponsePassesCorrectTargetToEvaluator) {
    setCurrentTarget("a");
    run(submittingCoordinateResponse, method);
    assertEqual("a", evaluator.correctFilePath());
}

class PreInitializedFixedLevelMethodTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    TargetPlaylistStub targetList;
    FiniteTargetPlaylistStub finiteTargetPlaylist;
    FixedLevelMethodImpl method{evaluator};
    FixedLevelFixedTrialsTest test{};
    InitializingMethod initializingMethod{targetList, test};
    InitializingMethodWithFiniteTargetPlaylist
        initializingMethodWithFiniteTargetPlaylist{finiteTargetPlaylist, test};
};

TEST_F(PreInitializedFixedLevelMethodTests, snrReturnsInitializedSnr) {
    test.snr.dB = 1;
    run(initializingMethod, method);
    assertEqual(1, method.snr().dB);
}

TEST_F(PreInitializedFixedLevelMethodTests,
    snrReturnsInitializedWithFiniteTargetPlaylistSnr) {
    test.snr.dB = 1;
    run(initializingMethodWithFiniteTargetPlaylist, method);
    assertEqual(1, method.snr().dB);
}

TEST_F(PreInitializedFixedLevelMethodTests,
    initializePassesTargetPlaylistDirectory) {
    test.targetsUrl.path = "a";
    run(initializingMethod, method);
    assertEqual("a", targetList.directory().path);
}

TEST_F(PreInitializedFixedLevelMethodTests,
    initializeWithFiniteTargetPlaylistPassesTargetPlaylistDirectory) {
    test.targetsUrl.path = "a";
    run(initializingMethodWithFiniteTargetPlaylist, method);
    assertEqual("a", finiteTargetPlaylist.directory().path);
}

void assertComplete(FixedLevelMethodImpl &method) {
    assertTrue(method.complete());
}

void assertIncomplete(FixedLevelMethodImpl &method) {
    assertFalse(method.complete());
}

void assertTestCompleteOnlyAfter(UseCase &useCase, FixedLevelMethodImpl &method,
    FiniteTargetPlaylistStub &list) {
    list.setEmpty();
    assertIncomplete(method);
    run(useCase, method);
    assertComplete(method);
}

auto reinsertCurrentCalled(FiniteTargetPlaylistStub &list) -> bool {
    return list.reinsertCurrentCalled();
}

void assertCurrentTargetNotReinserted(FiniteTargetPlaylistStub &list) {
    assertFalse(reinsertCurrentCalled(list));
}

void assertCurrentTargetReinserted(FiniteTargetPlaylistStub &list) {
    assertTrue(reinsertCurrentCalled(list));
}

class FixedLevelMethodWithFiniteTargetPlaylistTests : public ::testing::Test {
  protected:
    ResponseEvaluatorStub evaluator;
    FiniteTargetPlaylistStub targetList;
    FixedLevelMethodImpl method{evaluator};
    FixedLevelTest test{};
    InitializingMethodWithFiniteTargetPlaylist
        initializingMethodWithFiniteTargetPlaylist{targetList, test};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingFreeResponse submittingFreeResponse;

    FixedLevelMethodWithFiniteTargetPlaylistTests() {
        run(initializingMethodWithFiniteTargetPlaylist, method);
    }
};

#define FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(a)                     \
    TEST_F(FixedLevelMethodWithFiniteTargetPlaylistTests, a)

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(nextReturnsNextTarget) {
    setNext(targetList, "a");
    assertNextTargetEquals(method, "a");
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(writeTestPassesSettings) {
    OutputFileStub outputFile;
    method.writeTestingParameters(outputFile);
    assertEqual(&std::as_const(test), outputFile.fixedLevelTest());
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterCoordinateResponse) {
    assertTestCompleteOnlyAfter(
        submittingCoordinateResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterFreeResponse) {
    assertTestCompleteOnlyAfter(submittingFreeResponse, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    completeWhenTestCompleteAfterInitializing) {
    assertTestCompleteOnlyAfter(
        initializingMethodWithFiniteTargetPlaylist, method, targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    submitFreeResponseDoesNotReinsertCurrentTarget) {
    run(submittingFreeResponse, method);
    assertCurrentTargetNotReinserted(targetList);
}

FIXED_LEVEL_METHOD_WITH_FINITE_TARGET_LIST_TEST(
    submitFreeResponseReinsertsCurrentTargetIfFlagged) {
    submittingFreeResponse.setFlagged();
    run(submittingFreeResponse, method);
    assertCurrentTargetReinserted(targetList);
}

class TargetPlaylistTestConcluderComboStub : public FiniteTargetPlaylist {
  public:
    void loadFromDirectory(const LocalUrl &) override {}
    auto directory() -> LocalUrl override { return {}; }
    auto next() -> LocalUrl override { return {}; }
    auto current() -> LocalUrl override { return {}; }
    auto empty() -> bool override {
        insert(log_, "empty ");
        return {};
    }
    void reinsertCurrent() override { insert(log_, "reinsertCurrent "); }
    auto log() const -> const std::stringstream & { return log_; }

  private:
    std::stringstream log_;
};

TEST(FixedLevelMethodTestsTBD,
    submitFreeResponseReinsertsCurrentTargetIfFlaggedBeforeQueryingCompletion) {
    ResponseEvaluatorStub evaluator;
    TargetPlaylistTestConcluderComboStub combo;
    FixedLevelMethodImpl method{evaluator};
    FixedLevelTest test;
    method.initialize(test, &combo);
    FreeResponse response;
    response.flagged = true;
    method.submit(response);
    assertTrue(endsWith(combo.log(), "reinsertCurrent empty "));
}
}
}
