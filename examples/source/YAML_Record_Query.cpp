//
// Program: YAML_Record_Query
//
// Description: Demonstrates querying a YAML sequence of structured records:
// iterating all entries, counting active records, filtering by a field value,
// computing aggregate values (average salary), and finding the maximum.
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;
namespace fs = std::filesystem;

static const std::string kDataFile =
    (fs::current_path() / "files" / "employees.yaml").string();

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
    init(plog::debug, "YAML_Record_Query.log");
    PLOG_INFO << "YAML_Record_Query started ...";
    PLOG_INFO << YAML_Lib::YAML::version();

    yl::YAML yaml;
    yaml.parse(yl::FileSource{kDataFile});

    const auto &doc = yaml.document(0);
    const auto &employees = yl::NRef<yl::Array>(doc["employees"]);
    const auto total = static_cast<long long>(employees.size());

    PLOG_INFO << "Total records: " << total;
    PLOG_INFO << "--- All employees ---";

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

      PLOG_INFO << "  " << name << "  dept=" << dept
                << "  salary=" << salary
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

    PLOG_INFO << "--- Summary ---";
    PLOG_INFO << "Active employees          : " << activeCount << " / " << total;
    PLOG_INFO << "Average salary (all)      : " << totalSalary / total;
    PLOG_INFO << "Engineering headcount     : " << engCount;
    if (engCount > 0) {
      PLOG_INFO << "Average Engineering salary: "
                << engSalaryTotal / engCount;
    }
    PLOG_INFO << "Top earner                : " << topEarner
              << " at " << topSalary;

    PLOG_INFO << "--- Active Engineering team ---";
    for (std::size_t i = 0; i < employees.size(); ++i) {
      const auto &emp = employees[i];
      if (yl::NRef<yl::String>(emp["department"]).value() == "Engineering" &&
          yl::NRef<yl::Boolean>(emp["active"]).value()) {
        PLOG_INFO << "  "
                  << yl::NRef<yl::String>(emp["name"]).value()
                  << "  salary="
                  << yl::NRef<yl::Number>(emp["salary"]).value<long long>();
      }
    }

  } catch (const std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "YAML_Record_Query exited.";
  exit(EXIT_SUCCESS);
}
