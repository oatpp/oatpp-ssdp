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

#include "UdpStream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

#if defined(WIN32) || defined(_WIN32)
  #include <io.h>
  #include <WinSock2.h>
  #include <Ws2ipdef.h>
  #include <ws2tcpip.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif


#if defined(WIN32) || defined(_WIN32)
typedef int socklen_t;
typedef std::int64_t ssize_t;
#endif


namespace oatpp { namespace ssdp {

const char* const UdpStream::PROPERTY_PEER_ADDRESS = "peer_address";
const char* const UdpStream::PROPERTY_PEER_ADDRESS_FORMAT = "peer_address_format";
const char* const UdpStream::PROPERTY_PEER_PORT = "peer_port";

UdpStream::UdpStream(v_io_handle handle)
  : m_mode(data::stream::IOMode::BLOCKING)
  , m_inBuffer(nullptr)
  , m_in(nullptr, 0, 0)
  , m_handle(handle)
  , m_context(data::stream::STREAM_FINITE)
{
  populate();
}

v_io_size UdpStream::populate() {
  std::vector<char> buf;
  buf.resize(MAX_MESSAGE_SIZE);

  memset(buf.data(), 0, MAX_MESSAGE_SIZE);
  struct sockaddr_storage clientAddress;
  socklen_t clientAddressSize = sizeof(clientAddress);
  memset(&clientAddress, 0, sizeof(clientAddressSize));

#if defined(WIN32) || defined(_WIN32)
  static const int flags = 0;
#else
  static const int flags = MSG_WAITALL;
#endif

  ssize_t rc = recvfrom(m_handle, (char *)buf.data(), MAX_MESSAGE_SIZE,
                        flags, ( struct sockaddr *) &clientAddress,
                        &clientAddressSize);
  if (rc < 0) {
    OATPP_LOGE("[oatpp::ssdp::SimpleUDPConnectionProvider::ExtendedUDPConnection::ExtendedUDPConnection()]", "Call to recvfrom() failed (%l).", rc);
    throw std::runtime_error("[oatpp::ssdp::SimpleUDPConnectionProvider::ExtendedUDPConnection::ExtendedUDPConnection()]: Error. Call to recvfrom() failed.");
  }

  buf.resize(rc);
  m_inBuffer = std::make_shared<base::StrBuffer>(buf.data(), rc, true);
  m_in.reset(m_inBuffer, m_inBuffer->getData(), m_inBuffer->getSize());

  oatpp::data::stream::Context::Properties props;

  if (clientAddress.ss_family == AF_INET) {

    char strIp[INET_ADDRSTRLEN];
    struct sockaddr_in* sockAddress = (struct sockaddr_in*) &clientAddress;
    inet_ntop(AF_INET, &sockAddress->sin_addr, strIp, INET_ADDRSTRLEN);

    props.put_LockFree(PROPERTY_PEER_ADDRESS, oatpp::String((const char*) strIp));
    props.put_LockFree(PROPERTY_PEER_ADDRESS_FORMAT, "ipv4");
    props.put_LockFree(PROPERTY_PEER_PORT, oatpp::utils::conversion::int32ToStr(sockAddress->sin_port));

  } else if (clientAddress.ss_family == AF_INET6) {

    char strIp[INET6_ADDRSTRLEN];
    struct sockaddr_in6* sockAddress = (struct sockaddr_in6*) &clientAddress;
    inet_ntop(AF_INET6, &sockAddress->sin6_addr, strIp, INET6_ADDRSTRLEN);

    props.put_LockFree(PROPERTY_PEER_ADDRESS, oatpp::String((const char*) strIp));
    props.put_LockFree(PROPERTY_PEER_ADDRESS_FORMAT, "ipv6");
    props.put_LockFree(PROPERTY_PEER_PORT, oatpp::utils::conversion::int32ToStr(sockAddress->sin6_port));

  } else {

    OATPP_LOGE("[oatpp::ssdp::SimpleUDPConnectionProvider::getExtendedUDPConnection()]", "Error. Unknown address family %u.", clientAddress.ss_family);
    throw std::runtime_error("[oatpp::ssdp::SimpleUDPConnectionProvider::getExtendedUDPConnection()]: Error. Unknown address family");

  }

  m_context = oatpp::data::stream::DefaultInitializedContext(data::stream::STREAM_FINITE, std::move(props));

  return rc;
}

v_io_size UdpStream::write(const void *buff, v_buff_size count, async::Action& action) {

  // Send a single message
  if (m_context.getProperties().get(PROPERTY_PEER_ADDRESS_FORMAT) == "ipv4") {
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = strtol((const char*)m_context.getProperties().get(PROPERTY_PEER_PORT)->c_str(), nullptr, 10);
    inet_pton(AF_INET, m_context.getProperties().get(PROPERTY_PEER_ADDRESS)->c_str(), &(cliaddr.sin_addr));

    sendto(m_handle, (const char*)buff,count, 0,
           (struct sockaddr*)&cliaddr, sizeof(cliaddr));
  } else if  (m_context.getProperties().get(PROPERTY_PEER_ADDRESS_FORMAT) == "ipv6") {
    struct sockaddr_in6 cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin6_family = AF_INET6;
    cliaddr.sin6_port = strtol((const char*)m_context.getProperties().get(PROPERTY_PEER_PORT)->c_str(), nullptr, 10);
    inet_pton(AF_INET6, m_context.getProperties().get(PROPERTY_PEER_ADDRESS)->c_str(), &(cliaddr.sin6_addr));

    sendto(m_handle, (const char*)buff, count, 0,
           (struct sockaddr*)&cliaddr, sizeof(cliaddr));
  }

  return count;
}

v_io_size UdpStream::read(void *buff, v_buff_size count, async::Action& action) {
  return m_in.readSimple(buff, count);
}

void UdpStream::setOutputStreamIOMode(data::stream::IOMode ioMode) {
  m_mode = ioMode;
}

data::stream::IOMode UdpStream::getOutputStreamIOMode() {
  return m_mode;
}

data::stream::Context& UdpStream::getOutputStreamContext() {
  return m_context;
}

void UdpStream::setInputStreamIOMode(data::stream::IOMode ioMode) {
  m_mode = ioMode;
}

data::stream::IOMode UdpStream::getInputStreamIOMode() {
  return m_mode;
}

data::stream::Context& UdpStream::getInputStreamContext() {
  return m_context;
}

v_io_handle UdpStream::getHandle() {
  return m_handle;
}

}}
