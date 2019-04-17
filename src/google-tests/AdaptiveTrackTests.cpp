#include <recognition-test/AdaptiveTrack.hpp>
#include <gtest/gtest.h>

namespace {
    class AdaptiveTrackTests : public ::testing::Test {
    protected:
        av_coordinate_response_measure::AdaptiveTrack::Settings settings{};
        av_coordinate_response_measure::TrackingRule rule;
        av_coordinate_response_measure::AdaptiveTrack track{};
        
        AdaptiveTrackTests() {
            rule.resize(3);
            for (auto &sequence : rule)
                sequence.runCount = 0;
        }
        
        void reset() {
            settings.rule = &rule;
            track.reset(settings);
        }
        
        void push(std::string directions) {
            for (const auto &c : directions)
                if (c == 'd')
                    pushDown();
                else if (c == 'u')
                    pushUp();
        }
        
        void pushDown() {
            track.pushDown();
        }
        
        void pushUp() {
            track.pushUp();
        }
        
        auto x() {
            return track.x();
        }
        
        auto complete() {
            return track.complete();
        }
        
        auto reversals() {
            return track.reversals();
        }
        
        template<typename T>
        void assertXEquals(T expected) {
            EXPECT_EQ(expected, x());
        }
        
        void assertComplete() {
            EXPECT_TRUE(complete());
        }
        
        void assertIncomplete() {
            EXPECT_FALSE(complete());
        }
        
        void assertReversalsEquals(int expected) {
            EXPECT_EQ(expected, reversals());
        }
        
        template<typename T>
        void setStartingX(T x) {
            settings.startingX = x;
        }
    };

    TEST_F(AdaptiveTrackTests, xEqualToStartingX) {
        setStartingX(1);
        reset();
        assertXEquals(1);
    }

    TEST_F(AdaptiveTrackTests, noRunSequencesMeansNoChanges) {
        setStartingX(5);
        reset();
        pushDown();
        assertXEquals(5);
        pushDown();
        assertXEquals(5);
        pushUp();
        assertXEquals(5);
    }

    TEST_F(AdaptiveTrackTests, exhaustedRunSequencesMeansNoMoreStepChanges) {
        setStartingX(5);
        rule.at(0).runCount = 3;
        rule.at(0).stepSize = 4;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        reset();
        pushDown();
        assertXEquals(5);
        pushDown();
        assertXEquals(5 - 4);
        pushUp();
        assertXEquals(5 - 4 + 4);
        pushDown();
        assertXEquals(5 - 4 + 4);
        pushDown();
        assertXEquals(5 - 4 + 4 - 4);
        pushUp();
        assertXEquals(5 - 4 + 4 - 4);
        pushDown();
        assertXEquals(5 - 4 + 4 - 4);
        pushDown();
        assertXEquals(5 - 4 + 4 - 4);
    }

    TEST_F(AdaptiveTrackTests, completeWhenExhausted) {
        setStartingX(5);
        rule.at(0).runCount = 3;
        rule.at(0).stepSize = 4;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        reset();
        assertIncomplete();
        pushDown();
        assertIncomplete();
        pushDown();
        assertIncomplete();
        pushUp();
        assertIncomplete();
        pushDown();
        assertIncomplete();
        pushDown();
        assertIncomplete();
        pushUp();
        assertComplete();
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure4) {
        setStartingX(0);
        rule.at(0).runCount = 8;
        rule.at(0).stepSize = 1;
        rule.at(0).down = 1;
        rule.at(0).up = 1;
        reset();
        push("dduuuudduuuddddduuudduu");
        assertXEquals(1);
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure5) {
        setStartingX(0);
        rule.at(0).runCount = 5;
        rule.at(0).stepSize = 1;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        reset();
        push("dddduduududdddduuuddddd");
        assertXEquals(1);
    }

    TEST_F(AdaptiveTrackTests, twoSequences) {
        setStartingX(65);
        rule.at(0).runCount = 2;
        rule.at(0).stepSize = 8;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 4;
        rule.at(1).down = 2;
        rule.at(1).up = 1;
        reset();
        pushDown();
        assertXEquals(65);
        pushDown();
        assertXEquals(65 - 8);
        pushDown();
        assertXEquals(65 - 8);
        pushDown();
        assertXEquals(65 - 8 - 8);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
    }

    TEST_F(AdaptiveTrackTests, threeSequences) {
        setStartingX(0);
        rule.at(0).runCount = 1;
        rule.at(0).stepSize = 10;
        rule.at(0).down = 3;
        rule.at(0).up = 1;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 5;
        rule.at(1).down = 3;
        rule.at(1).up = 1;
        rule.at(2).runCount = 6;
        rule.at(2).stepSize = 2;
        rule.at(2).down = 3;
        rule.at(2).up = 1;
        reset();
        push("ddudddudddddudddddduddd");
        assertXEquals(3);
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        setStartingX(65);
        rule.at(0).runCount = 2;
        rule.at(0).stepSize = 8;
        rule.at(0).up = 1;
        rule.at(0).down = 2;
        rule.at(1).runCount = 1;
        rule.at(1).stepSize = 4;
        rule.at(1).up = 2;
        rule.at(1).down = 1;
        reset();
        pushDown();
        assertXEquals(65);
        pushDown();
        assertXEquals(65 - 8);
        pushDown();
        assertXEquals(65 - 8);
        pushDown();
        assertXEquals(65 - 8 - 8);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4);
        pushDown();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
        pushUp();
        assertXEquals(65 - 8 - 8 + 8 + 8 - 4 - 4);
    }
    
    TEST_F(AdaptiveTrackTests, reversals) {
        setStartingX(0);
        rule.at(0).runCount = 1000;
        rule.at(0).down = 2;
        rule.at(0).up = 1;
        reset();
        assertReversalsEquals(0);
        pushUp();
        assertReversalsEquals(0);
        pushDown();
        assertReversalsEquals(0);
        pushDown();
        assertReversalsEquals(1);
        pushUp();
        assertReversalsEquals(2);
        pushDown();
        assertReversalsEquals(2);
        pushDown();
        assertReversalsEquals(3);
    }
}
