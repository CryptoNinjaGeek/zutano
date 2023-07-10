//
// Created by Carsten Tang on 06/07/2023.
//

#include "docker/Controller.h"
#include <jsoncons/json.hpp>
#include <zutano/Tools.h>
#include <cpr/cpr.h>
#include "Tools.h"

using namespace zutano::tools;

namespace docker {

auto Controller::create_network(CreateNetworkInput input) -> std::string {
  jsoncons::json data = to_json{
      {"Name", input.name},
      {"CheckDuplicate", input.check_duplicate.value_or(false)},
  };

  auto response = send_request(RequestType::POST, "/networks/create", data);

  return response.get_value_or<std::string>("Id", "");
}

auto Controller::create_volume(CreateVolumeInput input) -> std::string {
  jsoncons::json data = to_json{{"name", input.name}};

  auto response = send_request(RequestType::POST, "/volumes/create", data);

  return response.get_value_or<std::string>("Mountpoint", "");
}

auto Controller::create_port_mapping(PortMappingInput) -> bool { return false; }

auto Controller::create_container(CreateContainerInput input) -> std::string {
  jsoncons::json data = to_json{{"Image", input.image}, {"HostName", input.name}};
  std::vector<std::pair<std::string, std::string>> param;

  param.push_back(std::pair<std::string, std::string>("name", input.name));

  if (input.enviroment) {
    data.merge(to_json{{"Env", input.enviroment.value()}});
  }

  if (input.command) {
    data.merge(to_json{{"Cmd", input.command.value()}});
  }

  if (input.labels) {
    data.merge(to_json{{"Labels", input.labels.value()}});
  }

  auto txt = arango_bench::tools::string_format(
      "{\"HostConfig\": { \
 \"PortBindings\" : {\"8529/tcp\" : [ {\"HostPort\" : \"%ld\"}]}, \
 \"Binds\": [\"%s\"]}}",
      input.port, input.volume.c_str());

  try {
    data.merge(jsoncons::json::parse(txt));
  } catch (const std::exception& e) {
    std::cout << e.what() << '\n';
  }

  auto response = send_request(RequestType::POST, "/containers/create", data, param);

  return response.get_value_or<std::string>("Id", "");
}

auto Controller::connect_container_to_network(ConnectNetworkInput input) -> bool {
  jsoncons::json data = to_json{{"Container", input.container}};

  auto response = send_request(RequestType::POST, arango_bench::tools::string_format("/networks/%s/connect", input.network.c_str()), data);
  auto error_message = response.get_value_or<std::string>("message", "");

  return error_message.empty();
}

auto Controller::start_container(std::string container_id) -> bool {
  auto response = send_request(RequestType::POST, arango_bench::tools::string_format("/containers/%s/start", container_id.c_str()));
  auto error_message = response.get_value_or<std::string>("message", "");

  return error_message.empty();
}

auto Controller::set_version(std::string version) { version_ = version; }

auto Controller::send_request(RequestType request_type, std::string path, jsoncons::json data,
                              std::vector<std::pair<std::string, std::string>> parameters) -> jsoncons::json {
  cpr::Session session;
  cpr::Header headers;
  cpr::Parameters create_parameters;

  std::string url_string = "http://localhost/" + version_ + path;
  auto url = cpr::Url{url_string};

  headers["Content-Type"] = "application/json";
  headers["charset"] = "utf-8";

  for (auto param : parameters) {
    create_parameters.Add(cpr::Parameter{param.first, param.second});
  }

  session.SetHeader(headers);
  session.SetUnixSocket(cpr::UnixSocket{"/var/run/docker.sock"});
  session.SetBody(cpr::Body{data.as_string()});
  session.SetUrl(url);
  session.SetHttpVersion(cpr::HttpVersion{cpr::HttpVersionCode::VERSION_1_1});
  session.SetParameters(create_parameters);

  cpr::Response r;
  switch (request_type) {
    case RequestType::GET:
      r = session.Get();
      break;
    case RequestType::POST:
      r = session.Post();
      break;
  }

  std::cout << r.text << std::endl;

  jsoncons::json j;
  if (not r.text.empty()) j = jsoncons::json::parse(r.text);

  return j;
}

auto Controller::list_containers(ListContainerInput input) -> std::vector<std::string> {
  std::vector<std::pair<std::string, std::string>> parameters;

  parameters.push_back(std::pair<std::string, std::string>("all", std::to_string(input.all)));
  parameters.push_back(std::pair<std::string, std::string>("limit", std::to_string(input.limit)));
  parameters.push_back(std::pair<std::string, std::string>("size", std::to_string(input.size)));
  parameters.push_back(std::pair<std::string, std::string>("filters", input.filters.to_string()));

  {
    "name" : { "MyNetwork" : true }
  }
  "

      std::cout
      << input.filters.to_string() << std::endl;
  auto response = send_request(RequestType::GET, "/containers/json", {}, parameters);

  return {};
}
auto Controller::list_networks() -> std::vector<std::string> { return std::vector<std::string>(); }
auto Controller::list_volumes() -> std::vector<std::string> { return std::vector<std::string>(); }

}  // namespace docker