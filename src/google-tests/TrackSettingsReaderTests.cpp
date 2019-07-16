#include "assert-utility.h"
#include <recognition-test/TrackSettingsReader.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    using Property = TrackSettingsReader::Property;
    
    class TrackSettingsReaderTests : public ::testing::Test {
    protected:
        void assertFileContentsYield(
            std::vector<std::string> v,
            const TrackingRule &expected
        ) {
            TrackSettingsReader reader{concatenate(std::move(v))};
            assertEqual(expected, reader.trackingRule());
        }
        
        std::string concatenate(std::vector<std::string> v) {
            std::string result;
            for (auto v_ : std::move(v))
                result.append(v_);
            return result;
        }
        
        std::string propertyEntry(Property p, std::string s) {
            return
                std::string{TrackSettingsReader::propertyName(p)} +
                ": " +
                std::move(s);
        }
        
        std::string propertyEntryWithNewline(Property p, std::string s) {
            return propertyEntry(p, std::move(s)) + '\n';
        }
    };
    
    TEST_F(TrackSettingsReaderTests, oneSequence) {
        TrackingSequence sequence;
        sequence.up = 1;
        sequence.down = 2;
        sequence.runCount = 3;
        sequence.stepSize = 4;
        assertFileContentsYield(
            {
                propertyEntryWithNewline(Property::up, "1"),
                propertyEntryWithNewline(Property::down, "2"),
                propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
                propertyEntry(Property::stepSizes, "4")
            },
            {sequence}
        );
    }
    
    TEST_F(TrackSettingsReaderTests, twoSequences) {
        TrackingSequence first;
        first.up = 1;
        first.down = 3;
        first.runCount = 5;
        first.stepSize = 7;
        TrackingSequence second;
        second.up = 2;
        second.down = 4;
        second.runCount = 6;
        second.stepSize = 8;
        assertFileContentsYield(
            {
                propertyEntryWithNewline(Property::up, "1 2"),
                propertyEntryWithNewline(Property::down, "3 4"),
                propertyEntryWithNewline(Property::reversalsPerStepSize, "5 6"),
                propertyEntry(Property::stepSizes, "7 8")
            },
            {first, second}
        );
    }
    
    TEST_F(TrackSettingsReaderTests, differentPropertyOrder) {
        TrackingSequence sequence;
        sequence.up = 1;
        sequence.down = 2;
        sequence.runCount = 3;
        sequence.stepSize = 4;
        assertFileContentsYield(
            {
                propertyEntryWithNewline(Property::down, "2"),
                propertyEntryWithNewline(Property::up, "1"),
                propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
                propertyEntry(Property::stepSizes, "4")
            },
            {sequence}
        );
    }
    
    TEST_F(TrackSettingsReaderTests, ignoresAnyUninterpretableEntries) {
        TrackingSequence sequence;
        sequence.up = 1;
        sequence.down = 2;
        sequence.runCount = 3;
        sequence.stepSize = 4;
        assertFileContentsYield(
            {
                " \n",
                propertyEntryWithNewline(Property::down, "2"),
                propertyEntryWithNewline(Property::up, "1"),
                "not: real\n",
                propertyEntryWithNewline(Property::reversalsPerStepSize, "3"),
                propertyEntryWithNewline(Property::stepSizes, "4"),
                " \n"
            },
            {sequence}
        );
    }
}
