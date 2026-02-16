#include "cal.grpc.pb.h"  // 包含gRPC生成的桩头文件（protobuf定义的服务接口）
#include <grpcpp/grpcpp.h> // gRPC核心库

// ==================== 服务实现类 ====================
class CalculatorServiceImpl final : public calculator::Calculator::Service {
    // 实现Add远程方法（覆盖基类虚函数）
    grpc::Status Add(grpc::ServerContext* context,
                    const calculator::CalculationRequest* request,
                    calculator::CalculationResult* reply) override {
        // 执行加法运算：取请求中的a和b字段值相加
        reply->set_result(request->a() + request->b());
        return grpc::Status::OK; // 返回成功状态
    }

    // 实现Subtract远程方法（覆盖基类虚函数）
    grpc::Status Subtract(grpc::ServerContext* context,
                         const calculator::CalculationRequest* request,
                         calculator::CalculationResult* reply) override {
        // 执行减法运算：a - b
        reply->set_result(request->a() - request->b());
        return grpc::Status::OK; // 返回成功状态
    }
};

// ==================== 服务器启动函数 ====================
void RunServer() {
    // 设置服务器监听地址和端口（0.0.0.0表示监听所有网络接口）
    std::string server_address("0.0.0.0:50051");

    // 创建服务实现实例
    CalculatorServiceImpl service;

    // 创建服务器构建器
    grpc::ServerBuilder builder;

    // 配置服务器：
    // 1. 添加监听端口（使用不安全连接凭证，适用于测试环境）
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // 2. 注册服务实例（将我们的实现绑定到服务器）
    builder.RegisterService(&service);

    // 构建并启动服务器（返回unique_ptr管理的服务器实例）
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    // 输出启动信息
    std::cout << "Server listening on " << server_address << std::endl;

    // 阻塞等待服务器终止（保持服务器运行直到手动终止）
    server->Wait();
}

// ==================== 主函数 ====================
int main() {
    RunServer(); // 启动gRPC服务器
    return 0;
}