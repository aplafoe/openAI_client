#ifndef CPPAI_OPENAI_H
#define CPPAI_OPENAI_H
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>
#include <boost/nowide/fstream.hpp>
#include <iostream>
#include <string_view>
#include "utility.h"

namespace cppai {
    class openAI {
    public:
        openAI();

        void set_api_key(std::string_view api_key);

        void set_organization_id(std::string_view org_id);

        boost::asio::awaitable<boost::json::value> model_list() const;

        boost::asio::awaitable<boost::json::value> completion(const boost::json::value& request_body) const;

        boost::asio::awaitable<boost::json::value> chat_completion(const boost::json::value& request_body) const;

        boost::asio::awaitable<boost::json::value> edit(const boost::json::value& request_body) const;

        boost::asio::awaitable<boost::json::value> create_image(const boost::json::value& request_body) const;

        boost::asio::awaitable<boost::json::value> image_edit(boost::filesystem::path image, std::string_view prompt,
            ::cppai::utility::img_req_builder&& opt_params = {}) const;

        boost::asio::awaitable<boost::json::value> create_img_variation(boost::filesystem::path image, ::cppai::utility::img_req_builder&& opt_params = {}) const;

        boost::asio::awaitable<boost::json::value> create_embedding(const boost::json::value& request_body) const;

        boost::asio::awaitable<boost::json::value> create_transcription(boost::filesystem::path file, std::string_view model,
            ::cppai::utility::audio_req_builder&& opt_params = {}) const;

        boost::asio::awaitable<boost::json::value> create_translation(boost::filesystem::path file, std::string_view model,
            ::cppai::utility::audio_req_builder&& opt_params = {}) const;

        boost::asio::awaitable<boost::json::value> files_list() const;

        boost::asio::awaitable<boost::json::value> delete_file(std::string_view id) const;

        boost::asio::awaitable<boost::json::value> retrieve_file(std::string_view id) const;

        boost::asio::awaitable<boost::json::value> create_fine_tune(const boost::json::value& request_body) const;

        boost::asio::awaitable<boost::json::value> list_fine_tunes() const;

        boost::asio::awaitable<boost::json::value> retrieve_fine_tune(std::string_view id) const;

        boost::asio::awaitable<boost::json::value> cancel_fine_tune(std::string_view id) const;

        boost::asio::awaitable<boost::json::value> fine_tune_events(std::string_view id) const;

        boost::asio::awaitable<boost::json::value> delete_fine_tune(std::string_view id) const;

        boost::asio::awaitable<boost::json::value> create_moderations(const boost::json::value& request_body) const;

    private:
        std::string key;
        std::string organization_id;

        mutable boost::asio::ssl::context ssl_ctx;

        static inline std::string host = "api.openai.com";
        static inline std::string port = "443";

        static constexpr std::uint16_t http_ver = 11;

        using default_executor = boost::asio::use_awaitable_t<>::executor_with_default<boost::asio::any_io_executor>;
        using tcp_stream = typename boost::beast::tcp_stream::rebind_executor<default_executor>::other;

        boost::asio::awaitable<boost::json::value> client(boost::beast::http::request<boost::beast::http::string_body>&& request) const;
    };
}

#endif
