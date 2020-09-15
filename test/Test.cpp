#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/TestImpl.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
class TestUIStub : public TestView, public TestControl {
  public:
    void declineContinuingTesting() { listener_->declineContinuingTesting(); }

    [[nodiscard]] auto continueTestingDialogMessage() const -> std::string {
        return continueTestingDialogMessage_;
    }

    void setContinueTestingDialogMessage(const std::string &s) override {
        continueTestingDialogMessage_ = s;
    }

    void acceptContinuingTesting() { listener_->acceptContinuingTesting(); }

    void showContinueTestingDialog() override {
        continueTestingDialogShown_ = true;
    }

    [[nodiscard]] auto continueTestingDialogShown() const -> bool {
        return continueTestingDialogShown_;
    }

    void hideContinueTestingDialog() override {
        continueTestingDialogHidden_ = true;
    }

    [[nodiscard]] auto continueTestingDialogHidden() const -> bool {
        return continueTestingDialogHidden_;
    }

    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    void attach(TestControl::Observer *e) override { listener_ = e; }

    void exitTest() { listener_->exitTest(); }

    void notifyThatPlayTrialButtonHasBeenClicked() { listener_->playTrial(); }

    void display(std::string s) override { displayed_ = std::move(s); }

    [[nodiscard]] auto displayed() const { return displayed_; }

    void secondaryDisplay(std::string s) override {
        secondaryDisplayed_ = std::move(s);
    }

    [[nodiscard]] auto secondaryDisplayed() const {
        return secondaryDisplayed_;
    }

    void showNextTrialButton() override { nextTrialButtonShown_ = true; }

    [[nodiscard]] auto nextTrialButtonShown() const {
        return nextTrialButtonShown_;
    }

    void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

    [[nodiscard]] auto nextTrialButtonHidden() const {
        return nextTrialButtonHidden_;
    }

    void showExitTestButton() override { exitTestButtonShown_ = true; }

    [[nodiscard]] auto exitTestButtonShown() const {
        return exitTestButtonShown_;
    }

    void hideExitTestButton() override { exitTestButtonHidden_ = true; }

    [[nodiscard]] auto exitTestButtonHidden() const {
        return exitTestButtonHidden_;
    }

  private:
    std::string displayed_;
    std::string secondaryDisplayed_;
    std::string continueTestingDialogMessage_;
    std::string response_;
    std::string correctKeywords_{"0"};
    TestControl::Observer *listener_{};
    bool exitTestButtonHidden_{};
    bool exitTestButtonShown_{};
    bool nextTrialButtonShown_{};
    bool nextTrialButtonHidden_{};
    bool continueTestingDialogShown_{};
    bool continueTestingDialogHidden_{};
    bool shown_{};
    bool hidden_{};
};

class ViewStub : public SessionView {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    auto audioDevice() -> std::string override { return audioDevice_; }

    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    void eventLoop() override { eventLoopCalled_ = true; }

    auto browseForDirectory() -> std::string override {
        return browseForDirectoryResult_;
    }

    auto browseCancelled() -> bool override { return browseCancelled_; }

    auto browseForOpeningFile() -> std::string override {
        return browseForOpeningFileResult_;
    }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(TestPresenter &) = 0;
};

class ConditionUseCase : public virtual UseCase {
  public:
    virtual auto condition(ModelStub &) -> Condition = 0;
};

class LevelUseCase : public virtual UseCase {
  public:
    virtual auto fullScaleLevel(ModelStub &) -> int = 0;
};

class TaskPresenterStub : public TaskPresenter {
  public:
    void showResponseSubmission() override {}
    void notifyThatTaskHasStarted() override {}
    void notifyThatUserIsDoneResponding() override {}
    void notifyThatTrialHasStarted() override {}
    void start() override {}
    void stop() override {}

  private:
};

class InitializingExperimenterPresenter : public virtual UseCase {};

class InitializingAdaptiveCoordinateResponseMeasureMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCoordinateResponseMeasure);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    }
};

class InitializingAdaptivePassFailMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptivePassFail);
    }
};

class InitializingAdaptivePassFailMethodWithEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptivePassFailWithEyeTracking);
    }
};

class InitializingAdaptiveCorrectKeywordsMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCorrectKeywords);
    }
};

class InitializingAdaptiveCorrectKeywordsMethodWithEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCorrectKeywordsWithEyeTracking);
    }
};

class InitializingFixedLevelFreeResponseWithTargetReplacementMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithTargetReplacement);
    }
};

class InitializingFixedLevelConsonantMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::fixedLevelConsonants);
    }
};

class InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    }
};

class
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::
                fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
    }
};

class
    InitializingFixedLevelCoordinateResponseMeasureMethodWithSilentIntervalTargets
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    }
};

class InitializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithSilentIntervalTargets);
    }
};

class InitializingFixedLevelFreeResponseMethodWithAllTargets
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(Method::fixedLevelFreeResponseWithAllTargets);
    }
};

class InitializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(TestPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
    }
};

class ExitingTest : public UseCase {
    TestUIStub *view;

  public:
    explicit ExitingTest(TestUIStub *view) : view{view} {}

    void run(TestPresenter &) override { view->exitTest(); }
};

class DecliningContinuingTesting : public UseCase {
  public:
    explicit DecliningContinuingTesting(TestUIStub &view) : view{view} {}

    void run(TestPresenter &) override { view.declineContinuingTesting(); }

  private:
    TestUIStub &view;
};

void exitTest(TestUIStub &view) { view.exitTest(); }

void notifyThatPlayTrialButtonHasBeenClicked(TestUIStub &view) {
    view.notifyThatPlayTrialButtonHasBeenClicked();
}

void acceptContinuingTesting(TestUIStub &view) {
    view.acceptContinuingTesting();
}

void notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
    TestController &responder) {
    responder
        .notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion();
}

void setAudioDevice(ViewStub &view, std::string s) {
    view.setAudioDevice(std::move(s));
}

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

class TaskControllerStub : public TaskController {
  public:
    void attach(Observer *) override {}
    void attach(TestController *) override {}
};

class PresenterStub : public SessionController {
  public:
    void notifyThatTestIsComplete() override {
        notifiedThatTestIsComplete_ = true;
    }

    [[nodiscard]] auto notifiedThatTestIsComplete() const -> bool {
        return notifiedThatTestIsComplete_;
    }
    void prepare(Method) override {}

  private:
    bool notifiedThatTestIsComplete_{};
};

class ExperimenterControllerListenerStub : public TestController::Observer {
  public:
    void notifyThatTrialHasStarted() override {
        notifiedThatTrialHasStarted_ = true;
    }
    [[nodiscard]] auto notifiedThatTrialHasStarted() const -> bool {
        return notifiedThatTrialHasStarted_;
    }
    void notifyThatNextTrialIsReady() override {
        notifiedThatNextTrialIsReady_ = true;
    }
    [[nodiscard]] auto notifiedThatNextTrialIsReady() const -> bool {
        return notifiedThatNextTrialIsReady_;
    }
    void setContinueTestingDialogMessage(const std::string &s) override {
        continueTestingDialogMessage_ = s;
    }
    auto continueTestingDialogMessage() -> std::string {
        return continueTestingDialogMessage_;
    }
    void showContinueTestingDialog() override {
        continueTestingDialogShown_ = true;
    }
    [[nodiscard]] auto continueTestingDialogShown() const -> bool {
        return continueTestingDialogShown_;
    }
    void display(const std::string &s) override { displayed_ = s; }
    auto displayed() -> std::string { return displayed_; }
    void secondaryDisplay(const std::string &s) override {
        displayedSecondary_ = s;
    }
    auto displayedSecondary() -> std::string { return displayedSecondary_; }

  private:
    std::string displayed_;
    std::string displayedSecondary_;
    std::string continueTestingDialogMessage_;
    bool notifiedThatTrialHasStarted_{};
    bool notifiedThatNextTrialIsReady_{};
    bool continueTestingDialogShown_{};
};

class ControllerUseCase {
  public:
    virtual ~ControllerUseCase() = default;
    virtual void run() = 0;
};

class AcceptingContinuingTesting : public ControllerUseCase {
  public:
    explicit AcceptingContinuingTesting(TestUIStub &view) : view{view} {}

    void run() override { acceptContinuingTesting(view); }

  private:
    TestUIStub &view;
};

class NotifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion
    : public ControllerUseCase {
  public:
    explicit NotifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
        TestControllerImpl &responder)
        : responder{responder} {}

    void run() override {
        notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
            responder);
    }

  private:
    TestControllerImpl &responder;
};

class NotifyingThatUserIsDoneResponding : public ControllerUseCase {
  public:
    explicit NotifyingThatUserIsDoneResponding(TestControllerImpl &responder)
        : responder{responder} {}

    void run() override { responder.notifyThatUserIsDoneResponding(); }

  private:
    TestControllerImpl &responder;
};

class NotifyingThatUserIsReadyForNextTrial : public ControllerUseCase {
  public:
    explicit NotifyingThatUserIsReadyForNextTrial(TestControllerImpl &responder)
        : responder{responder} {}

    void run() override { responder.notifyThatUserIsReadyForNextTrial(); }

  private:
    TestControllerImpl &responder;
};

class UninitializedTaskPresenterStub : public UninitializedTaskPresenter {
  public:
    void initialize(TaskPresenter *p) override { presenter_ = p; }
    auto presenter() -> TaskPresenter * { return presenter_; }
    void showResponseSubmission() override { responseSubmissionShown_ = true; }
    [[nodiscard]] auto responseSubmissionShown() const -> bool {
        return responseSubmissionShown_;
    }
    void notifyThatTaskHasStarted() override {}
    void notifyThatUserIsDoneResponding() override {}
    void notifyThatTrialHasStarted() override {
        notifiedThatTrialHasStarted_ = true;
    }
    void start() override { started_ = true; }
    [[nodiscard]] auto started() const -> bool { return started_; }
    void stop() override { stopped_ = true; }
    [[nodiscard]] auto stopped() const -> bool { return stopped_; }
    [[nodiscard]] auto notifiedThatTrialHasStarted() const -> bool {
        return notifiedThatTrialHasStarted_;
    }

  private:
    TaskPresenter *presenter_{};
    bool stopped_{};
    bool started_{};
    bool notifiedThatTrialHasStarted_{};
    bool responseSubmissionShown_{};
};

class ExperimenterTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub view;
    TestUIStub experimenterView;
    TaskControllerStub consonantController;
    TaskControllerStub coordinateResponseMeasureController;
    TaskControllerStub freeResponseController;
    TaskControllerStub passFailController;
    TaskControllerStub correctKeywordsController;
    TaskPresenterStub consonantPresenter;
    TaskPresenterStub coordinateResponseMeasurePresenter;
    TaskPresenterStub freeResponsePresenter;
    TaskPresenterStub correctKeywordsPresenter;
    TaskPresenterStub passFailPresenter;
    UninitializedTaskPresenterStub taskPresenter;
    TestControllerImpl experimenterController{model, view, experimenterView,
        &consonantController, &consonantPresenter,
        &coordinateResponseMeasureController,
        &coordinateResponseMeasurePresenter, &freeResponseController,
        &freeResponsePresenter, &correctKeywordsController,
        &correctKeywordsPresenter, &passFailController, &passFailPresenter};
    ExperimenterPresenterImpl experimenterPresenter{model, experimenterView,
        &consonantPresenter, &coordinateResponseMeasurePresenter,
        &freeResponsePresenter, &correctKeywordsPresenter, &passFailPresenter,
        &taskPresenter};
    InitializingAdaptiveCoordinateResponseMeasureMethod
        initializingAdaptiveCoordinateResponseMeasureMethod;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker
        initializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker
        initializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking
        initializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking;
    InitializingAdaptivePassFailMethod initializingAdaptivePassFailMethod;
    InitializingAdaptivePassFailMethodWithEyeTracking
        initializingAdaptivePassFailMethodWithEyeTracking;
    InitializingFixedLevelFreeResponseWithTargetReplacementMethod
        initializingFixedLevelFreeResponseWithTargetReplacementMethod;
    InitializingFixedLevelConsonantMethod initializingFixedLevelConsonantMethod;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking;
    InitializingAdaptiveCorrectKeywordsMethod
        initializingAdaptiveCorrectKeywordsMethod;
    InitializingAdaptiveCorrectKeywordsMethodWithEyeTracking
        initializingAdaptiveCorrectKeywordsMethodWithEyeTracking;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithSilentIntervalTargets
        initializingFixedLevelCoordinateResponseMeasureSilentIntervalsMethod;
    InitializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod
        initializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod;
    InitializingFixedLevelFreeResponseMethodWithAllTargets
        initializingFixedLevelFreeResponseMethodWithAllTargets;
    InitializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking
        initializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking;
    DecliningContinuingTesting decliningContinuingTesting{experimenterView};
    AcceptingContinuingTesting acceptingContinuingTesting{experimenterView};
    ExitingTest exitingTest{&experimenterView};
    NotifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion
        notifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion{
            experimenterController};
    NotifyingThatUserIsDoneResponding notifyingThatUserIsDoneResponding{
        experimenterController};
    NotifyingThatUserIsReadyForNextTrial notifyingThatUserIsReadyForNextTrial{
        experimenterController};
    PresenterStub presenter;
    ExperimenterControllerListenerStub experimenterControllerListener;

    ExperimenterTests() {
        experimenterController.attach(&presenter);
        experimenterController.attach(&experimenterControllerListener);
    }
};

#define AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(                             \
    model, useCase, experimenterControllerListener)                            \
    model.setTargetFileName("a");                                              \
    (useCase).run();                                                           \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"},                          \
        (experimenterControllerListener).displayedSecondary())

#define AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(                              \
    model, useCase, experimenterControllerListener)                            \
    model.setTrialNumber(1);                                                   \
    (useCase).run();                                                           \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        std::string{"Trial 1"}, experimenterControllerListener.displayed())

#define AV_SPEECH_IN_NOISE_EXPECT_NOTIFIES_THAT_NEXT_TRIAL_IS_READY(           \
    useCase, experimenterControllerListener)                                   \
    (useCase).run();                                                           \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(                                            \
        experimenterControllerListener.notifiedThatNextTrialIsReady())

#define AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(a)            \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).notifiedThatTestIsComplete())

#define AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(                  \
    useCase, experimenterPresenter, expected)                                  \
    useCase.run(experimenterPresenter);                                        \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                           \
        static_cast<TaskPresenter *>(&expected), taskPresenter.presenter())

#define EXPERIMENTER_TEST(a) TEST_F(ExperimenterTests, a)

EXPERIMENTER_TEST(
    responderNotifiesThatTestIsCompleteAfterExitTestButtonClicked) {
    exitTest(experimenterView);
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(presenter);
}

EXPERIMENTER_TEST(
    responderNotifiesThatTestIsCompleteAfterContinueTestingDialogIsDeclined) {
    experimenterController.declineContinuingTesting();
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(presenter);
}

EXPERIMENTER_TEST(
    responderNotifiesThatTestIsCompleteAfterUserIsDoneResponding) {
    setTestComplete(model);
    experimenterController.notifyThatUserIsDoneResponding();
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(presenter);
}

EXPERIMENTER_TEST(
    responderNotifiesThatTestIsCompleteAfterNotifyingThatUserIsReadyForNextTrial) {
    setTestComplete(model);
    experimenterController.notifyThatUserIsReadyForNextTrial();
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIED_THAT_TEST_IS_COMPLETE(presenter);
}

EXPERIMENTER_TEST(responderPlaysTrialAfterPlayTrialButtonClicked) {
    setAudioDevice(view, "a");
    notifyThatPlayTrialButtonHasBeenClicked(experimenterView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.trialParameters().audioDevice);
}

EXPERIMENTER_TEST(
    responderPlaysTrialAfterNotifyingThatUserIsReadyForNextTrial) {
    setAudioDevice(view, "a");
    experimenterController.notifyThatUserIsReadyForNextTrial();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.trialParameters().audioDevice);
}

EXPERIMENTER_TEST(
    responderNotifiesThatTrialHasStartedAfterPlayTrialButtonClicked) {
    notifyThatPlayTrialButtonHasBeenClicked(experimenterView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterControllerListener.notifiedThatTrialHasStarted());
}

EXPERIMENTER_TEST(
    responderNotifiesThatTrialHasStartedAfterNotifyingThatUserIsReadyForNextTrial) {
    experimenterController.notifyThatUserIsReadyForNextTrial();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterControllerListener.notifiedThatTrialHasStarted());
}

EXPERIMENTER_TEST(
    responderRestartsAdaptiveTestWhilePreservingTargetsAfterContinueTestingDialogIsAccepted) {
    acceptContinuingTesting(experimenterView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        model.adaptiveTestRestartedWhilePreservingCyclicTargets());
}

EXPERIMENTER_TEST(
    responderNotifiesThatNextTrialIsReadyAfterContinueTestingDialogIsAccepted) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIES_THAT_NEXT_TRIAL_IS_READY(
        acceptingContinuingTesting, experimenterControllerListener);
}

EXPERIMENTER_TEST(
    responderNotifiesThatNextTrialIsReadyAfterNotifyingThatUserIsDoneResponding) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIES_THAT_NEXT_TRIAL_IS_READY(
        notifyingThatUserIsDoneResponding, experimenterControllerListener);
}

EXPERIMENTER_TEST(
    responderNotifiesThatNextTrialIsReadyAfterNotShowingContinueTestingDialogWithResults) {
    AV_SPEECH_IN_NOISE_EXPECT_NOTIFIES_THAT_NEXT_TRIAL_IS_READY(
        notifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion,
        experimenterControllerListener);
}

EXPERIMENTER_TEST(responderDisplaysTargetAfterUserIsDoneResponding) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(model,
        notifyingThatUserIsDoneResponding, experimenterControllerListener);
}

EXPERIMENTER_TEST(
    responderDisplaysTargetFileNameAfterNotShowingContinueTestingDialogWithResults) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(model,
        notifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion,
        experimenterControllerListener);
}

EXPERIMENTER_TEST(
    responderDisplaysTargetFileNameAfterContinueTestingDialogIsAccepted) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(
        model, acceptingContinuingTesting, experimenterControllerListener);
}

EXPERIMENTER_TEST(
    responderDisplaysTargetFileNameAfterNotifyingThatUserIsReadyForNextTrial) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TARGET(model,
        notifyingThatUserIsReadyForNextTrial, experimenterControllerListener);
}

EXPERIMENTER_TEST(responderDisplaysTrialNumberAfterUserIsDoneResponding) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(model,
        notifyingThatUserIsDoneResponding, experimenterControllerListener);
}

EXPERIMENTER_TEST(
    responderDisplaysTrialNumberAfterNotShowingContinueTestingDialogWithResults) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(model,
        notifyingThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion,
        experimenterControllerListener);
}

EXPERIMENTER_TEST(
    responderDisplaysTrialNumberAfterContinueTestingDialogIsAccepted) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(
        model, acceptingContinuingTesting, experimenterControllerListener);
}

EXPERIMENTER_TEST(
    responderDisplaysTrialNumberAfterNotifyingThatUserIsReadyForNextTrial) {
    AV_SPEECH_IN_NOISE_EXPECT_DISPLAYS_TRIAL(model,
        notifyingThatUserIsReadyForNextTrial, experimenterControllerListener);
}

EXPERIMENTER_TEST(responderShowsContinueTestingDialog) {
    setTestComplete(model);
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
        experimenterController);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterControllerListener.continueTestingDialogShown());
}

EXPERIMENTER_TEST(responderShowsAdaptiveTestResults) {
    setTestComplete(model);
    model.setAdaptiveTestResults({{{"a"}, 1.}, {{"b"}, 2.}, {{"c"}, 3.}});
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion(
        experimenterController);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"thresholds (targets: dB SNR)\na: 1\nb: 2\nc: 3"},
        experimenterControllerListener.continueTestingDialogMessage());
}

EXPERIMENTER_TEST(presenterShowsViewAfterStarting) {
    experimenterPresenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.shown());
}

EXPERIMENTER_TEST(presenterHidesViewAfterStopping) {
    experimenterPresenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.hidden());
}

EXPERIMENTER_TEST(presenterHidesContinueTestingDialogAfterStopping) {
    experimenterPresenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterView.continueTestingDialogHidden());
}

EXPERIMENTER_TEST(presenterStopsTaskAfterStopping) {
    experimenterPresenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskPresenter.stopped());
}

EXPERIMENTER_TEST(presenterHidesExitTestButtonAfterTrialStarts) {
    experimenterPresenter.notifyThatTrialHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.exitTestButtonHidden());
}

EXPERIMENTER_TEST(presenterHidesNextTrialButtonAfterTrialStarts) {
    experimenterPresenter.notifyThatTrialHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.nextTrialButtonHidden());
}

EXPERIMENTER_TEST(presenterNotifiesTaskPresenterThatTrialHasStarted) {
    experimenterPresenter.notifyThatTrialHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskPresenter.notifiedThatTrialHasStarted());
}

EXPERIMENTER_TEST(presenterShowsExitTestButtonWhenTrialCompletes) {
    model.completeTrial();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.exitTestButtonShown());
}

EXPERIMENTER_TEST(presenterShowsTaskResponseSubmissionWhenTrialCompletes) {
    model.completeTrial();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskPresenter.responseSubmissionShown());
}

EXPERIMENTER_TEST(presenterHidesContinueTestingDialogAfterNextTrialIsReady) {
    experimenterPresenter.notifyThatNextTrialIsReady();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterView.continueTestingDialogHidden());
}

EXPERIMENTER_TEST(presenterShowsNextTrialButtonAfterNextTrialIsReady) {
    experimenterPresenter.notifyThatNextTrialIsReady();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.nextTrialButtonShown());
}

EXPERIMENTER_TEST(presenterDisplaysMessage) {
    experimenterPresenter.display("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, experimenterView.displayed());
}

EXPERIMENTER_TEST(presenterDisplaysSecondaryMessage) {
    experimenterPresenter.secondaryDisplay("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, experimenterView.secondaryDisplayed());
}

EXPERIMENTER_TEST(presenterShowsContinueTestingDialog) {
    experimenterPresenter.showContinueTestingDialog();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        experimenterView.continueTestingDialogShown());
}

EXPERIMENTER_TEST(presenterSetsContinueTestingDialogMessage) {
    experimenterPresenter.setContinueTestingDialogMessage("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, experimenterView.continueTestingDialogMessage());
}

EXPERIMENTER_TEST(presenterStartsTaskPresenterWhenInitializing) {
    experimenterPresenter.initialize(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskPresenter.started());
}

EXPERIMENTER_TEST(
    initializingAdaptiveCoordinateResponseMeasureMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCoordinateResponseMeasureMethod,
        experimenterPresenter, coordinateResponseMeasurePresenter);
}

EXPERIMENTER_TEST(
    initializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeakerInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker,
        experimenterPresenter, coordinateResponseMeasurePresenter);
}

EXPERIMENTER_TEST(
    initializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMaskerInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker,
        experimenterPresenter, coordinateResponseMeasurePresenter);
}

EXPERIMENTER_TEST(
    initializingAdaptiveCoordinateResponseMeasureMethodWithEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking,
        experimenterPresenter, coordinateResponseMeasurePresenter);
}

EXPERIMENTER_TEST(
    initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement,
        experimenterPresenter, coordinateResponseMeasurePresenter);
}

EXPERIMENTER_TEST(
    initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking,
        experimenterPresenter, coordinateResponseMeasurePresenter);
}

EXPERIMENTER_TEST(
    initializingFixedLevelCoordinateResponseMeasureSilentIntervalsMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelCoordinateResponseMeasureSilentIntervalsMethod,
        experimenterPresenter, coordinateResponseMeasurePresenter);
}

EXPERIMENTER_TEST(
    initializingFixedLevelFreeResponseMethodWithAllTargetsInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelFreeResponseMethodWithAllTargets,
        experimenterPresenter, freeResponsePresenter);
}

EXPERIMENTER_TEST(
    initializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking,
        experimenterPresenter, freeResponsePresenter);
}

EXPERIMENTER_TEST(
    initializingFixedLevelFreeResponseWithSilentIntervalTargetsMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod,
        experimenterPresenter, freeResponsePresenter);
}

EXPERIMENTER_TEST(
    initializingFixedLevelFreeResponseWithTargetReplacementMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelFreeResponseWithTargetReplacementMethod,
        experimenterPresenter, freeResponsePresenter);
}

EXPERIMENTER_TEST(initializingAdaptiveCorrectKeywordsMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCorrectKeywordsMethod, experimenterPresenter,
        correctKeywordsPresenter);
}

EXPERIMENTER_TEST(
    initializingAdaptiveCorrectKeywordsMethodWithEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptiveCorrectKeywordsMethodWithEyeTracking,
        experimenterPresenter, correctKeywordsPresenter);
}

EXPERIMENTER_TEST(initializingFixedLevelConsonantMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingFixedLevelConsonantMethod, experimenterPresenter,
        consonantPresenter);
}

EXPERIMENTER_TEST(initializingAdaptivePassFailMethodInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptivePassFailMethod, experimenterPresenter,
        passFailPresenter);
}

EXPERIMENTER_TEST(
    initializingAdaptivePassFailMethodWithEyeTrackingInitializesTask) {
    AV_SPEECH_IN_NOISE_EXPECT_TASK_PRESENTER_INITIALIZED(
        initializingAdaptivePassFailMethodWithEyeTracking,
        experimenterPresenter, passFailPresenter);
}

EXPERIMENTER_TEST(presenterDisplaysTrialNumberWhenInitializing) {
    model.setTrialNumber(1);
    experimenterPresenter.initialize(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"Trial 1"}, experimenterView.displayed());
}

EXPERIMENTER_TEST(presenterDisplaysTargetWhenInitializing) {
    model.setTargetFileName("a");
    experimenterPresenter.initialize(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, experimenterView.secondaryDisplayed());
}
}
}