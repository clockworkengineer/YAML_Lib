
//
// Class: YAML_Parser_Timestamp
//
// Description: YAML timestamp (!!timestamp) parsing.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Return true if the first 10 characters of s match the ISO 8601 date prefix
/// YYYY-MM-DD (4 digits, '-', 2 digits, '-', 2 digits).
/// </summary>
bool Default_Parser::looksLikeIso8601Date(const std::string &s) {
  if (s.size() < 10) return false;
  const auto dig = [&](int i) {
    return std::isdigit(static_cast<unsigned char>(s[i])) != 0;
  };
  return dig(0) && dig(1) && dig(2) && dig(3) && s[4] == '-' && dig(5) &&
         dig(6) && s[7] == '-' && dig(8) && dig(9);
}

/// <summary>
/// Check if current stream content looks like a YAML timestamp.
/// Matches ISO 8601 dates: YYYY-MM-DD or YYYY-MM-DDThh:mm or YYYY-MM-DD hh:mm
/// Uses a character-level lookahead to avoid saving/restoring state.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>True if stream content is a timestamp.</returns>
bool Default_Parser::isTimestamp(ISource &source) {
  SourceGuard guard(source);
  bool result = false;
  // Read up to 10 chars to check the DDDD-DD-DD pattern
  std::string sample;
  sample.reserve(11);
  int i = 0;
  while (source.more() && i < 10) {
    const char ch = source.current();
    if (ch == kLineFeed)
      break;
    sample += ch;
    source.next();
    i++;
  }

  // Check: 4 digits, '-', 2 digits, '-', 2 digits
  if (looksLikeIso8601Date(sample)) {
    result = true;
  }
  return result;
}

/// <summary>
/// Parse a YAML timestamp on the source stream.
/// Consumes the raw timestamp text (up to delimiter) and returns a Timestamp
/// Node.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse value.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Timestamp Node.</returns>
Node Default_Parser::parseTimestamp(
    ISource &source, const Delimiters &delimiters,
    [[maybe_unused]] unsigned long indentation) {
  SourceGuard guard(source);
  const std::string raw{extractTrimmed(source, delimiters)};
  // Verify the extracted string still looks like a timestamp (may have failed)
  if (looksLikeIso8601Date(raw)) {
    guard.release();
    return Node::make<Timestamp>(raw);
  }
  return {};
}

} // namespace YAML_Lib
