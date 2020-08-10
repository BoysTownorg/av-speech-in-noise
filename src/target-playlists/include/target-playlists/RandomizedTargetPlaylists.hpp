#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_

#include "SubdirectoryTargetPlaylistReader.hpp"
#include <recognition-test/TargetPlaylist.hpp>
#include <gsl/gsl>
#include <vector>
#include <string>
#include <memory>

namespace av_speech_in_noise {
namespace target_list {
class Randomizer {
  public:
    virtual ~Randomizer() = default;
    virtual void shuffle(gsl::span<LocalUrl>) = 0;
    virtual void shuffle(gsl::span<int>) = 0;
};
}

class RandomizedTargetPlaylistWithReplacement : public TargetPlaylist {
  public:
    class Factory : public TargetPlaylistFactory {
      public:
        Factory(DirectoryReader *reader, target_list::Randomizer *randomizer)
            : reader{reader}, randomizer{randomizer} {}

        auto make() -> std::shared_ptr<TargetPlaylist> override {
            return std::make_shared<RandomizedTargetPlaylistWithReplacement>(
                reader, randomizer);
        }

      private:
        DirectoryReader *reader;
        target_list::Randomizer *randomizer;
    };

    RandomizedTargetPlaylistWithReplacement(
        DirectoryReader *, target_list::Randomizer *);
    void loadFromDirectory(const LocalUrl &) override;
    auto next() -> LocalUrl override;
    auto current() -> LocalUrl override;
    auto directory() -> LocalUrl override;

  private:
    LocalUrls files{};
    LocalUrl directory_{};
    LocalUrl currentFile{};
    DirectoryReader *reader;
    target_list::Randomizer *randomizer;
};

class RandomizedTargetPlaylistWithoutReplacement
    : public FiniteTargetPlaylistWithRepeatables {
  public:
    RandomizedTargetPlaylistWithoutReplacement(
        DirectoryReader *, target_list::Randomizer *);
    auto empty() -> bool override;
    void loadFromDirectory(const LocalUrl &directory) override;
    auto next() -> LocalUrl override;
    auto current() -> LocalUrl override;
    void reinsertCurrent() override;
    auto directory() -> LocalUrl override;

  private:
    LocalUrls files{};
    LocalUrl directory_{};
    LocalUrl currentFile{};
    DirectoryReader *reader;
    target_list::Randomizer *randomizer;
};

class CyclicRandomizedTargetPlaylist : public TargetPlaylist {
  public:
    class Factory : public TargetPlaylistFactory {
      public:
        Factory(DirectoryReader *reader, target_list::Randomizer *randomizer)
            : reader{reader}, randomizer{randomizer} {}

        auto make() -> std::shared_ptr<TargetPlaylist> override {
            return std::make_shared<CyclicRandomizedTargetPlaylist>(
                reader, randomizer);
        }

      private:
        DirectoryReader *reader;
        target_list::Randomizer *randomizer;
    };

    CyclicRandomizedTargetPlaylist(
        DirectoryReader *, target_list::Randomizer *);
    void loadFromDirectory(const LocalUrl &directory) override;
    auto next() -> LocalUrl override;
    auto current() -> LocalUrl override;
    auto directory() -> LocalUrl override;

  private:
    LocalUrls files{};
    LocalUrl directory_{};
    DirectoryReader *reader;
    target_list::Randomizer *randomizer;
};

class EachTargetPlayedOnceThenShuffleAndRepeat : public FiniteTargetPlaylist {
  public:
    class Factory : public TargetPlaylistFactory {
      public:
        Factory(DirectoryReader *reader, target_list::Randomizer *randomizer)
            : reader{reader}, randomizer{randomizer} {}

        auto make() -> std::shared_ptr<TargetPlaylist> override {
            return std::make_shared<EachTargetPlayedOnceThenShuffleAndRepeat>(
                reader, randomizer);
        }

      private:
        DirectoryReader *reader;
        target_list::Randomizer *randomizer;
    };

    EachTargetPlayedOnceThenShuffleAndRepeat(
        DirectoryReader *, target_list::Randomizer *);
    void loadFromDirectory(const LocalUrl &directory) override;
    auto next() -> LocalUrl override;
    auto current() -> LocalUrl override;
    auto directory() -> LocalUrl override;
    auto empty() -> bool override;
    void setRepeats(gsl::index);

  private:
    LocalUrls files{};
    LocalUrl directory_{};
    LocalUrl currentFile{};
    DirectoryReader *reader;
    target_list::Randomizer *randomizer;
    gsl::index currentIndex{};
    gsl::index repeats{};
    gsl::index endOfPlaylistCount{};
};
}

#endif