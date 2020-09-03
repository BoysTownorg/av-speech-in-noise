#include "FreeResponse.hpp"

namespace av_speech_in_noise {
FreeResponseResponder::FreeResponseResponder(
    Model &model, FreeResponseInputView &view)
    : model{model}, view{view} {
    view.subscribe(this);
}

void FreeResponseResponder::subscribe(TaskResponder::EventListener *e) {
    listener = e;
}

void FreeResponseResponder::notifyThatSubmitButtonHasBeenClicked() {
    model.submit(FreeResponse{view.freeResponse(), view.flagged()});
    listener->notifyThatUserIsDoneResponding();
    parent->readyNextTrialIfNeeded();
}

void FreeResponseResponder::becomeChild(ParentPresenter *p) { parent = p; }

FreeResponsePresenter::FreeResponsePresenter(
    ExperimenterView &experimenterView, FreeResponseOutputView &view)
    : experimenterView{experimenterView}, view{view} {}

void FreeResponsePresenter::start() {
    experimenterView.show();
    experimenterView.showNextTrialButton();
}

void FreeResponsePresenter::stop() {
    experimenterView.hide();
    view.hideFreeResponseSubmission();
}

void FreeResponsePresenter::notifyThatTaskHasStarted() {
    experimenterView.hideNextTrialButton();
}

void FreeResponsePresenter::notifyThatUserIsDoneResponding() {
    view.hideFreeResponseSubmission();
}

void FreeResponsePresenter::showResponseSubmission() {
    view.clearFreeResponse();
    view.showFreeResponseSubmission();
}
}
