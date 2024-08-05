#pragma once

#include "IDestination.hpp"

namespace YAML_Lib {

class BufferDestination final : public IDestination
{
public:
  // BufferDestination Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("BufferDestination Error: " + message) {}
  };
  // Constructors/Destructors
  BufferDestination() = default;
  BufferDestination(const BufferDestination &other) = delete;
  BufferDestination &operator=(const BufferDestination &other) = delete;
  BufferDestination(BufferDestination &&other) = delete;
  BufferDestination &operator=(BufferDestination &&other) = delete;
  ~BufferDestination() override = default;

  void add(const std::string &bytes) override
  {
    std::ranges::copy(bytes, std::back_inserter(buffer));
  }
  void add(const Char ch) override { add(toUtf8(ch)); }
  void clear() override { buffer.clear(); }

  [[nodiscard]] std::string toString() const { return buffer; }
  [[nodiscard]] std::size_t size() const { return buffer.size(); }

private:
  std::string buffer;
};
}// namespace YAML_Lib