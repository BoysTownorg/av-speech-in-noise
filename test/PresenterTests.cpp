#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/Presenter.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
template <typename T> class Collection {
  public:
    explicit Collection(std::vector<T> items = {}) : items{std::move(items)} {}

    [[nodiscard]] auto contains(const T &item) const -> bool {
        return std::find(items.begin(), items.end(), item) != items.end();
    }

  private:
    std::vector<T> items{};
};

class ViewStub : public View {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    auto audioDevice() -> std::string override { return audioDevice_; }

    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() { return errorMessage_; }

    void eventLoop() override { eventLoopCalled_ = true; }

    [[nodiscard]] auto eventLoopCalled() const { return eventLoopCalled_; }

    auto browseForDirectory() -> std::string override {
        return browseForDirectoryResult_;
    }

    auto browseCancelled() -> bool override { return browseCancelled_; }

    auto browseForOpeningFile() -> std::string override {
        return browseForOpeningFileResult_;
    }

    void setBrowseForOpeningFileResult(std::string s) {
        browseForOpeningFileResult_ = std::move(s);
    }

    void setBrowseCancelled() { browseCancelled_ = true; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

    [[nodiscard]] auto audioDevices() const { return audioDevices_; }

    class TestSetupViewStub : public TestSetup {
      public:
        auto testSettingsFile() -> std::string override {
            return testSettingsFile_;
        }

        auto startingSnr() -> std::string override { return startingSnr_; }

        void setTestSettingsFile(std::string s) override {
            testSettingsFile_ = std::move(s);
        }

        void browseForTestSettingsFile() {
            listener_->browseForTestSettingsFile();
        }

        [[nodiscard]] auto transducers() const -> std::vector<std::string> {
            return transducers_;
        }

        void populateTransducerMenu(std::vector<std::string> v) override {
            transducers_ = std::move(v);
        }

        void confirmTestSetup() { listener_->confirmTestSetup(); }

        void playCalibration() { listener_->playCalibration(); }

        auto session() -> std::string override { return session_; }

        void setSession(std::string s) { session_ = std::move(s); }

        void show() override { shown_ = true; }

        [[nodiscard]] auto shown() const { return shown_; }

        void hide() override { hidden_ = true; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        void setRmeSetting(std::string s) { rmeSetting_ = std::move(s); }

        auto rmeSetting() -> std::string override { return rmeSetting_; }

        void setTransducer(std::string s) { transducer_ = std::move(s); }

        void setSubjectId(std::string s) { subjectId_ = std::move(s); }

        auto subjectId() -> std::string override { return subjectId_; }

        void setTesterId(std::string s) { testerId_ = std::move(s); }

        auto testerId() -> std::string override { return testerId_; }

        auto transducer() -> std::string override { return transducer_; }

        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }

        void setStartingSnr(std::string s) { startingSnr_ = std::move(s); }

      private:
        std::vector<std::string> transducers_;
        std::string startingSnr_{"0"};
        std::string subjectId_;
        std::string testerId_;
        std::string session_;
        std::string rmeSetting_;
        std::string transducer_;
        std::string testSettingsFile_;
        EventListener *listener_{};
        bool shown_{};
        bool hidden_{};
    };

    class SubjectViewStub : public CoordinateResponseMeasure {
      public:
        void show() override { shown_ = true; }

        [[nodiscard]] auto shown() const { return shown_; }

        void hide() override { hidden_ = true; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        auto whiteResponse() -> bool override { return grayResponse_; }

        void setGrayResponse() { grayResponse_ = true; }

        auto blueResponse() -> bool override { return blueResponse_; }

        void setBlueResponse() { blueResponse_ = true; }

        void setRedResponse() { redResponse_ = true; }

        void setGreenResponse() { greenResponse_ = true; }

        auto greenResponse() -> bool override { return greenResponse_; }

        void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

        [[nodiscard]] auto nextTrialButtonHidden() const {
            return nextTrialButtonHidden_;
        }

        void hideResponseButtons() override { responseButtonsHidden_ = true; }

        [[nodiscard]] auto responseButtonsHidden() const {
            return responseButtonsHidden_;
        }

        void showNextTrialButton() override { nextTrialButtonShown_ = true; }

        [[nodiscard]] auto nextTrialButtonShown() const {
            return nextTrialButtonShown_;
        }

        void showResponseButtons() override { responseButtonsShown_ = true; }

        [[nodiscard]] auto responseButtonsShown() const {
            return responseButtonsShown_;
        }

        void setNumberResponse(std::string s) {
            numberResponse_ = std::move(s);
        }

        auto numberResponse() -> std::string override {
            return numberResponse_;
        }

        void subscribe(EventListener *e) override { listener_ = e; }

        void submitResponse() { listener_->submitResponse(); }

        void playTrial() { listener_->playTrial(); }

      private:
        std::string numberResponse_{"0"};
        EventListener *listener_{};
        bool responseButtonsShown_{};
        bool responseButtonsHidden_{};
        bool shown_{};
        bool hidden_{};
        bool greenResponse_{};
        bool redResponse_{};
        bool blueResponse_{};
        bool grayResponse_{};
        bool nextTrialButtonHidden_{};
        bool nextTrialButtonShown_{};
    };

    class ExperimenterViewStub : public Experimenter {
      public:
        void declineContinuingTesting() {
            listener_->declineContinuingTesting();
        }

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

        void submitFailedTrial() { listener_->submitFailedTrial(); }

        void hideFreeResponseSubmission() override {
            responseSubmissionHidden_ = true;
        }

        [[nodiscard]] auto responseSubmissionHidden() const {
            return responseSubmissionHidden_;
        }

        void hideEvaluationButtons() override {
            evaluationButtonsHidden_ = true;
        }

        [[nodiscard]] auto evaluationButtonsHidden() const {
            return evaluationButtonsHidden_;
        }

        void showCorrectKeywordsSubmission() override {
            correctKeywordsEntryShown_ = true;
        }

        [[nodiscard]] auto correctKeywordsEntryShown() const {
            return correctKeywordsEntryShown_;
        }

        void hideCorrectKeywordsSubmission() override {
            correctKeywordsEntryHidden_ = true;
        }

        [[nodiscard]] auto correctKeywordsEntryHidden() const {
            return correctKeywordsEntryHidden_;
        }

        void showEvaluationButtons() override {
            evaluationButtonsShown_ = true;
        }

        [[nodiscard]] auto evaluationButtonsShown() const {
            return evaluationButtonsShown_;
        }

        void showFreeResponseSubmission() override {
            responseSubmissionShown_ = true;
        }

        [[nodiscard]] auto responseSubmissionShown() const {
            return responseSubmissionShown_;
        }

        void setCorrectKeywords(std::string s) {
            correctKeywords_ = std::move(s);
        }

        auto correctKeywords() -> std::string override {
            return correctKeywords_;
        }

        void show() override { shown_ = true; }

        [[nodiscard]] auto shown() const { return shown_; }

        void hide() override { hidden_ = true; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void setResponse(std::string s) { response_ = std::move(s); }

        auto freeResponse() -> std::string override { return response_; }

        void submitPassedTrial() { listener_->submitPassedTrial(); }

        void submitFreeResponse() { listener_->submitFreeResponse(); }

        void submitCorrectKeywords() { listener_->submitCorrectKeywords(); }

        void exitTest() { listener_->exitTest(); }

        void playTrial() { listener_->playTrial(); }

        void flagResponse() { flagged_ = true; }

        auto flagged() -> bool override { return flagged_; }

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

        [[nodiscard]] auto freeResponseCleared() const -> bool {
            return freeResponseCleared_;
        }

        void clearFreeResponse() override { freeResponseCleared_ = true; }

      private:
        std::string displayed_;
        std::string secondaryDisplayed_;
        std::string continueTestingDialogMessage_;
        std::string response_;
        std::string correctKeywords_{"0"};
        EventListener *listener_{};
        bool freeResponseCleared_{};
        bool exitTestButtonHidden_{};
        bool exitTestButtonShown_{};
        bool nextTrialButtonShown_{};
        bool nextTrialButtonHidden_{};
        bool evaluationButtonsShown_{};
        bool responseSubmissionShown_{};
        bool responseSubmissionHidden_{};
        bool evaluationButtonsHidden_{};
        bool correctKeywordsEntryShown_{};
        bool correctKeywordsEntryHidden_{};
        bool continueTestingDialogShown_{};
        bool continueTestingDialogHidden_{};
        bool shown_{};
        bool hidden_{};
        bool flagged_{};
    };

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};
};

class TestSettingsInterpreterStub : public TestSettingsInterpreter {
  public:
    explicit TestSettingsInterpreterStub(const Calibration &calibration_ = {})
        : calibration_{calibration_} {}

    auto calibration(const std::string &t) -> Calibration override {
        text_ = t;
        return calibration_;
    }

    void initialize(Model &m, const std::string &t, const TestIdentity &id,
        SNR snr) override {
        startingSnr_ = snr.dB;
        text_ = t;
        identity_ = id;
        if (initializeAnyTestOnApply_)
            m.initialize(AdaptiveTest{});
    }

    [[nodiscard]] auto text() const -> std::string { return text_; }

    [[nodiscard]] auto identity() const -> TestIdentity { return identity_; }

    [[nodiscard]] auto textForMethodQuery() const -> std::string {
        return textForMethodQuery_;
    }

    [[nodiscard]] auto startingSnr() const -> int { return startingSnr_; }

    void setMethod(Method m) { method_ = m; }

    auto method(const std::string &t) -> Method override {
        textForMethodQuery_ = t;
        return method_;
    }

    void initializeAnyTestOnApply() { initializeAnyTestOnApply_ = true; }

  private:
    std::string text_;
    std::string textForMethodQuery_;
    TestIdentity identity_{};
    int startingSnr_{};
    const Calibration &calibration_;
    Method method_{};
    bool initializeAnyTestOnApply_{};
};

class TextFileReaderStub : public TextFileReader {
  public:
    [[nodiscard]] auto filePath() const -> std::string { return filePath_; }

    auto read(const LocalUrl &s) -> std::string override {
        filePath_ = s.path;
        return read_;
    }

    void setRead(std::string s) { read_ = std::move(s); }

  private:
    std::string filePath_;
    std::string read_;
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run() = 0;
};

void run(UseCase &useCase) { useCase.run(); }

class ConditionUseCase : public virtual UseCase {
  public:
    virtual auto condition(ModelStub &) -> Condition = 0;
};

class LevelUseCase : public virtual UseCase {
  public:
    virtual auto fullScaleLevel(ModelStub &) -> int = 0;
};

class PlayingCalibration : public LevelUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit PlayingCalibration(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override { view->playCalibration(); }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return m.calibration().fullScaleLevel.dB_SPL;
    }
};

class ConfirmingTestSetup : public virtual UseCase {};

void confirmTestSetup(ViewStub::TestSetupViewStub *view) {
    view->confirmTestSetup();
}

void setMethod(TestSettingsInterpreterStub &interpeter, Method m) {
    interpeter.setMethod(m);
}

class ConfirmingAdaptiveCoordinateResponseMeasureTest
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCoordinateResponseMeasureTest(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptiveCoordinateResponseMeasure);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    explicit ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    explicit ConfirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptivePassFailTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptivePassFailTest(ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptivePassFail);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptivePassFailTestWithEyeTracking
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptivePassFailTestWithEyeTracking(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptivePassFailWithEyeTracking);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCorrectKeywordsTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCorrectKeywordsTest(ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptiveCorrectKeywords);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCorrectKeywordsTestWithEyeTracking
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCorrectKeywordsTestWithEyeTracking(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptiveCorrectKeywordsWithEyeTracking);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseWithTargetReplacementTest
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseWithTargetReplacementTest(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(
            interpreter, Method::fixedLevelFreeResponseWithTargetReplacement);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
        confirmTestSetup(view);
    }
};

class
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::
                fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargets
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargets(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {

        setMethod(interpreter,
            Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::fixedLevelFreeResponseWithSilentIntervalTargets);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseTestWithAllTargets
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseTestWithAllTargets(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::fixedLevelFreeResponseWithAllTargets);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
        confirmTestSetup(view);
    }
};

class TrialSubmission : public virtual UseCase {
  public:
    virtual auto nextTrialButtonShown() -> bool = 0;
    virtual auto responseViewShown() -> bool = 0;
    virtual auto responseViewHidden() -> bool = 0;
};

class RespondingFromSubject : public TrialSubmission {
    ViewStub::SubjectViewStub *view;

  public:
    explicit RespondingFromSubject(ViewStub::SubjectViewStub *view)
        : view{view} {}

    void run() override { view->submitResponse(); }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view->responseButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view->responseButtonsHidden();
    }
};

class SubmittingFreeResponse : public TrialSubmission {
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit SubmittingFreeResponse(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.submitFreeResponse(); }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view.responseSubmissionShown();
    }

    auto responseViewHidden() -> bool override {
        return view.responseSubmissionHidden();
    }
};

class ExitingTest : public UseCase {
    ViewStub::ExperimenterViewStub *view;

  public:
    explicit ExitingTest(ViewStub::ExperimenterViewStub *view) : view{view} {}

    void run() override { view->exitTest(); }
};

class SubmittingPassedTrial : public TrialSubmission {
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit SubmittingPassedTrial(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.submitPassedTrial(); }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view.evaluationButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view.evaluationButtonsHidden();
    }
};

class SubmittingFailedTrial : public TrialSubmission {
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit SubmittingFailedTrial(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.submitFailedTrial(); }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view.evaluationButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view.evaluationButtonsHidden();
    }
};

class SubmittingCorrectKeywords : public TrialSubmission {
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit SubmittingCorrectKeywords(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.submitCorrectKeywords(); }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view.correctKeywordsEntryShown();
    }

    auto responseViewHidden() -> bool override {
        return view.correctKeywordsEntryHidden();
    }
};

class DecliningContinuingTesting : public UseCase {
  public:
    explicit DecliningContinuingTesting(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.declineContinuingTesting(); }

  private:
    ViewStub::ExperimenterViewStub &view;
};

class AcceptingContinuingTesting : public UseCase {
  public:
    explicit AcceptingContinuingTesting(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.acceptContinuingTesting(); }

  private:
    ViewStub::ExperimenterViewStub &view;
};

class PlayingTrial : public virtual UseCase {
  public:
    virtual auto nextTrialButtonHidden() -> bool = 0;
    virtual auto nextTrialButtonShown() -> bool = 0;
};

class PlayingTrialFromSubject : public PlayingTrial {
    ViewStub::SubjectViewStub *view;

  public:
    explicit PlayingTrialFromSubject(ViewStub::SubjectViewStub *view)
        : view{view} {}

    void run() override { view->playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view->nextTrialButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }
};

class PlayingTrialFromExperimenter : public PlayingTrial {
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit PlayingTrialFromExperimenter(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view.nextTrialButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }
};

class BrowsingUseCase : public virtual UseCase {
  public:
    virtual void setResult(ViewStub &, std::string) = 0;
};

class BrowsingEnteredPathUseCase : public virtual BrowsingUseCase {
  public:
    virtual auto entry() -> std::string = 0;
    virtual void setEntry(std::string) = 0;
};

class BrowsingForTestSettingsFile : public BrowsingEnteredPathUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit BrowsingForTestSettingsFile(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override { view->browseForTestSettingsFile(); }

    void setResult(ViewStub &view_, std::string s) override {
        view_.setBrowseForOpeningFileResult(s);
    }

    auto entry() -> std::string override { return view->testSettingsFile(); }

    void setEntry(std::string s) override {
        view->setTestSettingsFile(std::move(s));
    }
};

class PresenterConstructionTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    ViewStub view;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::CoordinateResponseMeasure subject{&subjectView};
    Presenter::Experimenter experimenter{&experimenterView};
    TestSettingsInterpreterStub testSettingsInterpreter;
    TextFileReaderStub textFileReader;

    auto construct() -> Presenter {
        return {model, view, testSetup, subject, experimenter,
            testSettingsInterpreter, textFileReader};
    }
};

auto entry(BrowsingEnteredPathUseCase &useCase) -> std::string {
    return useCase.entry();
}

void assertEntryEquals(
    BrowsingEnteredPathUseCase &useCase, const std::string &s) {
    assertEqual(s, entry(useCase));
}

void assertHidesPlayTrialButton(PlayingTrial &useCase) {
    run(useCase);
    assertTrue(useCase.nextTrialButtonHidden());
}

void assertConfirmTestSetupShowsNextTrialButton(
    ConfirmingTestSetup &confirmingTest, PlayingTrial &playingTrial) {
    run(confirmingTest);
    assertTrue(playingTrial.nextTrialButtonShown());
}

void assertShowsNextTrialButton(TrialSubmission &useCase) {
    run(useCase);
    assertTrue(useCase.nextTrialButtonShown());
}

void assertResponseViewHidden(TrialSubmission &useCase) {
    run(useCase);
    assertTrue(useCase.responseViewHidden());
}

void submitResponse(ViewStub::SubjectViewStub &view) { view.submitResponse(); }

void submitFreeResponse(ViewStub::ExperimenterViewStub &view) {
    view.submitFreeResponse();
}

void exitTest(ViewStub::ExperimenterViewStub &view) { view.exitTest(); }

void playCalibration(ViewStub::TestSetupViewStub &view) {
    view.playCalibration();
}

auto shown(ViewStub::TestSetupViewStub &view) -> bool { return view.shown(); }

void assertShown(ViewStub::TestSetupViewStub &view) { assertTrue(shown(view)); }

auto hidden(ViewStub::TestSetupViewStub &view) -> bool { return view.hidden(); }

auto hidden(ViewStub::ExperimenterViewStub &view) -> bool {
    return view.hidden();
}

auto shown(ViewStub::SubjectViewStub &view) -> bool { return view.shown(); }

void assertHidden(ViewStub::SubjectViewStub &view) {
    assertTrue(view.hidden());
}

void completeTrial(ModelStub &model) { model.completeTrial(); }

auto errorMessage(ViewStub &view) -> std::string { return view.errorMessage(); }

void assertPassedColor(ModelStub &model, coordinate_response_measure::Color c) {
    assertEqual(c, model.responseParameters().color);
}

auto calibration(ModelStub &model) -> const Calibration & {
    return model.calibration();
}

void setAudioDevice(ViewStub &view, std::string s) {
    view.setAudioDevice(std::move(s));
}

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

auto trialPlayed(ModelStub &model) -> bool { return model.trialPlayed(); }

void setCorrectKeywords(ViewStub::ExperimenterViewStub &view, std::string s) {
    view.setCorrectKeywords(std::move(s));
}

class PresenterTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub view;
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::Experimenter experimenter{&experimenterView};
    Presenter::CoordinateResponseMeasure subject{&subjectView};
    Calibration interpretedCalibration;
    TestSettingsInterpreterStub testSettingsInterpreter{interpretedCalibration};
    TextFileReaderStub textFileReader;
    Presenter presenter{model, view, testSetup, subject, experimenter,
        testSettingsInterpreter, textFileReader};
    BrowsingForTestSettingsFile browsingForTestSettingsFile{&setupView};
    ConfirmingAdaptiveCoordinateResponseMeasureTest
        confirmingAdaptiveCoordinateResponseMeasureTest{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptivePassFailTest confirmingAdaptivePassFailTest{
        &setupView, testSettingsInterpreter};
    ConfirmingAdaptivePassFailTestWithEyeTracking
        confirmingAdaptivePassFailTestWithEyeTracking{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseWithTargetReplacementTest
        confirmingFixedLevelFreeResponseWithTargetReplacementTest{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCorrectKeywordsTest confirmingAdaptiveCorrectKeywordsTest{
        &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCorrectKeywordsTestWithEyeTracking
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargets
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseTestWithAllTargets
        confirmingFixedLevelFreeResponseTestWithAllTargets{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking{
            &setupView, testSettingsInterpreter};
    PlayingCalibration playingCalibration{&setupView};
    PlayingTrialFromSubject playingTrialFromSubject{&subjectView};
    PlayingTrialFromExperimenter playingTrialFromExperimenter{experimenterView};
    RespondingFromSubject respondingFromSubject{&subjectView};
    SubmittingFreeResponse submittingFreeResponse{experimenterView};
    SubmittingPassedTrial submittingPassedTrial{experimenterView};
    SubmittingCorrectKeywords submittingCorrectKeywords{experimenterView};
    SubmittingFailedTrial submittingFailedTrial{experimenterView};
    DecliningContinuingTesting decliningContinuingTesting{experimenterView};
    AcceptingContinuingTesting acceptingContinuingTesting{experimenterView};
    ExitingTest exitingTest{&experimenterView};

    void assertBrowseResultPassedToEntry(BrowsingEnteredPathUseCase &useCase) {
        setBrowsingResult(useCase, "a");
        run(useCase);
        assertEntryEquals(useCase, "a");
    }

    void setBrowsingResult(BrowsingEnteredPathUseCase &useCase, std::string s) {
        useCase.setResult(view, std::move(s));
    }

    void assertCancellingBrowseDoesNotChangePath(
        BrowsingEnteredPathUseCase &useCase) {
        useCase.setEntry("a");
        setBrowsingResult(useCase, "b");
        view.setBrowseCancelled();
        run(useCase);
        assertEntryEquals(useCase, "a");
    }

    void assertAudioDevicePassedToTrial(PlayingTrial &useCase) {
        setAudioDevice(view, "a");
        run(useCase);
        assertEqual("a", model.trialParameters().audioDevice);
    }

    void assertPlaysTrial(UseCase &useCase) {
        run(useCase);
        assertTrue(trialPlayed(model));
    }

    void assertHidesExitTestButton(PlayingTrial &useCase) {
        run(useCase);
        assertTrue(experimenterView.exitTestButtonHidden());
    }

    void assertCompleteTestShowsSetupView(TrialSubmission &useCase) {
        setTestComplete(model);
        run(useCase);
        assertShown(setupView);
    }

    void assertShowsSetupView(UseCase &useCase) {
        run(useCase);
        assertShown(setupView);
    }

    void assertHidesContinueTestingDialog(UseCase &useCase) {
        run(useCase);
        assertTrue(experimenterView.continueTestingDialogHidden());
    }

    void assertIncompleteTestDoesNotShowSetupView(TrialSubmission &useCase) {
        run(useCase);
        assertFalse(shown(setupView));
    }

    void assertCompleteTestHidesExperimenterView(UseCase &useCase) {
        setTestComplete(model);
        assertHidesExperimenterView(useCase);
    }

    void assertHidesExperimenterView(UseCase &useCase) {
        run(useCase);
        assertTrue(hidden(experimenterView));
    }

    void assertCompleteTestDoesNotPlayTrial(UseCase &useCase) {
        setTestComplete(model);
        run(useCase);
        assertFalse(trialPlayed(model));
    }

    void assertDoesNotHideExperimenterView(TrialSubmission &useCase) {
        run(useCase);
        assertFalse(hidden(experimenterView));
    }

    void assertHidesTestSetupView(UseCase &useCase) {
        run(useCase);
        assertTrue(hidden(setupView));
    }

    void assertDoesNotHideTestSetupView(UseCase &useCase) {
        run(useCase);
        assertFalse(hidden(setupView));
    }

    void assertShowsExperimenterView(UseCase &useCase) {
        run(useCase);
        assertTrue(experimenterView.shown());
    }

    void assertDoesNotShowSubjectView(UseCase &useCase) {
        run(useCase);
        assertFalse(shown(subjectView));
    }

    void assertPassesTestSettingsFileToTextFileReader(UseCase &useCase) {
        setupView.setTestSettingsFile("a");
        run(useCase);
        assertEqual("a", textFileReader.filePath());
    }

    void assertPassesTestSettingsTextToTestSettingsInterpreter(
        UseCase &useCase) {
        textFileReader.setRead("a");
        run(useCase);
        assertEqual("a", testSettingsInterpreter.text());
    }

    void assertPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery(
        ConfirmingTestSetup &useCase) {
        textFileReader.setRead("a");
        run(useCase);
        assertEqual("a", testSettingsInterpreter.textForMethodQuery());
    }

    void assertInvalidSnrShowsMessage(ConfirmingTestSetup &useCase) {
        setupView.setStartingSnr("a");
        run(useCase);
        assertEqual("\"a\" is not a valid starting SNR.", errorMessage(view));
    }

    void assertPassesStartingSnr(ConfirmingTestSetup &useCase) {
        setupView.setStartingSnr("1");
        run(useCase);
        assertEqual(1, testSettingsInterpreter.startingSnr());
    }

    void assertPassesSubjectId(ConfirmingTestSetup &useCase) {
        setupView.setSubjectId("b");
        run(useCase);
        assertEqual("b", testSettingsInterpreter.identity().subjectId);
    }

    void assertPassesTesterId(ConfirmingTestSetup &useCase) {
        setupView.setTesterId("c");
        run(useCase);
        assertEqual("c", testSettingsInterpreter.identity().testerId);
    }

    void assertPassesSession(ConfirmingTestSetup &useCase) {
        setupView.setSession("e");
        run(useCase);
        assertEqual("e", testSettingsInterpreter.identity().session);
    }

    void assertPassesRmeSetting(ConfirmingTestSetup &useCase) {
        setupView.setRmeSetting("e");
        run(useCase);
        assertEqual("e", testSettingsInterpreter.identity().rmeSetting);
    }

    void assertPassesTransducer(ConfirmingTestSetup &useCase) {
        setupView.setTransducer("a");
        run(useCase);
        assertEqual("a", testSettingsInterpreter.identity().transducer);
    }

    void assertCompleteTrialShowsResponseView(
        ConfirmingTestSetup &useCase, TrialSubmission &trialSubmission) {
        run(useCase);
        completeTrial(model);
        assertTrue(trialSubmission.responseViewShown());
    }

    void assertShowsTrialNumber(UseCase &useCase) {
        model.setTrialNumber(1);
        run(useCase);
        assertEqual("Trial 1", experimenterView.displayed());
    }

    void assertShowsTargetFileName(UseCase &useCase) {
        model.setTargetFileName("a");
        run(useCase);
        assertEqual("a", experimenterView.secondaryDisplayed());
    }

    void assertShowsSubjectView(UseCase &useCase) {
        run(useCase);
        assertTrue(shown(subjectView));
    }

    void assertExitTestAfterCompletingTrialHidesResponseSubmission(
        UseCase &useCase, TrialSubmission &submission) {
        run(useCase);
        completeTrial(model);
        exitTest(experimenterView);
        assertTrue(submission.responseViewHidden());
    }

    void assertCompleteTestShowsContinueTestingDialog(UseCase &useCase) {
        setTestComplete(model);
        run(useCase);
        assertTrue(experimenterView.continueTestingDialogShown());
    }

    void assertCompleteTestShowsThresholds(UseCase &useCase) {
        setTestComplete(model);
        model.setAdaptiveTestResults({{"a", 1.}, {"b", 2.}, {"c", 3.}});
        run(useCase);
        assertEqual("thresholds (targets: dB SNR)\na: 1\nb: 2\nc: 3",
            experimenterView.continueTestingDialogMessage());
    }

    void assertCompleteTestHidesResponse(TrialSubmission &useCase) {
        setTestComplete(model);
        run(useCase);
        assertTrue(useCase.responseViewHidden());
    }
};

class RequestFailingModel : public Model {
    std::string errorMessage{};

  public:
    auto trialNumber() -> int override { return 0; }

    auto targetFileName() -> std::string override { return {}; }

    void setErrorMessage(std::string s) { errorMessage = std::move(s); }

    void initialize(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithTargetReplacement(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithTargetReplacementAndEyeTracking(
        const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithSilentIntervalTargets(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithAllTargets(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithAllTargetsAndEyeTracking(
        const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithSingleSpeaker(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithDelayedMasker(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithCyclicTargets(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithCyclicTargetsAndEyeTracking(
        const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithEyeTracking(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void playTrial(const AudioSettings &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const coordinate_response_measure::Response &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const FreeResponse &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const CorrectKeywords &) override {
        throw RequestFailure{errorMessage};
    }

    void playCalibration(const Calibration &) override {
        throw RequestFailure{errorMessage};
    }

    auto testComplete() -> bool override { return {}; }
    auto audioDevices() -> AudioDevices override { return {}; }
    auto adaptiveTestResults() -> AdaptiveTestResults override { return {}; }
    void subscribe(EventListener *) override {}
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void restartAdaptiveTestWhilePreservingTargets() override {}
};

class PresenterFailureTests : public ::testing::Test {
  protected:
    RequestFailingModel failure;
    ModelStub defaultModel;
    Model *model{&defaultModel};
    ViewStub view;
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::CoordinateResponseMeasure subject{&subjectView};
    Presenter::Experimenter experimenter{&experimenterView};
    TestSettingsInterpreterStub testSettingsInterpreter;
    TextFileReaderStub textFileReader;

    void useFailingModel(std::string s = {}) {
        failure.setErrorMessage(std::move(s));
        model = &failure;
        testSettingsInterpreter.initializeAnyTestOnApply();
    }

    void confirmTestSetup() {
        Presenter presenter{*model, view, testSetup, subject, experimenter,
            testSettingsInterpreter, textFileReader};
        setupView.confirmTestSetup();
    }

    void assertConfirmTestSetupShowsErrorMessage(const std::string &s) {
        confirmTestSetup();
        assertEqual(s, errorMessage(view));
    }

    void assertConfirmTestSetupDoesNotHideSetupView() {
        confirmTestSetup();
        assertFalse(setupView.hidden());
    }
};

TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
    model.setAudioDevices({"a", "b", "c"});
    construct();
    assertEqual({"a", "b", "c"}, view.audioDevices());
}

TEST_F(PresenterConstructionTests, populatesTransducerMenu) {
    construct();
    assertEqual({name(Transducer::headphone), name(Transducer::oneSpeaker),
                    name(Transducer::twoSpeakers)},
        setupView.transducers());
}

#define PRESENTER_TEST(a) TEST_F(PresenterTests, a)

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestDoesNotShowExperimentersNextTrialButton) {
    run(confirmingAdaptiveCoordinateResponseMeasureTest);
    assertFalse(experimenterView.nextTrialButtonShown());
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesCorrectKeywordsSubmissionForAdaptiveCorrectKeywordsTest) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesCorrectKeywordsSubmissionForAdaptiveCorrectKeywordsTestWithEyeTracking) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking,
        submittingCorrectKeywords);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesFreeResponseSubmissionForFixedLevelFreeResponseTestWithAllTargets) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesFreeResponseSubmissionForFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking,
        submittingFreeResponse);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesPassFailSubmissionForAdaptivePassFailTest) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesPassFailSubmissionForAdaptivePassFailTestWithEyeTracking) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptivePassFailTestWithEyeTracking, submittingPassedTrial);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsTargetFileName) {
    assertShowsTargetFileName(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingShowsTargetFileName) {
    assertShowsTargetFileName(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(confirmingAdaptiveClosedSetTestShowsTargetFileName) {
    assertShowsTargetFileName(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsTargetFileName) {
    assertShowsTargetFileName(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingCoordinateResponseShowsTargetFileName) {
    assertShowsTargetFileName(respondingFromSubject);
}

PRESENTER_TEST(submittingFreeResponseShowsTargetFileName) {
    assertShowsTargetFileName(submittingFreeResponse);
}

PRESENTER_TEST(submittingCorrectKeywordsPassesCorrectKeywords) {
    setCorrectKeywords(experimenterView, "1");
    run(submittingCorrectKeywords);
    assertEqual(1, model.correctKeywords());
}

PRESENTER_TEST(submittingInvalidCorrectKeywordsShowsErrorMessage) {
    setCorrectKeywords(experimenterView, "a");
    run(submittingCorrectKeywords);
    assertEqual("\"a\" is not a valid number.", errorMessage(view));
}

PRESENTER_TEST(submittingInvalidCorrectKeywordsDoesNotHideEntry) {
    setCorrectKeywords(experimenterView, "a");
    run(submittingCorrectKeywords);
    assertFalse(submittingCorrectKeywords.responseViewHidden());
}

PRESENTER_TEST(
    acceptingContinuingTestingRestartsAdaptiveTestWhilePreservingCyclicTargets) {
    run(acceptingContinuingTesting);
    assertTrue(model.adaptiveTestRestartedWhilePreservingCyclicTargets());
}

PRESENTER_TEST(acceptingContinuingTestingHidesContinueTestingDialog) {
    assertHidesContinueTestingDialog(acceptingContinuingTesting);
}

PRESENTER_TEST(decliningContinuingTestingHidesContinueTestingDialog) {
    assertHidesContinueTestingDialog(decliningContinuingTesting);
}

PRESENTER_TEST(decliningContinuingTestingShowsSetupView) {
    assertShowsSetupView(decliningContinuingTesting);
}

PRESENTER_TEST(
    submittingCorrectKeywordsShowsContinueTestingDialogWhenComplete) {
    assertCompleteTestShowsContinueTestingDialog(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingPassedTrialShowsContinueTestingDialogWhenComplete) {
    assertCompleteTestShowsContinueTestingDialog(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsContinueTestingDialogWhenComplete) {
    assertCompleteTestShowsContinueTestingDialog(submittingFailedTrial);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsThresholdsWhenTestingComplete) {
    assertCompleteTestShowsThresholds(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingPassedTrialShowsThresholdsWhenTestingComplete) {
    assertCompleteTestShowsThresholds(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsThresholdsWhenTestingComplete) {
    assertCompleteTestShowsThresholds(submittingFailedTrial);
}

PRESENTER_TEST(submittingCorrectKeywordsHidesSubmissionEvenWhenTestComplete) {
    assertCompleteTestHidesResponse(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingPassedTrialHidesSubmissionEvenWhenTestComplete) {
    assertCompleteTestHidesResponse(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialHidesSubmissionEvenWhenTestComplete) {
    assertCompleteTestHidesResponse(submittingFailedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterCorrectKeywordsEntryForAdaptiveCorrectKeywordsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterCorrectKeywordsEntryForAdaptiveCorrectKeywordsTestWithEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking,
        submittingCorrectKeywords);
}

PRESENTER_TEST(callsEventLoopWhenRun) {
    presenter.run();
    assertTrue(view.eventLoopCalled());
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptivePassFailTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithEyeTrackingShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptivePassFailTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestDoesNotHideSetupViewWhenTestComplete) {
    setTestComplete(model);
    assertDoesNotHideTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsSubjectView) {
    assertShowsSubjectView(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTrackingShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsSubjectView) {
    assertShowsSubjectView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingShowsSubjectView) {
    assertShowsSubjectView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsSubjectView) {
    assertShowsSubjectView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestDoesNotShowSubjectViewWhenTestComplete) {
    setTestComplete(model);
    assertDoesNotShowSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithEyeTrackingDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptivePassFailTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTrackingDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(playCalibrationPassesLevel) {
    interpretedCalibration.level.dB_SPL = 1;
    playCalibration(setupView);
    assertEqual(1, calibration(model).level.dB_SPL);
}

PRESENTER_TEST(playingCalibrationPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(playingCalibration);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery) {
    assertPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    playingCalibrationPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(playingCalibration);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesStartingSnr) {
    assertPassesStartingSnr(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidStartingSnrShowsMessage) {
    assertInvalidSnrShowsMessage(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestWithEyeTrackingPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptivePassFailTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(playCalibrationPassesFilePath) {
    interpretedCalibration.fileUrl.path = "a";
    playCalibration(setupView);
    assertEqual("a", calibration(model).fileUrl.path);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSession) {
    assertPassesSession(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesSession) {
    assertPassesSession(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesSession) {
    assertPassesSession(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesSession) {
    assertPassesSession(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesRmeSetting) {
    assertPassesRmeSetting(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesTransducer) {
    assertPassesTransducer(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCorrectKeywordsTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCoordinateResponseMeasureTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptivePassFailTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithEyeTrackingShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptivePassFailTestWithEyeTracking,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTrackingShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(respondingFromSubjectPlaysTrial) {
    assertPlaysTrial(respondingFromSubject);
}

PRESENTER_TEST(playingTrialFromSubjectPlaysTrial) {
    assertPlaysTrial(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromExperimenterPlaysTrial) {
    assertPlaysTrial(playingTrialFromExperimenter);
}

PRESENTER_TEST(playingTrialHidesNextTrialButton) {
    assertHidesPlayTrialButton(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialHidesNextTrialButtonForExperimenter) {
    assertHidesPlayTrialButton(playingTrialFromExperimenter);
}

PRESENTER_TEST(playingTrialFromSubjectHidesExitTestButton) {
    assertHidesExitTestButton(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromSubjectPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromExperimenterPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromExperimenter);
}

PRESENTER_TEST(playCalibrationPassesAudioDevice) {
    setAudioDevice(view, "b");
    playCalibration(setupView);
    assertEqual("b", calibration(model).audioDevice);
}

PRESENTER_TEST(subjectResponsePassesNumberResponse) {
    subjectView.setNumberResponse("1");
    submitResponse(subjectView);
    assertEqual(1, model.responseParameters().number);
}

PRESENTER_TEST(subjectResponsePassesGreenColor) {
    subjectView.setGreenResponse();
    submitResponse(subjectView);
    assertPassedColor(model, coordinate_response_measure::Color::green);
}

PRESENTER_TEST(subjectResponsePassesRedColor) {
    subjectView.setRedResponse();
    submitResponse(subjectView);
    assertPassedColor(model, coordinate_response_measure::Color::red);
}

PRESENTER_TEST(subjectResponsePassesBlueColor) {
    subjectView.setBlueResponse();
    submitResponse(subjectView);
    assertPassedColor(model, coordinate_response_measure::Color::blue);
}

PRESENTER_TEST(subjectResponsePassesWhiteColor) {
    subjectView.setGrayResponse();
    submitResponse(subjectView);
    assertPassedColor(model, coordinate_response_measure::Color::white);
}

PRESENTER_TEST(experimenterResponsePassesResponse) {
    experimenterView.setResponse("a");
    submitFreeResponse(experimenterView);
    assertEqual("a", model.freeResponse().response);
}

PRESENTER_TEST(experimenterResponseFlagsResponse) {
    experimenterView.flagResponse();
    submitFreeResponse(experimenterView);
    assertTrue(model.freeResponse().flagged);
}

PRESENTER_TEST(passedTrialSubmitsCorrectResponse) {
    run(submittingPassedTrial);
    assertTrue(model.correctResponseSubmitted());
}

PRESENTER_TEST(failedTrialSubmitsIncorrectResponse) {
    run(submittingFailedTrial);
    assertTrue(model.incorrectResponseSubmitted());
}

PRESENTER_TEST(respondFromSubjectShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(respondingFromSubject);
}

PRESENTER_TEST(respondFromExperimenterShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingFreeResponse);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingCorrectKeywords);
}

PRESENTER_TEST(respondFromSubjectDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(respondingFromSubject);
}

PRESENTER_TEST(respondFromExperimenterDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingFailedTrial);
}

PRESENTER_TEST(respondFromSubjectHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(respondingFromSubject);
}

PRESENTER_TEST(respondFromExperimenterHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingFreeResponse);
}

PRESENTER_TEST(submitCoordinateResponseDoesNotPlayTrialWhenTestComplete) {
    assertCompleteTestDoesNotPlayTrial(respondingFromSubject);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingCorrectKeywords);
}

PRESENTER_TEST(
    respondFromSubjectDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(respondingFromSubject);
}

PRESENTER_TEST(
    respondFromExperimenterDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingFailedTrial);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsNextTrialButton) {
    assertShowsNextTrialButton(submittingCorrectKeywords);
}

PRESENTER_TEST(experimenterResponseShowsNextTrialButton) {
    assertShowsNextTrialButton(submittingFreeResponse);
}

PRESENTER_TEST(subjectPassedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingPassedTrial);
}

PRESENTER_TEST(subjectFailedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingFailedTrial);
}

PRESENTER_TEST(submittingCorrectKeywordsHidesCorrectKeywordsEntry) {
    assertResponseViewHidden(submittingCorrectKeywords);
}

PRESENTER_TEST(experimenterResponseHidesResponseSubmission) {
    assertResponseViewHidden(submittingFreeResponse);
}

PRESENTER_TEST(correctResponseHidesEvaluationButtons) {
    assertResponseViewHidden(submittingPassedTrial);
}

PRESENTER_TEST(incorrectResponseHidesEvaluationButtons) {
    assertResponseViewHidden(submittingFailedTrial);
}

PRESENTER_TEST(subjectResponseHidesResponseButtons) {
    assertResponseViewHidden(respondingFromSubject);
}

PRESENTER_TEST(subjectResponseHidesSubjectViewWhenTestComplete) {
    setTestComplete(model);
    submitResponse(subjectView);
    assertHidden(subjectView);
}

PRESENTER_TEST(exitTestHidesSubjectView) {
    exitTest(experimenterView);
    assertHidden(subjectView);
}

PRESENTER_TEST(decliningContinuingTestingHidesExperimenterView) {
    assertHidesExperimenterView(decliningContinuingTesting);
}

PRESENTER_TEST(exitTestHidesExperimenterView) {
    assertHidesExperimenterView(exitingTest);
}

PRESENTER_TEST(exitTestHidesResponseButtons) {
    run(exitingTest);
    assertTrue(respondingFromSubject.responseViewHidden());
}

PRESENTER_TEST(exitTestShowsTestSetupView) {
    exitTest(experimenterView);
    assertShown(setupView);
}

PRESENTER_TEST(browseForTestSettingsFileUpdatesTestSettingsFile) {
    assertBrowseResultPassedToEntry(browsingForTestSettingsFile);
}

PRESENTER_TEST(browseForTestSettingsCancelDoesNotChangeTestSettingsFile) {
    assertCancellingBrowseDoesNotChangePath(browsingForTestSettingsFile);
}

PRESENTER_TEST(completingTrialShowsExitTestButton) {
    completeTrial(model);
    assertTrue(experimenterView.exitTestButtonShown());
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsTrialNumber) {
    assertShowsTrialNumber(submittingCorrectKeywords);
}

PRESENTER_TEST(acceptingContinuingTestingDialogShowsTrialNumber) {
    assertShowsTrialNumber(acceptingContinuingTesting);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTrackingShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(submittingResponseFromExperimenterShowsTrialNumber) {
    assertShowsTrialNumber(submittingFreeResponse);
}

PRESENTER_TEST(submittingResponseFromSubjectShowsTrialNumber) {
    assertShowsTrialNumber(respondingFromSubject);
}

PRESENTER_TEST(submittingPassedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingFailedTrial);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTest, respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureSingleSpeakerTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTestWithDelayedMasker) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTestWithEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureTestWithTargetReplacement) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterEvaluationButtonsForAdaptivePassFailTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterEvaluationButtonsForAdaptivePassFailTestWithEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptivePassFailTestWithEyeTracking, submittingPassedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseTestWithAllTargets) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        submittingFreeResponse);
}

PRESENTER_TEST(playCalibrationPassesFullScaleLevel) {
    interpretedCalibration.fullScaleLevel.dB_SPL = 1;
    run(playingCalibration);
    assertEqual(1, calibration(model).fullScaleLevel.dB_SPL);
}

PRESENTER_TEST(completingTrialClearsFreeResponseForFixedLevelFreeResponseTest) {
    run(confirmingFixedLevelFreeResponseWithTargetReplacementTest);
    completeTrial(model);
    assertTrue(experimenterView.freeResponseCleared());
}

TEST_F(PresenterFailureTests,
    initializeTestShowsErrorMessageWhenModelFailsRequest) {
    useFailingModel("a");
    assertConfirmTestSetupShowsErrorMessage("a");
}

TEST_F(PresenterFailureTests,
    initializeTestDoesNotHideSetupViewWhenModelFailsRequest) {
    useFailingModel();
    assertConfirmTestSetupDoesNotHideSetupView();
}
}
}
