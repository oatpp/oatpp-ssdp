/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <oatpp@bamkrs.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "SimpleSsdpUdpStreamProvider.hpp"

#if defined(WIN32) || defined(_WIN32)

  #include <io.h>
  #include <WinSock2.h>
  #include <Ws2ipdef.h>

#else

  #include <netdb.h>
  #include <arpa/inet.h>
  #include <sys/socket.h>
  #include <netinet/udp.h>
  #include <unistd.h>
  #include <fcntl.h>

  #if defined(__FreeBSD__)
    #include <netinet/in.h>
  #endif

#endif

namespace oatpp { namespace ssdp {

SimpleSsdpUdpStreamProvider::SimpleSsdpUdpStreamProvider() : SimpleUdpStreamProvider(1900) {
  int yes = 1;
  int ret;

  ret = setsockopt(m_handle, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, sizeof(int));
  if (ret < 0) {
    OATPP_LOGE("[oatpp::ssdp::SimpleSsdpUdpStreamProvider::SimpleSsdpUdpStreamProvider()]", "Warning. Failed to set %s for accepting socket: %s", "SO_BROADCAST", strerror(errno));
  }

  struct ip_mreq mreq;

  /* the multicast group you want to join */
  mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
  /* the IP of the listening NIC */
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (0 != setsockopt(m_handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof mreq)) {
    SimpleUdpStreamProvider::stop();
    OATPP_LOGE("[oatpp::ssdp::SimpleSsdpUdpStreamProvider::SimpleSsdpUdpStreamProvider()]", "Error. Failed to setsockopt: %s", m_port, strerror(errno));
    throw std::runtime_error("[oatpp::ssdp::SimpleSsdpUdpStreamProvider::SimpleSsdpUdpStreamProvider()]: Error. Failed to setsockopt: %s");
  }

  setProperty(PROPERTY_HOST, "0.0.0.0");

}

provider::ResourceHandle<data::stream::IOStream> SimpleSsdpUdpStreamProvider::get() {
  return SimpleUdpStreamProvider::get();
}

}}
