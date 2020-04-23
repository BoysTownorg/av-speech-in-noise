#include "RandomizedTargetList.hpp"
#include <algorithm>

namespace target_list {
static auto filesIn(DirectoryReader *reader, std::string s)
    -> std::vector<std::string> {
    return reader->filesIn(std::move(s));
}

static auto shuffle(Randomizer *randomizer, gsl::span<std::string> v) {
    randomizer->shuffle(v);
}

static auto empty(const std::vector<std::string> &files) -> bool {
    return files.empty();
}

static auto currentFile(const std::vector<std::string> &v) -> std::string {
    return empty(v) ? "" : v.back();
}

static auto joinPaths(const std::string &directory, const std::string &file)
    -> std::string {
    return directory + '/' + file;
}

static auto fullPathToLastFile(const std::string &directory,
    const std::vector<std::string> &files) -> std::string {
    return empty(files) ? "" : joinPaths(directory, currentFile(files));
}

static void moveFrontToBack(std::vector<std::string> &files) {
    std::rotate(files.begin(), files.begin() + 1, files.end());
}

static auto allButLast(gsl::span<std::string> s) -> gsl::span<std::string> {
    return s.first(s.size() - 1);
}

RandomizedTargetListWithReplacement::RandomizedTargetListWithReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithReplacement::loadFromDirectory(
    const av_speech_in_noise::LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d.path));
}

auto RandomizedTargetListWithReplacement::next()
    -> av_speech_in_noise::LocalUrl {
    if (empty(files))
        return {""};

    moveFrontToBack(files);
    shuffle(randomizer, allButLast(files));
    return {fullPathToLastFile(directory_, files)};
}

auto RandomizedTargetListWithReplacement::current()
    -> av_speech_in_noise::LocalUrl {
    return {fullPathToLastFile(directory_, files)};
}

auto RandomizedTargetListWithReplacement::directory()
    -> av_speech_in_noise::LocalUrl {
    return {directory_};
}

RandomizedTargetListWithoutReplacement::RandomizedTargetListWithoutReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithoutReplacement::loadFromDirectory(
    const av_speech_in_noise::LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d.path));
}

auto RandomizedTargetListWithoutReplacement::empty() -> bool {
    return target_list::empty(files);
}

auto RandomizedTargetListWithoutReplacement::next()
    -> av_speech_in_noise::LocalUrl {
    if (target_list::empty(files))
        return {""};

    currentFile = files.front();
    files.erase(files.begin());
    return {joinPaths(directory_, currentFile)};
}

auto RandomizedTargetListWithoutReplacement::current()
    -> av_speech_in_noise::LocalUrl {
    return currentFile.empty()
        ? av_speech_in_noise::LocalUrl{""}
        : av_speech_in_noise::LocalUrl{joinPaths(directory_, currentFile)};
}

auto RandomizedTargetListWithoutReplacement::directory()
    -> av_speech_in_noise::LocalUrl {
    return {directory_};
}

void RandomizedTargetListWithoutReplacement::reinsertCurrent() {
    files.push_back(currentFile);
}

CyclicRandomizedTargetList::CyclicRandomizedTargetList(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void CyclicRandomizedTargetList::loadFromDirectory(
    const av_speech_in_noise::LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d.path));
}

auto CyclicRandomizedTargetList::next() -> av_speech_in_noise::LocalUrl {
    if (empty(files))
        return {""};

    moveFrontToBack(files);
    return {fullPathToLastFile(directory_, files)};
}

auto CyclicRandomizedTargetList::current() -> av_speech_in_noise::LocalUrl {
    return {fullPathToLastFile(directory_, files)};
}

auto CyclicRandomizedTargetList::directory() -> av_speech_in_noise::LocalUrl {
    return {directory_};
}
}
