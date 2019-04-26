#include "TargetListStub.h"
#include "assert-utility.h"
#include <recognition-test/SubdirectoryTargetListReader.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>

namespace {
    using namespace av_coordinate_response_measure;
    
    class TargetListFactoryStub : public TargetListFactory {
        std::vector<std::shared_ptr<TargetList>> lists_{};
    public:
        std::shared_ptr<TargetList> make() override {
            auto list = lists_.front();
            lists_.erase(lists_.begin());
            return list;
        }
        
        void setLists(std::vector<std::shared_ptr<TargetList>> v) {
            lists_ = std::move(v);
        }
    };
    
    class DirectoryReaderStub : public DirectoryReader {
        std::vector<std::string> subDirectories_{};
        std::string directory_{};
    public:
        std::vector<std::string> subDirectories(std::string d) override {
            directory_ = std::move(d);
            return subDirectories_;
        }
        
        auto directory() const {
            return directory_;
        }
        
        void setSubDirectories(std::vector<std::string> v) {
            subDirectories_ = std::move(v);
        }
    };
    
    class SubdirectoryTargetListReaderTests : public ::testing::Test {
    protected:
        TargetListFactoryStub targetListFactory;
        DirectoryReaderStub directoryReader;
        SubdirectoryTargetListReader listReader{&targetListFactory, &directoryReader};
        std::vector<std::shared_ptr<TargetListStub>> targetLists;
        
        void setListCount(int n) {
            targetLists.clear();
            for (int i = 0; i < n; ++i)
                targetLists.push_back(std::make_shared<TargetListStub>());
            targetListFactory.setLists({targetLists.begin(), targetLists.end()});
        }
        
        auto read(std::string s = {}) {
            return listReader.read(std::move(s));
        }
        
        void setSubDirectories(std::vector<std::string> v) {
            setListCount(gsl::narrow<int>(v.size()));
            directoryReader.setSubDirectories(std::move(v));
        }
    };
    
    TEST_F(SubdirectoryTargetListReaderTests, readLoadsEachSubDirectory) {
        setSubDirectories({"a", "b", "c"});
        read();
        assertEqual("a", targetLists.at(0)->directory());
        assertEqual("b", targetLists.at(1)->directory());
        assertEqual("c", targetLists.at(2)->directory());
    }
    
    TEST_F(SubdirectoryTargetListReaderTests, readPassesDirectory) {
        read("a");
        assertEqual("a", directoryReader.directory());
    }
    
    TEST_F(SubdirectoryTargetListReaderTests, readReturnsReadLists) {
        setSubDirectories(std::vector<std::string>(3));
        auto actual = read();
        EXPECT_EQ(targetLists.at(0).get(), actual.at(0).get());
        EXPECT_EQ(targetLists.at(1).get(), actual.at(1).get());
        EXPECT_EQ(targetLists.at(2).get(), actual.at(2).get());
    }
}
