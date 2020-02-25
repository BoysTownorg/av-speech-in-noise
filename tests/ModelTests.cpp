#include "ModelEventListenerStub.h"
#include "TargetListStub.h"
#include "TestConcluderStub.h"
#include "assert-utility.h"
#include <recognition-test/Model.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
namespace {
class AdaptiveMethodStub : public AdaptiveMethod {
    const AdaptiveTest *test_{};

  public:
    void initialize(const AdaptiveTest &t) override { test_ = &t; }

    [[nodiscard]] auto test() const { return test_; }

    auto complete() -> bool override { return {}; }
    auto nextTarget() -> std::string override { return {}; }
    auto currentTarget() -> std::string override { return {}; }
    auto snr_dB() -> int override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submit(const open_set::FreeResponse &) override {}
    void submit(const open_set::CorrectKeywords &) override {}
    void writeTestingParameters(OutputFile *) override {}
    void writeLastCoordinateResponse(OutputFile *) override {}
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void writeLastCorrectKeywords(OutputFile *) override {}
    void submit(const coordinate_response_measure::Response &) override {}
};

class FixedLevelMethodStub : public FixedLevelMethod {
    const FixedLevelTest *test_{};
    TargetList *targetList_{};
    TestConcluder *testConcluder_{};
    bool initializedWithFiniteTargetList_{};

  public:
    void initialize(const FixedLevelTest &t, TargetList *list,
        TestConcluder *concluder) override {
        testConcluder_ = concluder;
        targetList_ = list;
        test_ = &t;
    }

    void initialize(const FixedLevelTest &t, FiniteTargetList *list,
        TestConcluder *concluder) {
        testConcluder_ = concluder;
        targetList_ = list;
        test_ = &t;
        initializedWithFiniteTargetList_ = true;
    }

    [[nodiscard]] auto initializedWithFiniteTargetList() const -> bool {
        return initializedWithFiniteTargetList_;
    }

    [[nodiscard]] auto testConcluder() const { return testConcluder_; }

    [[nodiscard]] auto targetList() const { return targetList_; }

    [[nodiscard]] auto test() const { return test_; }

    auto complete() -> bool override { return {}; }
    auto nextTarget() -> std::string override { return {}; }
    auto currentTarget() -> std::string override { return {}; }
    auto snr_dB() -> int override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submit(const open_set::FreeResponse &) override {}
    void submit(const open_set::CorrectKeywords &) override {}
    void writeTestingParameters(OutputFile *) override {}
    void writeLastCoordinateResponse(OutputFile *) override {}
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void writeLastCorrectKeywords(OutputFile *) override {}
    void submit(const coordinate_response_measure::Response &) override {}
};

class RecognitionTestModelStub : public RecognitionTestModel {
    std::vector<std::string> audioDevices_{};
    std::string targetFileName_{};
    const Model::EventListener *listener_{};
    const Calibration *calibration_{};
    const AudioSettings *playTrialSettings_{};
    const Test *test_{};
    const TestMethod *testMethod_{};
    const coordinate_response_measure::Response *coordinateResponse_{};
    const open_set::CorrectKeywords *correctKeywords_{};
    int trialNumber_{};
    bool complete_{};
    bool initializedWithSingleSpeaker_{};
    bool initializedWithDelayedMasker_{};
    bool initializedWithEyeTracking_{};

  public:
    void initialize(TestMethod *method, const Test &test) override {
        testMethod_ = method;
        test_ = &test;
    }

    void initializeWithSingleSpeaker(
        TestMethod *method, const Test &test) override {
        testMethod_ = method;
        test_ = &test;
        initializedWithSingleSpeaker_ = true;
    }

    void initializeWithDelayedMasker(
        TestMethod *method, const Test &test) override {
        testMethod_ = method;
        test_ = &test;
        initializedWithDelayedMasker_ = true;
    }

    auto trialNumber() -> int override { return trialNumber_; }

    void setTrialNumber(int n) { trialNumber_ = n; }

    void setTargetFileName(std::string s) { targetFileName_ = std::move(s); }

    auto targetFileName() -> std::string override { return targetFileName_; }

    void playTrial(const AudioSettings &s) override { playTrialSettings_ = &s; }

    void submit(const coordinate_response_measure::Response &p) override {
        coordinateResponse_ = &p;
    }

    void submit(const open_set::CorrectKeywords &p) override {
        correctKeywords_ = &p;
    }

    auto testComplete() -> bool override { return complete_; }

    auto audioDevices() -> std::vector<std::string> override {
        return audioDevices_;
    }

    void subscribe(Model::EventListener *e) override { listener_ = e; }

    void playCalibration(const Calibration &c) override { calibration_ = &c; }

    [[nodiscard]] auto initializedWithSingleSpeaker() const -> bool {
        return initializedWithSingleSpeaker_;
    }

    [[nodiscard]] auto initializedWithDelayedMasker() const -> bool {
        return initializedWithDelayedMasker_;
    }

    [[nodiscard]] auto initializedWithEyeTracking() const -> bool {
        return initializedWithEyeTracking_;
    }

    [[nodiscard]] auto coordinateResponse() const {
        return coordinateResponse_;
    }

    [[nodiscard]] auto correctKeywords() const { return correctKeywords_; }

    [[nodiscard]] auto testMethod() const { return testMethod_; }

    [[nodiscard]] auto test() const { return test_; }

    [[nodiscard]] auto playTrialSettings() const { return playTrialSettings_; }

    [[nodiscard]] auto calibration() const { return calibration_; }

    [[nodiscard]] auto listener() const { return listener_; }

    void setComplete() { complete_ = true; }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submit(const open_set::FreeResponse &) override {}
    void throwIfTrialInProgress() override {}
};

class InitializingTestUseCase {
  public:
    virtual ~InitializingTestUseCase() = default;
    virtual void run(ModelImpl &) = 0;
    virtual auto test() -> const Test & = 0;
    virtual auto testMethod() -> const TestMethod * = 0;
};

class InitializingFixedLevelTest : public virtual InitializingTestUseCase {
  public:
    virtual void run(ModelImpl &model, const FixedLevelTest &test) = 0;
};

class InitializingAdaptiveTest : public virtual InitializingTestUseCase {
  public:
    virtual void run(ModelImpl &model, const AdaptiveTest &test) = 0;
};

void initialize(ModelImpl &model, const AdaptiveTest &test) {
    model.initialize(test);
}

void initializeWithTargetReplacement(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithTargetReplacement(test);
}

void initializeWithSingleSpeaker(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithSingleSpeaker(test);
}

void initializeWithDelayedMasker(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithDelayedMasker(test);
}

void initializeWithSilentIntervalTargets(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithSilentIntervalTargets(test);
}

void initializeWithTargetReplacementAndEyeTracking(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithTargetReplacementAndEyeTracking(test);
}

void initializeWithSilentIntervalTargetsAndEyeTracking(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithSilentIntervalTargetsAndEyeTracking(test);
}

class InitializingDefaultAdaptiveTest : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingDefaultAdaptiveTest(AdaptiveMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { initialize(model, test_); }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
        initialize(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingAdaptiveTestWithSingleSpeaker
    : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTestWithSingleSpeaker(
        AdaptiveMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithSingleSpeaker(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
        initializeWithSingleSpeaker(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingAdaptiveTestWithDelayedMasker
    : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTestWithDelayedMasker(
        AdaptiveMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithDelayedMasker(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
        initializeWithDelayedMasker(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithTargetReplacement
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithTargetReplacement(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithTargetReplacement(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initializeWithTargetReplacement(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithSilentIntervalTargets
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithSilentIntervalTargets(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithSilentIntervalTargets(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initializeWithSilentIntervalTargets(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithAllTargets
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithAllTargets(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        model.initializeWithAllTargets(test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        model.initializeWithAllTargets(test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class ModelTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    TargetListStub targetsWithReplacement;
    TestConcluderStub fixedTrialTestConcluder;
    FiniteTargetListStub silentIntervals;
    TestConcluderStub emptyTargetListTestConcluder;
    FiniteTargetListStub everyTargetOnce;
    RecognitionTestModelStub internalModel;
    ModelImpl model{adaptiveMethod, fixedLevelMethod, targetsWithReplacement,
        fixedTrialTestConcluder, silentIntervals, emptyTargetListTestConcluder,
        everyTargetOnce, internalModel};
    AdaptiveTest adaptiveTest;
    FixedLevelTest fixedLevelTest;
    InitializingDefaultAdaptiveTest initializingDefaultAdaptiveTest{
        &adaptiveMethod};
    InitializingAdaptiveTestWithSingleSpeaker
        initializingAdaptiveTestWithSingleSpeaker{&adaptiveMethod};
    InitializingAdaptiveTestWithDelayedMasker
        initializingAdaptiveTestWithDelayedMasker{&adaptiveMethod};
    InitializingFixedLevelTestWithTargetReplacement
        initializingFixedLevelTestWithTargetReplacement{&fixedLevelMethod};
    InitializingFixedLevelTestWithSilentIntervalTargets
        initializingFixedLevelTestWithSilentIntervalTargets{&fixedLevelMethod};
    InitializingFixedLevelTestWithAllTargets
        initializingFixedLevelTestWithAllTargets{&fixedLevelMethod};

    void run(InitializingTestUseCase &useCase) { useCase.run(model); }

    auto testComplete() -> bool { return model.testComplete(); }

    void assertInitializesInternalModel(InitializingTestUseCase &useCase) {
        run(useCase);
        assertEqual(useCase.testMethod(), internalModel.testMethod());
        assertEqual(&useCase.test(), internalModel.test());
    }

    void assertInitializesFixedLevelMethod(
        InitializingFixedLevelTest &useCase) {
        useCase.run(model, fixedLevelTest);
        assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethod.test());
    }

    void assertInitializesAdaptiveMethod(InitializingAdaptiveTest &useCase) {
        useCase.run(model, adaptiveTest);
        assertEqual(&std::as_const(adaptiveTest), adaptiveMethod.test());
    }

    void assertInitializesFixedLevelTestWithTestConcluder(
        InitializingTestUseCase &useCase, TestConcluder &concluder) {
        run(useCase);
        assertEqual(&concluder, fixedLevelMethod.testConcluder());
    }

    void assertInitializesFixedLevelTestWithTargetList(
        InitializingTestUseCase &useCase, TargetList &targetList) {
        run(useCase);
        assertEqual(&targetList, fixedLevelMethod.targetList());
    }

    void assertInitializesFixedLevelMethodWithFiniteTargetList(
        InitializingTestUseCase &useCase) {
        run(useCase);
        assertTrue(fixedLevelMethod.initializedWithFiniteTargetList());
    }
};

#define MODEL_TEST(a) TEST_F(ModelTests, a)

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithTargetReplacement);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithSilentIntervalTargets);
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementInitializesWithTargetsWithReplacement) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithTargetReplacement,
        targetsWithReplacement);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesWithSilentIntervals) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithSilentIntervalTargets, silentIntervals);
}

MODEL_TEST(initializeFixedLevelTestWithAllTargetsInitializesWithAllTargets) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithAllTargets, everyTargetOnce);
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementInitializesWithFixedTrialTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelTestWithTargetReplacement,
        fixedTrialTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesWithEmptyTargetListTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelTestWithSilentIntervalTargets,
        emptyTargetListTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelTestWithAllTargetsInitializesWithEmptyTargetListTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelTestWithAllTargets, emptyTargetListTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesWithFiniteTargetList) {
    assertInitializesFixedLevelMethodWithFiniteTargetList(
        initializingFixedLevelTestWithSilentIntervalTargets);
}

MODEL_TEST(
    initializingFixedLevelTestWithAllTargetsInitializesWithFiniteTargetList) {
    assertInitializesFixedLevelMethodWithFiniteTargetList(
        initializingFixedLevelTestWithAllTargets);
}

MODEL_TEST(initializeDefaultAdaptiveTestInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingDefaultAdaptiveTest);
}

MODEL_TEST(initializeAdaptiveTestWithSingleSpeakerInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTestWithSingleSpeaker);
}

MODEL_TEST(initializeAdaptiveTestWithDelayedMaskerInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTestWithDelayedMasker);
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithTargetReplacement);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithSilentIntervalTargets);
}

MODEL_TEST(initializeFixedLevelTestWithAllTargetsInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelTestWithAllTargets);
}

MODEL_TEST(initializeDefaultAdaptiveTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingDefaultAdaptiveTest);
}

MODEL_TEST(initializeAdaptiveTestWithSingleSpeakerInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithSingleSpeaker);
}

MODEL_TEST(initializeAdaptiveTestWithDelayedMaskerInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithDelayedMasker);
}

MODEL_TEST(initializeAdaptiveTestWithSingleSpeakerInitializesSingleSpeaker) {
    run(initializingAdaptiveTestWithSingleSpeaker);
    assertTrue(internalModel.initializedWithSingleSpeaker());
}

MODEL_TEST(initializeAdaptiveTestWithDelayedMaskerInitializesSingleSpeaker) {
    run(initializingAdaptiveTestWithDelayedMasker);
    assertTrue(internalModel.initializedWithDelayedMasker());
}

MODEL_TEST(submitResponsePassesCoordinateResponse) {
    coordinate_response_measure::Response response;
    model.submit(response);
    assertEqual(&std::as_const(response), internalModel.coordinateResponse());
}

MODEL_TEST(submitCorrectKeywordsPassesCorrectKeywords) {
    open_set::CorrectKeywords k;
    model.submit(k);
    assertEqual(&std::as_const(k), internalModel.correctKeywords());
}

MODEL_TEST(playTrialPassesAudioSettings) {
    AudioSettings settings;
    model.playTrial(settings);
    assertEqual(&std::as_const(settings), internalModel.playTrialSettings());
}

MODEL_TEST(playCalibrationPassesCalibration) {
    Calibration calibration;
    model.playCalibration(calibration);
    assertEqual(&std::as_const(calibration), internalModel.calibration());
}

MODEL_TEST(testCompleteWhenComplete) {
    assertFalse(testComplete());
    internalModel.setComplete();
    assertTrue(testComplete());
}

MODEL_TEST(returnsAudioDevices) {
    internalModel.setAudioDevices({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

MODEL_TEST(returnsTrialNumber) {
    internalModel.setTrialNumber(1);
    assertEqual(1, model.trialNumber());
}

MODEL_TEST(returnsTargetFileName) {
    internalModel.setTargetFileName("a");
    assertEqual("a", model.targetFileName());
}

MODEL_TEST(subscribesToListener) {
    ModelEventListenerStub listener;
    model.subscribe(&listener);
    assertEqual(static_cast<const Model::EventListener *>(&listener),
        internalModel.listener());
}
}
}
