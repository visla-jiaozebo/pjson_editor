#include <stdio.h>
#include <string.h>
#include "../http_client.hpp"
#include <emscripten/fetch.h>

void downloadSucceeded(emscripten_fetch_t *fetch) {
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFailed(emscripten_fetch_t *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}


Response HttpClient::post(const std::string &url, const std::string &json_body,
                          const std::string &method,
                          const std::string &auth_token) {
  Response resp;

  emscripten_fetch_attr_t attr;
  // attr.mode = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, method.c_str());
  attr.requestData = json_body.c_str();
  attr.requestDataSize = json_body.size();
  char *tmp_headers[] = {"Content-Type", "application/json", "token", nullptr};
  attr.requestHeaders = tmp_headers;
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
  emscripten_fetch_t *fetch = emscripten_fetch(&attr, url.c_str());
        // printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  resp.status_code = fetch->status;
  resp.body = std::string(fetch->data, fetch->numBytes);
  emscripten_fetch_close(fetch);

  return resp;
}

Response HttpClient::get(const std::string &url,
                         const std::string &auth_token) {
  Response resp;
emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.requestDataSize = 0;
  char *tmp_headers[] = {"Content-Type", "application/json", "token", nullptr};
  attr.requestHeaders = tmp_headers;
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
  emscripten_fetch_t *fetch = emscripten_fetch(&attr, url.c_str());
  resp.status_code = fetch->status;
  resp.body = std::string(fetch->data, fetch->numBytes);
  emscripten_fetch_close(fetch);

  return resp;
}
