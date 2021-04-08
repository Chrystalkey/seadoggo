//
// Created by Chrystalkey on 28.02.2021.
//

#ifndef SEADOGGO_COMMANDOCENTRAL_H
#define SEADOGGO_COMMANDOCENTRAL_H

#include <cstdint>
#include <microhttpd.h>
#include <thread>
#include <vector>
#include <unordered_map>
#include <user.h>
#include <random>
#include <unordered_set>
#include <optional>
#include "storagedirectory.h"
#include "SDDefinitions.h"

#define PORT 725

typedef struct {
    ClientHandle ref;
    SeaDoggo::User *user;
    std::unordered_set<std::string> requested_files;
} ClientReference;

/**
 * 1 from here on: client signals himself to be interested in backup by sending a POST and his file list
 *      client: send filelist
 *      server: receive filelist
 * 2 server creates client reference in map, responds to client with reference number and HTTP_OK
 *      client: receive serverreturn
 *      server: prepare client backup by signalling to storagedirectory and send client OK or not OK
 * 3 connection closed
 *      client & server: close connection peacefully
 *
 * 4 server crunches the numbers and creates a file list for the client to upload
 *      client: nothing
 *      server: storagedirectory does preparation, difflisting and directory preparation. signals back to the client
 *              reference when it is done
 *
 * 5 client checks back periodically if filelist is ready [*] with GET and his reference number
 *      client: periodically checks back onto the server. TODO: interval to be specified
 *      server: busy with step 4
 * 6 server sends list of files to client as response
 *      client: receive difflistjson
 *      server: convert difflist to json and send it to the client.(maybe optimize it a little, strip whitespaces and compress or stuff.)
 * 7 client sends files one at a time with reference number [*]
 *      client: send files to the server, one after the other
 *      server: receive files from client. if a wrong file is sent, the server responds with not OK and closes connection
 * 8 client sends "done" with POST, Server responds with "done" or "failed", resets the client ID
 *                "fail" with POST, Server responds with "fail" and resets client ID
 *      client: when he thinks hes done, sends ok, if not not
 *      server: if he aggrees, he confirms successful transaction, if not jump back to step 6 with the missing files.
 * 9 if done:
 *      client: nothing
 *      server: processes received client files to backup structure
 *   if failed:
 *      files deleted, client update "purged"
 *      client: reset
 *      server: reset
 *
 * (10) client can try again.
 *
 */
class CommandoCentral {
public:
    explicit CommandoCentral(int port);

    ~CommandoCentral() { cleanup(); }

    void run();

private:
    void cleanup();

    uint16_t setup_client(const std::string &usertag, const std::string &filelist);

    bool finish_client(ClientHandle client_ref);

    //helpers
    static MHD_Result
    answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method,
                         const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls
    );

    /**
     * collects the ref list from ClientReference and sends it in a response body
     * @param client_reference -> all-seeing handle
     * @param resp the indirect pointer to response, as always
     * @return response pointer
     */
    HttpResponseCode missing_file_request(ClientHandle client_reference, MHD_Response **resp);

    /**
     * checks if the given file was part of
     * @param client_reference
     * @param resp
     * @return
     */
    HttpResponseCode file_upload(ClientHandle client_reference, MHD_Response **resp);

    static std::unordered_map<std::string, std::string> parse_header_args(const std::string &url);

    static HttpResponseCode http_error(MHD_Response **resp);

    static HttpResponseCode http_giveup(MHD_Response **resp);

    static HttpResponseCode step2_OK(ClientHandle client_ref, MHD_Response **resp);

    HttpResponseCode step6_check(ClientHandle client_ref, MHD_Response **response);

    HttpResponseCode step8_fail(ClientHandle client_ref, MHD_Response **resp);

    HttpResponseCode step8_done(ClientHandle client_ref, MHD_Response **resp);

    inline uint16_t random() { return dist(generator); }

    bool set_requestfiles_for_client(ClientHandle handle, std::unordered_set<std::string> &&requ_files);

    void handle_get_requests(std::unordered_map<std::string, std::string> &,
                             struct MHD_Response **resp,
                             HttpResponseCode *http_resp_code
    );

    void handle_post_requests(std::unordered_map<std::string, std::string> &,
                              struct MHD_Response **resp,
                              HttpResponseCode *http_resp_code,
                              const char *upload_data,
                              size_t *upload_data_size
    );

private:
    uint16_t port;
    struct MHD_Daemon *daemon;
    std::unordered_map<ClientHandle, ClientReference> pool;
    std::random_device rd;
    std::mt19937 generator;
    std::uniform_int_distribution<uint16_t> dist;

    StorageDirectory storage;

    friend class StorageDirectory;
};

#endif //SEADOGGO_COMMANDOCENTRAL_H
