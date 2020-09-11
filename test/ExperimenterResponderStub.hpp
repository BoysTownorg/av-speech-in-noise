#ifndef AV_SPEECH_IN_NOISE_TESTS_EXPERIMENTERRESPONDERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_EXPERIMENTERRESPONDERSTUB_HPP_

#include <presentation/Experimenter.hpp>

namespace av_speech_in_noise {
class ExperimenterResponderStub : public ExperimenterResponder {
  public:
    void subscribe(EventListener *) override {}
    void subscribe(IPresenter *) override {}
    void notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion()
        override {
        notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion_ =
            true;
    }
    [[nodiscard]] auto
    notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion()
        const -> bool {
        return notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion_;
    }
    void notifyThatUserIsDoneResponding() override {
        notifiedThatUserIsDoneResponding_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsDoneResponding() const -> bool {
        return notifiedThatUserIsDoneResponding_;
    }
    void notifyThatUserIsReadyForNextTrial() override {
        notifiedThatUserIsReadyForNextTrial_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsReadyForNextTrial() const -> bool {
        return notifiedThatUserIsReadyForNextTrial_;
    }

  private:
    bool notifiedThatUserIsReadyForNextTrial_{};
    bool
        notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion_{};
    bool notifiedThatUserIsDoneResponding_{};
};
}

#endif