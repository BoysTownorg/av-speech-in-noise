#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "TaskControllerListenerStub.hpp"
#include "ExperimenterControllerStub.hpp"
#include <presentation/Consonant.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class ConsonantControlStub : public ConsonantTaskControl {
  public:
    void notifyThatReadyButtonHasBeenClicked() {
        listener_->notifyThatReadyButtonHasBeenClicked();
    }

    void attach(Observer *e) override { listener_ = e; }

    void notifyThatResponseButtonHasBeenClicked() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void setConsonant(std::string c) { consonant_ = std::move(c); }

    auto consonant() -> std::string override { return consonant_; }

  private:
    std::string consonant_;
    Observer *listener_{};
};

class ConsonantViewStub : public ConsonantTaskView {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void showReadyButton() override { readyButtonShown_ = true; }

    [[nodiscard]] auto readyButtonShown() const -> bool {
        return readyButtonShown_;
    }

    void hideReadyButton() override { readyButtonHidden_ = true; }

    [[nodiscard]] auto readyButtonHidden() const -> bool {
        return readyButtonHidden_;
    }

    void hideResponseButtons() override { responseButtonsHidden_ = true; }

    [[nodiscard]] auto responseButtonsHidden() const -> bool {
        return responseButtonsHidden_;
    }

    void showResponseButtons() override { responseButtonsShown_ = true; }

    [[nodiscard]] auto responseButtonsShown() const -> bool {
        return responseButtonsShown_;
    }

    [[nodiscard]] auto cursorHidden() const -> bool { return cursorHidden_; }

    void hideCursor() override { cursorHidden_ = true; }

    void showCursor() override { cursorShown_ = true; }

  private:
    bool shown_{};
    bool hidden_{};
    bool responseButtonsShown_{};
    bool responseButtonsHidden_{};
    bool readyButtonShown_{};
    bool readyButtonHidden_{};
    bool cursorHidden_{};
    bool cursorShown_{};
};

void notifyThatReadyButtonHasBeenClicked(ConsonantControlStub &view) {
    view.notifyThatReadyButtonHasBeenClicked();
}

auto cursorHidden(ConsonantViewStub &view) -> bool {
    return view.cursorHidden();
}

void notifyThatUserIsDoneResponding(TaskPresenter &presenter) {
    presenter.notifyThatUserIsDoneResponding();
}

void notifyThatTrialHasStarted(TaskPresenter &presenter) {
    presenter.notifyThatTrialHasStarted();
}

void stop(TaskPresenter &presenter) { presenter.stop(); }

void start(TaskPresenter &presenter) { presenter.start(); }

void notifyThatResponseButtonHasBeenClicked(ConsonantControlStub &view) {
    view.notifyThatResponseButtonHasBeenClicked();
}

class ConsonantTests : public ::testing::Test {
  protected:
    ModelStub model;
    ConsonantControlStub control;
    ConsonantViewStub view;
    ConsonantTaskController controller{model, control};
    ConsonantTaskPresenter presenter{view};
    ExperimenterControllerStub testController;
    TaskControllerListenerStub taskController;

    ConsonantTests() {
        controller.attach(&testController);
        controller.attach(&taskController);
    }
};

#define CONSONANT_TEST(a) TEST_F(ConsonantTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).responseButtonsHidden())

#define AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(a)                             \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(cursorHidden(a))

CONSONANT_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.readyButtonHidden());
}

CONSONANT_TEST(presenterHidesCursorAfterTrialHasStarted) {
    notifyThatTrialHasStarted(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_CURSOR_HIDDEN(view);
}

CONSONANT_TEST(presenterHidesResponseButtonsAfterUserIsDoneResponding) {
    notifyThatUserIsDoneResponding(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CONSONANT_TEST(presenterHidesViewWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

CONSONANT_TEST(presenterHidesResponseButtonsWhenStopped) {
    stop(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CONSONANT_TEST(presenterShowsViewWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

CONSONANT_TEST(presenterShowsReadyButtonWhenStarted) {
    start(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.readyButtonShown());
}

CONSONANT_TEST(presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseButtonsShown());
}

CONSONANT_TEST(
    controllerNotifiesThatUserIsReadyForNextTrialAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsReadyForNextTrial());
}

CONSONANT_TEST(controllerNotifiesThatTaskHasStartedAfterReadyButtonIsClicked) {
    notifyThatReadyButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(taskController.notifiedThatTaskHasStarted());
}

CONSONANT_TEST(controllerSubmitsConsonantAfterResponseButtonIsClicked) {
    control.setConsonant("b");
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL('b', model.consonantResponse().consonant);
}

CONSONANT_TEST(
    controllerNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked) {
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsReadyForNextTrial());
}

CONSONANT_TEST(
    controllerNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}
}
}
