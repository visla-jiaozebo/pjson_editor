#include "../api_request_client.hpp"
#include <cassert>

using json = nlohmann::json;

bool api_request_client::login(const std::string &email,
                               const std::string &password) {
  std::cout << "\n=== Attempting Real Login ===" << std::endl;
  if (!auth_token.empty()) {
    std::cout << "Already logged in with token: " << auth_token << std::endl;
    return true;
  }

  std::string login_url = BASE_URL + "/v3/auth/login";

  // Create JSON request body using nlohmann_json
  json login_request;
  login_request["email"] = email;
  login_request["password"] = password;

  std::string login_body = login_request.dump();

  std::cout << "Login URL: " << login_url << std::endl;
  std::cout << "Login Body: " << login_body << std::endl;

  CurlHttpClient::Response response = http_client.post(login_url, login_body);

  std::cout << "Response Status: " << response.status_code << std::endl;
  std::cout << "Response Body: " << response.body << std::endl;

  assert(response.success() && "Login request failed");
  json response_json = json::parse(response.body);
  assert(response_json["code"] == 0 && "Login failed");
  assert(response_json.contains("data") && "No data in response");
  auth_token = response_json["data"]["visToken"];

  // Cache token to /tmp/TOKEN_FILE
  std::ofstream ofs("/tmp/TOKEN_FILE");
  ofs << auth_token;
  ofs.close();

  return true;
}

bool api_request_client::create_test_project() {
  if (auth_token.empty()) {
    std::cout << "No auth token - cannot create project" << std::endl;
    return false;
  }

  std::cout << "\n=== Creating Real Test Project ===" << std::endl;

  std::string project_url = BASE_URL + "/v3/project";

  // Create JSON request body using nlohmann_json
  json project_request;
  project_request["name"] = "API Comparison Test Project";
  project_request["description"] =
      "Project created for testing API parity between client and server";
  project_request["templateType"] = "BLANK";

  std::string project_body = project_request.dump();

  CurlHttpClient::Response response =
      http_client.post(project_url, project_body, auth_token);

  std::cout << "Create Project Response Status: " << response.status_code
            << std::endl;
  std::cout << "Create Project Response: " << response.body << std::endl;

  assert(response.success() && "Create project request failed");
  json response_json = json::parse(response.body);
  assert(response_json["code"] == 0 && "Create project failed");
  assert(response_json.contains("data") &&
         "No data in create project response");
  project_uuid = response_json["data"]["uuid"];
  return true;
}

void api_request_client::remove_test_project() {
  if (auth_token.empty() || project_uuid.empty()) {
    std::cout << "No auth token or project UUID - cannot delete project"
              << std::endl;
    return;
  }

  std::cout << "\n=== Deleting Real Test Project ===" << std::endl;

  std::string delete_url = BASE_URL + "/v3/project/" + project_uuid;

  CurlHttpClient::Response response =
      http_client.post(delete_url, "", auth_token);

  std::cout << "Delete Project Response Status: " << response.status_code
            << std::endl;
  std::cout << "Delete Project Response: " << response.body << std::endl;

  assert(response.success() && "Delete project request failed");
  json response_json = json::parse(response.body);
  assert(response_json["code"] == 0 && "Delete project failed");
}

const std::string api_request_client::BASE_URL =
    "https://api-snapshot.dev01.vislaus.cn";

// Generic POST method for API testing
nlohmann::json
api_request_client::post(const std::string &endpoint,
                         const nlohmann::json &request_body,
                         const std::string &method) {
  std::string url = BASE_URL + endpoint;
  std::string json_body = request_body.dump();

  std::cout << "Executing: " << method << " " << url << std::endl;
  CurlHttpClient::Response response =
      http_client.post(url, json_body, method, auth_token);
  json response_json;
  try {
    response_json = json::parse(response.body);
    if (response_json.contains("code") && response_json["code"] != 0) {
      std::cerr << "API Error: " << response_json["code"]  << " Message: "<< response_json["message"] << std::endl << "Body: " << request_body << std::endl;
      
    }
  } catch (const json::exception &e) {
    assert(false && "JSON parse error in response");
  }

  return response_json;
}

// Generic GET method for API testing
nlohmann::json
api_request_client::get(const std::string &endpoint) {
  std::string url = BASE_URL + endpoint;
  CurlHttpClient::Response response = http_client.get(url, auth_token);
  json response_json;
  try {
    response_json = json::parse(response.body);
  } catch (const json::exception &e) {
    assert(false && "JSON parse error in response");
  }
  return response_json;
}