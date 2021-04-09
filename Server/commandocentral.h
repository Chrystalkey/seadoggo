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
 * @attention: client = specific machine connecting right now and staying the same over the whole process
 * @attention: user = tag for itentification over more than one backup session
 *
 * 1 from here on: client signals himself to be interested in backup by sending a POST and his file list
 *      client: send filelist @attention: the filelist has the root directory /, meaning no absolute paths in there.
 *                            @attention: the filelist must end directory entries with a "/" otherwise it is
 *                                        intepreted as file (like with ls -pA1 in every directory)
 *                            @attention: the filelist must be of the format "<path>[/] <mtime>\ n"
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
 *              reference struct when it is done
 * 5 client checks back periodically if filelist is ready [*] with GET and his reference number
 *      client: periodically checks back onto the server. TODO: interval to be specified
 *      server: busy with step 4
 * 6 server sends list of files to client as response
 *      client: receive file list
 *      server: send it to the client.(maybe optimize it a little, strip whitespaces and compress or stuff.)
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
#include "SDHelpers.h"

class CommandoCentral {
public:
    static CommandoCentral init(uint16_t port);

    ~CommandoCentral() { cleanup(); }

    void run();

private:
    explicit CommandoCentral(uint16_t port);

    void cleanup();

    /**
     * step one function to setup a new client.
     * @param usertag
     * @param filelist
     * @return
     */
    uint16_t step1_clientsetup(const std::string &usertag, const std::string &filelist);

    bool finish_client(ClientHandle client_ref);

    //helpers

    /**
     * the microhttp anser callback
     * @param cls a pointer to a user argument, in this case the CommandoCentral instance
     * @param connection the connection reference
     * @param url the url of the request
     * @param method the request Method, either POST or GET in this case
     * @param version the HTTP version string
     * @param upload_data the data to be uploaded
     * @param upload_data_size the size of the former argument
     * @param con_cls some other callback, idk, idu(i don't use)
     * @return the microhttp return
     */
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
    HttpResponseCode
    missing_file_request(ClientHandle client_reference, MHD_Response **resp);

    /**
     * checks if the given file was part of
     * @param client_reference
     * @param resp
     * @return
     */
    HttpResponseCode
    file_upload(ClientHandle client_reference, MHD_Response **resp);

    /**
     * parses the url arguments and assembles a map with key=value association for further processing
     * @param url the url with the arguments
     * @return the specified map.
     */
    static std::unordered_map<std::string, std::string>
    parse_header_args(const std::string &url);

    /**
     * assembles and assigns to resp statically a http error message.
     * @param resp the response parameter this response is assigned to.
     * @return MHD_HTTP_BAD_REQUEST, always.
     */
    static HttpResponseCode
    http_error(MHD_Response **resp);

    /**
     * assembles and assigns the "give up" answer message for the client
     * @param resp the parameter the response is assigned to
     * @return MHD_HTTP_GONE, always
     */
    static HttpResponseCode
    http_giveup(MHD_Response **resp);

    /**
     * checks if the client was validated for further processing and either responses with a call to http_error
     * or an empty HTTP_OK message
     * @param client_ref the client reference of the new client
     * @param resp the parameter the response is assigned to
     * @return HTTP_OK or HTTP_BAD_REQUEST
     */
    static HttpResponseCode
    step2_OK(ClientHandle client_ref, MHD_Response **resp);

    /**
     * checks for step 5 and 6
     * @addindex 1 if the client should give up as no current task is assigned to work it out,
     * @addindex 2 if the client should try again later as the processing has not finished
     * @addindex 3 if the client can start working, and sends the requested files request to the client
     * @param client_ref the client reference number
     * @param response the parameter the response is assigned to
     * @return HTTP_GONE if , HTTP_ACCEPTED, HTTP_OK
     */
    HttpResponseCode
    step6_check(ClientHandle client_ref, MHD_Response **response);

    /**
     * handles failure for step 8. can be triggered in two cases
     * @addindex 1 if the client responds with fail
     * @addindex 2 if the cleanup of the client fails for whatever reason
     * @param client_ref the client reference number
     * @param resp the parameter the response is assigned to
     * @return HTTP_OK, as the fail failed successfully
     */
    HttpResponseCode
    step8_fail(ClientHandle client_ref, MHD_Response **resp);

    /**
     * handles success for step 8. is triggered when a client with a reference in the pool responds "done"
     * in case everything is alright, responds with state=done and step 9 kicks in. in case files are missing,
     * skip to step 6 and resend the required files list.
     * @param client_ref the client reference number
     * @param resp the parameter the response is assigned to
     * @return HTTP_OK or something from step6_check
     */
    HttpResponseCode
    step8_done(ClientHandle client_ref, MHD_Response **resp);

    /**
     * simple random generator for clientref numbers
     * @return an integer ID. 2^16 possible outcomes.
     */
    uint16_t random() { return dist(generator); }

    /**
     * a setter to put a list of files into the user-client struct in the pool
     * @param handle the client handle
     * @param requ_files the set of files
     * @return true, if the client exists, false if not
     */
    bool set_requestfiles_for_client(ClientHandle handle, std::unordered_set<std::string> &&requ_files);

    /**
     * helper to handle "GET" requests
     * @param resp the parameter the response is assigned to
     * @param http_resp_code the response code issued
     */
    void handle_get_requests(std::unordered_map<std::string, std::string> &,
                             struct MHD_Response **resp,
                             HttpResponseCode *http_resp_code
    );

    /**
     * helper to handle "POST" requests
     * @param resp the parameter the response is assigned to
     * @param http_resp_code the response code issued
     * @param upload_data the forwarded upload_data argument from the @refitem answer_to_connection thingy
     * @param upload_data_size the size of the upload_data array
     */
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
    static CommandoCentral *only_central;

    friend class StorageDirectory;
};

#endif //SEADOGGO_COMMANDOCENTRAL_H
