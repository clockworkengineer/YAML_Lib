#ifdef YAML_LIB_SAX_API

#include "YAML_Lib_Tests.hpp"

#include <string>
#include <vector>
#include <utility>

using namespace YAML_Lib;

// ---------------------------------------------------------------------------
// Helpers — concrete IYAMLEvents implementations for test assertions
// ---------------------------------------------------------------------------

/// Records every event in a flat event log so tests can inspect ordering.
struct EventRecorder final : IYAMLEvents {
  enum class Tag {
    DocStart, DocEnd,
    MapStart, MapEnd,
    SeqStart, SeqEnd,
    Key, Scalar
  };
  struct Event {
    Tag tag;
    std::string text;      // key text or scalar value
    NodeType nodeType{NodeType::Any}; // for Scalar events
  };
  std::vector<Event> events;

  void onDocumentStart() override { events.push_back({Tag::DocStart, {}}); }
  void onDocumentEnd()   override { events.push_back({Tag::DocEnd,   {}}); }
  void onMappingStart()  override { events.push_back({Tag::MapStart, {}}); }
  void onMappingEnd()    override { events.push_back({Tag::MapEnd,   {}}); }
  void onSequenceStart() override { events.push_back({Tag::SeqStart, {}}); }
  void onSequenceEnd()   override { events.push_back({Tag::SeqEnd,   {}}); }
  void onKey(std::string_view k) override {
    events.push_back({Tag::Key, std::string{k}});
  }
  void onScalar(NodeType t, std::string_view v) override {
    events.push_back({Tag::Scalar, std::string{v}, t});
  }

  // Convenience: count events of a given tag
  [[nodiscard]] std::size_t count(Tag t) const {
    std::size_t n = 0;
    for (const auto &e : events) { if (e.tag == t) ++n; }
    return n;
  }
  // Convenience: find first event of tag, return text (empty if not found)
  [[nodiscard]] std::string firstText(Tag t) const {
    for (const auto &e : events) { if (e.tag == t) return e.text; }
    return {};
  }
};

/// Key-filter handler — only records scalar values for a target key.
struct KeyCapture final : IYAMLEvents {
  explicit KeyCapture(std::string_view target) : target_(target) {}
  void onKey(std::string_view k) override { capturing_ = (k == target_); }
  void onScalar(NodeType, std::string_view v) override {
    if (capturing_) { captured = std::string{v}; capturing_ = false; }
  }
  // re-set capture flag on structural events to avoid stale state
  void onMappingStart()  override { capturing_ = false; }
  void onSequenceStart() override { capturing_ = false; }

  std::string captured;
private:
  std::string_view target_;
  bool capturing_{false};
};

// ---------------------------------------------------------------------------
// Test cases
// ---------------------------------------------------------------------------

TEST_CASE("SAX event API fires correct events.", "[YAML][SAX]") {

  SECTION("Document start/end bracket every traversal.",
          "[YAML][SAX][DocumentBounds]") {
    const YAML yaml;
    BufferSource src{"---\nkey: value\n"};
    REQUIRE_NOTHROW(yaml.parse(src));

    EventRecorder rec;
    yaml.traverseEvents(rec);

    REQUIRE(rec.count(EventRecorder::Tag::DocStart) == 1);
    REQUIRE(rec.count(EventRecorder::Tag::DocEnd)   == 1);
    REQUIRE(rec.events.front().tag == EventRecorder::Tag::DocStart);
    REQUIRE(rec.events.back().tag  == EventRecorder::Tag::DocEnd);
  }

  SECTION("Flat mapping produces key and scalar events.",
          "[YAML][SAX][Mapping]") {
    const YAML yaml;
    BufferSource src{"---\nhost: example.com\nport: 8080\n"};
    REQUIRE_NOTHROW(yaml.parse(src));

    EventRecorder rec;
    yaml.traverseEvents(rec);

    REQUIRE(rec.count(EventRecorder::Tag::MapStart) == 1);
    REQUIRE(rec.count(EventRecorder::Tag::MapEnd)   == 1);
    REQUIRE(rec.count(EventRecorder::Tag::Key)      == 2);
    REQUIRE(rec.count(EventRecorder::Tag::Scalar)   == 2);
  }

  SECTION("Sequence produces sequence-start/end and scalar events.",
          "[YAML][SAX][Sequence]") {
    const YAML yaml;
    BufferSource src{"---\n- alpha\n- beta\n- gamma\n"};
    REQUIRE_NOTHROW(yaml.parse(src));

    EventRecorder rec;
    yaml.traverseEvents(rec);

    REQUIRE(rec.count(EventRecorder::Tag::SeqStart) == 1);
    REQUIRE(rec.count(EventRecorder::Tag::SeqEnd)   == 1);
    REQUIRE(rec.count(EventRecorder::Tag::Scalar)   == 3);
  }

  SECTION("Scalar types carry correct NodeType.",
          "[YAML][SAX][ScalarTypes]") {
    const YAML yaml;
    BufferSource src{"---\nflag: true\ncount: 42\nlabel: hello\n"};
    REQUIRE_NOTHROW(yaml.parse(src));

    EventRecorder rec;
    yaml.traverseEvents(rec);

    // Collect scalar events
    std::vector<std::pair<NodeType, std::string>> scalars;
    for (const auto &e : rec.events) {
      if (e.tag == EventRecorder::Tag::Scalar) {
        scalars.emplace_back(e.nodeType, e.text);
      }
    }
    REQUIRE(scalars.size() == 3);
    // Order: flag=Boolean, count=Number, label=String
    REQUIRE(scalars[0].first == NodeType::Boolean);
    REQUIRE(scalars[0].second == "true");
    REQUIRE(scalars[1].first == NodeType::Number);
    REQUIRE(scalars[2].first == NodeType::String);
    REQUIRE(scalars[2].second == "hello");
  }

  SECTION("KeyCapture filter retrieves a specific scalar value.",
          "[YAML][SAX][KeyFilter]") {
    const YAML yaml;
    BufferSource src{
        "---\nhost: example.com\nport: 9090\ndebug: false\n"};
    REQUIRE_NOTHROW(yaml.parse(src));

    KeyCapture cap{"host"};
    yaml.traverseEvents(cap);
    REQUIRE(cap.captured == "example.com");

    KeyCapture capPort{"port"};
    yaml.traverseEvents(capPort);
    REQUIRE(capPort.captured == "9090");
  }

  SECTION("Nested mapping produces correct nesting events.",
          "[YAML][SAX][Nested]") {
    const YAML yaml;
    BufferSource src{
        "---\nserver:\n  host: localhost\n  port: 80\n"};
    REQUIRE_NOTHROW(yaml.parse(src));

    EventRecorder rec;
    yaml.traverseEvents(rec);

    // outer map + inner map = 2 MapStart / MapEnd pairs
    REQUIRE(rec.count(EventRecorder::Tag::MapStart) == 2);
    REQUIRE(rec.count(EventRecorder::Tag::MapEnd)   == 2);
    // keys: "server", "host", "port"
    REQUIRE(rec.count(EventRecorder::Tag::Key) == 3);
  }

  SECTION("Null scalar fires onScalar with NodeType::Null.",
          "[YAML][SAX][NullScalar]") {
    const YAML yaml;
    BufferSource src{"---\nmissing: null\n"};
    REQUIRE_NOTHROW(yaml.parse(src));

    EventRecorder rec;
    yaml.traverseEvents(rec);

    bool foundNull = false;
    for (const auto &e : rec.events) {
      if (e.tag == EventRecorder::Tag::Scalar && e.nodeType == NodeType::Null) {
        foundNull = true;
      }
    }
    REQUIRE(foundNull);
  }

  SECTION("Multi-document YAML fires document events per document.",
          "[YAML][SAX][MultiDocument]") {
    const YAML yaml;
    BufferSource src{"---\nfirst: 1\n---\nsecond: 2\n"};
    REQUIRE_NOTHROW(yaml.parse(src));

    EventRecorder rec;
    yaml.traverseEvents(rec);

    REQUIRE(rec.count(EventRecorder::Tag::DocStart) == 2);
    REQUIRE(rec.count(EventRecorder::Tag::DocEnd)   == 2);
  }
}

#endif // YAML_LIB_SAX_API
