#include <string>
#include <thread>
#include <vector>

#include "generated/demo.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

int main(int argc, char **argv) {
  // Collect each server port as CLI arguments
  std::vector<std::string> ports;
  for (int i = 1; i < argc; ++i) {
    ports.emplace_back(argv[i]);
  }

  // Create the stubs
  std::vector<std::shared_ptr<demo::Demo::Stub>> stubs;
  for (auto &it : ports) {
    auto address = "0.0.0.0:" + it;
    auto channel =
        grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    stubs.push_back(demo::Demo::NewStub(channel));
  }

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Starting ping multicast" << std::endl;
    auto start = std::chrono::system_clock::now();

    // Start all RPC calls onto the completion queue
    grpc::CompletionQueue cq;
    demo::PingRequest request;
    auto replies = std::vector<std::unique_ptr<demo::PingReply>>();
    auto statuses = std::vector<std::unique_ptr<grpc::Status>>();
    auto contexts = std::vector<std::unique_ptr<grpc::ClientContext>>();
    for (size_t i = 0; i < ports.size(); ++i) {
      auto reply = std::unique_ptr<demo::PingReply>(new demo::PingReply);
      auto status = std::unique_ptr<grpc::Status>(new grpc::Status);
      auto context =
          std::unique_ptr<grpc::ClientContext>(new grpc::ClientContext);

      // Configure the deadline
      auto deadline_time =
          std::chrono::system_clock::now() + std::chrono::seconds(1);
      context->set_deadline(deadline_time);

      // Get a connection to the node and make the RPC call
      auto rpc = stubs[i]->PrepareAsyncPing(context.get(), request, &cq);
      rpc->StartCall();
      auto tag = static_cast<intptr_t>(i);
      rpc->Finish(reply.get(), status.get(), (void *)tag);

      replies.push_back(std::move(reply));
      statuses.push_back(std::move(status));
      contexts.push_back(std::move(context));
    }
    cq.Shutdown();

    // Await for each of them as they finish
    intptr_t tag;
    bool ok;
    while (cq.Next(reinterpret_cast<void **>(&tag), &ok)) {
      auto end = std::chrono::system_clock::now();
      auto duraton_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();
      if (ok && statuses[tag]->ok()) {
        std::cout << "Server at 0.0.0.0" << ports[tag] << " responded in "
                  << duraton_ms << "ms" << std::endl;
      } else {
        std::cout << "Server at 0.0.0.0 " << ports[tag]
                  << " failed to respond after " << duraton_ms << "ms"
                  << std::endl;
      }
    }
  }
}
