/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_ON_DEMAND_OS_TRANSPORT_SERVER_GRPC_HPP
#define IROHA_ON_DEMAND_OS_TRANSPORT_SERVER_GRPC_HPP

#include "ordering/on_demand_os_transport.hpp"

#include "network/impl/async_grpc_client.hpp"
#include "ordering.grpc.pb.h"

namespace iroha {
  namespace ordering {
    namespace transport {

      /**
       * gRPC client for on demand ordering service
       */
      class OnDemandOsClientGrpc
          : public OdOsNotification,
            private network::AsyncGrpcClient<google::protobuf::Empty> {
       public:
        OnDemandOsClientGrpc(
            std::unique_ptr<proto::OnDemandOrdering::StubInterface> stub,
            std::chrono::milliseconds timeout);

        // OnDemandOrderingService

        void onTransactions(CollectionType &&transactions) override;

        boost::optional<ProposalType> onRequestProposal(
            transport::RoundType round) override;

       private:
        logger::Logger log_;
        std::unique_ptr<proto::OnDemandOrdering::StubInterface> stub_;
        std::chrono::milliseconds timeout_;
      };

      class OnDemandOsClientGrpcFactory : public OdOsNotificationFactory {
       public:
        OnDemandOsClientGrpcFactory(std::chrono::milliseconds timeout);

        std::unique_ptr<OdOsNotification> create(
            const shared_model::interface::Peer &to) override;

       private:
        std::chrono::milliseconds timeout_;
      };

    }  // namespace transport
  }    // namespace ordering
}  // namespace iroha

#endif  // IROHA_ON_DEMAND_OS_TRANSPORT_SERVER_GRPC_HPP
