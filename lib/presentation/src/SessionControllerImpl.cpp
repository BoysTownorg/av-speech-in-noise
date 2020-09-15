#include "SessionControllerImpl.hpp"

namespace av_speech_in_noise {
SessionControllerImpl::SessionControllerImpl(Model &model, SessionView &view,
    TestSetupController *testSetupController,
    TestSetupPresenter *testSetupPresenter,
    TestController *experimenterController,
    TestPresenter *experimenterPresenter)
    : view{view}, testSetupPresenter{testSetupPresenter},
      experimenterPresenter{experimenterPresenter} {
    view.populateAudioDeviceMenu(model.audioDevices());
}

void SessionControllerImpl::run() { view.eventLoop(); }

void SessionControllerImpl::prepare(Method m) {
    testSetupPresenter->stop();
    experimenterPresenter->initialize(m);
    experimenterPresenter->start();
}

void SessionControllerImpl::notifyThatTestIsComplete() {
    experimenterPresenter->stop();
    testSetupPresenter->start();
}
}
