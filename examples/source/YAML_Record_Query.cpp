//
// Program: YAML_Record_Query
//
// Description: Demonstrates querying a YAML sequence of structured records:
// iterating all entries, counting active records, filtering by a field value,
// computing aggregate values (average salary), and finding the maximum.
//
// Dependencies: C++20, YAML_Lib.
//

#include "YAML_Utility.hpp"
#include <iostream>

namespace yl = YAML_Lib;
namespace fs = std::filesystem;

static const std::string kDataFile =
    (fs::current_path() / "files" / "employees.yaml").string();

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
        std::cout << "YAML_Record_Query started ...";
    std::cout << YAML_Lib::YAML::version();

    yl::YAML yaml;
    yaml.parse(yl::FileSource{kDataFile});

    const auto &doc = yaml.document(0);
    const auto &employees = yl::NRef<yl::Array>(doc["employees"]);
    const auto total = static_cast<long long>(employees.size());

    std::cout << "Total records: " << total;
    std::cout << "--- All employees ---";

    long long activeCount = 0;
    long long totalSalary = 0;
    long long engSalaryTotal = 0;
    long long engCount = 0;
    std::string topEarner;
    long long topSalary = 0;

    for (std::size_t i = 0; i < employees.size(); ++i) {
      const auto &emp = employees[i];
      const auto name = yl::NRef<yl::String>(emp["name"]).value();
      const auto dept = yl::NRef<yl::String>(emp["department"]).value();
      const auto salary =
          yl::NRef<yl::Number>(emp["salary"]).value<long long>();
      const auto active = yl::NRef<yl::Boolean>(emp["active"]).value();

      std::cout << "  " << name << "  dept=" << dept << "  salary=" << salary
                << "  active=" << (active ? "yes" : "no");

      if (active) {
        ++activeCount;
      }
      totalSalary += salary;
      if (dept == "Engineering") {
        engSalaryTotal += salary;
        ++engCount;
      }
      if (salary > topSalary) {
        topSalary = salary;
        topEarner = name;
      }
    }

    std::cout << "--- Summary ---";
    std::cout << "Active employees          : " << activeCount << " / "
              << total;
    std::cout << "Average salary (all)      : " << totalSalary / total;
    std::cout << "Engineering headcount     : " << engCount;
    if (engCount > 0) {
      std::cout << "Average Engineering salary: " << engSalaryTotal / engCount;
    }
    std::cout << "Top earner                : " << topEarner << " at "
              << topSalary;

    std::cout << "--- Active Engineering team ---";
    for (std::size_t i = 0; i < employees.size(); ++i) {
      const auto &emp = employees[i];
      if (yl::NRef<yl::String>(emp["department"]).value() == "Engineering" &&
          yl::NRef<yl::Boolean>(emp["active"]).value()) {
        std::cout << "  " << yl::NRef<yl::String>(emp["name"]).value()
                  << "  salary="
                  << yl::NRef<yl::Number>(emp["salary"]).value<long long>();
      }
    }

  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what();
  }
  std::cout << "YAML_Record_Query exited.";
  exit(EXIT_SUCCESS);
}
