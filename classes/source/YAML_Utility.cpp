#include <string_view>
#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "implementation/io/YAML_Sources.hpp"
#include "implementation/io/YAML_Destinations.hpp"
#include <sstream>
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
    std::ostringstream oss;
    class StringDestination : public IDestination {
    public:
        explicit StringDestination(std::ostringstream &oss) : oss_(oss) {}
        void add(char ch) override { oss_ << ch; }
        void clear() override { oss_.str(""); }
        char last() override { auto s = oss_.str(); return s.empty() ? '\0' : s.back(); }
    private:
        std::ostringstream &oss_;
    } dest(oss);
    this->stringify(dest);
    return oss.str();
}

} // namespace YAML_Lib
