#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TEST_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TEST_HPP_

#include "Interface.hpp"
#include "View.hpp"
#include "Presenter.hpp"
#include "Method.hpp"
#include <string>

namespace av_speech_in_noise {
class TestControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void exitTest() = 0;
        virtual void playTrial() = 0;
        virtual void declineContinuingTesting() = 0;
        virtual void acceptContinuingTesting() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TestControl);
    virtual void attach(Observer *) = 0;
};

class TestView : public virtual View {
  public:
    virtual void showContinueTestingDialog() = 0;
    virtual void hideContinueTestingDialog() = 0;
    virtual void setContinueTestingDialogMessage(const std::string &) = 0;
    virtual void hideExitTestButton() = 0;
    virtual void showExitTestButton() = 0;
    virtual void showNextTrialButton() = 0;
    virtual void hideNextTrialButton() = 0;
    virtual void display(std::string) = 0;
    virtual void secondaryDisplay(std::string) = 0;
    virtual void tellSubject(const std::string &) {}
};

class TestController {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatTrialHasStarted() = 0;
        virtual void setContinueTestingDialogMessage(const std::string &) = 0;
        virtual void showContinueTestingDialog() = 0;
        virtual void display(const std::string &) = 0;
        virtual void secondaryDisplay(const std::string &) = 0;
        virtual void notifyThatNextTrialIsReady() = 0;
        virtual void tellSubject(const std::string &) = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TestController);
    virtual void attach(Observer *) = 0;
    virtual void attach(SessionController *) = 0;
    virtual void
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion() = 0;
    virtual void notifyThatUserIsDoneResponding() = 0;
    virtual void notifyThatUserIsReadyForNextTrial() = 0;
    virtual void
    notifyThatUserIsReadyForNextTrialForATestThatCongratulatesAtTheEnd() = 0;
};

class TestPresenter : public virtual TestController::Observer,
                      public virtual Presenter {
  public:
    virtual void initialize(Method) = 0;
};
}

#endif
