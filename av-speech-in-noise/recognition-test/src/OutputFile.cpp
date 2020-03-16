#include "OutputFile.hpp"
#include <sstream>

namespace av_speech_in_noise {
static auto operator<<(std::ostream &os, const std::vector<int> &v)
    -> std::ostream & {
    if (!v.empty()) {
        auto first{true};
        os << v.front();
        for (auto x : v) {
            if (first) {
                first = false;
                continue;
            }
            os << " ";
            os << x;
        }
    }
    return os;
}

template <typename T>
void writeLabeledLine(
    std::stringstream &stream, const std::string &label, T thing) {
    stream << label;
    stream << ": ";
    stream << thing;
    stream << '\n';
}

template <typename T> void insert(std::stringstream &stream, T item) {
    stream << item;
}

void insert(std::stringstream &stream, HeadingItem item) {
    stream << headingItemName(item);
}

void insertCommaAndSpace(std::stringstream &stream) { stream << ", "; }

void insertNewLine(std::stringstream &stream) { stream << '\n'; }

auto str(std::stringstream &stream) { return stream.str(); }

void writeSubjectId(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "subject", p.subjectId);
}

void writeTester(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "tester", p.testerId);
}

void writeSession(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "session", p.session);
}

void writeMethod(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "method", p.method);
}

void writeRmeSetting(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "RME setting", p.rmeSetting);
}

void writeTransducer(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "transducer", name(p.transducer));
}

void writeMasker(std::stringstream &stream, const Test &p) {
    writeLabeledLine(stream, "masker", p.maskerFilePath);
}

void writeTargetList(std::stringstream &stream, const Test &p) {
    writeLabeledLine(stream, "targets", p.targetListDirectory);
}

void writeMaskerLevel(std::stringstream &stream, const Test &p) {
    writeLabeledLine(stream, "masker level (dB SPL)", p.maskerLevel_dB_SPL);
}

void writeCondition(std::stringstream &stream, const Test &p) {
    writeLabeledLine(stream, "condition", conditionName(p.condition));
}

constexpr auto correct{"correct"};
constexpr auto incorrect{"incorrect"};

static auto evaluation(const open_set::AdaptiveTrial &trial) -> std::string {
    return trial.correct ? correct : incorrect;
}

static auto evaluation(const coordinate_response_measure::Trial &trial)
    -> std::string {
    return trial.correct ? correct : incorrect;
}

static auto format(const AdaptiveTest &test) -> std::string {
    std::stringstream stream;
    const auto &identity = test.identity;
    writeSubjectId(stream, identity);
    writeTester(stream, identity);
    writeSession(stream, identity);
    writeMethod(stream, identity);
    writeRmeSetting(stream, identity);
    writeTransducer(stream, identity);
    writeMasker(stream, test);
    writeTargetList(stream, test);
    writeMaskerLevel(stream, test);
    writeLabeledLine(stream, "starting SNR (dB)", test.startingSnr_dB);
    writeCondition(stream, test);
    std::vector<int> up;
    std::vector<int> down;
    std::vector<int> runCounts;
    std::vector<int> stepSizes;
    for (auto sequence : test.trackingRule) {
        up.push_back(sequence.up);
        down.push_back(sequence.down);
        runCounts.push_back(sequence.runCount);
        stepSizes.push_back(sequence.stepSize);
    }
    writeLabeledLine(stream, "up", up);
    writeLabeledLine(stream, "down", down);
    writeLabeledLine(stream, "reversals per step size", runCounts);
    writeLabeledLine(stream, "step sizes (dB)", stepSizes);
    insertNewLine(stream);
    return stream.str();
}

static auto format(const FixedLevelTest &test) -> std::string {
    std::stringstream stream;
    const auto &identity = test.identity;
    writeSubjectId(stream, identity);
    writeTester(stream, identity);
    writeSession(stream, identity);
    writeMethod(stream, identity);
    writeRmeSetting(stream, identity);
    writeTransducer(stream, identity);
    writeMasker(stream, test);
    writeTargetList(stream, test);
    writeMaskerLevel(stream, test);
    writeLabeledLine(stream, "SNR (dB)", test.snr_dB);
    writeCondition(stream, test);
    insertNewLine(stream);
    return stream.str();
}

static auto format(const coordinate_response_measure::FixedLevelTrial &trial)
    -> std::string {
    std::stringstream stream;
    insert(stream, trial.correctNumber);
    insertCommaAndSpace(stream);
    insert(stream, trial.subjectNumber);
    insertCommaAndSpace(stream);
    insert(stream, colorName(trial.correctColor));
    insertCommaAndSpace(stream);
    insert(stream, colorName(trial.subjectColor));
    insertCommaAndSpace(stream);
    insert(stream, evaluation(trial));
    insertCommaAndSpace(stream);
    insert(stream, trial.target);
    insertNewLine(stream);
    return stream.str();
}

static auto format(const coordinate_response_measure::AdaptiveTrial &trial)
    -> std::string {
    std::stringstream stream;
    insert(stream, trial.SNR_dB);
    insertCommaAndSpace(stream);
    insert(stream, trial.correctNumber);
    insertCommaAndSpace(stream);
    insert(stream, trial.subjectNumber);
    insertCommaAndSpace(stream);
    insert(stream, colorName(trial.correctColor));
    insertCommaAndSpace(stream);
    insert(stream, colorName(trial.subjectColor));
    insertCommaAndSpace(stream);
    insert(stream, evaluation(trial));
    insertCommaAndSpace(stream);
    insert(stream, trial.reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto format(const open_set::FreeResponseTrial &trial) -> std::string {
    std::stringstream stream;
    insert(stream, trial.target);
    insertCommaAndSpace(stream);
    insert(stream, trial.response);
    if (trial.flagged) {
        insertCommaAndSpace(stream);
        insert(stream, "FLAGGED");
    }
    insertNewLine(stream);
    return stream.str();
}

static auto format(const open_set::AdaptiveTrial &trial) -> std::string {
    std::stringstream stream;
    insert(stream, trial.SNR_dB);
    insertCommaAndSpace(stream);
    insert(stream, trial.target);
    insertCommaAndSpace(stream);
    insert(stream, evaluation(trial));
    insertCommaAndSpace(stream);
    insert(stream, trial.reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto format(const open_set::CorrectKeywordsTrial &trial) -> std::string {
    std::stringstream stream;
    insert(stream, trial.SNR_dB);
    insertCommaAndSpace(stream);
    insert(stream, trial.target);
    insertCommaAndSpace(stream);
    insert(stream, trial.count);
    insertCommaAndSpace(stream);
    insert(stream, evaluation(trial));
    insertCommaAndSpace(stream);
    insert(stream, trial.reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto format(const AdaptiveTestResult &result) -> std::string {
    std::stringstream stream;
    writeLabeledLine(stream, "threshold", result.threshold);
    return stream.str();
}

static auto formatOpenSetFreeResponseTrialHeading() -> std::string {
    std::stringstream stream;
    insert(stream, HeadingItem::target);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::freeResponse);
    insertNewLine(stream);
    return stream.str();
}

static auto formatCorrectKeywordsTrialHeading() -> std::string {
    std::stringstream stream;
    insert(stream, HeadingItem::snr_dB);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::target);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::correctKeywords);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::evaluation);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto formatAdaptiveCoordinateResponseTrialHeading() -> std::string {
    std::stringstream stream;
    insert(stream, HeadingItem::snr_dB);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::correctNumber);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::subjectNumber);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::correctColor);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::subjectColor);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::evaluation);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::reversals);
    insertNewLine(stream);
    return stream.str();
}

static auto formatFixedLevelCoordinateResponseTrialHeading() -> std::string {
    std::stringstream stream;
    insert(stream, HeadingItem::correctNumber);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::subjectNumber);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::correctColor);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::subjectColor);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::evaluation);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::stimulus);
    insertNewLine(stream);
    return stream.str();
}

static auto formatOpenSetAdaptiveTrialHeading() -> std::string {
    std::stringstream stream;
    insert(stream, HeadingItem::snr_dB);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::target);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::evaluation);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::reversals);
    insertNewLine(stream);
    return stream.str();
}

OutputFileImpl::OutputFileImpl(Writer *writer, OutputFilePath *path)
    : writer{writer}, path{path} {}

void OutputFileImpl::write(std::string s) { writer->write(std::move(s)); }

void OutputFileImpl::write(
    const coordinate_response_measure::AdaptiveTrial &trial) {
    if (!justWroteAdaptiveCoordinateResponseTrial)
        write(formatAdaptiveCoordinateResponseTrialHeading());
    write(format(trial));
    justWroteAdaptiveCoordinateResponseTrial = true;
}

void OutputFileImpl::write(
    const coordinate_response_measure::FixedLevelTrial &trial) {
    if (!justWroteFixedLevelCoordinateResponseTrial)
        write(formatFixedLevelCoordinateResponseTrialHeading());
    write(format(trial));
    justWroteFixedLevelCoordinateResponseTrial = true;
}

void OutputFileImpl::write(const open_set::FreeResponseTrial &trial) {
    if (!justWroteFreeResponseTrial)
        write(formatOpenSetFreeResponseTrialHeading());
    write(format(trial));
    justWroteFreeResponseTrial = true;
}

void OutputFileImpl::write(const open_set::CorrectKeywordsTrial &trial) {
    if (!justWroteCorrectKeywordsTrial)
        write(formatCorrectKeywordsTrialHeading());
    write(format(trial));
    justWroteCorrectKeywordsTrial = true;
}

void OutputFileImpl::write(const open_set::AdaptiveTrial &trial) {
    if (!justWroteOpenSetAdaptiveTrial)
        write(formatOpenSetAdaptiveTrialHeading());
    write(format(trial));
    justWroteOpenSetAdaptiveTrial = true;
}

void OutputFileImpl::writeTest(const AdaptiveTest &test) {
    write(format(test));
}

void OutputFileImpl::writeTest(const FixedLevelTest &test) {
    write(format(test));
}

void OutputFileImpl::openNewFile(const TestIdentity &test) {
    writer->open(generateNewFilePath(test));
    if (writer->failed())
        throw OpenFailure{};
    justWroteAdaptiveCoordinateResponseTrial = false;
    justWroteFixedLevelCoordinateResponseTrial = false;
    justWroteFreeResponseTrial = false;
    justWroteOpenSetAdaptiveTrial = false;
    justWroteCorrectKeywordsTrial = false;
}

auto OutputFileImpl::generateNewFilePath(const TestIdentity &test)
    -> std::string {
    return path->outputDirectory() + "/" + path->generateFileName(test) +
        ".txt";
}

void OutputFileImpl::close() { writer->close(); }

void OutputFileImpl::save() { writer->save(); }

void OutputFileImpl::write(const AdaptiveTestResult &result) {
    write(format(result));
}
}
