#pragma once

#include "ITranslator.hpp"
#include "YAML.hpp"
#include "YAML_Converter.hpp"

namespace YAML_Lib {

    class Bencode_Translator final : public ITranslator
    {
    public:
        // Bencode translator error
        struct Error final : std::runtime_error
        {
            explicit Error(const std::string &message) : std::runtime_error("Bencode Translator Error: " + message) {}
        };

        Bencode_Translator() = default;
        Bencode_Translator(const Bencode_Translator &other) = delete;
        Bencode_Translator &operator=(const Bencode_Translator &other) = delete;
        Bencode_Translator(Bencode_Translator &&other) = delete;
        Bencode_Translator &operator=(Bencode_Translator &&other) = delete;
        ~Bencode_Translator() override = default;

        // Convert to/from Bencode escaped characters
        [[nodiscard]] std::string from([[maybe_unused]] const std::string &escapedString) const override { return escapedString; }
        [[nodiscard]] std::string to(const std::string &rawString) const override { return rawString; }

    };
}// namespace YAML_Lib