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

namespace oatpp { namespace ssdp {

data::stream::DefaultInitializedContext UdpStream::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_INFINITE);

UdpStream::UdpStream()
  : m_mode(data::stream::IOMode::BLOCKING)
  , m_inBuffer(nullptr)
  , m_in(nullptr, 0, 0)
{}

v_io_size UdpStream::write(const void *buff, v_buff_size count, async::Action& action) {
  // TODO - create a single UDP-packet.
  oatpp::String msg((const char*)buff, count, true);
  OATPP_LOGD("UdpStream", "write: '%s'", msg->c_str());
  return count;
}

v_io_size UdpStream::read(void *buff, v_buff_size count, async::Action& action) {

  if(!m_inBuffer) {
    oatpp::String dummyPacket = "Hello World!!!"; // TODO - read udp packet instead.
    m_inBuffer = dummyPacket.getPtr();
    m_in.reset(m_inBuffer, m_inBuffer->getData(), m_inBuffer->getSize());
  }

  return m_in.readSimple(buff, count);

}

void UdpStream::setOutputStreamIOMode(data::stream::IOMode ioMode) {
  m_mode = ioMode;
}

data::stream::IOMode UdpStream::getOutputStreamIOMode() {
  return m_mode;
}

data::stream::Context& UdpStream::getOutputStreamContext() {
  return DEFAULT_CONTEXT;
}

void UdpStream::setInputStreamIOMode(data::stream::IOMode ioMode) {
  m_mode = ioMode;
}

data::stream::IOMode UdpStream::getInputStreamIOMode() {
  return m_mode;
}

data::stream::Context& UdpStream::getInputStreamContext() {
  return DEFAULT_CONTEXT;
}

}}
