//
// Program:  YAML_YAML_Torrent_Files_To_XML
//
// Description: Use YAML_Lib to read in torrent file then write
// it out as XML using a custom encoder.
//
// Dependencies: C++20, PLOG,  YAML_Lib.
//

#include "YAML_Utility.hpp"
#include "XML_Stringify.hpp"

namespace yl = YAML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    try {
        const yl::YAML yaml(yl::makeStringify<yl::XML_Stringify>());
        // Initialise logging.
        init(plog::debug, "YAML_Files_To_XML.log");
        PLOG_INFO << "YAML_Torrent_Files_To_XML started ...";
        PLOG_INFO << yaml.version();
        for (const auto &torrentFileName : Utility::createYAMLFileList()) {
            yaml.parse(yl::FileSource(torrentFileName));
            yaml.stringify(yl::FileDestination(
                Utility::createFileName(torrentFileName, ".xml")));
            PLOG_INFO << "Created file "
                      << Utility::createFileName(torrentFileName, ".xml") << " from "
                      << torrentFileName;
        }
    } catch (const std::exception &ex) {
        PLOG_ERROR << "Error Processing YAML File: [" << ex.what() << "]\n";
    }
    PLOG_INFO << "YAML_Files_To_XML exited.";
    exit(EXIT_SUCCESS);
}
