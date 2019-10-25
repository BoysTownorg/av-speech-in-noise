#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_

#include "SubdirectoryTargetListReader.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <vector>
#include <string>
#include <memory>

namespace target_list {
class Randomizer {
  public:
    virtual ~Randomizer() = default;
    using shuffle_iterator = std::vector<std::string>::iterator;
    virtual void shuffle(shuffle_iterator begin, shuffle_iterator end) = 0;
    using int_shuffle_iterator = std::vector<int>::iterator;
    virtual void shuffle(
        int_shuffle_iterator begin, int_shuffle_iterator end) = 0;
};

class RandomizedTargetList : public av_speech_in_noise::TargetList {
    std::vector<std::string> files{};
    std::string directory_{};
    std::string currentFile_{};
    DirectoryReader *reader;
    Randomizer *randomizer;
    bool noFilesGotten{};

  public:
    RandomizedTargetList(DirectoryReader *, Randomizer *);
    void loadFromDirectory(std::string) override;
    std::string next() override;
    std::string current() override;
    bool empty() override;

  private:
    std::string fullPath(std::string file);
    void shuffle();
    bool empty_();
    void replaceLastFile();
};

class RandomizedTargetListFactory : public TargetListFactory {
    DirectoryReader *reader;
    Randomizer *randomizer;

  public:
    RandomizedTargetListFactory(DirectoryReader *reader, Randomizer *randomizer)
        : reader{reader}, randomizer{randomizer} {}

    std::shared_ptr<av_speech_in_noise::TargetList> make() override {
        return std::make_shared<RandomizedTargetList>(reader, randomizer);
    }
};

class RandomizedFiniteTargetList : public av_speech_in_noise::TargetList {
    std::vector<std::string> files{};
    std::string directory_{};
    std::string currentFile_{};
    DirectoryReader *reader;
    Randomizer *randomizer;

  public:
    RandomizedFiniteTargetList(DirectoryReader *, Randomizer *);
    bool empty() override;
    void loadFromDirectory(std::string directory) override;
    std::string next() override;
    std::string current() override;

  private:
    bool empty_();
    std::string fullPath(std::string file);
};
}

#endif
