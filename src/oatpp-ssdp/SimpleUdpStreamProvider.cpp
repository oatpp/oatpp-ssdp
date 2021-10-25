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


#include "SimpleUdpStreamProvider.hpp"
#include "UdpStream.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

#if defined(WIN32) || defined(_WIN32)
  #include <io.h>
  #include <WinSock2.h>
  #include <WS2udpip.h>
#else

  #include <netdb.h>
  #include <arpa/inet.h>
  #include <sys/select.h>
  #include <sys/socket.h>
  #include <netinet/udp.h>
  #include <unistd.h>
  #include <fcntl.h>

  #if defined(__FreeBSD__)
    #include <netinet/in.h>
  #endif

#endif

#define MAX_UDP_PAYLOAD_SIZE 65507

namespace oatpp { namespace ssdp {

void SimpleUdpStreamProvider::Invalidator::invalidate(const std::shared_ptr<data::stream::IOStream> &stream) {

  /************************************************
   * WARNING!!!
   *
   * shutdown(handle, SHUT_RDWR)    <--- DO!
   * close(handle);                 <--- DO NOT!
   *
   * DO NOT CLOSE file handle here -
   * USE shutdown instead.
   * Using close prevent FDs popping out of epoll,
   * and they'll be stuck there forever.
   ************************************************/

  auto c = std::static_pointer_cast<UdpStream>(stream);
  v_io_handle handle = c->getHandle();

#if defined(WIN32) || defined(_WIN32)
  shutdown(handle, SD_BOTH);
#else
  shutdown(handle, SHUT_RDWR);
#endif

}

SimpleUdpStreamProvider::SimpleUdpStreamProvider(v_uint16 port)
  : m_invalidator(std::make_shared<Invalidator>())
  , m_port(port)
  , m_closed(true)
{
  m_handle = instantiateServer();
  setProperty(PROPERTY_HOST, "localhost");
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(port));
}

#if defined(WIN32) || defined(_WIN32)

#error SimpleUdpStreamProvider not implemented for Windows yet. Feel free to implement it and create a PR!

#else

v_io_handle SimpleUdpStreamProvider::instantiateServer() {
  oatpp::v_io_handle serverHandle;
  v_int32 ret;
  int yes = 1;

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_flags = AI_PASSIVE;
  auto portStr = oatpp::utils::conversion::int32ToStr(m_port);

  ret = getaddrinfo(NULL, portStr->data(), &hints, &result);
  if (ret != 0) {
    OATPP_LOGE("[oatpp::ssdp::SimpleUdpStreamProvider::instantiateServer()]", "Error. Call to getaddrinfo() failed with result=%d: %s", ret, strerror(errno));
    throw std::runtime_error("[oatpp::ssdp::SimpleUdpStreamProvider::instantiateServer()]: Error. Call to getaddrinfo() failed.");
  }

  serverHandle = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (serverHandle < 0) {
    OATPP_LOGE("[oatpp::ssdp::SimpleUdpStreamProvider::instantiateServer()]", "Error. Couldn't open a socket: socket(%d, %d, %d) %s",
               result->ai_family, result->ai_socktype, result->ai_protocol, strerror(errno));
    throw std::runtime_error("[oatpp::ssdp::SimpleUdpStreamProvider::instantiateServer()]: Error. Couldn't open a socket");
  }

  m_closed = false;

  ret = setsockopt(serverHandle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if(ret < 0) {
    OATPP_LOGE("[oatpp::ssdp::SimpleUdpStreamProvider::instantiateServer()]", "Warning. Failed to set %s for accepting socket: %s", "SO_REUSEADDR", strerror(errno));
  }


  ret = bind(serverHandle, result->ai_addr, (int) result->ai_addrlen);
  if(ret != 0) {
    ::close(serverHandle);
    OATPP_LOGE("[oatpp::ssdp::SimpleUdpStreamProvider::instantiateServer()]", "Error. Failed to bind port %d: %s", m_port, strerror(errno));
    throw std::runtime_error("[oatpp::ssdp::SimpleUdpStreamProvider::instantiateServer()]: Error. Can't bind to address: %s");
  }

  fcntl(serverHandle, F_SETFL, O_NONBLOCK);

  return serverHandle;
}

#endif

void SimpleUdpStreamProvider::stop() {
  if (!m_closed) {
    m_closed = true;
#if defined(WIN32) || defined(_WIN32)
    ::closesocket(m_serverHandle);
#else
    ::close(m_handle);
#endif
  }
}

provider::ResourceHandle<data::stream::IOStream> SimpleUdpStreamProvider::get() {
  fd_set set;
  struct timeval timeout;
  FD_ZERO(&set);
  FD_SET(m_handle, &set);

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  while(!m_closed) {

    auto res = select(m_handle + 1, &set, nullptr, nullptr, &timeout);

    if (res == 0) {
      return nullptr;
    }

    if (res > 0) {
      break;
    }

  }

  return provider::ResourceHandle<data::stream::IOStream>(
    std::make_shared<UdpStream>(m_handle),
    m_invalidator
  );
}

}}