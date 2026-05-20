//
// Program: YAML_YAML_Torrent_Files_To_XML
//
// Description: Use YAML_Lib to read in YAML file then write
// it out as XML using a custom encoder.
//
// Dependencies: C++20, YAML_Lib.
//

#include "YAML_Utility.hpp"
#include "XML_Stringify.hpp"
#include <iostream>

namespace yl = YAML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    try {
        const yl::YAML yaml(yl::makeStringify<yl::XML_Stringify>());
        // Initialise logging.
                std::cout << "YAML_Torrent_Files_To_XML started ...";
        std::cout << YAML_Lib::YAML::version();
        for (const auto &torrentFileName : Utility::createYAMLFileList()) {
            yaml.parse(yl::FileSource(torrentFileName));
            yaml.stringify(yl::FileDestination(
                Utility::createFileName(torrentFileName, ".xml")));
            std::cout << "Created file "
                      << Utility::createFileName(torrentFileName, ".xml") << " from "
                      << torrentFileName;
        }
    } catch (const std::exception &ex) {
        std::cerr << "Error Processing YAML File: [" << ex.what() << "]\n";
    }
    std::cout << "YAML_Files_To_XML exited.";
    exit(EXIT_SUCCESS);
}
