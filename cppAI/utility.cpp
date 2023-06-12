#include "utility.h"

cppai::utility::img_req_builder&& cppai::utility::img_req_builder::set_mask(std::optional<boost::filesystem::path> _mask) && {
    req.mask = std::move(_mask);
    return std::move(*this);
}

cppai::utility::img_req_builder&& cppai::utility::img_req_builder::set_n(std::optional<std::uint16_t> _n) && {
    req.n = _n;
    return std::move(*this);
}

cppai::utility::img_req_builder&& cppai::utility::img_req_builder::set_size(std::optional<std::string> _size)&& {
    req.size = std::move(_size);
    return std::move(*this);
}

cppai::utility::img_req_builder&& cppai::utility::img_req_builder::set_user(std::optional<std::string> _user)&& {
    req.user = std::move(_user);
    return std::move(*this);
}

cppai::utility::audio_req_builder&& cppai::utility::audio_req_builder::set_prompt(std::optional<std::string> _prompt)&& {
    req.prompt = std::move(_prompt);
    return std::move(*this);
}

cppai::utility::audio_req_builder&& cppai::utility::audio_req_builder::set_temperature(std::optional<std::double_t> _temp)&& {
    req.temp = std::move(_temp);
    return std::move(*this);
}

cppai::utility::audio_req_builder&& cppai::utility::audio_req_builder::set_language(std::optional<std::string> _lang)&& {
    req.lang = std::move(_lang);
    return std::move(*this);
}