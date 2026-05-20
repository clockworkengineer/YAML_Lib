//
// Program: YAML_Advanced_Types_Demo
//
// Description: Demonstrates advanced YAML_Lib features: timestamps, named
// tag handles (%TAG), verbatim tags, binary (base64), anchors & aliases,
// merge keys, and multi-document streams.
//
// Dependencies: C++20, YAML_Lib.
//

#include "YAML_Utility.hpp"
#include <iostream>

namespace yl = YAML_Lib;

// ---------------------------------------------------------------------------
// Timestamps
// ---------------------------------------------------------------------------
static void demoTimestamps() {
  std::cout << "=== Timestamps ===";
  yl::YAML yaml;
  yl::BufferSource source{"---\n"
                          "date_only: 2024-04-05\n"
                          "datetime_utc: 2024-04-05T14:30:00Z\n"
                          "datetime_tz: 2024-04-05T14:30:00+05:30\n"
                          "events:\n"
                          "  - 2020-01-01\n"
                          "  - 2021-06-15\n"
                          "  - 2022-12-31\n"};
  yaml.parse(source);
  const auto &doc = yaml.document(0);
  std::cout << "  date_only     : "
            << yl::NRef<yl::Timestamp>(doc["date_only"]).value();
  std::cout << "  datetime_utc  : "
            << yl::NRef<yl::Timestamp>(doc["datetime_utc"]).value();
  std::cout << "  datetime_tz   : "
            << yl::NRef<yl::Timestamp>(doc["datetime_tz"]).value();
  const auto &events = yl::NRef<yl::Array>(doc["events"]);
  std::cout << "  events[0]     : "
            << yl::NRef<yl::Timestamp>(events[0]).value();
}

// ---------------------------------------------------------------------------
// Standard !! tags (type coercion)
// ---------------------------------------------------------------------------
static void demoStandardTags() {
  std::cout << "=== Standard !! Tags ===";
  yl::YAML yaml;
  yl::BufferSource source{"---\n"
                          "id:       !!str 007\n"
                          "count:    !!int \"99\"\n"
                          "ratio:    !!float 1.618\n"
                          "enabled:  !!bool yes\n"
                          "nothing:  !!null ~\n"};
  yaml.parse(source);
  const auto &doc = yaml.document(0);
  std::cout << "  id      (str)  : " << yl::NRef<yl::String>(doc["id"]).value()
            << "  tag=" << doc["id"].getTag();
  std::cout << "  count   (int)  : "
            << yl::NRef<yl::Number>(doc["count"]).value<int>()
            << "  tag=" << doc["count"].getTag();
  std::cout << "  enabled (bool) : "
            << (yl::NRef<yl::Boolean>(doc["enabled"]).value() ? "true"
                                                              : "false")
            << "  tag=" << doc["enabled"].getTag();
  std::cout << "  nothing (null) : "
            << (yl::isA<yl::Null>(doc["nothing"]) ? "null" : "not null")
            << "  tag=" << doc["nothing"].getTag();
}

// ---------------------------------------------------------------------------
// Named %TAG handles
// ---------------------------------------------------------------------------
static void demoNamedTagHandles() {
  std::cout << "=== Named %TAG Handles ===";
  yl::YAML yaml;
  // %TAG !e! registers a handle that expands !e!widget ->
  // tag:example.com,:widget
  yl::BufferSource source{"%TAG !e! tag:example.com,2024:\n"
                          "%TAG !m! !my-\n"
                          "---\n"
                          "component: !e!widget button\n"
                          "theme:     !m!color dark\n"};
  yaml.parse(source);
  const auto &doc = yaml.document(0);
  std::cout << "  component tag  : " << doc["component"].getTag();
  std::cout << "  component value: "
            << yl::NRef<yl::String>(doc["component"]).value();
  std::cout << "  theme tag      : " << doc["theme"].getTag();
  std::cout << "  theme value    : "
            << yl::NRef<yl::String>(doc["theme"]).value();
}

// ---------------------------------------------------------------------------
// Verbatim tags  !<uri>
// ---------------------------------------------------------------------------
static void demoVerbatimTags() {
  std::cout << "=== Verbatim Tags ===";
  yl::YAML yaml;
  yl::BufferSource source{"---\n"
                          "item: !<tag:example.com,2024:product> widget\n"};
  yaml.parse(source);
  const auto &doc = yaml.document(0);
  std::cout << "  item tag  : " << doc["item"].getTag();
  std::cout << "  item value: " << yl::NRef<yl::String>(doc["item"]).value();
}

// ---------------------------------------------------------------------------
// Binary (base64) with !!binary tag
// ---------------------------------------------------------------------------
static void demoBinary() {
  std::cout << "=== Binary (!!binary) ===";
  yl::YAML yaml;
  // !!binary stores the base64 value as-is (raw string).
  yl::BufferSource source{"---\n"
                          "thumbnail: !!binary |\n"
                          "  R0lGODlhDAAMAIQAAP//9tX\n"
                          "  2RstVAAAnAAACg8=\n"};
  yaml.parse(source);
  const auto &doc = yaml.document(0);
  std::cout << "  thumbnail tag   : " << doc["thumbnail"].getTag();
  // The value is the raw base64 string
  std::cout << "  thumbnail (raw) : "
            << yl::NRef<yl::String>(doc["thumbnail"]).value().substr(0, 20)
            << "...";
}

// ---------------------------------------------------------------------------
// Anchors, aliases, and merge keys
// ---------------------------------------------------------------------------
static void demoAnchorsAliasesMerge() {
  std::cout << "=== Anchors, Aliases, Merge Keys ===";
  yl::YAML yaml;
  yl::BufferSource source{"---\n"
                          "defaults: &defaults\n"
                          "  adapter: postgresql\n"
                          "  host: localhost\n"
                          "  port: 5432\n"
                          "\n"
                          "development:\n"
                          "  <<: *defaults\n"
                          "  database: myapp_dev\n"
                          "\n"
                          "production:\n"
                          "  <<: *defaults\n"
                          "  host: db.prod.example.com\n"
                          "  database: myapp_prod\n"};
  yaml.parse(source);
  const auto &doc = yaml.document(0);

  const auto devPort =
      yl::NRef<yl::Number>(doc["development"]["port"]).value<int>();
  const auto devDb =
      yl::NRef<yl::String>(doc["development"]["database"]).value();
  const auto prodHost = yl::NRef<yl::String>(doc["production"]["host"]).value();

  std::cout << "  development.adapter : "
            << yl::NRef<yl::String>(doc["development"]["adapter"]).value();
  std::cout << "  development.port    : " << devPort;
  std::cout << "  development.database: " << devDb;
  std::cout << "  production.host     : " << prodHost;
  std::cout << "  production.database : "
            << yl::NRef<yl::String>(doc["production"]["database"]).value();
}

// ---------------------------------------------------------------------------
// Multi-document stream
// ---------------------------------------------------------------------------
static void demoMultiDocument() {
  std::cout << "=== Multi-Document Stream ===";
  yl::YAML yaml;
  yl::BufferSource source{"---\n"
                          "document: 1\n"
                          "title: First\n"
                          "...\n"
                          "---\n"
                          "document: 2\n"
                          "title: Second\n"
                          "...\n"
                          "---\n"
                          "document: 3\n"
                          "title: Third\n"};
  yaml.parse(source);
  std::cout << "  Total documents: "
            << static_cast<unsigned long>(yaml.getNumberOfDocuments());
  for (std::size_t i = 0; i < yaml.getNumberOfDocuments(); ++i) {
    std::cout << "  doc[" << i << "] title = "
              << yl::NRef<yl::String>(yaml.document(i)["title"]).value();
  }
}

// ---------------------------------------------------------------------------
// Explicit ? mapping keys (set-like structures)
// ---------------------------------------------------------------------------
static void demoExplicitKeys() {
  std::cout << "=== Explicit '?' Mapping Keys ===";
  yl::YAML yaml;
  yl::BufferSource source{"--- !!set\n"
                          "? Alice\n"
                          "? Bob\n"
                          "? Charlie\n"};
  yaml.parse(source);
  const auto &doc = yaml.document(0);
  std::cout << "  set size: " << yl::NRef<yl::Dictionary>(doc).size();
  std::cout << "  contains 'Alice': "
            << (yl::NRef<yl::Dictionary>(doc).contains("Alice") ? "yes" : "no");
  std::cout << "  contains 'Dave' : "
            << (yl::NRef<yl::Dictionary>(doc).contains("Dave") ? "yes" : "no");
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
        std::cout << "YAML_Advanced_Types_Demo started ...";
    std::cout << yl::YAML::version();

    demoTimestamps();
    demoStandardTags();
    demoNamedTagHandles();
    demoVerbatimTags();
    demoBinary();
    demoAnchorsAliasesMerge();
    demoMultiDocument();
    demoExplicitKeys();

  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what();
  }
  std::cout << "YAML_Advanced_Types_Demo exited.";
  exit(EXIT_SUCCESS);
}
