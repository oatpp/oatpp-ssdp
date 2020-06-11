
#include "oatpp-test/UnitTest.hpp"

#include "oatpp-ssdp/UDPStreamProvider.hpp"
#include "oatpp-ssdp/Message.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <iostream>

namespace {

class Test : public oatpp::test::UnitTest {
public:
  Test() : oatpp::test::UnitTest("MyTag")
  {}

  void onRun() override {

    oatpp::ssdp::UDPStreamProvider messageProvider;

    // strange that MessageProvider provides 'Connection' ?
    // I guess it should be changed to 'getStream'
    // And 'MessageProvider' maybe should be changed to 'MessageStreamProvider'
    auto msg = messageProvider.getConnection();

    {
      v_char8 buff[1024];
      auto res = msg->readSimple(buff, 1024);
      buff[res] = 0;
      OATPP_LOGD(TAG, "message='%s'", buff);
    }

    {
      msg->writeSimple("To Outer Space!", 15);
      //msg->flush();
    }

  }
};

void runTests() {
  OATPP_RUN_TEST(Test);
}

}

int main() {

  oatpp::base::Environment::init();

  runTests();

  /* Print how much objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

  OATPP_ASSERT(oatpp::base::Environment::getObjectsCount() == 0);

  oatpp::base::Environment::destroy();

  return 0;
}
