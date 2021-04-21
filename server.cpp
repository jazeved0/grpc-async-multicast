#include <string>

#include "generated/demo.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

class DemoImpl final : public demo::Demo::Service {
  grpc::Status Ping(grpc::ServerContext *context,
                    const demo::PingRequest *request,
                    demo::PingReply *reply) override {
    return grpc::Status::OK;
  }
};

int main(int argc, char **argv) {
  std::string port(argv[1]);
  auto server_address = "0.0.0.0:" + port;
  DemoImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}
