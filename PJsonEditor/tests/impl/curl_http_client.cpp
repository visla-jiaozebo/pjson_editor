// HTTP client using curl command line tool
#include "../http_client.hpp"
static std::string exec_command(const char *cmd) {
  char buffer[128];
  std::string result = "";
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer, sizeof buffer, pipe.get()) != nullptr) {
    result += buffer;
  }
  return result;
}

Response HttpClient::post(const std::string &url, const std::string &json_body,
                          const std::string &method,
                          const std::string &auth_token) {
  Response resp;

  // Create temporary file for JSON body
  std::string temp_file = "/tmp/api_test_body.json";
  std::ofstream ofs(temp_file);
  ofs << json_body;
  ofs.close();
  // dump to local file for debugging
  // get the url path after the domain
  auto url_path = extract_url_path(url);
  if (!_dump_folder.empty()) {
    std::filesystem::create_directory(_dump_folder);
    // assert(ok && "Failed to create dump folder");
    auto url_path = extract_url_path(url);
    std::replace(url_path.begin(), url_path.end(), '/', '_');
    std::filesystem::path dump_file_path =
        _dump_folder + "/" + method + url_path + "_req.json";
    std::ofstream dump_file(dump_file_path.c_str());
    dump_file << json_body << "\n";
    dump_file.close();
  }
  // Build curl command
  std::string cmd = "curl -s -w \"HTTP_CODE:%{http_code}\" -X " + method + " ";
  cmd += "-H \"Content-Type: application/json\" ";
  if (!auth_token.empty()) {
    cmd += "-H \"token: " + auth_token + "\" ";
  }
  cmd += "-d @" + temp_file + " ";
  cmd += "\"" + url + "\"";
  try {
    std::string output = exec_command(cmd.c_str());

    // Parse response
    size_t code_pos = output.find("HTTP_CODE:");
    if (code_pos != std::string::npos) {
      std::string code_str = output.substr(code_pos + 10);
      resp.status_code = std::stoi(code_str);
      resp.body = output.substr(0, code_pos);
      auto _json = nlohmann::json::parse(resp.body);
      resp.body = _json.dump(2);
    } else {
      resp.status_code = 0;
      resp.body = output;
    }
    if (!_dump_folder.empty()) {
      std::filesystem::create_directory(_dump_folder);
      // assert(ok && "Failed to create dump folder");
      auto url_path = extract_url_path(url);
      std::replace(url_path.begin(), url_path.end(), '/', '_');
      std::filesystem::path dump_file_path =
          _dump_folder + "/" + method + url_path + "_rsp.json";
      std::ofstream dump_file(dump_file_path.c_str());
      dump_file << resp.body << "\n";
    }
  } catch (const std::exception &e) {
    resp.status_code = 0;
    resp.body = "Error: " + std::string(e.what());
  }
  // Clean up temp file
  std::remove(temp_file.c_str());

  return resp;
}

Response HttpClient::get(const std::string &url,
                         const std::string &auth_token) {
  Response resp;

  std::string cmd = "curl -s -w \"HTTP_CODE:%{http_code}\" ";
  if (!auth_token.empty()) {
    cmd += "-H \"token: " + auth_token + "\" ";
  }
  cmd += "\"" + url + "\"";

  // dump to local file for debugging
  // get the url path after the domain
  auto url_path = extract_url_path(url);
  // replace '/' with '_'
  std::replace(url_path.begin(), url_path.end(), '/', '_');
  try {
    std::string output = exec_command(cmd.c_str());

    // Parse response
    size_t code_pos = output.find("HTTP_CODE:");
    if (code_pos != std::string::npos) {
      std::string code_str = output.substr(code_pos + 10);
      resp.status_code = std::stoi(code_str);
      resp.body = output.substr(0, code_pos);
    } else {
      resp.status_code = 0;
      resp.body = output;
    }
    if (!_dump_folder.empty()) {
      std::filesystem::create_directory(_dump_folder);
      // assert(ok && "Failed to create dump folder");
      auto url_path = extract_url_path(url);
      std::replace(url_path.begin(), url_path.end(), '/', '_');
      std::filesystem::path dump_file_path =
          _dump_folder + "/GET" + url_path + "_rsp.json";
      std::ofstream dump_file(dump_file_path.c_str());
      dump_file << resp.body << "\n";
    }
  } catch (const std::exception &e) {
    resp.status_code = 0;
    resp.body = "Error: " + std::string(e.what());
  }

  return resp;
}
