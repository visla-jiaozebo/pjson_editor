## client应该如何调用
### pjson_editor.hpp
#### 创建实例
```c++
 PJsonEditor(const nlohmann::json &)
```
PJsonEditor 需要传入 scene 列表作为参数来初始化整个 pjson 树。scene 列表从 `GET /v3/project/{projectUuid}/scene/list` 返回。
##### example:
```c++
nlohmann::json serverResponse = apiClient.get("/v3/project/" + PROPJECT_UUID + "/scene/list");
auto executor = std::make_shared<pjson::PJsonEditor>(serverResponse);
```

#### Response call(Request req)
```c++
struct Request {
  std::string method;
  std::string url;
  nlohmann::json body;
  std::map<std::string, std::string> headers;
};

struct Response {
  int status_code;
  std::map<std::string, std::string> headers;
  nlohmann::json body;
};
```
该接口类似 http 的调用形式。client 在发送一个 http 请求的同时调用此方法。
- 如果返回的 `Response` 成功（status_code == 200），则 body 同 server 端返回的 `response body` 一致。client 用这个 body 就可以了，忽略远端的 response .
- 如果返回的 `Response` 不成功，则不能使用 body，需用远端的 response 。
##### example
```c++
    // "scene/rename"
    nlohmann::json requestBody{{"sceneUuid", sceneUuid}, {"name", "Renamed Scene"}};
    {
        // 先调用 call
      auto r = executor->call(
          {.method = "PUT",
           .url = "/v3/project/" + PROPJECT_UUID + "/scene/rename",
           .body = requestBody});
      if (r.status_code == 200){
        //  如果成功了，则直接使用其 body 作为 response.
        assert(r.body["data"]["scene"]["name"] == name); 
      }else{
        // 失败了。不能使用此 body
      }
    }
    {
        // 同时需要保留原有逻辑
        nlohmann::json serverResponse = apiClient.post(
        "/v3/project/" + PROPJECT_UUID + "/scene/rename", requestBody, "PUT");
        check_scene_name(serverResponse, requestBody["name"]); 
    }
```


