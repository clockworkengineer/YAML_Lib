//
// Program: YAML_Simple_Read_Write
//
// Description: Reads a simple YAML file and writes it back out, demonstrating basic parse and stringify operations.
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    try {
        // Initialise logging.
        init(plog::debug, "YAML_Simple_Read_Write.log");
        PLOG_INFO << "YAML_Simple_Read_Write started ...";
        PLOG_INFO << YAML_Lib::YAML::version();
        yl::YAML yaml;
        std::string inputFile = "files/simple.yaml";
        std::string outputFile = "files/simple_out.yaml";
        yaml.parse(yl::FileSource(inputFile));
        yaml.stringify(yl::FileDestination(outputFile));
        PLOG_INFO << "Parsed and wrote YAML file.";
    } catch (const std::exception &ex) {
        PLOG_ERROR << "Error: [" << ex.what() << "]\n";
    }
    PLOG_INFO << "YAML_Simple_Read_Write exited.";
    exit(EXIT_SUCCESS);
}
