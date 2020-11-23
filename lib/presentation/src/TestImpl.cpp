#include "TestImpl.hpp"
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static void displayTrialInformation(
    Model &model, TestController::Observer *presenter) {
    presenter->display("Trial " + std::to_string(model.trialNumber()));
    presenter->secondaryDisplay(model.targetFileName());
}

static void readyNextTrial(Model &model, TestController::Observer *presenter) {
    displayTrialInformation(model, presenter);
    presenter->notifyThatNextTrialIsReady();
}

TestControllerImpl::TestControllerImpl(
    Model &model, SessionView &sessionView, TestControl &control)
    : model{model}, sessionView{sessionView} {
    control.attach(this);
}

void TestControllerImpl::attach(TestController::Observer *e) { observer = e; }

static void notifyThatTestIsComplete(SessionController *presenter) {
    presenter->notifyThatTestIsComplete();
}

void TestControllerImpl::exitTest() { notifyThatTestIsComplete(controller); }

static void playTrial(
    Model &model, SessionView &view, TestController::Observer *observer) {
    model.playTrial(AudioSettings{view.audioDevice()});
    observer->notifyThatTrialHasStarted();
}

void TestControllerImpl::playTrial() {
    av_speech_in_noise::playTrial(model, sessionView, observer);
}

void TestControllerImpl::declineContinuingTesting() {
    notifyThatTestIsComplete(controller);
}

void TestControllerImpl::acceptContinuingTesting() {
    model.restartAdaptiveTestWhilePreservingTargets();
    readyNextTrial(model, observer);
}

static void ifTestCompleteElse(Model &model, const std::function<void()> &f,
    const std::function<void()> &g) {
    if (model.testComplete())
        f();
    else
        g();
}

static void readyNextTrialIfTestIncompleteElse(Model &model,
    TestController::Observer *observer, const std::function<void()> &f) {
    ifTestCompleteElse(model, f, [&]() { readyNextTrial(model, observer); });
}

static void notifyIfTestIsCompleteElse(Model &model,
    SessionController *controller, const std::function<void()> &f) {
    ifTestCompleteElse(
        model, [&]() { notifyThatTestIsComplete(controller); }, f);
}

void TestControllerImpl::
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion() {
    readyNextTrialIfTestIncompleteElse(model, observer, [&] {
        observer->showContinueTestingDialog();
        std::stringstream thresholds;
        thresholds << "thresholds (targets: dB SNR)";
        for (const auto &result : model.adaptiveTestResults())
            thresholds << '\n'
                       << result.targetsUrl.path << ": " << result.threshold;
        observer->setContinueTestingDialogMessage(thresholds.str());
    });
}

void TestControllerImpl::
    notifyThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd() {
    ifTestCompleteElse(
        model,
        [&]() {
            observer->tellSubject("Condition complete, great work!");
            notifyThatTestIsComplete(controller);
        },
        [&]() {
            displayTrialInformation(model, observer);
            av_speech_in_noise::playTrial(model, sessionView, observer);
        });
}

void TestControllerImpl::notifyThatUserIsDoneResponding() {
    notifyIfTestIsCompleteElse(
        model, controller, [&]() { readyNextTrial(model, observer); });
}

void TestControllerImpl::notifyThatUserIsReadyForNextTrial() {
    notifyIfTestIsCompleteElse(model, controller, [&]() {
        displayTrialInformation(model, observer);
        av_speech_in_noise::playTrial(model, sessionView, observer);
    });
}

void TestControllerImpl::attach(SessionController *p) { controller = p; }

TestPresenterImpl::TestPresenterImpl(Model &model, TestView &view,
    TaskPresenter *consonantPresenter,
    TaskPresenter *coordinateResponseMeasurePresenter,
    TaskPresenter *freeResponsePresenter,
    TaskPresenter *correctKeywordsPresenter, TaskPresenter *passFailPresenter,
    UninitializedTaskPresenter *taskPresenter_)
    : model{model}, view{view}, consonantPresenter{consonantPresenter},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      freeResponsePresenter{freeResponsePresenter},
      correctKeywordsPresenter{correctKeywordsPresenter},
      passFailPresenter{passFailPresenter}, taskPresenter_{taskPresenter_} {
    model.attach(this);
}

void TestPresenterImpl::start() { view.show(); }

void TestPresenterImpl::stop() {
    taskPresenter_->stop();
    view.hideContinueTestingDialog();
    view.hide();
}

void TestPresenterImpl::notifyThatTrialHasStarted() {
    view.hideExitTestButton();
    view.hideNextTrialButton();
    taskPresenter_->notifyThatTrialHasStarted();
}

void TestPresenterImpl::trialComplete() {
    view.showExitTestButton();
    taskPresenter_->showResponseSubmission();
}

void TestPresenterImpl::notifyThatNextTrialIsReady() {
    view.hideContinueTestingDialog();
    view.showNextTrialButton();
}

void TestPresenterImpl::display(const std::string &s) { view.display(s); }

void TestPresenterImpl::secondaryDisplay(const std::string &s) {
    view.secondaryDisplay(s);
}

void TestPresenterImpl::showContinueTestingDialog() {
    view.showContinueTestingDialog();
}

void TestPresenterImpl::setContinueTestingDialogMessage(const std::string &s) {
    view.setContinueTestingDialogMessage(s);
}

static auto coordinateResponseMeasure(Method m) -> bool {
    return m == Method::adaptiveCoordinateResponseMeasure ||
        m == Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker ||
        m == Method::adaptiveCoordinateResponseMeasureWithDelayedMasker ||
        m == Method::adaptiveCoordinateResponseMeasureWithEyeTracking ||
        m == Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement ||
        m ==
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking ||
        m ==
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets;
}

static auto freeResponse(Method m) -> bool {
    return m == Method::fixedLevelFreeResponseWithAllTargets ||
        m == Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking ||
        m == Method::fixedLevelFreeResponseWithSilentIntervalTargets ||
        m == Method::fixedLevelFreeResponseWithTargetReplacement;
}

static auto correctKeywords(Method m) -> bool {
    return m == Method::adaptiveCorrectKeywords ||
        m == Method::adaptiveCorrectKeywordsWithEyeTracking;
}

static auto consonant(Method m) -> bool {
    return m == Method::fixedLevelConsonants;
}

void TestPresenterImpl::initialize(Method m) {
    displayTrialInformation(model, this);
    taskPresenter_->initialize(taskPresenter(m));
    taskPresenter_->start();
}

auto TestPresenterImpl::taskPresenter(Method m) -> TaskPresenter * {
    if (coordinateResponseMeasure(m))
        return coordinateResponseMeasurePresenter;
    if (consonant(m))
        return consonantPresenter;
    if (freeResponse(m))
        return freeResponsePresenter;
    if (correctKeywords(m))
        return correctKeywordsPresenter;
    return passFailPresenter;
}

void TestPresenterImpl::tellSubject(const std::string &s) {
    view.tellSubject(s);
}
}
