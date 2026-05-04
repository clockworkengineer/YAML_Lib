#pragma once

namespace YAML_Lib {

class BufferSource final : public ISource {
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
  void next() override {
    const auto uc = static_cast<unsigned char>(current());
    if (kForbiddenChar[uc]) {
      char buf[5];
      std::snprintf(buf, sizeof(buf), "%04X", static_cast<unsigned>(uc));
      throw SyntaxError(getPosition(),
                        std::string("Disallowed control character U+") + buf +
                        " in YAML stream.");
    }
    if (current() == kLineFeed) {
      lineNo++;
      column = 1;
    } else {
      column++;
    }
    if (!more()) {
      throw Error("Tried to read past and of buffer.");
    }
    bufferPosition++;
  }
  [[nodiscard]] bool more() const override {
    return bufferPosition < bufferView.size();
  }
  void reset() override {
    bufferPosition = 0;
    lineNo = 1;
    column = 1;
  }
  [[nodiscard]] std::size_t position() override { return bufferPosition; }
  void save() override {
    contexts.push_back(Context(lineNo, column, bufferPosition));
  }
  void restore() override {
    const Context context{contexts.back()};
    contexts.pop_back();
    lineNo = context.lineNo;
    column = context.column;
    bufferPosition = context.bufferPosition;
  }
  void discardSave() override {
    contexts.pop_back();
  }

protected:
  void backup(const unsigned long length) override {
    if (column - length < 1) {
      throw Error("Backup past start column.");
    }
    bufferPosition -= length;
    column -= length;
  }

private:
  std::string      ownedBuffer; // non-empty only when constructed from rvalue std::string
  std::string_view bufferView;  // always valid: points into ownedBuffer or caller's data
};
} // namespace YAML_Lib