//
// Program: YAML_Performance_Profile
//
// Description: Generate a large synthetic YAML document, parse it, stringify it,
// and report elapsed times to help profile parser/stringifier performance.
//
// Dependencies: C++20, YAML_Lib.
//

#include <chrono>
#include <iostream>
#include <string>
#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;
using HighResClock = std::chrono::high_resolution_clock;
using duration_ms = std::chrono::duration<double, std::milli>;

static std::string makeLargeYaml(std::size_t entries) {
  std::string yaml = "---\n";
  yaml.reserve(entries * 16);
  for (std::size_t i = 0; i < entries; ++i) {
    yaml += "item";
    yaml += std::to_string(i);
    yaml += ": ";
    yaml += std::to_string(i * 10);
    yaml += "\n";
  }
  return yaml;
}

int main(int argc, char **argv) {
  const std::size_t entries = (argc > 1) ? std::stoull(argv[1]) : 50000;
  const std::string yamlText = makeLargeYaml(entries);

  std::cout << "Profiling YAML parse/stringify with " << entries << " entries\n";
  std::cout << "Input size: " << yamlText.size() << " bytes\n";

  yl::YAML yaml;
  const auto parseStart = HighResClock::now();
  yaml.parse(yl::BufferSource{yamlText});
  const auto parseStop = HighResClock::now();

  std::cout << "Parse time: "
            << duration_ms(parseStop - parseStart).count()
            << " ms\n";

  yl::BufferDestination dest;
  const auto stringifyStart = HighResClock::now();
  yaml.stringify(dest);
  const auto stringifyStop = HighResClock::now();

  std::cout << "Stringify time: "
            << duration_ms(stringifyStop - stringifyStart).count()
            << " ms\n";
  std::cout << "Output size: " << dest.size() << " bytes\n";

  const auto reparseStart = HighResClock::now();
  yaml.parse(yl::BufferSource{dest.toString()});
  const auto reparseStop = HighResClock::now();

  std::cout << "Reparse time: "
            << duration_ms(reparseStop - reparseStart).count()
            << " ms\n";

  return 0;
}
