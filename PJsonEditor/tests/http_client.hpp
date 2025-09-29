#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>

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
    
public:
    Response post(const std::string& url, const std::string& json_body, const std::string& auth_token = "") {
        Response resp;
        
        // Create temporary file for JSON body
        std::string temp_file = "/tmp/api_test_body.json";
        std::ofstream ofs(temp_file);
        ofs << json_body;
        ofs.close();
        
        // Build curl command
        std::string cmd = "curl -s -w \"HTTP_CODE:%{http_code}\" -X POST ";
        cmd += "-H \"Content-Type: application/json\" ";
        if (!auth_token.empty()) {
            cmd += "-H \"token: " + auth_token + "\" ";
        }
        cmd += "-d @" + temp_file + " ";
        cmd += "\"" + url + "\"";
        
        std::cout << "Executing: " << cmd << std::endl;
        
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
            
        } catch (const std::exception& e) {
            resp.status_code = 0;
            resp.body = "Error: " + std::string(e.what());
        }
        
        return resp;
    }
};
