#include "assert-utility.h"
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
    template<typename T>
    class Collection {
        std::vector<T> items{};
    public:
        Collection(std::vector<T> items = {}) : items{std::move(items)} {}
        
        bool contains(T item) const {
            return std::find(items.begin(), items.end(), item) != items.end();
        }
    };
    
    class ModelStub : public av_coordinated_response_measure::Model {
        Test testParameters_{};
        Calibration calibrationParameters_{};
        AudioSettings trialParameters_{};
        SubjectResponse responseParameters_{};
        std::vector<std::string> audioDevices_{};
        EventListener *listener_{};
        bool testComplete_{};
        bool trialPlayed_{};
    public:
        void completeTrial() {
            listener_->trialComplete();
        }
        
        void setAudioDevices(std::vector<std::string> v) {
            audioDevices_ = std::move(v);
        }
        
        auto &responseParameters() const {
            return responseParameters_;
        }
        
        auto &trialParameters() const {
            return trialParameters_;
        }
        
        void setTestIncomplete() {
            testComplete_ = false;
        }
        
        void setTestComplete() {
            testComplete_ = true;
        }
        
        bool testComplete() override {
            return testComplete_;
        }
        
        void playTrial(const AudioSettings &p) override {
            trialParameters_ = p;
            trialPlayed_ = true;
        }
        
        void initializeTest(const Test &p) override {
            testParameters_ = p;
        }
        
        std::vector<std::string> audioDevices() override {
            return audioDevices_;
        }
        
        void submitResponse(const SubjectResponse &p) override {
            responseParameters_ = p;
        }
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void playCalibration(const Calibration &p) override {
            calibrationParameters_ = p;
        }
        
        auto trialPlayed() const {
            return trialPlayed_;
        }
        
        auto &testParameters() const {
            return testParameters_;
        }
        
        auto &calibrationParameters() const {
            return calibrationParameters_;
        }
    };

    class ViewStub : public presentation::View {
        std::string errorMessage_{};
        std::string browseForDirectoryResult_{};
        std::string browseForOpeningFileResult_{};
        DialogResponse dialogResponse_{};
        EventListener *listener_{};
        TestSetup *setupView_;
        Tester *testerView_;
        Subject *subjectView_;
        bool eventLoopCalled_{};
        bool confirmationDialogShown_{};
        bool browseCancelled_{};
    public:
        ViewStub(
            TestSetup *setupView,
            Tester *testerView,
            Subject *subjectView
        ) :
            setupView_{setupView},
            testerView_{testerView},
            subjectView_{subjectView} {}
        
        void submitResponse() {
            listener_->submitResponse();
        }
        
        void playTrial() {
            listener_->playTrial();
        }
        
        void confirmTestSetup() {
            listener_->confirmTestSetup();
        }
        
        void playCalibration() {
            listener_->playCalibration();
        }
        
        void showErrorMessage(std::string s) override {
            errorMessage_ = std::move(s);
        }
        
        auto errorMessage() {
            return errorMessage_;
        }
        
        auto listener() const {
            return listener_;
        }
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void eventLoop() override {
            eventLoopCalled_ = true;
        }
        
        TestSetup *testSetup() override {
            return setupView_;
        }
        
        Tester *tester() override {
            return testerView_;
        }
        
        DialogResponse showConfirmationDialog() override {
            confirmationDialogShown_ = true;
            return dialogResponse_;
        }
        
        Subject *subject() override {
            return subjectView_;
        }
        
        std::string browseForDirectory() override {
            return browseForDirectoryResult_;
        }
        
        bool browseCancelled() override {
            return browseCancelled_;
        }
        
        std::string browseForOpeningFile() override {
            return browseForOpeningFileResult_;
        }
        
        auto eventLoopCalled() const {
            return eventLoopCalled_;
        }
        
        void newTest() {
            listener_->newTest();
        }
        
        void openTest() {
            listener_->openTest();
        }
        
        void close() {
            listener_->closeTest();
        }
        
        auto confirmationDialogShown() const {
            return confirmationDialogShown_;
        }
        
        void setDialogResponse(DialogResponse response) {
            dialogResponse_ = response;
        }
        
        void setBrowseForDirectoryResult(std::string s) {
            browseForDirectoryResult_ = std::move(s);
        }
        
        void setBrowseForOpeningFileResult(std::string s) {
            browseForOpeningFileResult_ = std::move(s);
        }
        
        void setBrowseCancelled() {
            browseCancelled_ = true;
        }

        class TestSetupViewStub : public TestSetup {
            Collection<std::string> conditions_{};
            std::string signalLevel_{"0"};
            std::string startingSnr_{"0"};
            std::string condition_{};
            std::string masker_{};
            std::string stimulusList_{};
            std::string subjectId_{};
            std::string testerId_{};
            std::string session_{};
            std::string calibrationFilePath_{};
            bool shown_{};
            bool hidden_{};
        public:
            std::string session() override {
                return session_;
            }
            
            std::string startingSnr_dB() override {
                return startingSnr_;
            }
            
            void populateConditionMenu(std::vector<std::string> items) override {
                conditions_ = std::move(items);
            }
            
            auto &conditions() const {
                return conditions_;
            }
            
            auto shown() const {
                return shown_;
            }
            
            void show() override {
                shown_ = true;
            }
            
            void hide() override {
                hidden_ = true;
            }
            
            std::string signalLevel_dB_SPL() override {
                return signalLevel_;
            }
            
            auto hidden() const {
                return hidden_;
            }
            
            void setStartingSnr(std::string s) {
                startingSnr_ = std::move(s);
            }
            
            void setSignalLevel(std::string s) {
                signalLevel_ = std::move(s);
            }
            
            void setCalibrationFilePath(std::string s) {
                calibrationFilePath_ = std::move(s);
            }
            
            void setCondition(std::string s) {
                condition_ = std::move(s);
            }

            void setMasker(std::string s) override {
                masker_ = std::move(s);
            }
            
            void setSession(std::string s) {
                session_ = std::move(s);
            }
            
            void setStimulusList(std::string s) override {
                stimulusList_ = std::move(s);
            }
            
            void setSubjectId(std::string s) {
                subjectId_ = std::move(s);
            }
            
            void setTesterId(std::string s) {
                testerId_ = std::move(s);
            }

            std::string maskerFilePath() override {
                return masker_;
            }

            std::string stimulusListDirectory() override {
                return stimulusList_;
            }

            std::string testerId() override {
                return testerId_;
            }

            std::string subjectId() override {
                return subjectId_;
            }

            std::string condition() override {
                return condition_;
            }
            
            std::string calibrationFilePath() override {
                return calibrationFilePath_;
            }
        };
        
        class TesterViewStub : public Tester {
            std::vector<std::string> audioDevices_{};
            std::string audioDevice_{};
            bool shown_{};
            bool hidden_{};
        public:
            auto audioDevices() const {
                return audioDevices_;
            }
            
            std::string audioDevice() override {
                return audioDevice_;
            }
            
            auto shown() const {
                return shown_;
            }
            
            void show() override {
                shown_ = true;
            }
            
            void hide() override {
                hidden_ = true;
            }
            
            void populateAudioDeviceMenu(std::vector<std::string> v) override {
                audioDevices_ = std::move(v);
            }
            
            auto hidden() const {
                return hidden_;
            }
            
            void setAudioDevice(std::string s) {
                audioDevice_ = std::move(s);
            }
        };
        
        class SubjectViewStub : public Subject {
            std::string numberResponse_{"0"};
            bool greenResponse_{};
            bool redResponse_{};
            bool blueResponse_{};
            bool grayResponse_{};
            bool responseButtonsShown_{};
            bool nextTrialButtonShown_{};
            bool responseButtonsHidden_{};
            bool nextTrialButtonHidden_{};
        public:
            bool grayResponse() override {
                return grayResponse_;
            }
            
            void setGrayResponse() {
                grayResponse_ = true;
            }
            
            bool blueResponse() override {
                return blueResponse_;
            }
            
            void setBlueResponse() {
                blueResponse_ = true;
            }
            
            void setRedResponse() {
                redResponse_ = true;
            }
            
            void hideNextTrialButton() override {
                nextTrialButtonHidden_ = true;
            }
            
            auto nextTrialButtonHidden() const {
                return nextTrialButtonHidden_;
            }
            
            void hideResponseButtons() override {
                responseButtonsHidden_ = true;
            }
            
            auto responseButtonsHidden() const {
                return responseButtonsHidden_;
            }
            
            void showNextTrialButton() override {
                nextTrialButtonShown_ = true;
            }
            
            auto nextTrialButtonShown() const {
                return nextTrialButtonShown_;
            }
            
            auto responseButtonsShown() const {
                return responseButtonsShown_;
            }
            
            void setGreenResponse() {
                greenResponse_ = true;
            }
            
            void setNumberResponse(std::string s) {
                numberResponse_ = s;
            }
            
            std::string numberResponse() override {
                return numberResponse_;
            }
            
            bool greenResponse() override {
                return greenResponse_;
            }
            
            void showResponseButtons() override {
                responseButtonsShown_ = true;
            }
        };
    };
    
    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(presentation::View::EventListener *) = 0;
    };

    class BrowsingUseCase : public UseCase {
    public:
        virtual void setResult(ViewStub &, std::string) = 0;
    };

    class BrowsingEnteredPathUseCase : public BrowsingUseCase {
    public:
        virtual std::string entry(ViewStub::TestSetupViewStub &) = 0;
        virtual void setEntry(ViewStub::TestSetupViewStub &, std::string) = 0;
    };
    
    class BrowsingForMasker : public BrowsingEnteredPathUseCase {
    public:
        std::string entry(ViewStub::TestSetupViewStub &view) override {
            return view.maskerFilePath();
        }
        
        void setEntry(ViewStub::TestSetupViewStub &view, std::string s) override {
            view.setMasker(s);
        }
        
        void setResult(ViewStub &view, std::string s) override {
            return view.setBrowseForOpeningFileResult(s);
        }
        
        void run(presentation::View::EventListener *listener) override {
            listener->browseForMasker();
        }
    };

    class BrowsingForStimulusList : public BrowsingEnteredPathUseCase {
    public:
        void run(presentation::View::EventListener *listener) override {
            listener->browseForStimulusList();
        }

        void setResult(ViewStub &view, std::string s) override {
            view.setBrowseForDirectoryResult(s);
        }
        
        std::string entry(ViewStub::TestSetupViewStub &view) override {
            return view.stimulusListDirectory();
        }
        
        void setEntry(ViewStub::TestSetupViewStub &view, std::string s) override {
            view.setStimulusList(s);
        }
    };

    class PresenterConstructionTests : public ::testing::Test {
    protected:
        ModelStub model{};
        ViewStub::TestSetupViewStub setupView{};
        ViewStub::TesterViewStub testerView{};
        ViewStub view{&setupView, &testerView, nullptr};
        
        presentation::Presenter construct() {
            return {&model, &view};
        }
    };
    
    TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
        model.setAudioDevices({"a", "b", "c"});
        auto presenter = construct();
        assertEqual({"a", "b", "c"}, testerView.audioDevices());
    }

    class PresenterTests : public ::testing::Test {
    protected:
        ModelStub model{};
        ViewStub::TestSetupViewStub setupView{};
        ViewStub::TesterViewStub testerView{};
        ViewStub::SubjectViewStub subjectView{};
        ViewStub view{&setupView, &testerView, &subjectView};
        presentation::Presenter presenter{&model, &view};
        BrowsingForStimulusList browsingForStimulusList{};
        BrowsingForMasker browinsgForMasker{};
        
        std::string auditoryOnlyConditionName() {
            return conditionName(
                av_coordinated_response_measure::Condition::auditoryOnly
            );
        }
        
        std::string audioVisualConditionName() {
            return conditionName(
                av_coordinated_response_measure::Condition::audioVisual
            );
        }
        
        void submitResponse() {
            view.submitResponse();
        }
        
        void playTrial() {
            view.playTrial();
        }
        
        void confirmTestSetup() {
            view.confirmTestSetup();
        }
        
        void playCalibration() {
            view.playCalibration();
        }
        
        void confirmTestSetupWithInvalidInput() {
            setupView.setStartingSnr("?");
            confirmTestSetup();
        }
        
        bool testerViewShown() {
            return testerView.shown();
        }
        
        bool testerViewHidden() {
            return testerView.hidden();
        }
        
        bool setupViewHidden() {
            return setupView.hidden();
        }
        
        bool setupViewShown() {
            return setupView.shown();
        }
        
        void assertTesterViewShown() {
            EXPECT_TRUE(testerViewShown());
        }
        
        void assertTesterViewNotShown() {
            EXPECT_FALSE(testerViewShown());
        }
        
        void assertTesterViewHidden() {
            EXPECT_TRUE(testerViewHidden());
        }
        
        void assertTesterViewNotHidden() {
            EXPECT_FALSE(testerViewHidden());
        }
        
        void assertSetupViewShown() {
            EXPECT_TRUE(setupViewShown());
        }
        
        void assertSetupViewHidden() {
            EXPECT_TRUE(setupViewHidden());
        }
        
        void assertSetupViewNotHidden() {
            EXPECT_FALSE(setupViewHidden());
        }

        void runUseCase(UseCase *useCase) {
            useCase->run(&presenter);
        }

        void assertBrowseResultPassedToEntry(BrowsingEnteredPathUseCase *useCase) {
            useCase->setResult(view, "a");
            runUseCase(useCase);
            assertEqual("a", useCase->entry(setupView));
        }

        void assertCancellingBrowseDoesNotChangePath(
            BrowsingEnteredPathUseCase *useCase
        ) {
            useCase->setEntry(setupView, "a");
            useCase->setResult(view, "b");
            view.setBrowseCancelled();
            runUseCase(useCase);
            assertEqual("a", useCase->entry(setupView));
        }
        
        void completeTrial() {
            model.completeTrial();
        }
        
        void assertResponseButtonsShown() {
            EXPECT_TRUE(subjectView.responseButtonsShown());
        }
        
        void assertNextTrialButtonShown() {
            EXPECT_TRUE(subjectView.nextTrialButtonShown());
        }
        
        void assertNextTrialButtonNotShown() {
            EXPECT_FALSE(subjectView.nextTrialButtonShown());
        }
        
        void assertResponseButtonsHidden() {
            EXPECT_TRUE(subjectView.responseButtonsHidden());
        }
        
        void assertNextTrialButtonHidden() {
            EXPECT_TRUE(subjectView.nextTrialButtonHidden());
        }
        
        void assertSetupViewConditionsContains(std::string s) {
            EXPECT_TRUE(setupView.conditions().contains(std::move(s)));
        }
        
        void setCondition(std::string s) {
            setupView.setCondition(std::move(s));
        }
        
        std::string errorMessage() {
            return view.errorMessage();
        }
        
        void assertModelPassedCondition(av_coordinated_response_measure::Color c) {
            EXPECT_EQ(c, model.responseParameters().color);
        }
        
        void close() {
            view.close();
        }
        
        void setDialogResponse(presentation::View::DialogResponse r) {
            view.setDialogResponse(r);
        }
        
        const av_coordinated_response_measure::Model::Test &modelTestParameters() {
            return model.testParameters();
        }
        
        const av_coordinated_response_measure::Model::Calibration &modelCalibrationParameters() {
            return model.calibrationParameters();
        }
    };

    TEST_F(PresenterTests, subscribesToViewEvents) {
        EXPECT_EQ(&presenter, view.listener());
    }

    TEST_F(PresenterTests, populatesConditionMenu) {
        assertSetupViewConditionsContains(auditoryOnlyConditionName());
        assertSetupViewConditionsContains(audioVisualConditionName());
    }

    TEST_F(PresenterTests, callsEventLoopWhenRun) {
        presenter.run();
        EXPECT_TRUE(view.eventLoopCalled());
    }

    TEST_F(PresenterTests, openTestShowsTesterView) {
        view.openTest();
        assertTesterViewShown();
    }

    TEST_F(PresenterTests, newTestShowsTestSetupView) {
        view.newTest();
        assertSetupViewShown();
    }

    TEST_F(PresenterTests, confirmTestSetupHidesTestSetupView) {
        confirmTestSetup();
        assertSetupViewHidden();
    }

    TEST_F(PresenterTests, confirmTestSetupPassesParametersToModel) {
        setupView.setStartingSnr("1");
        setupView.setSignalLevel("2");
        setupView.setStimulusList("a");
        setupView.setSubjectId("b");
        setupView.setTesterId("c");
        setupView.setMasker("d");
        setupView.setSession("e");
        confirmTestSetup();
        EXPECT_EQ(1, modelTestParameters().startingSnr_dB);
        EXPECT_EQ(2, modelTestParameters().signalLevel_dB_SPL);
        assertEqual("a", modelTestParameters().targetListDirectory);
        assertEqual("b", modelTestParameters().subjectId);
        assertEqual("c", modelTestParameters().testerId);
        assertEqual("d", modelTestParameters().maskerFilePath);
        assertEqual("e", modelTestParameters().session);
    }

    TEST_F(PresenterTests, playCalibrationPassesParametersToModel) {
        setupView.setSignalLevel("1");
        setupView.setCalibrationFilePath("a");
        playCalibration();
        EXPECT_EQ(1, modelCalibrationParameters().level_dB_SPL);
        assertEqual("a", modelCalibrationParameters().filePath);
    }

    TEST_F(PresenterTests, confirmTestSetupPassesAudioVisualCondition) {
        setCondition(audioVisualConditionName());
        confirmTestSetup();
        EXPECT_EQ(
            av_coordinated_response_measure::Condition::audioVisual,
            modelTestParameters().condition
        );
    }

    TEST_F(PresenterTests, confirmTestSetupPassesAuditoryOnlyCondition) {
        setCondition(auditoryOnlyConditionName());
        confirmTestSetup();
        EXPECT_EQ(
            av_coordinated_response_measure::Condition::auditoryOnly,
            modelTestParameters().condition
        );
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidSignalLevelShowsErrorMessage) {
        setupView.setSignalLevel("a");
        confirmTestSetup();
        assertEqual("'a' is not a valid signal level.", errorMessage());
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidSnrShowsErrorMessage) {
        setupView.setStartingSnr("a");
        confirmTestSetup();
        assertEqual("'a' is not a valid SNR.", errorMessage());
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidInputDoesNotHideSetupView) {
        confirmTestSetupWithInvalidInput();
        assertSetupViewNotHidden();
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidInputDoesNotShowTesterView) {
        confirmTestSetupWithInvalidInput();
        assertTesterViewNotShown();
    }

    TEST_F(PresenterTests, confirmTestSetupShowsTesterView) {
        confirmTestSetup();
        assertTesterViewShown();
    }

    TEST_F(PresenterTests, confirmTestSetupShowsNextTrialButton) {
        confirmTestSetup();
        assertNextTrialButtonShown();
    }

    TEST_F(PresenterTests, playingTrialPlaysTrial) {
        playTrial();
        EXPECT_TRUE(model.trialPlayed());
    }

    TEST_F(PresenterTests, playingTrialHidesNextTrialButton) {
        playTrial();
        assertNextTrialButtonHidden();
    }

    TEST_F(PresenterTests, playingTrialPassesAudioDevice) {
        testerView.setAudioDevice("a");
        playTrial();
        assertEqual("a", model.trialParameters().audioDevice);
    }

    TEST_F(PresenterTests, subjectResponsePassesNumberResponse) {
        subjectView.setNumberResponse("1");
        submitResponse();
        EXPECT_EQ(1, model.responseParameters().number);
    }

    TEST_F(PresenterTests, subjectResponsePassesGreenColor) {
        subjectView.setGreenResponse();
        submitResponse();
        assertModelPassedCondition(av_coordinated_response_measure::Color::green);
    }

    TEST_F(PresenterTests, subjectResponsePassesRedColor) {
        subjectView.setRedResponse();
        submitResponse();
        assertModelPassedCondition(av_coordinated_response_measure::Color::red);
    }

    TEST_F(PresenterTests, subjectResponsePassesBlueColor) {
        subjectView.setBlueResponse();
        submitResponse();
        assertModelPassedCondition(av_coordinated_response_measure::Color::blue);
    }

    TEST_F(PresenterTests, subjectResponsePassesGrayColor) {
        subjectView.setGrayResponse();
        submitResponse();
        assertModelPassedCondition(av_coordinated_response_measure::Color::gray);
    }

    TEST_F(PresenterTests, submitResponseDoesNotHideTesterViewWhileTestInProgress) {
        model.setTestIncomplete();
        submitResponse();
        assertTesterViewNotHidden();
    }

    TEST_F(PresenterTests, submitResponseHidesTesterViewWhenTestComplete) {
        model.setTestComplete();
        submitResponse();
        assertTesterViewHidden();
    }

    TEST_F(PresenterTests, submitResponseShowsSetupViewWhenTestComplete) {
        model.setTestComplete();
        submitResponse();
        assertSetupViewShown();
    }

    TEST_F(PresenterTests, subjectResponseShowsNextTrialButton) {
        submitResponse();
        assertNextTrialButtonShown();
    }

    TEST_F(PresenterTests, subjectResponseDoesNotShowNextTrialButtonWhenTestComplete) {
        model.setTestComplete();
        submitResponse();
        assertNextTrialButtonNotShown();
    }

    TEST_F(PresenterTests, subjectResponseHidesResponseButtons) {
        submitResponse();
        assertResponseButtonsHidden();
    }

    TEST_F(PresenterTests, closingTestPromptsTesterToSave) {
        close();
        EXPECT_TRUE(view.confirmationDialogShown());
    }

    TEST_F(PresenterTests, closingTestHidesTesterViewIfUserDeclinesSaving) {
        setDialogResponse(presentation::View::DialogResponse::decline);
        close();
        assertTesterViewHidden();
    }

    TEST_F(PresenterTests, closingTestHidesTesterViewIfUserAcceptsSaving) {
        setDialogResponse(presentation::View::DialogResponse::accept);
        close();
        assertTesterViewHidden();
    }

    TEST_F(PresenterTests, closingTestDoesNotHideTesterViewIfUserCancels) {
        setDialogResponse(presentation::View::DialogResponse::cancel);
        close();
        assertTesterViewNotHidden();
    }

    TEST_F(PresenterTests, browseForStimulusListUpdatesStimulusList) {
        assertBrowseResultPassedToEntry(&browsingForStimulusList);
    }

    TEST_F(PresenterTests, browseForMaskerUpdatesMasker) {
        assertBrowseResultPassedToEntry(&browinsgForMasker);
    }

    TEST_F(PresenterTests, browseForStimulusListCancelDoesNotChangeStimulusList) {
        assertCancellingBrowseDoesNotChangePath(&browsingForStimulusList);
    }

    TEST_F(PresenterTests, browseForMaskerCancelDoesNotChangeMasker) {
        assertCancellingBrowseDoesNotChangePath(&browinsgForMasker);
    }

    TEST_F(PresenterTests, trialCompleteShowsResponseButtons) {
        completeTrial();
        assertResponseButtonsShown();
    }

    class RequestFailingModel : public av_coordinated_response_measure::Model {
        std::string errorMessage{};
    public:
        void setErrorMessage(std::string s) {
            errorMessage = std::move(s);
        }
        
        void initializeTest(const Test &) override {
            throw RequestFailure{errorMessage};
        }
        
        void playTrial(const AudioSettings &) override {
            throw RequestFailure{errorMessage};
        }
        
        void submitResponse(const SubjectResponse &) override {
            throw RequestFailure{errorMessage};
        }
        
        bool testComplete() override { return {}; }
        std::vector<std::string> audioDevices() override { return {}; }
        void subscribe(EventListener *) override {}
        void playCalibration(const Calibration &) override {}
    };

    class PresenterFailureTests : public ::testing::Test {
    protected:
        RequestFailingModel failure{};
        ModelStub defaultModel{};
        av_coordinated_response_measure::Model *model{&defaultModel};
        ViewStub::TestSetupViewStub setupView{};
        ViewStub::TesterViewStub testerView{};
        ViewStub view{&setupView, &testerView, nullptr};
        
        void useFailingModel(std::string s = {}) {
            failure.setErrorMessage(std::move(s));
            model = &failure;
        }
        
        void confirmTestSetup() {
            presentation::Presenter presenter{model, &view};
            view.confirmTestSetup();
        }
        
        void assertConfirmTestSetupShowsErrorMessage(std::string s) {
            confirmTestSetup();
            assertEqual(std::move(s), view.errorMessage());
        }
        
        void assertConfirmTestSetupDoesNotHideSetupView() {
            confirmTestSetup();
            ASSERT_FALSE(setupView.hidden());
        }
        
        void assertConfirmTestSetupDoesNotShowTesterView() {
            confirmTestSetup();
            ASSERT_FALSE(testerView.shown());
        }
    };

    TEST_F(
        PresenterFailureTests,
        initializeTestShowsErrorMessageWhenModelFailsRequest
    ) {
        useFailingModel("a");
        assertConfirmTestSetupShowsErrorMessage("a");
    }

    TEST_F(
        PresenterFailureTests,
        initializeTestDoesNotHideSetupViewWhenModelFailsRequest
    ) {
        useFailingModel();
        assertConfirmTestSetupDoesNotHideSetupView();
    }

    TEST_F(
        PresenterFailureTests,
        initializeTestDoesNotShowTesterViewWhenModelFailsRequest
    ) {
        useFailingModel();
        assertConfirmTestSetupDoesNotShowTesterView();
    }
}
