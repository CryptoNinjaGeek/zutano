#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/Types.h>

#include <nlohmann/json.hpp>

namespace zutano {

class Request {
 public:
  Request();

  ~Request() = default;

  auto Method(HttpMethod method) -> Request;

  auto Database(std::string endpoint) -> Request;

  auto Collection(std::string endpoint) -> Request;

  auto Endpoint(std::string endpoint) -> Request;

  auto Data(std::string) -> Request;

  auto Parameters(std::vector<StringPair>) -> Request;

  auto database() -> std::string;

  auto collection() -> std::string;

  auto endpoint() -> std::string;

  auto method() -> HttpMethod;

  auto data() -> std::string;

  auto parameters() -> std::vector<StringPair>;
 private:
  PrivateImplPtr p_;
};

typedef std::shared_ptr<Request> RequestPtr;

} // zutano
