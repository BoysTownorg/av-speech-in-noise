#ifndef target_list_RandomizedStimulusList_hpp
#define target_list_RandomizedStimulusList_hpp

#include <recognition-test/RecognitionTestModel.hpp>
#include <recognition-test/SubdirectoryTargetListReader.hpp>
#include <vector>
#include <string>

namespace target_list {
    class DirectoryReader {
    public:
        virtual ~DirectoryReader() = default;
        virtual std::vector<std::string> filesIn(std::string directory) = 0;
    };
    
    class Randomizer {
    public:
        virtual ~Randomizer() = default;
        using shuffle_iterator = std::vector<std::string>::iterator;
        virtual void shuffle(shuffle_iterator begin, shuffle_iterator end) = 0;
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
        bool empty();
        std::string next() override;
        std::string current() override;
    private:
        std::string fullPath(std::string file);
        void shuffle();
        bool empty_();
        void replaceLastFile();
    };
    
    class RandomizedTargetListFactory : public av_speech_in_noise::TargetListFactory {
        DirectoryReader *reader;
        Randomizer *randomizer;
    public:
        RandomizedTargetListFactory(DirectoryReader *reader, Randomizer *randomizer) :
            reader{reader},
            randomizer{randomizer} {}
        
        std::shared_ptr<av_speech_in_noise::TargetList> make() override {
            return std::make_shared<RandomizedTargetList>(reader, randomizer);
        }
    };
    
    class RandomizedFiniteTargetList : public av_speech_in_noise::FiniteTargetList {
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
        std::string fullPath(std::string file);
    };
}

#endif
