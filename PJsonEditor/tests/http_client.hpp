#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <algorithm>

// HTTP client using curl command line tool
class CurlHttpClient {
public:
    struct Response {
        int status_code = 0;
        std::string body;
        
        bool success() const { return status_code >= 200 && status_code < 300; }
    };
    
private:
    std::string exec_command(const char* cmd) {
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
    std::string extract_url_path(const std::string& url) {
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
    Response post(const std::string& url, const std::string& json_body, const std::string& method="POST", const std::string& auth_token = "") {
        Response resp;
        
        // Create temporary file for JSON body
        std::string temp_file = "/tmp/api_test_body.json";
        std::ofstream ofs(temp_file);
        ofs << json_body;
        ofs.close();
        // dump to local file for debugging
        // get the url path after the domain
        auto url_path = extract_url_path(url);
        // replace '/' with '_'
        std::replace(url_path.begin(), url_path.end(), '/', '_');
        // url_path = std::replace(url_path, std::regex("/"), "_");
        {
            std::ofstream debug_ofs(method + url_path + "_request.json");
            debug_ofs << json_body;
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
            } else {
                resp.status_code = 0;
                resp.body = output;
            }
            {
                std::ofstream resp_debug_ofs(method + url_path + "_response.json");
                resp_debug_ofs << resp.body;
            }
        } catch (const std::exception& e) {
            resp.status_code = 0;
            resp.body = "Error: " + std::string(e.what());
        }
        // Clean up temp file
        std::remove(temp_file.c_str());
        
        return resp;
    }
    
    Response get(const std::string& url, const std::string& auth_token = "") {
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
            {
                std::ofstream resp_debug_ofs("GET" + url_path + "_response.json");
                resp_debug_ofs << resp.body;
            }
        } catch (const std::exception& e) {
            resp.status_code = 0;
            resp.body = "Error: " + std::string(e.what());
        }
        
        return resp;
    }
};
