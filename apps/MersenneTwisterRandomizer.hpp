#ifndef APPS_MERSENNETWISTERRANDOMIZER_HPP_
#define APPS_MERSENNETWISTERRANDOMIZER_HPP_

#include <target-list/RandomizedTargetList.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <random>

class MersenneTwisterRandomizer : public target_list::Randomizer,
                                  public av_speech_in_noise::Randomizer {
    std::mt19937 engine{std::random_device{}()};

  public:
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }

    void shuffle(
        int_shuffle_iterator begin, int_shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }

    auto randomFloatBetween(double a, double b) -> double override {
        std::uniform_real_distribution<> distribution{a, b};
        return distribution(engine);
    }

    auto randomIntBetween(int a, int b) -> int override {
        std::uniform_int_distribution<> distribution{a, b};
        return distribution(engine);
    }
};

#endif
