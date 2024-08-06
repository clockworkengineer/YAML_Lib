
#pragma once

#include "IDestination.hpp"

namespace YAML_Lib {

class FileDestination final : public IDestination
{
public:
  explicit FileDestination(const std::string &filename) : filename(filename)
  {
    destination.open(filename.c_str(), std::ios_base::binary | std::ios_base::trunc);
  }
  FileDestination() = delete;
  FileDestination(const FileDestination &other) = delete;
  FileDestination &operator=(const FileDestination &other) = delete;
  FileDestination(FileDestination &&other) = delete;
  FileDestination &operator=(FileDestination &&other) = delete;
  ~FileDestination() override = default;

  void add(const char ch) override
  {
    if (ch == '\n') {
      destination.write("\r\n", 2);
      fileSize += 2;
    } else {
      destination.put(ch);
      fileSize++;
    }
  }
  void add(const std::string &bytes) override
  {
    for (const auto ch : bytes) { add(ch); }
  }
  void clear() override
  {
    if (destination.is_open()) { destination.close(); }
    destination.open(filename.c_str(), std::ios_base::binary | std::ios_base::trunc);
    if (!destination.is_open()) { throw Error("File output stream failed to open or could not be created."); }
    fileSize = 0;
  }
  void close() { destination.flush(); }

  std::size_t size() const { return fileSize; }

private:
  std::ofstream destination;
  std::string filename;
  std::size_t fileSize{};
};
}// namespace YAML_Lib
