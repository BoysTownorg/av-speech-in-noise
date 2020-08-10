#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_FIXEDLEVELMETHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_FIXEDLEVELMETHOD_HPP_

#include "Model.hpp"

namespace av_speech_in_noise {
class FixedLevelMethodImpl : public FixedLevelMethod {
  public:
    explicit FixedLevelMethodImpl(ResponseEvaluator &);
    void initialize(const FixedLevelTestWithEachTargetNTimes &,
        FiniteTargetPlaylist *) override {}
    void initialize(
        const FixedLevelFixedTrialsTest &, TargetPlaylist *) override;
    void initialize(
        const FixedLevelTest &, FiniteTargetPlaylistWithRepeatables *) override;
    void submit(const coordinate_response_measure::Response &) override;
    void submit(const FreeResponse &) override;
    void submit(const CorrectKeywords &) override {}
    void submitIncorrectResponse() override;
    void submitCorrectResponse() override;
    void writeLastCoordinateResponse(OutputFile &) override;
    void writeLastCorrectResponse(OutputFile &) override {}
    void writeLastIncorrectResponse(OutputFile &) override {}
    void writeLastCorrectKeywords(OutputFile &) override {}
    void writeTestingParameters(OutputFile &) override;
    void writeTestResult(OutputFile &) override {}
    auto snr() -> SNR override;
    auto nextTarget() -> LocalUrl override;
    auto currentTarget() -> LocalUrl override;
    auto complete() -> bool override;

  private:
    coordinate_response_measure::FixedLevelTrial lastTrial{};
    const FixedLevelTest *test_{};
    TargetPlaylist *targetList{};
    FiniteTargetPlaylistWithRepeatables *finiteTargetPlaylist{};
    ResponseEvaluator &evaluator;
    SNR snr_{};
    int trials_{};
    bool finiteTargetsExhausted_{};
    bool usingFiniteTargetPlaylist_{};
};
}

#endif