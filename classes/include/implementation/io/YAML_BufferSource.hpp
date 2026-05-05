#pragma once

namespace YAML_Lib {

class BufferSource final : public BufferedSourceBase {
public:
  // Zero-copy: string_view, lvalue std::string, string literals, and
  // brace-init-list arguments all resolve here. Caller guarantees lifetime.
  explicit BufferSource(std::string_view view) : bufferView(view) {}
  // Owning: rvalue std::string only (template deduction fails for braces and
  // string literals; is_same<T,std::string> excludes lvalues). Moves the
  // string in so the view remains valid after the caller's temporary is gone.
  template <typename T,
            std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
  explicit BufferSource(T &&owned)
      : ownedBuffer(std::move(owned)), bufferView(ownedBuffer) {}
  BufferSource() = delete;
  BufferSource(const BufferSource &other) = delete;
  BufferSource &operator=(const BufferSource &other) = delete;
  BufferSource(BufferSource &&other) = delete;
  BufferSource &operator=(BufferSource &&other) = delete;
  ~BufferSource() override = default;

  [[nodiscard]] char current() const override {
    if (more()) {
      return bufferView[bufferPosition];
    }
    return EOF;
  }
  [[nodiscard]] bool more() const override {
    return bufferPosition < bufferView.size();
  }

protected:
  [[nodiscard]] const char *endOfInputMessage() const noexcept override {
    return "Tried to read past and of buffer.";
  }

private:
  std::string      ownedBuffer; // non-empty only when constructed from rvalue std::string
  std::string_view bufferView;  // always valid: points into ownedBuffer or caller's data
};
} // namespace YAML_Lib