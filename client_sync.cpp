#include <string>
#include <thread>
#include <vector>

#include "generated/demo.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#define DEADLINE_SEC 1

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

  // Make each RPC call in series
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Starting ping multicast" << std::endl;
    auto start = std::chrono::system_clock::now();

    for (size_t i = 0; i < ports.size(); ++i) {
      auto context =
          std::unique_ptr<grpc::ClientContext>(new grpc::ClientContext);

      // Configure the deadline
      auto deadline_time =
          std::chrono::system_clock::now() + std::chrono::seconds(DEADLINE_SEC);
      context->set_deadline(deadline_time);

      demo::PingRequest request;
      demo::PingReply reply;
      auto status = stubs[i]->Ping(context.get(), request, &reply);

      auto end = std::chrono::system_clock::now();
      auto duraton_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();
      if (status.ok()) {
        std::cout << "Server at 0.0.0.0:" << ports[i] << " responded in "
                  << duraton_ms << "ms" << std::endl;
      } else {
        std::cout << "Server at 0.0.0.0:" << ports[i]
                  << " failed to respond after " << duraton_ms << "ms"
                  << std::endl;
      }
    }
  }
}
