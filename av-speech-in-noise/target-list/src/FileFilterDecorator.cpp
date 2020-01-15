#include "FileFilterDecorator.hpp"
#include <gsl/gsl>
#include <numeric>
#include <algorithm>

namespace target_list {
static auto at(const std::vector<int> &x, gsl::index n) -> int {
    return x.at(n);
}

static auto at(const std::vector<std::string> &x, gsl::index n) -> std::string {
    return x.at(n);
}

static auto vectorOfStrings(gsl::index size) -> std::vector<std::string> {
    return std::vector<std::string>(size);
}

static auto size(const std::vector<std::string> &s) -> gsl::index {
    return s.size();
}

FileFilterDecorator::FileFilterDecorator(
    DirectoryReader *reader, FileFilter *filter)
    : reader{reader}, filter{filter} {}

auto FileFilterDecorator::filesIn(std::string directory)
    -> std::vector<std::string> {
    return filter->filter(reader->filesIn(std::move(directory)));
}

auto FileFilterDecorator::subDirectories(std::string directory)
    -> std::vector<std::string> {
    return reader->subDirectories(std::move(directory));
}

FileExtensionFilter::FileExtensionFilter(std::vector<std::string> filters)
    : filters{std::move(filters)} {}

static auto endingMatchesFilter(
    const std::string &file, const std::string &filter) -> bool {
    return file.length() >= filter.length() &&
        0 ==
        file.compare(file.length() - filter.length(), filter.length(), filter);
}

auto FileExtensionFilter::filter(std::vector<std::string> files)
    -> std::vector<std::string> {
    std::vector<std::string> filtered_{};
    for (const auto &file : files)
        for (const auto &filter : filters)
            if (endingMatchesFilter(file, filter))
                filtered_.push_back(file);
    return filtered_;
}

FileIdentifierExcluderFilter::FileIdentifierExcluderFilter(
    std::vector<std::string> identifiers)
    : identifiers{std::move(identifiers)} {}

static auto endsWith(const std::string &s, const std::string &what) -> bool {
    auto withoutExtension = s.substr(0, s.find('.'));
    return endingMatchesFilter(withoutExtension, what);
}

auto FileIdentifierExcluderFilter::filter(std::vector<std::string> files)
    -> std::vector<std::string> {
    std::vector<std::string> filtered_{};
    for (const auto &file : files) {
        bool exclude = false;
        for (const auto &identifier : identifiers)
            if (endsWith(file, identifier)) {
                exclude = true;
                break;
            }
        if (!exclude)
            filtered_.push_back(file);
    }
    return filtered_;
}

FileIdentifierFilter::FileIdentifierFilter(std::string identifier)
    : identifier{std::move(identifier)} {}

auto FileIdentifierFilter::filter(std::vector<std::string> files)
    -> std::vector<std::string> {
    std::vector<std::string> filtered_{};
    for (const auto &file : files)
        if (containsIdentifier(file))
            filtered_.push_back(file);
    return filtered_;
}

auto FileIdentifierFilter::containsIdentifier(const std::string &file) -> bool {
    return file.find(identifier) != std::string::npos;
}

RandomSubsetFiles::RandomSubsetFiles(Randomizer *randomizer, int N)
    : randomizer{randomizer}, N{N} {}

auto RandomSubsetFiles::filter(std::vector<std::string> files)
    -> std::vector<std::string> {
    if (size(files) < N)
        return files;
    std::vector<int> indices(files.size());
    std::iota(indices.begin(), indices.end(), 0);
    randomizer->shuffle(indices.begin(), indices.end());
    auto subset = vectorOfStrings(N);
    std::generate(subset.begin(), subset.end(),
        [&, n = 0]() mutable { return at(files, at(indices, n++)); });
    return subset;
}

DirectoryReaderComposite::DirectoryReaderComposite(
    std::vector<DirectoryReader *> readers)
    : readers{std::move(readers)} {}

auto DirectoryReaderComposite::subDirectories(std::string directory)
    -> std::vector<std::string> {
    return readers.front()->subDirectories(std::move(directory));
}

auto DirectoryReaderComposite::filesIn(std::string directory)
    -> std::vector<std::string> {
    std::vector<std::string> files;
    for (auto r : readers) {
        auto next = r->filesIn(directory);
        files.insert(files.end(), next.begin(), next.end());
    }
    return files;
}
}
