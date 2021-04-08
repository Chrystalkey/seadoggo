//
// Created by Chrystalkey on 28.02.2021.
//

#include <iostream>
#include "../dependencies/cJSON-1.7.14/cJSON.h"
#include "commandocentral.h"
#include "SDHelpers.h"

CommandoCentral::CommandoCentral(int port) : port(port), storage(this) {
    daemon = MHD_start_daemon(MHD_USE_AUTO_INTERNAL_THREAD, port, nullptr, nullptr,
                              &CommandoCentral::answer_to_connection,
                              (void *) this, MHD_OPTION_END);
    if (daemon == nullptr)
        throw std::ios_base::failure("Daemon not started");

    std::mt19937::result_type seed = rd() ^(
            (std::mt19937::result_type)
                    std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::system_clock::now().time_since_epoch()
                    ).count() +
            (std::mt19937::result_type)
                    std::chrono::duration_cast<std::chrono::microseconds>(
                            std::chrono::high_resolution_clock::now().time_since_epoch()
                    ).count());
    generator = std::mt19937(seed);
}

MHD_Result
CommandoCentral::answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method,
                                      const char *version, const char *upload_data, size_t *upload_data_size,
                                      void **con_cls) {
    auto *ths = (CommandoCentral *) cls;
    struct MHD_Response *resp = nullptr;
    MHD_Result ret;
    auto args = parse_header_args(std::string(url));
    HttpResponseCode http_resp_code = MHD_HTTP_OK;

    if (!args.contains("client")) {
        http_resp_code = http_error(&resp);
    }

    if (resp == nullptr) {
        if (strcmp(method, "POST") == 0) {
            ths->handle_post_requests(args, &resp, &http_resp_code, upload_data, upload_data_size);
        } else if (strcmp(method, "GET") == 0) {
            ths->handle_get_requests(args, &resp, &http_resp_code);
        }
    }
    //resp = MHD_create_response_from_buffer(strlen(page),(void*)page,MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, http_resp_code, resp);
    MHD_destroy_response(resp);
    return ret;
}

void
CommandoCentral::handle_get_requests(std::unordered_map<std::string, std::string> &args, struct MHD_Response **resp,
                                     HttpResponseCode *http_resp_code) {
    if (args.at("client") == "0") {
        *http_resp_code = http_error(resp);
    } else {
        uint16_t clRef = (uint16_t) std::strtoul(args.at("client").c_str(), nullptr, 10);
        *http_resp_code = step6_check(clRef, resp);
    }
}

void
CommandoCentral::handle_post_requests(std::unordered_map<std::string, std::string> &args, struct MHD_Response **resp,
                                      HttpResponseCode *http_resp_code, const char *upload_data,
                                      size_t *upload_data_size
) {
    if (args.at("client") == "0") { //if step 1
        if (!args.contains("usertag")) {
            *http_resp_code = http_error(resp);
        } else {
            uint16_t clref = setup_client(args.at("usertag"), std::string(upload_data, *upload_data_size));
            *http_resp_code = step2_OK(clref, resp);
        }
    } else if (args.at("client") != "0" && args.at("state") == "done") {
        uint16_t clRef = (uint16_t) std::strtoul(args.at("client").c_str(), nullptr, 10);
        if (finish_client(clRef)) {
            *http_resp_code = step8_done(clRef, resp);
        } else {
            *http_resp_code = step8_fail(clRef, resp);
        }
    } else if (args.at("client") != "0" && args.at("state") == "fail") {
        uint16_t clRef = (uint16_t) std::strtoul(args.at("client").c_str(), nullptr, 10);
        *http_resp_code = step8_fail(clRef, resp);
    }
}

void
CommandoCentral::run() {
    getchar();
}

void
CommandoCentral::cleanup() {
    MHD_stop_daemon(daemon);
}

HttpResponseCode
CommandoCentral::http_error(MHD_Response **resp) {
    static std::string error_http = "ERROR";
    (*resp) = MHD_create_response_from_buffer(error_http.length(),
                                              (void *) error_http.c_str(), MHD_RESPMEM_PERSISTENT);
    return MHD_HTTP_BAD_REQUEST;
}

HttpResponseCode
CommandoCentral::http_giveup(MHD_Response **resp) {
    static std::string error_http = "GIVEUP";
    (*resp) = MHD_create_response_from_buffer(error_http.length(),
                                              (void *) error_http.c_str(), MHD_RESPMEM_PERSISTENT);
    return MHD_HTTP_GONE;
}

HttpResponseCode
CommandoCentral::step2_OK(ClientHandle client_ref, MHD_Response **resp) {
    if (client_ref == 0) {
        return http_error(resp);
    } else {
        (*resp) = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_PERSISTENT);
        return MHD_HTTP_OK;
    }
}

std::unordered_map<std::string, std::string>
CommandoCentral::parse_header_args(const std::string &url) {
    std::unordered_map<std::string, std::string> arguments;
    std::vector<std::string> split_result = split(url, std::regex("\\?"));
    if (split_result.size() != 2)
        return std::move(arguments);
    split_result = split(split_result[1], std::regex("&"));
    for (auto &element: split_result) {
        std::vector<std::string> linesplit = split(element, std::regex("="));
        if (linesplit.size() != 2)
            continue;
        arguments.insert({linesplit[0], linesplit[1]});
    }
    return std::move(arguments);
}

uint16_t
CommandoCentral::setup_client(const std::string &usertag, const std::string &filelist) {
    ClientHandle handle;
    do {
        handle = random();
    } while (pool.contains(handle)
             || handle == SEADOGGO_CLIENT_UNDEFINED
             || handle == (uint16_t) SEADOGGO_CLIENT_INVALID);

    ClientReference ref;
    ref.ref = handle;
    ref.user = storage.request_user(usertag);
    if (ref.user == nullptr)
        return 0;

    ref.user->setClient(handle);
    storage.new_backup_request(ref.user, filelist);
    pool.insert({handle, ref});
    return handle;
}

HttpResponseCode
CommandoCentral::step6_check(ClientHandle client_ref, MHD_Response **response) {
    if (!pool.contains(client_ref))
        return http_giveup(response);
    ClientReference *ref = &pool.at(client_ref);
    if (ref->user == nullptr)
        return http_giveup(response);
    if (ref->requested_files.empty()) {
        (*response) = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_PERSISTENT);
        return MHD_HTTP_ACCEPTED;
    }
    return missing_file_request(client_ref, response);
}

bool
CommandoCentral::finish_client(ClientHandle client_ref) {
    if (!pool.contains(client_ref))
        return false;
    storage.cancel_operations(pool.at(client_ref).user);
    pool.at(client_ref).user->resetClient();
    return pool.erase(client_ref);
}

HttpResponseCode
CommandoCentral::step8_fail(ClientHandle client_ref, MHD_Response **resp) {
    static std::string fail = "state=fail";
    if (!pool.contains(client_ref)) {
        (*resp) = MHD_create_response_from_buffer(fail.length(), (void *) fail.c_str(), MHD_RESPMEM_PERSISTENT);
        return MHD_HTTP_OK;
    } else {
        storage.cancel_operations(pool.at(client_ref).user);
        pool.erase(client_ref);
        (*resp) = MHD_create_response_from_buffer(fail.length(), (void *) fail.c_str(), MHD_RESPMEM_PERSISTENT);
        return MHD_HTTP_OK;
    }
}

HttpResponseCode
CommandoCentral::step8_done(ClientHandle client_ref, MHD_Response **resp) {
    static std::string done = "state=done";
    if (pool.at(client_ref).requested_files.empty()) {
        (*resp) = MHD_create_response_from_buffer(done.length(), (void *) done.c_str(), MHD_RESPMEM_PERSISTENT);
        return MHD_HTTP_OK;
    } else {
        // if client has sent done before all files are received, skip back to step 6
        return step6_check(client_ref, resp);
    }
}

HttpResponseCode
CommandoCentral::missing_file_request(ClientHandle client_reference, MHD_Response **resp) {
    std::string req_body;
    for (auto &path: pool.at(client_reference).requested_files) {
        req_body += path + "\n";
    }
    (*resp) = MHD_create_response_from_buffer(req_body.length(), (void *) req_body.c_str(), MHD_RESPMEM_MUST_COPY);
    return MHD_HTTP_OK;
}

HttpResponseCode
CommandoCentral::file_upload(ClientHandle client_reference, MHD_Response **resp) {
    return 0;
}

bool
CommandoCentral::set_requestfiles_for_client(ClientHandle handle, std::unordered_set<std::string> &&requ_files) {
    if (!pool.contains(handle))
        return false;
    pool[handle].requested_files = std::move(requ_files);
}