#include "cal.grpc.pb.h"  // 包含gRPC生成的桩头文件（由protoc工具生成）
#include <grpcpp/grpcpp.h> // 包含gRPC核心库

int main() {
    // ==================== 创建通信通道 ====================
    // 创建连接到localhost的50051端口的gRPC通道
    // grpc::InsecureChannelCredentials()表示使用不加密的连接（仅用于测试）
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

    // 使用通道创建服务存根(Stub)，这是与服务端交互的代理
    std::unique_ptr<calculator::Calculator::Stub> stub = calculator::Calculator::NewStub(channel);

    // ==================== 调用Add方法 ====================
    // 创建加法请求对象
    calculator::CalculationRequest add_request;
    add_request.set_a(5);  // 设置第一个运算参数
    add_request.set_b(3);  // 设置第二个运算参数

    // 创建响应对象（将存放服务器返回的结果）
    calculator::CalculationResult add_reply;
    // 创建客户端上下文（用于控制调用行为，如超时设置、元数据传输等）
    grpc::ClientContext add_context;

    // 同步调用Add方法（阻塞直到收到响应）
    grpc::Status add_status = stub->Add(&add_context, add_request, &add_reply);

    // 检查调用状态（RPC是否成功）
    if (add_status.ok()) {
        // 成功：从响应对象中提取结果并打印
        std::cout << "5 + 3 = " << add_reply.result() << std::endl;
    } else {
        // 失败：打印错误信息（实际生产中应增加错误处理）
        std::cerr << "Add RPC failed: " << add_status.error_message() << std::endl;
    }

    // ==================== 调用Subtract方法 ====================
    // 创建减法请求对象
    calculator::CalculationRequest sub_request;
    sub_request.set_a(10);  // 设置被减数
    sub_request.set_b(4);   // 设置减数

    calculator::CalculationResult sub_reply;  // 响应对象
    grpc::ClientContext sub_context;           // 独立的上下文（每个RPC应使用独立的context）

    // 同步调用Subtract方法
    grpc::Status sub_status = stub->Subtract(&sub_context, sub_request, &sub_reply);

    if (sub_status.ok()) {
        std::cout << "10 - 4 = " << sub_reply.result() << std::endl;
    } else {
        std::cerr << "Subtract RPC failed: " << sub_status.error_message() << std::endl;
    }

    return 0;
}