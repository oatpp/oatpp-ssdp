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

#ifndef oatpp_ssdp_SimpleSsdpUdpStreamProvider_hpp
#define oatpp_ssdp_SimpleSsdpUdpStreamProvider_hpp

#include "SimpleUdpStreamProvider.hpp"

namespace oatpp { namespace ssdp {

/**
 * Simple provider of UDP streams from SSDP Messages
 */
class SimpleSsdpUdpStreamProvider : public SimpleUdpStreamProvider {
public:
  /**
   * Constructor.
   */
  SimpleSsdpUdpStreamProvider();

  /**
   * Create shared SimpleUDPConnectionProvider.
   * @return - `std::shared_ptr` to SimpleUDPConnectionProvider.
   */
  static std::shared_ptr<SimpleSsdpUdpStreamProvider> createShared(){
    return std::make_shared<SimpleSsdpUdpStreamProvider>();
  }

  /**
   * Get incoming connection.
   * @return &id:oatpp::data::stream::IOStream;.
   */
  provider::ResourceHandle<data::stream::IOStream> get() override;

};

}}


#endif //oatpp_ssdp_SimpleSsdpUdpStreamProvider_hpp
