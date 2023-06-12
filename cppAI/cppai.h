#ifndef CPPAI_H
#define CPPAI_H
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>
#include <boost/nowide/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

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
            inline img_req_builder&& set_mask(std::optional<boost::filesystem::path> _mask)&& {
                req.mask = std::move(_mask);
                return std::move(*this);
            }
            inline img_req_builder&& set_n(std::optional<std::uint16_t> _n)&& {
                req.n = _n;
                return std::move(*this);
            }
            inline img_req_builder&& set_size(std::optional<std::string> _size)&& {
                req.size = std::move(_size);
                return std::move(*this);
            }
            inline img_req_builder&& set_user(std::optional<std::string> _user)&& {
                req.user = std::move(_user);
                return std::move(*this);
            }
        };

        struct audio_req_builder {
            audio_req req;
            audio_req_builder() = default;
            inline audio_req_builder&& set_prompt(std::optional<std::string> _prompt)&& {
                req.prompt = std::move(_prompt);
                return std::move(*this);
            }
            inline audio_req_builder&& set_temperature(std::optional<std::double_t> _temp)&& {
                req.temp = std::move(_temp);
                return std::move(*this);
            }
            inline audio_req_builder&& set_language(std::optional<std::string> _lang)&& {
                req.lang = std::move(_lang);
                return std::move(*this);
            }
        };

        inline void pretty_print(const boost::json::value& jvalue, std::uint16_t indent = 0u, std::uint16_t padding = 4u) {
            const std::string indent_str(indent, ' ');
            switch (jvalue.kind()) {
            case boost::json::kind::object: {
                std::cout << "\n{\n";
                const auto& obj = jvalue.as_object();
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    std::cout << indent_str << '"' << it->key() << "\": ";
                    pretty_print(it->value(), indent + padding);
                    if (std::next(it) != obj.end()) {
                        std::cout << ",";
                    }
                    std::cout << "\n";
                }
                std::cout << indent_str << "}";
                break;
            }
            case boost::json::kind::array: {
                std::cout << "[\n";
                const auto& arr = jvalue.as_array();
                for (auto it = arr.begin(); it != arr.end(); ++it) {
                    std::cout << indent_str;
                    pretty_print(*it, indent + padding);
                    if (std::next(it) != arr.end()) {
                        std::cout << ",";
                    }
                    std::cout << "\n";
                }
                std::cout << indent_str << "]";
                break;
            }
            case boost::json::kind::string: {
                std::cout << '"' << jvalue.as_string().c_str() << '"';
                break;
            }
            default: {
                std::cout << jvalue;
                break;
            }
            }
        }

        inline void pretty_print(boost::json::value&& jvalue, std::uint16_t indent = 0u, std::uint16_t padding = 4u) {
            const std::string indent_str(indent, ' ');
            switch (jvalue.kind()) {
            case boost::json::kind::object: {
                std::cout << "\n{\n";
                const auto& obj = jvalue.as_object();
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    std::cout << indent_str << '"' << it->key() << "\": ";
                    pretty_print(it->value(), indent + padding);
                    if (std::next(it) != obj.end()) {
                        std::cout << ",";
                    }
                    std::cout << "\n";
                }
                std::cout << indent_str << "}";
                break;
            }
            case boost::json::kind::array: {
                std::cout << "[\n";
                const auto& arr = jvalue.as_array();
                for (auto it = arr.begin(); it != arr.end(); ++it) {
                    std::cout << indent_str;
                    pretty_print(*it, indent + padding);
                    if (std::next(it) != arr.end()) {
                        std::cout << ",";
                    }
                    std::cout << "\n";
                }
                std::cout << indent_str << "]";
                break;
            }
            case boost::json::kind::string: {
                std::cout << '"' << jvalue.as_string().c_str() << '"';
                break;
            }
            default: {
                std::cout << jvalue;
                break;
            }
            }
        }
    }

    class openAI {
    public:
        openAI() : ssl_ctx{ boost::asio::ssl::context::tlsv12_client } {
            ssl_ctx.set_default_verify_paths();
            ssl_ctx.set_verify_callback(boost::asio::ssl::rfc2818_verification("api.openai.com"));
        }
        inline void set_api_key(std::string_view api_key) {
            key = api_key;
        }
        inline void set_organization_id(std::string_view org_id) {
            organization_id = org_id;
        }

        inline boost::asio::awaitable<boost::json::value> model_list() {
            boost::beast::http::request<boost::beast::http::string_body> model_list_req;
            model_list_req.method(boost::beast::http::verb::get);
            model_list_req.set(boost::beast::http::field::host, host);
            model_list_req.version(http_ver);
            model_list_req.target("/v1/models");
            model_list_req.set(boost::beast::http::field::organization, organization_id);
            model_list_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            model_list_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            model_list_req.prepare_payload();
            boost::json::value model_list_res = co_await client(std::move(model_list_req));
            co_return model_list_res;
        }

        inline boost::asio::awaitable<boost::json::value> completion(const boost::json::value& request_body) {
            boost::beast::http::request<boost::beast::http::string_body> chat_req;
            chat_req.method(boost::beast::http::verb::post);
            chat_req.set(boost::beast::http::field::host, host);
            chat_req.version(http_ver);
            chat_req.target("/v1/completions");
            chat_req.set(boost::beast::http::field::organization, organization_id);
            chat_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            chat_req.set(boost::beast::http::field::content_type, "application/json");
            chat_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            chat_req.body() = boost::json::serialize(request_body);
            chat_req.prepare_payload();
            boost::json::value chat_res = co_await client(std::move(chat_req));
            co_return chat_res;
        }

        inline boost::asio::awaitable<boost::json::value> chat_completion(const boost::json::value& request_body) {
            boost::beast::http::request<boost::beast::http::string_body> chat_req;
            chat_req.method(boost::beast::http::verb::post);
            chat_req.set(boost::beast::http::field::host, host);
            chat_req.version(http_ver);
            chat_req.target("/v1/chat/completions");
            chat_req.set(boost::beast::http::field::organization, organization_id);
            chat_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            chat_req.set(boost::beast::http::field::content_type, "application/json");
            chat_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            chat_req.body() = boost::json::serialize(request_body);
            chat_req.prepare_payload();
            boost::json::value chat_res = co_await client(std::move(chat_req));
            co_return chat_res;
        }

        inline boost::asio::awaitable<boost::json::value> edit(const boost::json::value& request_body) {
            boost::beast::http::request<boost::beast::http::string_body> edit_req;
            edit_req.method(boost::beast::http::verb::post);
            edit_req.set(boost::beast::http::field::host, host);
            edit_req.version(http_ver);
            edit_req.target("/v1/edits");
            edit_req.set(boost::beast::http::field::organization, organization_id);
            edit_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            edit_req.set(boost::beast::http::field::content_type, "application/json");
            edit_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            edit_req.body() = boost::json::serialize(request_body);
            edit_req.prepare_payload();
            boost::json::value edit_res = co_await client(std::move(edit_req));
            co_return edit_res;
        }

        inline boost::asio::awaitable<boost::json::value> create_image(const boost::json::value& request_body) {
            boost::beast::http::request<boost::beast::http::string_body> img_create_req;
            img_create_req.method(boost::beast::http::verb::post);
            img_create_req.set(boost::beast::http::field::host, host);
            img_create_req.version(http_ver);
            img_create_req.target("/v1/images/generations");
            img_create_req.set(boost::beast::http::field::organization, organization_id);
            img_create_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            img_create_req.set(boost::beast::http::field::content_type, "application/json");
            img_create_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            img_create_req.body() = boost::json::serialize(request_body);
            img_create_req.prepare_payload();
            boost::json::value img_create_res = co_await client(std::move(img_create_req));
            co_return img_create_res;
        }

        inline boost::asio::awaitable<boost::json::value> image_edit(boost::filesystem::path image, std::string_view prompt,
            ::cppai::utility::img_req_builder&& opt_params = {}) {
            ::cppai::utility::img_req opt_vals = std::move(opt_params.req);
            boost::nowide::nowide_filesystem();
            boost::nowide::ifstream img_stream{ image, std::ios_base::binary };
            boost::nowide::ifstream mask_stream;
            if (opt_vals.mask.has_value()) {
                mask_stream.open(opt_vals.mask.value(), std::ios_base::binary);
            }

            boost::beast::http::request<boost::beast::http::string_body> img_edit_req;
            img_edit_req.method(boost::beast::http::verb::post);
            img_edit_req.set(boost::beast::http::field::host, host);
            img_edit_req.version(http_ver);
            img_edit_req.target("/v1/images/edits");
            img_edit_req.set(boost::beast::http::field::organization, organization_id);
            img_edit_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            img_edit_req.set(boost::beast::http::field::content_type, "multipart/form-data; boundary=boundary");
            img_edit_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            const std::string boundary = "boundary";
            std::stringstream body_stream;
            body_stream << "--" << boundary << "\r\n"
                << "Content-Disposition: form-data; name=\"image\"; filename=\"" << image.filename().string() << "\"\r\n"
                << "Content-Type: image/*\r\n\r\n"
                << img_stream.rdbuf() << "\r\n"
                << "--" << boundary << "\r\n";
            if (opt_vals.mask.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"mask\"; filename=\"" << opt_vals.mask.value().string() << "\"\r\n"
                    << "Content-Type: image/*\r\n\r\n"
                    << mask_stream.rdbuf() << "\r\n"
                    << "--" << boundary << "\r\n";
            }
            body_stream << "Content-Disposition: form-data; name=\"prompt\"\r\n\r\n"
                << prompt << "\r\n"
                << "--" << boundary << "\r\n";
            if (opt_vals.n.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"n\"\r\n\r\n"
                    << opt_vals.n.value() << "\r\n"
                    << "--" << boundary << "\r\n";
            }
            if (opt_vals.size.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"size\"\r\n\r\n"
                    << opt_vals.size.value() << "\r\n"
                    << "--" << boundary << "--\r\n";
            }
            if (opt_vals.user.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"user\"\r\n\r\n"
                    << opt_vals.user.value() << "\r\n"
                    << "--" << boundary << "--\r\n";
            }

            img_edit_req.body() = body_stream.str();
            img_edit_req.set(boost::beast::http::field::content_length, std::to_string(img_edit_req.body().size()));
            img_edit_req.prepare_payload();

            boost::json::value img_edit_res = co_await client(std::move(img_edit_req));
            co_return img_edit_res;
        }

        inline boost::asio::awaitable<boost::json::value> create_img_variation(boost::filesystem::path image, ::cppai::utility::img_req_builder opt_params = {}) {
            ::cppai::utility::img_req opt_vals = std::move(opt_params.req);
            boost::nowide::nowide_filesystem();
            boost::nowide::ifstream img_stream{ image, std::ios_base::binary };
            boost::beast::http::request<boost::beast::http::string_body> img_var_req;
            img_var_req.method(boost::beast::http::verb::post);
            img_var_req.set(boost::beast::http::field::host, host);
            img_var_req.version(http_ver);
            img_var_req.target("/v1/images/variations");
            img_var_req.set(boost::beast::http::field::organization, organization_id);
            img_var_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            img_var_req.set(boost::beast::http::field::content_type, "multipart/form-data; boundary=boundary");
            img_var_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            const std::string boundary = "boundary";
            std::stringstream body_stream;
            body_stream << "--" << boundary << "\r\n"
                << "Content-Disposition: form-data; name=\"image\"; filename=\"" << image.filename().string() << "\"\r\n"
                << "Content-Type: image/*\r\n"
                << "\r\n"
                << img_stream.rdbuf() << "\r\n"
                << "--" << boundary;
            if (opt_vals.n.has_value()) {
                body_stream << "\r\n"
                    << "Content-Disposition: form-data; name=\"n\"\r\n"
                    << "\r\n"
                    << opt_vals.n.value() << "\r\n"
                    << "--" << boundary;
            }
            if (opt_vals.size.has_value()) {
                body_stream << "\r\n"
                    << "Content-Disposition: form-data; name=\"size\"\r\n"
                    << "\r\n"
                    << opt_vals.size.value() << "\r\n"
                    << "--" << boundary << "--";
            }
            if (opt_vals.user.has_value()) {
                body_stream << "\r\n"
                    << "Content-Disposition: form-data; name=\"user\"\r\n"
                    << "\r\n"
                    << opt_vals.user.value() << "\r\n"
                    << "--" << boundary << "--";
            }


            img_var_req.body() = body_stream.str();
            img_var_req.set(boost::beast::http::field::content_length, std::to_string(img_var_req.body().size()));
            img_var_req.prepare_payload();

            boost::json::value img_var_res = co_await client(std::move(img_var_req));
            co_return img_var_res;
        }

        inline boost::asio::awaitable<boost::json::value> create_embedding(const boost::json::value& request_body) {
            boost::beast::http::request<boost::beast::http::string_body> embedded_req;
            embedded_req.method(boost::beast::http::verb::post);
            embedded_req.set(boost::beast::http::field::host, host);
            embedded_req.version(http_ver);
            embedded_req.target("/v1/embeddings");
            embedded_req.set(boost::beast::http::field::organization, organization_id);
            embedded_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            embedded_req.set(boost::beast::http::field::content_type, "application/json");
            embedded_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            embedded_req.body() = boost::json::serialize(request_body);
            embedded_req.prepare_payload();
            boost::json::value embedded_res = co_await client(std::move(embedded_req));
            co_return embedded_res;
        }

        inline boost::asio::awaitable<boost::json::value> create_transcription(boost::filesystem::path file, std::string_view model,
            ::cppai::utility::audio_req_builder opt_params = {}) {
            ::cppai::utility::audio_req opt_vals = std::move(opt_params.req);
            boost::nowide::nowide_filesystem();
            boost::nowide::ifstream file_stream{ file, std::ios_base::binary };
            boost::beast::http::request<boost::beast::http::string_body> trnscrp_req;
            trnscrp_req.method(boost::beast::http::verb::post);
            trnscrp_req.set(boost::beast::http::field::host, host);
            trnscrp_req.version(http_ver);
            trnscrp_req.target("/v1/audio/transcriptions");
            trnscrp_req.set(boost::beast::http::field::organization, organization_id);
            trnscrp_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            trnscrp_req.set(boost::beast::http::field::content_type, "multipart/form-data; boundary=boundary");
            trnscrp_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            const std::string boundary = "boundary";
            std::stringstream body_stream;
            body_stream << "--" << boundary << "\r\n"
                << "Content-Disposition: form-data; name=\"file\"; filename=\"" << file.filename().string() << "\"\r\n"
                << "Content-Type: application/octet-stream\r\n"
                << "\r\n" << file_stream.rdbuf() << "\r\n"
                << "--" << boundary << "\r\n"
                << "Content-Disposition: form-data; name=\"model\"\r\n"
                << "\r\n" << model << "\r\n"
                << "--" << boundary << "\r\n";
            if (opt_vals.prompt.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"prompt\"\r\n"
                    << "\r\n" << opt_vals.prompt.value() << "\r\n"
                    << "--" << boundary << "\r\n";
            }
            if (opt_vals.temp.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"temperature\"\r\n"
                    << "\r\n" << opt_vals.temp.value() << "\r\n"
                    << "--" << boundary << "\r\n";
            }
            if (opt_vals.lang.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"language\"\r\n"
                    << "\r\n" << opt_vals.lang.value() << "\r\n"
                    << "--" << boundary << "\r\n";
            }


            trnscrp_req.body() = body_stream.str();
            trnscrp_req.set(boost::beast::http::field::content_length, std::to_string(trnscrp_req.body().size()));
            trnscrp_req.prepare_payload();

            boost::json::value trnscrp_res = co_await client(std::move(trnscrp_req));
            co_return trnscrp_res;
        }

        inline boost::asio::awaitable<boost::json::value> create_translation(boost::filesystem::path file, std::string_view model,
            ::cppai::utility::audio_req_builder opt_params = {}) {
            ::cppai::utility::audio_req opt_vals = std::move(opt_params.req);
            boost::nowide::nowide_filesystem();
            boost::nowide::ifstream file_stream{ file, std::ios_base::binary };
            boost::beast::http::request<boost::beast::http::string_body> translate_req;
            translate_req.method(boost::beast::http::verb::post);
            translate_req.set(boost::beast::http::field::host, host);
            translate_req.version(http_ver);
            translate_req.target("/v1/audio/translations");
            translate_req.set(boost::beast::http::field::organization, organization_id);
            translate_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            translate_req.set(boost::beast::http::field::content_type, "multipart/form-data; boundary=boundary");
            translate_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            const std::string boundary = "boundary";
            std::stringstream body_stream;
            body_stream << "--" << boundary << "\r\n"
                << "Content-Disposition: form-data; name=\"file\"; filename=\"" << file.filename().string() << "\"\r\n"
                << "Content-Type: application/octet-stream\r\n"
                << "\r\n" << file_stream.rdbuf() << "\r\n"
                << "--" << boundary << "\r\n"
                << "Content-Disposition: form-data; name=\"model\"\r\n"
                << "\r\n" << model << "\r\n"
                << "--" << boundary << "\r\n";
            if (opt_vals.prompt.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"prompt\"\r\n"
                    << "\r\n" << opt_vals.prompt.value() << "\r\n"
                    << "--" << boundary << "\r\n";
            }
            if (opt_vals.temp.has_value()) {
                body_stream << "Content-Disposition: form-data; name=\"temperature\"\r\n"
                    << "\r\n" << opt_vals.temp.value() << "\r\n"
                    << "--" << boundary << "\r\n";
            }

            translate_req.body() = body_stream.str();
            translate_req.set(boost::beast::http::field::content_length, std::to_string(translate_req.body().size()));
            translate_req.prepare_payload();

            boost::json::value translate_res = co_await client(std::move(translate_req));
            co_return translate_res;
        }

        inline boost::asio::awaitable<boost::json::value> files_list() {
            boost::beast::http::request<boost::beast::http::string_body> files_list_req;
            files_list_req.method(boost::beast::http::verb::get);
            files_list_req.set(boost::beast::http::field::host, host);
            files_list_req.version(http_ver);
            files_list_req.target("/v1/files");
            files_list_req.set(boost::beast::http::field::organization, organization_id);
            files_list_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            files_list_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            files_list_req.prepare_payload();
            boost::json::value files_res = co_await client(std::move(files_list_req));
            co_return files_res;
        }

        inline boost::asio::awaitable<boost::json::value> delete_file(std::string_view id) {
            boost::beast::http::request<boost::beast::http::string_body> file_del_req;
            file_del_req.method(boost::beast::http::verb::delete_);
            file_del_req.set(boost::beast::http::field::host, host);
            file_del_req.version(http_ver);
            file_del_req.target("/v1/files/" + std::string{ id });
            file_del_req.set(boost::beast::http::field::organization, organization_id);
            file_del_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            file_del_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            file_del_req.prepare_payload();
            boost::json::value file_del_res = co_await client(std::move(file_del_req));
            co_return file_del_res;
        }

        inline boost::asio::awaitable<boost::json::value> retrieve_file(std::string_view id) {
            boost::beast::http::request<boost::beast::http::string_body> file_ret_req;
            file_ret_req.method(boost::beast::http::verb::get);
            file_ret_req.set(boost::beast::http::field::host, host);
            file_ret_req.version(http_ver);
            file_ret_req.target("/v1/files/" + std::string{ id } + "/content");
            file_ret_req.set(boost::beast::http::field::organization, organization_id);
            file_ret_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            file_ret_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            file_ret_req.prepare_payload();
            boost::json::value file_ret_res = co_await client(std::move(file_ret_req));
            co_return file_ret_res;
        }

        inline boost::asio::awaitable<boost::json::value> create_fine_tune(const boost::json::value& request_body) {
            boost::beast::http::request<boost::beast::http::string_body> fine_tune_req;
            fine_tune_req.method(boost::beast::http::verb::post);
            fine_tune_req.set(boost::beast::http::field::host, host);
            fine_tune_req.version(http_ver);
            fine_tune_req.target("/v1/fine-tunes");
            fine_tune_req.set(boost::beast::http::field::organization, organization_id);
            fine_tune_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            fine_tune_req.set(boost::beast::http::field::content_type, "application/json");
            fine_tune_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            fine_tune_req.body() = boost::json::serialize(request_body);
            fine_tune_req.prepare_payload();
            boost::json::value fine_tune_res = co_await client(std::move(fine_tune_req));
            co_return fine_tune_res;
        }

        inline boost::asio::awaitable<boost::json::value> list_fine_tunes() {
            boost::beast::http::request<boost::beast::http::string_body> fn_list_req;
            fn_list_req.method(boost::beast::http::verb::get);
            fn_list_req.set(boost::beast::http::field::host, host);
            fn_list_req.version(http_ver);
            fn_list_req.target("/v1/fine-tunes");
            fn_list_req.set(boost::beast::http::field::organization, organization_id);
            fn_list_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            fn_list_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            fn_list_req.prepare_payload();
            boost::json::value fn_list_res = co_await client(std::move(fn_list_req));
            co_return fn_list_res;
        }

        inline boost::asio::awaitable<boost::json::value> retrieve_fine_tune(std::string_view id) {
            boost::beast::http::request<boost::beast::http::string_body> ftune_ret_req;
            ftune_ret_req.method(boost::beast::http::verb::get);
            ftune_ret_req.set(boost::beast::http::field::host, host);
            ftune_ret_req.version(http_ver);
            ftune_ret_req.target("/v1/fine-tunes/" + std::string{ id });
            ftune_ret_req.set(boost::beast::http::field::organization, organization_id);
            ftune_ret_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            ftune_ret_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            ftune_ret_req.prepare_payload();
            boost::json::value ftune_ret_res = co_await client(std::move(ftune_ret_req));
            co_return ftune_ret_res;
        }

        inline boost::asio::awaitable<boost::json::value> cancel_fine_tune(std::string_view id) {
            boost::beast::http::request<boost::beast::http::string_body> ftune_cancel_req;
            ftune_cancel_req.method(boost::beast::http::verb::post);
            ftune_cancel_req.set(boost::beast::http::field::host, host);
            ftune_cancel_req.version(http_ver);
            ftune_cancel_req.target("/v1/fine-tunes/" + std::string{ id } + "/cancel");
            ftune_cancel_req.set(boost::beast::http::field::organization, organization_id);
            ftune_cancel_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            ftune_cancel_req.set(boost::beast::http::field::content_type, "application/json");
            ftune_cancel_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            ftune_cancel_req.prepare_payload();
            boost::json::value ftune_cancel_res = co_await client(std::move(ftune_cancel_req));
            co_return ftune_cancel_res;
        }

        inline boost::asio::awaitable<boost::json::value> fine_tune_events(std::string_view id) {
            boost::beast::http::request<boost::beast::http::string_body> ftune_events_req;
            ftune_events_req.method(boost::beast::http::verb::get);
            ftune_events_req.set(boost::beast::http::field::host, host);
            ftune_events_req.version(http_ver);
            ftune_events_req.target("/v1/fine-tunes/" + std::string{ id } + "/events");
            ftune_events_req.set(boost::beast::http::field::organization, organization_id);
            ftune_events_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            ftune_events_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            ftune_events_req.prepare_payload();
            boost::json::value ftune_events_res = co_await client(std::move(ftune_events_req));
            co_return ftune_events_res;
        }

        inline boost::asio::awaitable<boost::json::value> delete_fine_tune(std::string_view id) {
            boost::beast::http::request<boost::beast::http::string_body> ftune_del_req;
            ftune_del_req.method(boost::beast::http::verb::delete_);
            ftune_del_req.set(boost::beast::http::field::host, host);
            ftune_del_req.version(http_ver);
            ftune_del_req.target("/v1/models/" + std::string{ id });
            ftune_del_req.set(boost::beast::http::field::organization, organization_id);
            ftune_del_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            ftune_del_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            ftune_del_req.prepare_payload();
            boost::json::value ftune_del_res = co_await client(std::move(ftune_del_req));
            co_return ftune_del_res;
        }

        inline boost::asio::awaitable<boost::json::value> create_moderations(const boost::json::value& request_body) {
            boost::beast::http::request<boost::beast::http::string_body> moderations_req;
            moderations_req.method(boost::beast::http::verb::post);
            moderations_req.set(boost::beast::http::field::host, host);
            moderations_req.version(http_ver);
            moderations_req.target("/v1/moderations");
            moderations_req.set(boost::beast::http::field::organization, organization_id);
            moderations_req.set(boost::beast::http::field::authorization, "Bearer " + key);
            moderations_req.set(boost::beast::http::field::content_type, "application/json");
            moderations_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            moderations_req.body() = boost::json::serialize(request_body);
            moderations_req.prepare_payload();
            boost::json::value moderations_res = co_await client(std::move(moderations_req));
            co_return moderations_res;
        }

    private:
        std::string key;
        std::string organization_id;
        boost::asio::ssl::context ssl_ctx;
        static inline std::string host = "api.openai.com";
        static inline std::string port = "443";
        static constexpr std::uint16_t http_ver = 11;
        using default_executor = boost::asio::use_awaitable_t<>::executor_with_default<boost::asio::any_io_executor>;
        using tcp_stream = typename boost::beast::tcp_stream::rebind_executor<default_executor>::other;

        inline boost::asio::awaitable<boost::json::value> client(boost::beast::http::request<boost::beast::http::string_body>&& request) {
            auto resolver = boost::asio::use_awaitable.as_default_on(boost::asio::ip::tcp::resolver(co_await boost::asio::this_coro::executor));
            boost::beast::ssl_stream<tcp_stream> stream{
            boost::asio::use_awaitable.as_default_on(boost::beast::tcp_stream(co_await boost::asio::this_coro::executor)), ssl_ctx };

            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                throw::boost::system::system_error(static_cast<std::int32_t>(ERR_get_error()), boost::asio::ssl::error::get_stream_category());
            }

            boost::asio::ip::tcp::resolver::results_type endpoints;
            endpoints = co_await resolver.async_resolve(host, port);
            co_await boost::beast::get_lowest_layer(stream).async_connect(endpoints);
            co_await stream.async_handshake(boost::asio::ssl::stream_base::client);

            co_await boost::beast::http::async_write(stream, request);
            boost::beast::flat_buffer buffer;
            boost::beast::http::response<boost::beast::http::dynamic_body> response;
            co_await boost::beast::http::async_read(stream, buffer, response);

            boost::beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));

            boost::beast::get_lowest_layer(stream).cancel();

            auto [error_code] = co_await stream.async_shutdown(boost::asio::as_tuple(boost::asio::use_awaitable));

            if (error_code == boost::asio::error::eof) {
                error_code = decltype(error_code){};
            }
            else if (error_code == boost::asio::ssl::error::stream_truncated) {
                error_code = decltype(error_code){};
            }
            else {
                throw boost::system::system_error(error_code, "Shutdown exit");
            }

            co_return boost::json::parse(boost::beast::buffers_to_string(response.body().data()));
        }
    };
}

#endif
