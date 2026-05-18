#include <string_view>
#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "implementation/io/YAML_Sources.hpp"
#include "implementation/io/YAML_Destinations.hpp"
#include <fstream>

namespace YAML_Lib {

std::unique_ptr<YAML> YAML::fromString(const std::string_view &yaml_string) {
    return std::make_unique<YAML>(yaml_string);
}

#ifdef YAML_LIB_FILE_IO
std::unique_ptr<YAML> YAML::fromFileToYAML(const std::string_view &file_name) {
    std::string content = YAML::fromFile(file_name);
    return std::make_unique<YAML>(content);
}
#endif

std::string YAML::toString() const {
    std::string output;
    output.reserve(4096);
    class StringDestination : public IDestination {
    public:
        explicit StringDestination(std::string &output) : output_(output) {}
        void add(char ch) override { output_.push_back(ch); }
        void clear() override { output_.clear(); }
        char last() override { return output_.empty() ? '\0' : output_.back(); }
    private:
        std::string &output_;
    } dest(output);
    this->stringify(dest);
    return output;
}

} // namespace YAML_Lib
