#include "assert-utility.h"
#include <stimulus-list/RandomizedStimulusList.hpp>
#include <stimulus-list/FileFilterDecorator.hpp>
#include <gtest/gtest.h>

namespace {
    class DirectoryReaderStub : public stimulus_list::DirectoryReader {
        std::vector<std::string> fileNames_{};
        std::string directory_{};
    public:
        void setFileNames(std::vector<std::string> files) {
            fileNames_ = files;
        }
        
        std::vector<std::string> filesIn(std::string directory) override {
            directory_ = directory;
            return fileNames_;
        }
        
        auto directory() const {
            return directory_;
        }
    };

    class RandomizerStub : public stimulus_list::Randomizer {
        std::vector<std::string> toShuffle_;
    public:
        auto toShuffle() const {
            return toShuffle_;
        }
        
        void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
            toShuffle_ = {begin, end};
        }
    };

    class RandomizedStimulusListTests : public ::testing::Test {
    protected:
        DirectoryReaderStub reader{};
        RandomizerStub randomizer{};
        stimulus_list::RandomizedStimulusList list{&reader, &randomizer};
        
        void initialize(std::string s = {}) {
            list.initialize(std::move(s));
        }
    };

    TEST_F(
        RandomizedStimulusListTests,
        initializePassesDirectoryToDirectoryReader
    ) {
        initialize("a");
        assertEqual("a", reader.directory());
    }

    TEST_F(
        RandomizedStimulusListTests,
        testCompleteWhenStimulusFilesExhausted
    ) {
        reader.setFileNames({ "a", "b", "c" });
        initialize();
        EXPECT_FALSE(list.empty());
        list.next();
        EXPECT_FALSE(list.empty());
        list.next();
        EXPECT_FALSE(list.empty());
        list.next();
        EXPECT_TRUE(list.empty());
    }

    TEST_F(
        RandomizedStimulusListTests,
        nextReturnsFullPathToFileAtFront
    ) {
        reader.setFileNames({ "a", "b", "c" });
        initialize("C:");
        assertEqual("C:/a", list.next());
        assertEqual("C:/b", list.next());
        assertEqual("C:/c", list.next());
    }

    TEST_F(
        RandomizedStimulusListTests,
        initializeShufflesFileNames
    ) {
        reader.setFileNames({ "a", "b", "c" });
        initialize();
        assertEqual({ "a", "b", "c" }, randomizer.toShuffle());
    }
    
    class FileFilterDecoratorTests : public ::testing::Test {
    protected:
        DirectoryReaderStub reader{};
        
        stimulus_list::FileFilterDecorator construct(std::string filter = {}) {
            return {&reader, std::move(filter)};
        }
    };

    TEST_F(FileFilterDecoratorTests, passesDirectoryToDecorated) {
        auto decorator = construct();
        decorator.filesIn({"a"});
        assertEqual("a", reader.directory());
    }

    TEST_F(FileFilterDecoratorTests, returnsFilteredFiles) {
        auto decorator = construct(".c");
        reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
        assertEqual({ "b.c", "f.c" }, decorator.filesIn({}));
    }
}
