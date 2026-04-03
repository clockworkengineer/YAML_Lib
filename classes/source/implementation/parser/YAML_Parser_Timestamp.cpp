
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
/// Check if current stream content looks like a YAML timestamp.
/// Matches ISO 8601 dates: YYYY-MM-DD or YYYY-MM-DDThh:mm or YYYY-MM-DD hh:mm
/// Uses a character-level lookahead to avoid saving/restoring state.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>True if stream content is a timestamp.</returns>
bool Default_Parser::isTimestamp(ISource &source) {
  source.save();
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
  source.restore();

  // Check: 4 digits, '-', 2 digits, '-', 2 digits
  if (sample.size() >= 10 &&
      std::isdigit(static_cast<unsigned char>(sample[0])) &&
      std::isdigit(static_cast<unsigned char>(sample[1])) &&
      std::isdigit(static_cast<unsigned char>(sample[2])) &&
      std::isdigit(static_cast<unsigned char>(sample[3])) && sample[4] == '-' &&
      std::isdigit(static_cast<unsigned char>(sample[5])) &&
      std::isdigit(static_cast<unsigned char>(sample[6])) && sample[7] == '-' &&
      std::isdigit(static_cast<unsigned char>(sample[8])) &&
      std::isdigit(static_cast<unsigned char>(sample[9]))) {
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
  source.save();
  std::string raw{extractToNext(source, delimiters)};
  rightTrim(raw);
  // Verify the extracted string still looks like a timestamp (may have failed)
  if (raw.size() >= 10 && std::isdigit(static_cast<unsigned char>(raw[0])) &&
      std::isdigit(static_cast<unsigned char>(raw[1])) &&
      std::isdigit(static_cast<unsigned char>(raw[2])) &&
      std::isdigit(static_cast<unsigned char>(raw[3])) && raw[4] == '-' &&
      std::isdigit(static_cast<unsigned char>(raw[5])) &&
      std::isdigit(static_cast<unsigned char>(raw[6])) && raw[7] == '-' &&
      std::isdigit(static_cast<unsigned char>(raw[8])) &&
      std::isdigit(static_cast<unsigned char>(raw[9]))) {
    return Node::make<Timestamp>(raw);
  }
  source.restore();
  return {};
}

} // namespace YAML_Lib
