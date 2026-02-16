#include <grpcpp/grpcpp.h>
#include "example.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using example::HelloRequest;
using example::HelloReply;
using example::Greeter;

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}

    std::string SayHello(const std::string& user) {
        HelloRequest request;
        request.set_name(user);
        HelloReply reply;
        ClientContext context;
        Status status = stub_->SayHello(&context, request, &reply);
        if (status.ok()) return reply.message();
        else return "RPC failed: " + status.error_message();
    }

private:
    std::unique_ptr<Greeter::Stub> stub_;
};

int main() {
    GreeterClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    std::string reply = client.SayHello("world");
    std::cout << "Response: " << reply << std::endl;
    return 0;
}