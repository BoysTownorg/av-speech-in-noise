#include "assert-utility.hpp"
#include "TestControllerStub.hpp"

#include <av-speech-in-noise/ui/Emotion.hpp>

#include <gtest/gtest.h>

namespace av_speech_in_noise::submitting_emotion {
namespace {
class ViewStub : public virtual View {
  public:
    void show() override { shown = true; }

    void hide() override { hidden = true; }

    bool shown{};
    bool hidden{};
};

class UIStub : public UI, public ViewStub {
  public:
    void attach(Observer *ob) override { observer = ob; }

    auto emotion() -> Emotion override { return emotion_; }

    auto playButton() -> View & override { return playButton_; }

    auto responseButtons() -> View & override { return responseButtons_; }

    ViewStub playButton_;
    ViewStub responseButtons_;
    Emotion emotion_{Emotion::unknown};
    Observer *observer{};
};

class InteractorStub : public Interactor {
  public:
    void submit(const EmotionResponse &e) override { response = e; }

    EmotionResponse response;
};

class PresenterTests : public ::testing::Test {
  protected:
    UIStub ui;
    InteractorStub model;
    TestControllerStub testController;
    Presenter presenter{ui, testController, model};
};

#define EMOTION_PRESENTER_TEST(a) TEST_F(PresenterTests, a)

EMOTION_PRESENTER_TEST(showsViewsWhenStarted) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.shown);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.playButton_.shown);
}

EMOTION_PRESENTER_TEST(hidesViewsWhenStopped) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.hidden);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.responseButtons_.hidden);
}

EMOTION_PRESENTER_TEST(hidesResponseSubmission) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.responseButtons_.hidden);
}

EMOTION_PRESENTER_TEST(showsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.responseButtons_.shown);
}

EMOTION_PRESENTER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterPlayButtonIsClicked) {
    ui.observer->notifyThatPlayButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsReadyForNextTrial());
}

EMOTION_PRESENTER_TEST(hidesPlayButtonAfterClicked) {
    ui.observer->notifyThatPlayButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(ui.playButton_.hidden);
}

/*
CONSONANT_TASK_CONTROLLER_TEST(submitsConsonantAfterResponseButtonIsClicked) {
    control.setConsonant(Consonant::bi);
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(Consonant::bi, model.response().consonant);
}

CONSONANT_TASK_CONTROLLER_TEST(
    notifiesThatUserIsDoneRespondingAndIsReadyForNextTrialAfterResponseButtonIsClicked)
{ notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController
            .notifiedThatUserIsDoneRespondingAndIsReadyForNextTrial());
}
*/
}
}
