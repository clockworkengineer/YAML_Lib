//
// Program: YAML_Files_To_Bencode
//
// Description: Use YAML_Lib to read in a torrent file then write
// it out as YAML using a custom encoder.
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"
#include "Bencode_Stringify.hpp"

namespace yl = YAML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    try {
        const yl::YAML yaml(yl::makeStringify<yl::Bencode_Stringify>());
        // Initialise logging.
        init(plog::debug, "YAML_Files_To_Bencode.log");
        PLOG_INFO << "YAML_Files_To_Bencode started ...";
        PLOG_INFO << YAML_Lib::YAML::version();
        for (const auto &yamlFileName : Utility::createYAMLFileList()) {
            yaml.parse(yl::FileSource(yamlFileName));
            yaml.stringify(yl::FileDestination(Utility::createFileName(yamlFileName, ".ben")));
            PLOG_INFO << "Created file " << Utility::createFileName(yamlFileName, ".ben") << " from " << yamlFileName;
        }
    } catch (const std::exception &ex) {
        PLOG_ERROR << "Error Processing Torrent File: [" << ex.what() << "]\n";
    }
    PLOG_INFO << "YAML_Files_To_Bencode exited.";
    exit(EXIT_SUCCESS);
}
