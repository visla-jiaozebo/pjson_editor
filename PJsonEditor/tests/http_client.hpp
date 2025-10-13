#pragma once
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

// HTTP client using curl command line tool

struct Response {
  int status_code = 0;
  std::string body;

  bool success() const { return status_code >= 200 && status_code < 300; }
};
class HttpClient {
public:
  inline void dump_folder(const std::string &path) { _dump_folder = path; };

private:
  std::string _dump_folder;
  std::string extract_url_path(const std::string &url) {
    // Find the position after the protocol (http:// or https://)
    size_t protocol_pos = url.find("://");
    if (protocol_pos == std::string::npos) {
      return url; // No protocol found, assume it's already a path
    }

    // Find the first '/' after the protocol and domain
    size_t path_start = url.find('/', protocol_pos + 3);
    if (path_start == std::string::npos) {
      return "/"; // No path found, return root
    }

    // Extract everything after the domain
    std::string path = url.substr(path_start);

    // Remove query parameters if present
    size_t query_pos = path.find('?');
    if (query_pos != std::string::npos) {
      path = path.substr(0, query_pos);
    }

    // Remove fragment if present
    size_t fragment_pos = path.find('#');
    if (fragment_pos != std::string::npos) {
      path = path.substr(0, fragment_pos);
    }

    return path;
  }

public:
  Response post(const std::string &url, const std::string &json_body,
                const std::string &method = "POST",
                const std::string &auth_token = "");

  Response get(const std::string &url, const std::string &auth_token = "");
};
