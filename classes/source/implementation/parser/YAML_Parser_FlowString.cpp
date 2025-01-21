
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
/// Check for the end of a plain flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="end">== true  then and of string found.</param>
bool YAML_Parser::endOfPlainFlowString(ISource &source) {
  return isKey(source) || isArray(source) || isComment(source) ||
         isDocumentStart(source) || isDocumentEnd(source);
}

/// <summary>
/// Parse plain flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <returns>String YNode.</returns>
YNode YAML_Parser::parsePlainFlowString(ISource &source,
                                        const Delimiters &delimiters) {
  std::string yamlString{extractToNext(source, delimiters)};
  if (source.current() != kLineFeed) {
    rightTrim(yamlString);
  } else {
    while (source.more() && !endOfPlainFlowString(source)) {
      appendCharacterToString(source, yamlString);
    }
    if (yamlString.back() == kSpace || yamlString.back() == kLineFeed) {
      yamlString.pop_back();
    }
  }
  return YNode::make<String>(yamlString, '\0');
}
/// <summary>
/// Parse quoted flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <returns>String YNode.</returns>
YNode YAML_Parser::parseQuotedFlowString(ISource &source,
                                         const Delimiters &delimiters) {
  const char quote = source.append();
  std::string yamlString;
  if (quote == '"') {
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
  moveToNextIndent(source);
  return YNode::make<String>(yamlString, quote);
}

} // namespace YAML_Lib