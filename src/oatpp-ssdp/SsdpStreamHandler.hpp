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

#ifndef oatpp_ssdp_SsdpStreamHandler_hpp
#define oatpp_ssdp_SsdpStreamHandler_hpp

#include "oatpp/web/server/HttpProcessor.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/network/server/ConnectionHandler.hpp"

namespace oatpp { namespace ssdp {

class SsdpStreamHandler : public base::Countable, public network::server::ConnectionHandler {
private:
  std::shared_ptr<web::server::HttpProcessor::Components> m_components;
public:

  /**
   * Constructor.
   * @param components - &id:oatpp::web::server::HttpProcessor::Components;.
   */
  SsdpStreamHandler(const std::shared_ptr<web::server::HttpProcessor::Components>& components);

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   */
  SsdpStreamHandler(const std::shared_ptr<web::server::HttpRouter>& router)
    : SsdpStreamHandler(std::make_shared<web::server::HttpProcessor::Components>(router))
  {}

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param config - &id:oatpp::web::server::HttpProcessor::Config;.
   */
  SsdpStreamHandler(const std::shared_ptr<web::server::HttpRouter>& router,
                    const std::shared_ptr<web::server::HttpProcessor::Config>& config)
    : SsdpStreamHandler(std::make_shared<web::server::HttpProcessor::Components>(router, config))
  {}

public:

  /**
   * Create shared SsdpStreamHandler.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @return - `std::shared_ptr` to SsdpStreamHandler.
   */
  static std::shared_ptr<SsdpStreamHandler> createShared(const std::shared_ptr<web::server::HttpRouter>& router);

  /**
   * Set root error handler for all requests coming through this Connection Handler.
   * All unhandled errors will be handled by this error handler.
   * @param errorHandler - &id:oatpp::web::server::handler::ErrorHandler;.
   */
  void setErrorHandler(const std::shared_ptr<web::server::handler::ErrorHandler>& errorHandler);

  /**
   * Set request interceptor. Request intercepted after route is resolved but before corresponding route endpoint is called.
   * @param interceptor - &id:oatpp::web::server::handler::RequestInterceptor;.
   */
  void addRequestInterceptor(const std::shared_ptr<web::server::handler::RequestInterceptor>& interceptor);

  /**
   * Implementation of &id:oatpp::network::server::ConnectionHandler::handleConnection;.
   * @param connection - &id:oatpp::data::stream::IOStream; representing connection.
   */
  void handleConnection(const std::shared_ptr<IOStream>& connection, const std::shared_ptr<const ParameterMap>& params) override;

  /**
   * Tell all worker threads to exit when done.
   */
  void stop() override;

};

}}

#endif //oatpp_ssdp_SsdpStreamHandler_hpp
