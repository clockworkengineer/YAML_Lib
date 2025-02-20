#include "YAML_Lib_Tests.hpp"
#include "XML_Stringify.hpp"

TEST_CASE("Check YAML stringification to XML of simple types.", "[YAML][Stringify][Simple][XML]")
{
  const YAML yaml(makeStringify<XML_Stringify>());
  SECTION("Stringify a string (abcdefghijklmnopqrstuvwxyz) to XML.", "[YAML][Stringify][String][XML]")
  {
    BufferDestination yamlDestination;
    yaml.parse(BufferSource{ R"("abcdefghijklmnopqrstuvwxyz")" });
    yaml.stringify(yamlDestination);
    REQUIRE(
      yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz</root>)");
  }
  // SECTION("Stringify a string (abcdefghijklmnopqrstuvwxyz &<>'\") to XML with default character escapes.",
  //   "[YAML][Stringify][String][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ R"("abcdefghijklmnopqrstuvwxyz &<>'\"")" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(
  //     yamlDestination.toString()
  //     == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz &amp;&lt;&gt;&apos;&quot;</root>)");
  // }
  // SECTION(
  //   "XML encode an string with unprintable characters "
  //   "('abcdefghijklmnopqrstuvwxyz') ",
  //   "[YAML][Stringify][XML][String]")
  // {
  //   std::string escaped;
  //   escaped += "\"abcdefghijklmnopqrstuvwxyz";
  //   escaped += 1;
  //   escaped += 2;
  //   escaped += '"';
  //   BufferSource source{ escaped };
  //   BufferDestination destination;
  //   yaml.parse(source);
  //   yaml.stringify(destination);
  //   REQUIRE(destination.toString()
  //           == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz&#x0001;&#x0002;</root>)");
  // }
  // SECTION("XML encode an string with unprintable characters (1-127) ", "[YAML][Stringify][XML][String]")
  // {
  //   std::string escaped{ "\"abcdefghijklmnopqrstuvwxyz" };
  //   // Add all ASCII except '"' and '\'
  //   for (int ch = 1; ch < 128; ch++) {
  //     if (static_cast<char>(ch) != '"' && static_cast<char>(ch) != '\\') { escaped += static_cast<char>(ch); }
  //   }
  //   escaped += '"';
  //   BufferSource source{ escaped };
  //   BufferDestination destination;
  //   yaml.parse(source);
  //   yaml.stringify(destination);
  //   REQUIRE(
  //     destination.toString()
  //     == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz&#x0001;&#x0002;&#x0003;&#x0004;&#x0005;&#x0006;&#x0007;&#x0008;&#x0009;&#x000A;&#x000B;&#x000C;&#x000D;&#x000E;&#x000F;&#x0010;&#x0011;&#x0012;&#x0013;&#x0014;&#x0015;&#x0016;&#x0017;&#x0018;&#x0019;&#x001A;&#x001B;&#x001C;&#x001D;&#x001E;&#x001F; !#$%&amp;&apos;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~&#x007F;</root>)");
  // }
  // // Disable tests for present
  // // SECTION("XML encode an string with unprintable characters (128-255) ", "[YAML][Stringify][XML][String]")
  // // {
  // //   std::string escaped{ "\"abcdefghijklmnopqrstuvwxyz" };
  // //   // Add all ASCII except '"' and '\'
  // //   for (int ch = 128; ch < 256; ch++) {
  // //     if ((static_cast<char>(ch) != '"') && (static_cast<char>(ch) != '\\')) { escaped += static_cast<char>(ch); }
  // //   }
  // //   escaped += '"';
  // //   BufferSource source{ escaped };
  // //   BufferDestination destination;
  // //   yaml.parse(source);
  // //   yaml.stringify(destination);
  // //   REQUIRE(
  // //     destination.toString()
  // //     == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;&#xFFFD;</root>)");
  // //   // 0xFFFD means no such character in UTF8 (generated for 128-255; but can use syntax \u0080 etc).
  // // }
  // SECTION("Stringify a boolean (true) to XML.", "[YAML][Stringify][Boolean][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ "true" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root>True</root>)");
  // }
  // SECTION("Stringify a boolean (false) to XML.", "[YAML][Stringify][Boolean][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ "false" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root>False</root>)");
  // }
  // SECTION("Stringify a integer (98345) to XML.", "[YAML][Stringify][Integer][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ "98345" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root>98345</root>)");
  // }
  // SECTION("Stringify a integer (-98345) to XML.", "[YAML][Stringify][Integer][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ "-98345" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root>-98345</root>)");
  // }
  // SECTION("Stringify a floating point (55.6667) to XML.", "[YAML][Stringify][Float][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ "55.6667" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root>55</root>)");
  // }
  // SECTION("Stringify a floating point (-55.6667) to XML.", "[YAML][Stringify][Float][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ "-55.6667" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root>-55</root>)");
  // }
  // SECTION("Stringify a null to XML.", "[YAML][Stringify][Simple][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ { "null" } });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root></root>)");
  // }
  // SECTION("Stringify an array to XML.", "[YAML][Stringify][Array][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ { "[1,444,555,666,67.99]" } });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(
  //     yamlDestination.toString()
  //     == R"(<?xml version="1.0" encoding="UTF-8"?><root><Row>1</Row><Row>444</Row><Row>555</Row><Row>666</Row><Row>67</Row></root>)");
  // }
  // SECTION("Stringify an empty array to XML.", "[YAML][Stringify][Array][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ { "[]" } });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root></root>)");
  // }
  // SECTION("Stringify an object to XML.", "[YAML][Stringify][Object][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ { R"({"Age":77,"Name":"Rob"})" } });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString()
  //           == R"(<?xml version="1.0" encoding="UTF-8"?><root><Age>77</Age><Name>Rob</Name></root>)");
  // }
  // SECTION("Stringify an empty object to XML.", "[YAML][Stringify][Object][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ { R"({})" } });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString() == R"(<?xml version="1.0" encoding="UTF-8"?><root></root>)");
  // }
  // SECTION(R"(Stringify an nested array ({"City":"London","Population":[1,2,3,4,5]}) to XML.)",
  //   "[YAML][Stringify][Array][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ R"({"City":"London","Population":[1,2,3,4,5]})" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(
  //     yamlDestination.toString()
  //     == R"(<?xml version="1.0" encoding="UTF-8"?><root><City>London</City><Population><Row>1</Row><Row>2</Row><Row>3</Row><Row>4</Row><Row>5</Row></Population></root>)");
  // }
  // SECTION(
  //   R"(Stringify a nested object ([true,"Out of time",7.89043e+18,{"key":4444}]) to a buffer and check its
  //     value.)",
  //   "[YAML][Stringify][Object][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ R"([true,"Out of time",7.89043e+18,{"key":4444}])" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(
  //     yamlDestination.toString()
  //     == R"(<?xml version="1.0" encoding="UTF-8"?><root><Row>True</Row><Row>Out of time</Row><Row>7890430242211233792</Row><Row><key>4444</key></Row></root>)");
  // }
  // SECTION(R"(Stringify XML string with escapes '\u007F (non-printable ASCII)' to buffer and check value.)",
  //   "[YAML][Stringify][XML][Escapes]")
  // {
  //   const std::string source{ R"("abcdefghijklmnopqrstuvwxyz \u007F")" };
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ source });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString()
  //           == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz &#x007F;</root>)");
  // }
  // SECTION(R"(Stringify XML string with escapes (1-127)' to buffer and check value.)", "[YAML][Stringify][XML][Escapes]")
  // {
  //   std::string source{ R"("abcdefghijklmnopqrstuvwxyz )" };
  //   source += generateEscapes(1, 127);
  //   source += "\"";
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ source });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(
  //     yamlDestination.toString()
  //     == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz &#x0001;&#x0002;&#x0003;&#x0004;&#x0005;&#x0006;&#x0007;&#x0008;&#x0009;&#x000A;&#x000B;&#x000C;&#x000D;&#x000E;&#x000F;&#x0010;&#x0011;&#x0012;&#x0013;&#x0014;&#x0015;&#x0016;&#x0017;&#x0018;&#x0019;&#x001A;&#x001B;&#x001C;&#x001D;&#x001E;&#x001F; !&quot;#$%&amp;&apos;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~&#x007F;</root>)");
  // }
  // SECTION(R"(Stringify XML string with escapes '\u0080 (non-printable ASCII)' to buffer and check value.)",
  //   "[YAML][Stringify][XML][Escapes]")
  // {
  //   const std::string source{ R"("abcdefghijklmnopqrstuvwxyz \u0080")" };
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ source });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString()
  //           == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz &#x0080;</root>)");
  // }
  // SECTION(
  //   R"(Stringify XML string with escapes (128-255)' to buffer and check value.)", "[YAML][Stringify][XML][Escapes]")
  // {
  //   std::string source{ R"("abcdefghijklmnopqrstuvwxyz )" };
  //   source += generateEscapes(128, 255);
  //   source += "\"";
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ source });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(
  //     yamlDestination.toString()
  //     == R"(<?xml version="1.0" encoding="UTF-8"?><root>abcdefghijklmnopqrstuvwxyz &#x0080;&#x0081;&#x0082;&#x0083;&#x0084;&#x0085;&#x0086;&#x0087;&#x0088;&#x0089;&#x008A;&#x008B;&#x008C;&#x008D;&#x008E;&#x008F;&#x0090;&#x0091;&#x0092;&#x0093;&#x0094;&#x0095;&#x0096;&#x0097;&#x0098;&#x0099;&#x009A;&#x009B;&#x009C;&#x009D;&#x009E;&#x009F;&#x00A0;&#x00A1;&#x00A2;&#x00A3;&#x00A4;&#x00A5;&#x00A6;&#x00A7;&#x00A8;&#x00A9;&#x00AA;&#x00AB;&#x00AC;&#x00AD;&#x00AE;&#x00AF;&#x00B0;&#x00B1;&#x00B2;&#x00B3;&#x00B4;&#x00B5;&#x00B6;&#x00B7;&#x00B8;&#x00B9;&#x00BA;&#x00BB;&#x00BC;&#x00BD;&#x00BE;&#x00BF;&#x00C0;&#x00C1;&#x00C2;&#x00C3;&#x00C4;&#x00C5;&#x00C6;&#x00C7;&#x00C8;&#x00C9;&#x00CA;&#x00CB;&#x00CC;&#x00CD;&#x00CE;&#x00CF;&#x00D0;&#x00D1;&#x00D2;&#x00D3;&#x00D4;&#x00D5;&#x00D6;&#x00D7;&#x00D8;&#x00D9;&#x00DA;&#x00DB;&#x00DC;&#x00DD;&#x00DE;&#x00DF;&#x00E0;&#x00E1;&#x00E2;&#x00E3;&#x00E4;&#x00E5;&#x00E6;&#x00E7;&#x00E8;&#x00E9;&#x00EA;&#x00EB;&#x00EC;&#x00ED;&#x00EE;&#x00EF;&#x00F0;&#x00F1;&#x00F2;&#x00F3;&#x00F4;&#x00F5;&#x00F6;&#x00F7;&#x00F8;&#x00F9;&#x00FA;&#x00FB;&#x00FC;&#x00FD;&#x00FE;&#x00FF;</root>)");
  // }
  // SECTION(R"(Stringify [{},{},{}] to a XML.)", "[YAML][Stringify][Array][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ R"([{},{},{}])" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString()
  //           == R"(<?xml version="1.0" encoding="UTF-8"?><root><Row></Row><Row></Row><Row></Row></root>)");
  // }
  // SECTION(R"(Stringify [[], [],[]] to a XML.)", "[YAML][Stringify][Array][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ R"([[], [],[]])" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString()
  //           == R"(<?xml version="1.0" encoding="UTF-8"?><root><Row></Row><Row></Row><Row></Row></root>)");
  // }
  // SECTION(R"(Stringify {"Test" : [[],[],[]]} to a XML.)", "[YAML][Stringify][Object][XML]")
  // {
  //   BufferDestination yamlDestination;
  //   yaml.parse(BufferSource{ R"({"Test" : [[],[],[]]})" });
  //   yaml.stringify(yamlDestination);
  //   REQUIRE(yamlDestination.toString()
  //           == R"(<?xml version="1.0" encoding="UTF-8"?><root><Test><Row></Row><Row></Row><Row></Row></Test></root>)");
  // }
}