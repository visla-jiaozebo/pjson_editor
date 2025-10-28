## client应该如何调用
### 编译 web 库
```bash
./build_wasm.sh
```
该脚本会生成 wasm 库文件在 `./build_wasm/PJsonEditor/pjson_editor_wasm.wasm` 和 `./build_wasm/PJsonEditor/pjson_editor_wasm.js` 。
### lunch web demo
```bash
python ./py_static_proxy.py
```
访问 `http://localhost:8000/demo/index.html` 即可看到 demo 页面。demo 内部示例创建 scene, add scene, rename scene, delete scene 等功能。

### pjson_editor.hpp
#### PJsonEditor::update
PJsonEditor 需要传入 scene 列表作为参数来初始化整个 pjson 树。scene 列表从 `GET /v3/project/{projectUuid}/scene/list` 返回。
##### example:
```c++
nlohmann::json serverResponse = apiClient.get("/v3/project/" + PROPJECT_UUID + "/scene/list");
auto executor = std::make_shared<pjson::PJsonEditor>();
executor->update(serverResponse["data"])
```

```javascript
const editor = new Module.PJsonEditorModule();
let scene_list = await fetch(`${baseURL}/v3/project/${PROPJECT_UUID}/scene/list`, {
    headers: {
        "Content-Type": "application/json",
        "token": token
    }
});
if (!scene_list.ok) {
    return log('❌ Failed to fetch scene list: ' + scene_list.status);
}
editor.update(await scene_list.json());
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
```javascript
  // request: {method, url, body, headers}
  async function testAnyRequest(editor, token, request) {
      let localResp = editor.call(request);
      let r = fetch(request.url, {
          method: request.method,
          headers: request.headers,
          body: JSON.stringify(request.body)
      });

      if (localResp.status_code !== 200) {
          let resp = await r;
          if (resp.ok) {
              let respBody = await resp.json();
              editor.feedServerResponse(localResp, respBody);
              return respBody;
          } else {
              throw new Error("Request failed with status: " + resp.status);
          }
      } else {
          return localResp.body;
      }
  }
```
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
```javascript
  // "scene/rename"
  let renameBody = {
      "sceneUuid": sceneUuid,
      "name": "Renamed Scene" + sceneUuid
  };

  let localResp = editor.call({
      method: "PUT",
      url: `/v3/project/${PROPJECT_UUID}/scene/rename`,
      body: renameBody,
      headers: {
          "Content-Type": "application/json",
          "token": token
      }
  });
  CHECK(localResp.status_code === 200, "Rename scene call failed with:" + localResp.status_code);
  let localBody = localResp.body;
  CHECK(localBody["data"]["scene"]["name"] === "Renamed Scene" + sceneUuid, "name is not same");

  let renameResponse = await fetch(`${baseURL}/v3/project/${PROPJECT_UUID}/scene/rename`, {
      method: "PUT",
      headers: {
          "Content-Type": "application/json",
          "token": token
      },
      body: JSON.stringify(renameBody)
  });
  CHECK(renameResponse.status === 200, "Rename scene call failed with:" + renameResponse.status);

  let serverBody = await renameResponse.json();
  CHECK(localBody["data"]["scene"]["name"] === serverBody["data"]["scene"]["name"], "name is not same");
  log('✅ Test passed: Scene renamed successfully: ' + localBody["data"]["scene"]["name"]);
```
#### void feedServerResponse(resp_ptr, const nlohmann::json &)
该接口用来同步 server 段的数据到本地. 有些接口无法通过 `call` 方法在`local`实现，需要调用`server`端的接口后，将响应同步给`local`处理。比如添加一个scene。
##### example
```javascript
    // add scene
    let addSceneBody = {
        "addPosition": 1,
        "duration": 10000
    };

    let localResp = editor.call({
        method: "POST",
        url: `/v3/project/${PROPJECT_UUID}/scene/add`,
        body: addSceneBody,
        headers: {
            "Content-Type": "application/json",
            "token": token
        }
    });
    // 添加 scene 这个功能，在 local 未实现，status_code返回了 404.
    CHECK(localResp.status_code === 404, "Add scene call failed with:" + JSON.stringify(localResp));
    // 调用 server 端的接口
    let addSceneResponse = await (await fetch(`${baseURL}/v3/project/${PROPJECT_UUID}/scene/add`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            "token": token
        },
        body: JSON.stringify(addSceneBody)
    })).json();
    sceneUuid = addSceneResponse["data"]["sceneUuid"];

    // 同步 server 端响应到本地。这样本地就可以继续用新的 scene 来做操作了。
    editor.feedServerResponse(localResp, addSceneResponse);

```
## TODO