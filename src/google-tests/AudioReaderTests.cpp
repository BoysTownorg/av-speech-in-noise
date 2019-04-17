#include "assert-utility.h"
#include <stimulus-players/AudioReaderImpl.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>

namespace {
    class AudioBufferStub : public stimulus_players::AudioBuffer {
        std::vector<std::vector<int>> audio_{};
    public:
        void setAudio(std::vector<std::vector<int>> x) {
            audio_ = std::move(x);
        }
        
        int channels() override {
            return gsl::narrow<int>(audio_.size());
        }
        
        std::vector<int> channel(int i) override {
            return audio_.at(i);
        }
        
        bool empty() override {
            return audio_.empty();
        }
    };
    
    class BufferedAudioReaderStub : public stimulus_players::BufferedAudioReader {
        std::vector<std::vector<std::vector<int>>> buffersPartTwo_{};
        std::shared_ptr<AudioBufferStub> buffer =
            std::make_shared<AudioBufferStub>();
        std::string file_{};
        int minimumPossibleSample_{};
        bool failOnLoad_{};
        bool failed_{};
    public:
        void failOnLoad() {
            failOnLoad_ = true;
        }
        
        auto file() const {
            return file_;
        }
        
        std::shared_ptr<stimulus_players::AudioBuffer> readNextBuffer() override {
            std::vector<std::vector<int>> next{};
            if (buffersPartTwo_.size()) {
                next = buffersPartTwo_.front();
                buffersPartTwo_.erase(buffersPartTwo_.begin());
            }
            buffer->setAudio(next);
            return buffer;
        }
        
        void loadFile(std::string s) override {
            file_ = std::move(s);
            if (failOnLoad_)
                failed_ = true;
        }
        
        bool failed() override {
            return failed_;
        }
        
        int minimumPossibleSample() override {
            return minimumPossibleSample_;
        }
        
        void setMinimumPossibleSample(int x) {
            minimumPossibleSample_ = x;
        }
        
        void setBuffers(std::vector<std::vector<std::vector<int>>> x) {
            buffersPartTwo_ = std::move(x);
        }
    };

    class AudioReaderTests : public ::testing::Test {
    protected:
        BufferedAudioReaderStub bufferedReader{};
        stimulus_players::AudioReaderImpl reader{&bufferedReader};
        
        template<typename T>
        std::vector<T> dividedBy(std::vector<T> x, T c) {
            std::for_each(x.begin(), x.end(), [&](T &x_) { x_ /= c; });
            return x;
        }
    };
    
    TEST_F(AudioReaderTests, loadsFile) {
        reader.read("a");
        assertEqual("a", bufferedReader.file());
    }
    
    TEST_F(AudioReaderTests, readThrowsInvalidFileOnFailure) {
        bufferedReader.failOnLoad();
        try {
            reader.read({});
            FAIL() << "Expected stimulus_players::AudioReader::InvalidFile";
        } catch (const stimulus_players::AudioReaderImpl::InvalidFile &) {
        }
    }
    
    TEST_F(AudioReaderTests, readConcatenatesNormalizedBuffers) {
        bufferedReader.setBuffers({{
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        }, {
            { 10, 11, 12 },
            { 13, 14, 15 },
            { 16, 17, 18 }
        }, {
            { 19, 20, 21 },
            { 22, 23, 24 },
            { 25, 26, 27 }
        }});
        bufferedReader.setMinimumPossibleSample(-3);
        assertEqual({
                dividedBy({ 1, 2, 3, 10, 11, 12, 19, 20, 21 }, -3.f),
                dividedBy({ 4, 5, 6, 13, 14, 15, 22, 23, 24 }, -3.f),
                dividedBy({ 7, 8, 9, 16, 17, 18, 25, 26, 27 }, -3.f)
            },
            reader.read({})
        );
    }
}
