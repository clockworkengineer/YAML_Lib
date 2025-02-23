
//
// Program: YAML_Files_To_YAML
//
// Description: Use YAML_Lib to read in YAML file then write
// it out as YAML using a custom encoder.
//
// Dependencies: C++20, PLOG,  YAML_Lib.
//

#include "YAML_Utility.hpp"
#include "JSON_Stringify.hpp"

namespace yl = YAML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    try {
        const yl::YAML yaml(yl::makeStringify<yl::JSON_Stringify>());
        // Initialise logging.
        init(plog::debug, "YAML_Files_To_JSON.log");
        PLOG_INFO << "YAML_Files_To_YAML started ...";
        PLOG_INFO << YAML_Lib::YAML::version();
        for (const auto &yamlFileName : Utility::createYAMLFileList()) {
            yaml.parse(yl::FileSource(yamlFileName));
            yaml.stringify(yl::FileDestination(
                Utility::createFileName(yamlFileName, ".json")));
            PLOG_INFO << "Created file "
                      << Utility::createFileName(yamlFileName, ".json") << " from "
                      << yamlFileName;
            yaml.parse(yl::FileSource(Utility::createFileName(yamlFileName, ".json")));
        }
    } catch (const std::exception &ex) {
        PLOG_ERROR << "Error Processing File: [" << ex.what() << "]\n";
    }
    PLOG_INFO << "YAML_Files_To_YAML exited.";
    exit(EXIT_SUCCESS);
}