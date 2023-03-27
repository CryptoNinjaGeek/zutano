#pragma one

#include <zutano/PrivateImpl.h>
#include <zutano/Connection.h>
#include <zutano/Database.h>

namespace zutano::pimp {
class CursorPimpl : public PrivateImpl {
 public:
  Connection connection_;
  std::string id_;
  std::string type_{"cursor"};
  std::string profile_;
  std::string warnings_;
  std::string cached_;
  std::string db_name_;
  long count_{0};
  bool has_more_{false};
  jsoncons::json stats_;
  std::vector<jsoncons::json> rows_;

  static inline auto Pimpl(const std::shared_ptr<PrivateImpl> &p) {
	return std::dynamic_pointer_cast<CursorPimpl>(p);
  }
};
} // zutano