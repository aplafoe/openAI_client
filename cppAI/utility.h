#ifndef CPPAI_UTILITY_H
#define CPPAI_UTILITY_H

#include <boost/nowide/filesystem.hpp>
#include <optional>
#include <string>

namespace cppai {
    namespace utility {
        struct img_req {
            std::optional<boost::filesystem::path> mask;
            std::optional<std::uint16_t> n;
            std::optional<std::string> size;
            std::optional<std::string> user;
        };

        struct audio_req {
            std::optional<std::string> prompt;
            std::optional<std::double_t> temp;
            std::optional<std::string> lang;
        };

        struct img_req_builder {
            img_req req;
            img_req_builder() = default;
            img_req_builder&& set_mask(std::optional<boost::filesystem::path> _mask) &&;
            img_req_builder&& set_n(std::optional<std::uint16_t> _n) &&;
            img_req_builder&& set_size(std::optional<std::string> _size) &&;
            img_req_builder&& set_user(std::optional<std::string> _user) &&;
        };

        struct audio_req_builder {
            audio_req req;
            audio_req_builder() = default;
            audio_req_builder&& set_prompt(std::optional<std::string> _prompt) &&;
            audio_req_builder&& set_temperature(std::optional<std::double_t> _temp) &&;
            audio_req_builder&& set_language(std::optional<std::string> _lang) &&;
        };
    }
}
#endif
