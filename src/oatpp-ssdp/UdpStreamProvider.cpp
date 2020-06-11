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

#include "UdpStreamProvider.hpp"

#include "UdpStream.hpp"

namespace oatpp { namespace ssdp {

void UdpStreamProvider::close() {
  throw std::runtime_error("[oatpp::ssdp::UdpStreamProvider::close()]: Error. Not Implemented!!!");
}

std::shared_ptr<data::stream::IOStream> UdpStreamProvider::getConnection() {
  return std::make_shared<oatpp::ssdp::UdpStream>();
}

void UdpStreamProvider::invalidateConnection(const std::shared_ptr<IOStream>& connection) {
  (void) connection;
  // DO Nothing
}

}}
