// Copyright 2022, Roman Gershman.  All rights reserved.
// See LICENSE for licensing terms.
//
#pragma once

#include <absl/container/flat_hash_map.h>

#include <string_view>

#include "server/conn_context.h"

namespace dfly {

// Database holding pubsub subscribers.
class ChannelSlice {
 public:
  struct Subscriber {
    ConnectionContext* conn_cntx;
    util::fibers_ext::BlockingCounter borrow_token;
    uint32_t thread_id;

    Subscriber(ConnectionContext* cntx, uint32_t tid);
    // Subscriber() : borrow_token(0) {}

    Subscriber(Subscriber&&) noexcept = default;
    Subscriber& operator=(Subscriber&&) noexcept = default;

    Subscriber(const Subscriber&) = delete;
    void operator=(const Subscriber&) = delete;
  };

  std::vector<Subscriber> FetchSubscribers(std::string_view channel);

  void RemoveSubscription(std::string_view channel, ConnectionContext* me);
  void AddSubscription(std::string_view channel, ConnectionContext* me, uint32_t thread_id);

 private:
  struct SubscriberInternal {
    uint32_t thread_id;  // proactor thread id.

    SubscriberInternal(uint32_t tid) : thread_id(tid) {}
  };

  struct Channel {
    absl::flat_hash_map<ConnectionContext*, SubscriberInternal> subscribers;
  };

  absl::flat_hash_map<std::string, std::unique_ptr<Channel>> channels_;
  absl::flat_hash_map<std::string, std::unique_ptr<Channel>> patterns_;
};

}  // namespace dfly
