#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/OutputFileImpl.hpp>
#include <gtest/gtest.h>

namespace {
    using namespace av_speech_in_noise;
    
    class WriterStub : public Writer {
        LogString written_;
        std::string filePath_;
        bool closed_{};
        bool saved_{};
    public:
        void save() override {
            saved_ = true;
        }
        
        auto saved() const {
            return saved_;
        }
        
        void close() override {
            closed_ = true;
        }
        
        void open(std::string f) override {
            filePath_ = std::move(f);
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        auto closed() const {
            return closed_;
        }
        
        auto &written() const {
            return written_;
        }
        
        void write(std::string s) override {
            written_.insert(std::move(s));
        }
        
        bool failed() override {
            return {};
        }
    };
    
    class OutputFilePathStub : public OutputFilePath {
        std::string fileName_;
        std::string homeDirectory_;
        std::string outputDirectory_;
        const TestInformation *testInformation_{};
    public:
        std::string outputDirectory() override {
            return outputDirectory_;
        }
        
        void setOutputDirectory(std::string s) {
            outputDirectory_ = std::move(s);
        }
        
        void setFileName(std::string s) {
            fileName_ = std::move(s);
        }
        
        std::string generateFileName(const TestInformation &p) override {
            testInformation_ = &p;
            return fileName_;
        }
        
        std::string homeDirectory() override {
            return homeDirectory_;
        }
        
        auto testInformation() const {
            return testInformation_;
        }
    };
    
    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(OutputFileImpl &) = 0;
    };
    
    class WritingTestUseCase : public virtual UseCase {
    public:
        virtual void setCondition(Condition) = 0;
        virtual void setTestInfo(const TestInformation &) = 0;
    };
    
    class WritingAdaptiveTest : public WritingTestUseCase {
        AdaptiveTest test{};
    public:
        void setCondition(Condition c) override {
            test.common.condition = c;
        }
        
        void setTestInfo(const TestInformation &p) override {
            test.information = p;
        }
        
        void run(OutputFileImpl &file) override {
            file.writeTest(test);
        }
    };
    
    class WritingFixedLevelTest : public WritingTestUseCase {
        FixedLevelTest test{};
    public:
        void setCondition(Condition c) override {
            test.common.condition = c;
        }
        
        void setTestInfo(const TestInformation &p) override {
            test.information = p;
        }
        
        void run(OutputFileImpl &file) override {
            file.writeTest(test);
        }
    };
    
    class WritingTrialUseCase : public virtual UseCase {
    public:
        virtual void incorrect() = 0;
        virtual void correct() = 0;
        virtual int evaluationEntryIndex() = 0;
    };
    
    void setCorrect(coordinate_response_measure::Trial &trial) {
        trial.correct = true;
    }
    
    void setIncorrect(coordinate_response_measure::Trial &trial) {
        trial.correct = false;
    }
    
    class WritingAdaptiveCoordinateResponseTrial : public WritingTrialUseCase {
        coordinate_response_measure::AdaptiveTrial trial_{};
    public:
        auto &trial() {
            return trial_;
        }
        
        void incorrect() override {
            setIncorrect(trial_.trial);
        }
        
        void correct() override {
            setCorrect(trial_.trial);
        }
        
        int evaluationEntryIndex() override {
            return 6;
        }
        
        void run(av_speech_in_noise::OutputFileImpl &file) override {
            file.writeTrial(trial_);
        }
    };
    
    class WritingFixedLevelCoordinateResponseTrial : public WritingTrialUseCase {
        coordinate_response_measure::FixedLevelTrial trial_{};
    public:
        auto &trial() {
            return trial_;
        }
        
        void incorrect() override {
            setIncorrect(trial_.trial);
        }
        
        void correct() override {
            setCorrect(trial_.trial);
        }
        
        void run(av_speech_in_noise::OutputFileImpl &file) override {
            file.writeTrial(trial_);
        }
        
        int evaluationEntryIndex() override {
            return 5;
        }
    };

    class OutputFileTests : public ::testing::Test {
    protected:
        WriterStub writer;
        OutputFilePathStub path;
        OutputFileImpl file{&writer, &path};
        WritingAdaptiveCoordinateResponseTrial
            writingAdaptiveCoordinateResponseTrial;
        WritingFixedLevelCoordinateResponseTrial
            writingFixedLevelCoordinateResponseTrial;
        FreeResponseTrial freeResponseTrial;
        AdaptiveTest adaptiveTest;
        FixedLevelTest fixedLevelTest;
        TestInformation testInformation;
        WritingFixedLevelTest writingFixedLevelTest;
        WritingAdaptiveTest writingAdaptiveTest;
        
        void run(UseCase &useCase) {
            useCase.run(file);
        }
        
        void openNewFile() {
            file.openNewFile(testInformation);
        }
        
        void writeFreeResponseTrial() {
            file.writeTrial(freeResponseTrial);
        }
        
        const auto &written() {
            return writer.written();
        }
        
        void assertWriterContainsConditionName(Condition c) {
            std::string name = conditionName(c);
            assertWriterContains("condition: " + name + "\n");
        }
        
        void assertWriterContains(std::string s) {
            assertTrue(written().contains(std::move(s)));
        }
        
        void assertWrittenLast(std::string s) {
            assertTrue(written().endsWith(std::move(s)));
        }
        
        std::string nthWrittenEntryOfLine(int n, int line) {
            std::string written_ = written();
            auto precedingNewLine = find_nth_element(written_, line - 1, '\n');
            auto line_ = written_.substr(precedingNewLine + 1);
            auto precedingComma = find_nth_element(line_, n - 1, ',');
            auto entryBeginning = (precedingComma == std::string::npos)
                ? 0U
                : precedingComma + 2;
            return upUntilFirstOfAny(line_.substr(entryBeginning), {',', '\n'});
        }

        std::string::size_type find_nth_element(const std::string &content, int n, char what) {
            auto found = std::string::npos;
            for (int i = 0; i < n; ++i)
                found = content.find(what, found + 1U);
            return found;
        }

        std::string upUntilFirstOfAny(const std::string &content, std::vector<char> v) {
            return content.substr(0, content.find_first_of({v.begin(), v.end()}));
        }
        
        void assertConditionNameWritten(
            WritingTestUseCase &useCase,
            Condition c
        ) {
            useCase.setCondition(c);
            useCase.run(file);
            assertWriterContainsConditionName(c);
        }
        
        void assertTestInformationWritten(WritingTestUseCase &useCase) {
            TestInformation info;
            info.subjectId = "a";
            info.testerId = "b";
            info.session = "c";
            useCase.setTestInfo(info);
            useCase.run(file);
            assertWriterContains("subject: a\n");
            assertWriterContains("tester: b\n");
            assertWriterContains("session: c\n");
        }
        
        void assertNthEntryOfFirstLine(std::string what, int n) {
            assertEqual(std::move(what), nthWrittenEntryOfLine(n, 1));
        }
        
        void assertNthEntryOfSecondLine(std::string what, int n) {
            assertEqual(std::move(what), nthWrittenEntryOfLine(n, 2));
        }
        
        void assertNthEntryOfThirdLine(std::string what, int n) {
            assertEqual(std::move(what), nthWrittenEntryOfLine(n, 3));
        }
        
        void assertIncorrectTrialWritesEvaluation(WritingTrialUseCase &useCase) {
            useCase.incorrect();
            run(useCase);
            assertNthEntryOfSecondLine("incorrect", useCase.evaluationEntryIndex());
        }
        
        void assertCorrectTrialWritesEvaluation(WritingTrialUseCase &useCase) {
            useCase.correct();
            run(useCase);
            assertNthEntryOfSecondLine("correct", useCase.evaluationEntryIndex());
        }
    };

    TEST_F(OutputFileTests, writeAdaptiveCoordinateResponseTrialHeading) {
        run(writingAdaptiveCoordinateResponseTrial);
        assertNthEntryOfFirstLine("SNR (dB)", 1);
        assertNthEntryOfFirstLine("correct number", 2);
        assertNthEntryOfFirstLine("subject number", 3);
        assertNthEntryOfFirstLine("correct color", 4);
        assertNthEntryOfFirstLine("subject color", 5);
        assertNthEntryOfFirstLine("evaluation", writingAdaptiveCoordinateResponseTrial.evaluationEntryIndex());
        assertNthEntryOfFirstLine("reversals", 7);
    }

    TEST_F(OutputFileTests, writeFixedLevelCoordinateResponseTrialHeading) {
        run(writingFixedLevelCoordinateResponseTrial);
        assertNthEntryOfFirstLine("correct number", 1);
        assertNthEntryOfFirstLine("subject number", 2);
        assertNthEntryOfFirstLine("correct color", 3);
        assertNthEntryOfFirstLine("subject color", 4);
        assertNthEntryOfFirstLine("evaluation", writingFixedLevelCoordinateResponseTrial.evaluationEntryIndex());
    }

    TEST_F(OutputFileTests, writeAdaptiveCoordinateResponseTrial) {
        writingAdaptiveCoordinateResponseTrial.trial().SNR_dB = 1;
        writingAdaptiveCoordinateResponseTrial.trial().trial.correctNumber = 2;
        writingAdaptiveCoordinateResponseTrial.trial().trial.subjectNumber = 3;
        writingAdaptiveCoordinateResponseTrial.trial().trial.correctColor =
            coordinate_response_measure::Color::green;
        writingAdaptiveCoordinateResponseTrial.trial().trial.subjectColor =
            coordinate_response_measure::Color::red;
        writingAdaptiveCoordinateResponseTrial.trial().reversals = 4;
        run(writingAdaptiveCoordinateResponseTrial);
        assertNthEntryOfSecondLine("1", 1);
        assertNthEntryOfSecondLine("2", 2);
        assertNthEntryOfSecondLine("3", 3);
        assertNthEntryOfSecondLine("green", 4);
        assertNthEntryOfSecondLine("red", 5);
        assertNthEntryOfSecondLine("4", 7);
    }

    TEST_F(OutputFileTests, writeAdaptiveCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
        run(writingAdaptiveCoordinateResponseTrial);
        writingAdaptiveCoordinateResponseTrial.trial().SNR_dB = 1;
        writingAdaptiveCoordinateResponseTrial.trial().trial.correctNumber = 2;
        writingAdaptiveCoordinateResponseTrial.trial().trial.subjectNumber = 3;
        writingAdaptiveCoordinateResponseTrial.trial().trial.correctColor =
            coordinate_response_measure::Color::green;
        writingAdaptiveCoordinateResponseTrial.trial().trial.subjectColor =
            coordinate_response_measure::Color::red;
        writingAdaptiveCoordinateResponseTrial.trial().reversals = 4;
        run(writingAdaptiveCoordinateResponseTrial);
        assertNthEntryOfThirdLine("1", 1);
        assertNthEntryOfThirdLine("2", 2);
        assertNthEntryOfThirdLine("3", 3);
        assertNthEntryOfThirdLine("green", 4);
        assertNthEntryOfThirdLine("red", 5);
        assertNthEntryOfThirdLine("4", 7);
    }

    TEST_F(OutputFileTests, writeFixedLevelCoordinateResponseTrial) {
        writingFixedLevelCoordinateResponseTrial.trial().trial.correctNumber = 2;
        writingFixedLevelCoordinateResponseTrial.trial().trial.subjectNumber = 3;
        writingFixedLevelCoordinateResponseTrial.trial().trial.correctColor =
            coordinate_response_measure::Color::green;
        writingFixedLevelCoordinateResponseTrial.trial().trial.subjectColor =
            coordinate_response_measure::Color::red;
        run(writingFixedLevelCoordinateResponseTrial);
        assertNthEntryOfSecondLine("2", 1);
        assertNthEntryOfSecondLine("3", 2);
        assertNthEntryOfSecondLine("green", 3);
        assertNthEntryOfSecondLine("red", 4);
    }

    TEST_F(OutputFileTests, writeFixedLevelCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
        run(writingFixedLevelCoordinateResponseTrial);
        writingFixedLevelCoordinateResponseTrial.trial().trial.correctNumber = 2;
        writingFixedLevelCoordinateResponseTrial.trial().trial.subjectNumber = 3;
        writingFixedLevelCoordinateResponseTrial.trial().trial.correctColor =
            coordinate_response_measure::Color::green;
        writingFixedLevelCoordinateResponseTrial.trial().trial.subjectColor =
            coordinate_response_measure::Color::red;
        run(writingFixedLevelCoordinateResponseTrial);
        assertNthEntryOfThirdLine("2", 1);
        assertNthEntryOfThirdLine("3", 2);
        assertNthEntryOfThirdLine("green", 3);
        assertNthEntryOfThirdLine("red", 4);
    }

    TEST_F(OutputFileTests, writeIncorrectAdaptiveCoordinateResponseTrial) {
        assertIncorrectTrialWritesEvaluation(writingAdaptiveCoordinateResponseTrial);
    }

    TEST_F(OutputFileTests, writeCorrectAdaptiveCoordinateResponseTrial) {
        assertCorrectTrialWritesEvaluation(writingAdaptiveCoordinateResponseTrial);
    }

    TEST_F(OutputFileTests, writeIncorrectFixedLevelCoordinateResponseTrial) {
        assertIncorrectTrialWritesEvaluation(writingFixedLevelCoordinateResponseTrial);
    }

    TEST_F(OutputFileTests, writeCorrectFixedLevelCoordinateResponseTrial) {
        assertCorrectTrialWritesEvaluation(writingFixedLevelCoordinateResponseTrial);
    }

    TEST_F(OutputFileTests, uninitializedColorDoesNotBreak) {
        coordinate_response_measure::AdaptiveTrial uninitialized;
        file.writeTrial(uninitialized);
    }

    TEST_F(OutputFileTests, writeFreeResponseTrial) {
        freeResponseTrial.target = "a";
        freeResponseTrial.response = "b";
        writeFreeResponseTrial();
        assertNthEntryOfSecondLine("a", 1);
        assertNthEntryOfSecondLine("b", 2);
    }

    TEST_F(OutputFileTests, writeFreeResponseTrialTwiceDoesNotWriteHeadingTwice) {
        writeFreeResponseTrial();
        freeResponseTrial.target = "a";
        freeResponseTrial.response = "b";
        writeFreeResponseTrial();
        assertNthEntryOfThirdLine("a", 1);
        assertNthEntryOfThirdLine("b", 2);
    }

    TEST_F(OutputFileTests, writeFreeResponseTrialHeading) {
        writeFreeResponseTrial();
        assertNthEntryOfFirstLine("target", 1);
        assertNthEntryOfFirstLine("response", 2);
    }

    TEST_F(OutputFileTests, writeAdaptiveTest) {
        adaptiveTest.common.maskerFilePath = "a";
        adaptiveTest.common.targetListDirectory = "d";
        adaptiveTest.common.maskerLevel_dB_SPL = 1;
        adaptiveTest.startingSnr_dB = 2;
        file.writeTest(adaptiveTest);
        assertWriterContains("masker: a\n");
        assertWriterContains("targets: d\n");
        assertWriterContains("masker level (dB SPL): 1\n");
        assertWriterContains("starting SNR (dB): 2\n");
        assertWrittenLast("\n\n");
    }

    TEST_F(OutputFileTests, writeFixedLevelTest) {
        fixedLevelTest.common.maskerFilePath = "a";
        fixedLevelTest.common.targetListDirectory = "d";
        fixedLevelTest.common.maskerLevel_dB_SPL = 1;
        fixedLevelTest.snr_dB = 2;
        file.writeTest(fixedLevelTest);
        assertWriterContains("masker: a\n");
        assertWriterContains("targets: d\n");
        assertWriterContains("masker level (dB SPL): 1\n");
        assertWriterContains("SNR (dB): 2\n");
        assertWrittenLast("\n\n");
    }

    TEST_F(OutputFileTests, writeAdaptiveTestInformation) {
        assertTestInformationWritten(writingAdaptiveTest);
    }

    TEST_F(OutputFileTests, writeFixedLevelTestInformation) {
        assertTestInformationWritten(writingFixedLevelTest);
    }

    TEST_F(OutputFileTests, writeAdaptiveTestWithAvCondition) {
        assertConditionNameWritten(writingAdaptiveTest, Condition::audioVisual);
    }

    TEST_F(OutputFileTests, writeAdaptiveTestWithAuditoryOnlyCondition) {
        assertConditionNameWritten(writingAdaptiveTest, Condition::auditoryOnly);
    }

    TEST_F(OutputFileTests, writeFixedLevelTestWithAvCondition) {
        assertConditionNameWritten(writingFixedLevelTest, Condition::audioVisual);
    }

    TEST_F(OutputFileTests, writeFixedLevelTestWithAuditoryOnlyCondition) {
        assertConditionNameWritten(writingFixedLevelTest, Condition::auditoryOnly);
    }

    TEST_F(OutputFileTests, openPassesFormattedFilePath) {
        path.setFileName("a");
        path.setOutputDirectory("b");
        openNewFile();
        assertEqual("b/a.txt", writer.filePath());
    }

    TEST_F(OutputFileTests, closeClosesWriter) {
        file.close();
        assertTrue(writer.closed());
    }

    TEST_F(OutputFileTests, saveSavesWriter) {
        file.save();
        assertTrue(writer.saved());
    }
 
    TEST_F(OutputFileTests, openPassesTestInformation) {
        openNewFile();
        EXPECT_EQ(&testInformation, path.testInformation());
    }
    
    class FailingWriter : public Writer {
        bool failed_{};
    public:
        void open(std::string) override {
            failed_ = true;
        }
        
        bool failed() override {
            return failed_;
        }
        
        void close() override {}
        void write(std::string) override {}
        void save() override {}
    };
    
    TEST(
        FailingOutputFileTests,
        openThrowsOpenFailureWhenWriterFails
    ) {
        FailingWriter writer;
        OutputFilePathStub path;
        OutputFileImpl file{&writer, &path};
        try {
            file.openNewFile({});
            FAIL() << "Expected OutputFileImpl::OpenFailure";
        }
        catch (const OutputFileImpl::OpenFailure &) {
        }
    }
}
