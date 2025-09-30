#pragma once
#include "http_client.hpp"
#include <filesystem>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

// Real server API tester using nlohmann_json
class api_request_client {
private:
  static const std::string BASE_URL;
  CurlHttpClient http_client;
  std::string auth_token;
  std::string project_uuid;

public:
  api_request_client() {
    std::cout << "=== Real Server API Tester Initialized ===" << std::endl;
    std::cout << "Base URL: " << BASE_URL << std::endl;
    // check if /tmp/TOKEN_FILE exists and last modified time is within 1 hour
    if (std::filesystem::exists("/tmp/TOKEN_FILE")) {
      std::cout << "/tmp/TOKEN_FILE exists, checking timestamp..." << std::endl;
      std::filesystem::file_time_type ftime =
          std::filesystem::last_write_time("/tmp/TOKEN_FILE");
      std::filesystem::file_time_type current_time =
          std::filesystem::file_time_type::clock::now();
      std::chrono::duration<double> seconds = current_time - ftime;
      if (seconds.count() < 3600) { // within 1 hour
        std::ifstream ifs("/tmp/TOKEN_FILE");
        std::getline(ifs, auth_token);
        ifs.close();
        std::cout << "Using cached auth token from /tmp/TOKEN_FILE"
                  << std::endl;
      }
    }
  }

  // Getter for auth token (for use in other tests)
  const std::string &getAuthToken() const { return auth_token; }
  const std::string &getProjectUuid() const { return project_uuid; }

  bool login(const std::string &email, const std::string &password);
  bool create_test_project();
  void remove_test_project();

  // Generic HTTP methods for API testing
  nlohmann::json post(const std::string &endpoint,
                      const nlohmann::json &request_body,
                      const std::string &method = "POST");
  nlohmann::json get(const std::string &endpoint);
  inline nlohmann::json
  del(const std::string &endpoint, const nlohmann::json &request_body) {
    return post(endpoint, request_body, "DELETE");
  }
};