#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_FIXEDLEVELMETHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_FIXEDLEVELMETHOD_HPP_

#include "Model.hpp"

namespace av_speech_in_noise {
class EmptyTargetListTestConcluder : public TestConcluder {
  public:
    void initialize(const FixedLevelTest &) override {}

    void submitResponse() override {}

    auto complete(TargetList *t) -> bool override { return t->empty(); }
};

class FixedTrialTestConcluder : public TestConcluder {
    int trials_{};

  public:
    void initialize(const FixedLevelTest &p) override { trials_ = p.trials; }

    void submitResponse() override { --trials_; }

    auto complete(TargetList *) -> bool override { return trials_ == 0; }
};

class FixedLevelMethodImpl : public FixedLevelMethod {
  public:
    explicit FixedLevelMethodImpl(ResponseEvaluator *);
    void initialize(
        const FixedLevelTest &, TargetList *, TestConcluder *) override;
    auto snr_dB() -> int override;
    auto next() -> std::string override;
    auto complete() -> bool override;
    auto current() -> std::string override;
    void submitIncorrectResponse() override;
    void submitCorrectResponse() override;
    void writeLastCoordinateResponse(OutputFile *) override;
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void writeTestingParameters(OutputFile *) override;
    void submitResponse(const coordinate_response_measure::Response &) override;
    void submitResponse(const open_set::FreeResponse &) override;

  private:
    coordinate_response_measure::FixedLevelTrial lastTrial{};
    const FixedLevelTest *test{};
    TargetList *targetList{};
    ResponseEvaluator *evaluator;
    TestConcluder *concluder{};
    int snr_dB_{};
    bool complete_{};
};
}
#endif
