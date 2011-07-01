/* Copyright (c) 2010 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAIDSAFE_DHT_TRANSPORT_UDP_TRANSPORT_H_
#define MAIDSAFE_DHT_TRANSPORT_UDP_TRANSPORT_H_

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/asio/strand.hpp"
#include "boost/cstdint.hpp"
#include "maidsafe/dht/transport/transport.h"
#include "maidsafe/dht/transport/udp_request.h"
#include "maidsafe/dht/version.h"

#if MAIDSAFE_DHT_VERSION != 3001
#  error This API is not compatible with the installed library.\
    Please update the maidsafe-dht library.
#endif


namespace maidsafe {

namespace dht {

namespace transport {

class UdpTransport : public Transport,
                     public std::enable_shared_from_this<UdpTransport> {
 public:
  explicit UdpTransport(boost::asio::io_service &asio_service);  // NOLINT
  ~UdpTransport();

  virtual TransportCondition StartListening(const Endpoint &endpoint);
  virtual TransportCondition Bootstrap(const std::vector<Endpoint> &candidates);
  virtual void StopListening();
  virtual void Send(const std::string &data,
                    const Endpoint &endpoint,
                    const Timeout &timeout);

 private:
  UdpTransport(const UdpTransport&);
  UdpTransport &operator=(const UdpTransport&);

  typedef std::shared_ptr<boost::asio::ip::udp::socket> SocketPtr;
  typedef std::shared_ptr<boost::asio::ip::udp::endpoint> EndpointPtr;
  typedef std::shared_ptr<std::vector<unsigned char>> BufferPtr;
  typedef std::shared_ptr<UdpRequest> RequestPtr;
  typedef std::unordered_map<boost::uint64_t, RequestPtr> RequestMap;

  void DoSend(RequestPtr request);
  static void CloseSocket(SocketPtr socket);

  void StartRead();
  void HandleRead(SocketPtr socket,
                  BufferPtr read_buffer,
                  EndpointPtr sender_endpoint,
                  const boost::system::error_code &ec,
                  size_t bytes_transferred);
  void DispatchMessage(const std::string &data,
                       const Info &info,
                       boost::uint64_t reply_to_id);
  void HandleTimeout(boost::uint64_t request_id,
                     const boost::system::error_code &ec);

  boost::asio::io_service::strand strand_;
  SocketPtr socket_;
  BufferPtr read_buffer_;
  EndpointPtr sender_endpoint_;
  boost::uint64_t next_request_id_;
  RequestMap outstanding_requests_;
};

}  // namespace transport

}  // namespace dht

}  // namespace maidsafe

#endif  // MAIDSAFE_DHT_TRANSPORT_UDP_TRANSPORT_H_