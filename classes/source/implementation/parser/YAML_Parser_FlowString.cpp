
//
// Class: YAML_Parser_FlowString
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Append character to YAML string performing any necessary newline folding.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="yamlString">YAML string appended too.</param>
void YAML_Parser::appendCharacterToString(ISource &source,
                                          std::string &yamlString) {
  if (source.current() == kLineFeed) {
    source.next();
    source.ignoreWS();
    if (source.current() == kLineFeed) {
      yamlString += source.append();
      source.ignoreWS();
    } else {
      yamlString += kSpace;
    }
  } else {
    yamlString += source.append();
  }
}
/// <summary>
/// Parse plain flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>String YNode.</returns>
YNode YAML_Parser::parsePlainFlowString(ISource &source,
                                        const Delimiters &delimiters,
                                        const unsigned long indentation) {
  std::string yamlString{extractToNext(source, delimiters) + kSpace};
  if (source.current() != kLineFeed) {
    rightTrim(yamlString);
  } else {
    moveToNextIndent(source);
    while (source.more() && indentation < source.getPosition().second) {
      appendCharacterToString(source, yamlString);
      if (source.match(": ")) {
        throw SyntaxError("Invalid YAML encountered.");
      }
    }
    if (yamlString.back() == kSpace || yamlString.back() == kLineFeed) {
      yamlString.pop_back();
    }
  }
  return YNode::make<String>(yamlString, kNull);
}
/// <summary>
/// Parse quoted flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>String YNode.</returns>
YNode YAML_Parser::parseQuotedFlowString(ISource &source,
                                         const Delimiters &delimiters,
                                         [[maybe_unused]] unsigned long indentation) {
  const char quote = source.append();
  std::string yamlString;
  if (quote == kDoubleQuote) {
    while (source.more() && source.current() != quote) {
      if (source.current() == '\\') {
        yamlString += source.append();
        yamlString += source.append();
      } else {
        appendCharacterToString(source, yamlString);
      }
    }
    yamlString = yamlTranslator->from(yamlString);
  } else {
    while (source.more()) {
      if (source.current() == quote) {
        source.next();
        if (source.current() == quote) {
          yamlString += source.append();
        } else {
          break;
        }
      } else {
        appendCharacterToString(source, yamlString);
      }
    }
  }
  moveToNext(source, delimiters);
  return YNode::make<String>(yamlString, quote);
}

} // namespace YAML_Lib