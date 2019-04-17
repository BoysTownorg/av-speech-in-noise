#ifndef target_list_FileFilterDecorator_hpp
#define target_list_FileFilterDecorator_hpp

#include <target-list/RandomizedTargetList.hpp>

namespace target_list {
    class FileFilterDecorator : public DirectoryReader {
        std::vector<std::string> filters;
        DirectoryReader *reader;
    public:
        FileFilterDecorator(DirectoryReader *, std::vector<std::string> filters);
        std::vector<std::string> filesIn(std::string directory) override;
    private:
        bool endingMatchesFilter(
            const std::string &,
            const std::string &filter
        );
        std::vector<std::string> filtered(std::vector<std::string>);
    };
}

#endif
