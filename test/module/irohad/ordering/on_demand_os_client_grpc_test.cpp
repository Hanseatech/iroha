/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include "ordering/impl/on_demand_os_client_grpc.hpp"

using namespace iroha::ordering::transport;

struct OnDemandOsClientGrpcTest : public ::testing::Test {
  proto::OnDemandOrdering::StubInterface *stub;
  std::chrono::milliseconds timeout;
};

TEST_F(OnDemandOsClientGrpcTest, onTransactions) {}

TEST_F(OnDemandOsClientGrpcTest, onRequestProposal) {}
