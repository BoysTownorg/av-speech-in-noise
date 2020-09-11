#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTER_HPP_

#include "PresenterSimple.hpp"
#include "Method.hpp"
#include <string>

namespace av_speech_in_noise {
class ExperimenterInputView {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void exitTest() = 0;
        virtual void playTrial() = 0;
        virtual void declineContinuingTesting() = 0;
        virtual void acceptContinuingTesting() = 0;
    };
    virtual void attach(Observer *) = 0;
    virtual ~ExperimenterInputView() = default;
};

class ExperimenterOutputView {
  public:
    virtual ~ExperimenterOutputView() = default;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void showContinueTestingDialog() = 0;
    virtual void hideContinueTestingDialog() = 0;
    virtual void setContinueTestingDialogMessage(const std::string &) = 0;
    virtual void hideExitTestButton() = 0;
    virtual void showExitTestButton() = 0;
    virtual void showNextTrialButton() = 0;
    virtual void hideNextTrialButton() = 0;
    virtual void display(std::string) = 0;
    virtual void secondaryDisplay(std::string) = 0;
};

class ExperimenterView : public virtual ExperimenterInputView,
                         public virtual ExperimenterOutputView {};

class ExperimenterController {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void notifyThatTrialHasStarted() = 0;
        virtual void setContinueTestingDialogMessage(const std::string &) = 0;
        virtual void showContinueTestingDialog() = 0;
        virtual void display(const std::string &) = 0;
        virtual void secondaryDisplay(const std::string &) = 0;
        virtual void notifyThatNextTrialIsReady() = 0;
    };
    virtual ~ExperimenterController() = default;
    virtual void attach(Observer *) = 0;
    virtual void attach(SessionController *) = 0;
    virtual void
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion() = 0;
    virtual void notifyThatUserIsDoneResponding() = 0;
    virtual void notifyThatUserIsReadyForNextTrial() = 0;
};

class ExperimenterPresenter
    : public virtual ExperimenterController::Observer,
      public virtual PresenterSimple {
  public:
    virtual void initialize(Method) = 0;
};
}

#endif
