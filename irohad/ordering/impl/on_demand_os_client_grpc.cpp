/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ordering/impl/on_demand_os_client_grpc.hpp"

#include "backend/protobuf/proposal.hpp"
#include "network/impl/grpc_channel_builder.hpp"

using namespace iroha::ordering::transport;

OnDemandOsClientGrpc::OnDemandOsClientGrpc(
    std::unique_ptr<proto::OnDemandOrdering::StubInterface> stub,
    std::chrono::milliseconds timeout)
    : network::AsyncGrpcClient<google::protobuf::Empty>(
          logger::log("OnDemandOsClientGrpc")),
      log_(logger::log("OnDemandOsClientGrpc")),
      stub_(std::move(stub)),
      timeout_(timeout) {}

// OnDemandOrderingService

void OnDemandOsClientGrpc::onTransactions(CollectionType &&transactions) {
  proto::TransactionsCollection message;
  for (auto &transaction : transactions) {
    *message.add_transactions() = std::move(
        static_cast<shared_model::proto::Transaction *>(transaction.get())
            ->getTransport());
  }

  auto call = new AsyncClientCall;

  call->response_reader =
      stub_->AsyncSendTransactions(&call->context, message, &cq_);

  call->response_reader->Finish(&call->reply, &call->status, call);
}

boost::optional<OdOsNotification::ProposalType>
OnDemandOsClientGrpc::onRequestProposal(transport::RoundType round) {
  grpc::ClientContext context;
  context.set_deadline(std::chrono::system_clock::now() + timeout_);
  proto::ProposalRequest request;
  request.set_block_round(round.first);
  request.set_reject_round(round.second);
  proto::ProposalResponse response;
  auto status = stub_->RequestProposal(&context, request, &response);
  if (not status.ok()) {
    log_->warn("RPC failed: {}", status.error_message());
    return boost::none;
  }
  if (not response.has_proposal()) {
    return boost::none;
  }
  return ProposalType{std::make_unique<shared_model::proto::Proposal>(
      std::move(response.proposal()))};
}

OnDemandOsClientGrpcFactory::OnDemandOsClientGrpcFactory(
    std::chrono::milliseconds timeout)
    : timeout_(timeout) {}

std::unique_ptr<OdOsNotification> OnDemandOsClientGrpcFactory::create(
    const shared_model::interface::Peer &to) {
  return std::make_unique<OnDemandOsClientGrpc>(
      network::createClient<proto::OnDemandOrdering>(to.address()), timeout_);
}
