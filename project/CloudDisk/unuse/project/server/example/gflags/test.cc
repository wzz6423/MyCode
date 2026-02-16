// C++
#include <print>
// gflags
#include <gflags/gflags.h>

DEFINE_string(ip, "0.0.0.0", "服务器默认监听 IP 地址, 格式: n.n.n.n");
DEFINE_int32(port, 6423, "服务器默认监听 port, 格式: 0-65535");
DEFINE_bool(debug_enable, true, "是否开启 debug 模式, 格式: true/false");

auto main(int argc, char **argv) -> int
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::println("ip: {}, port: {}, debug: {}", FLAGS_ip, FLAGS_port, FLAGS_debug_enable);

  return 0;
}

// bash:

// ./test --help
// ......
//   Flags from test.cc:
// -debug_enable (是否开启 debug 模式, 格式: true/false) type: bool
//   default: true
// -ip (服务器默认监听 IP 地址, 格式: n.n.n.n) type: string
//   default: "0.0.0.0"
// -port (服务器默认监听 port, 格式: 0-65535) type: int32
//   default: 6423

// 直接使用
// usr@Linux$ ./test
// ip: 0.0.0.0, port: 6423, debug: true

// 命令行指定参数
// usr@Linux$ ./test --ip=127.0.0.1 --port=8080 --debug_enable=false
// ip: 127.0.0.1, port: 8080, debug: false

// 配置文件
// usr@Linux$ ./test --flagfile config.conf
// 或者
// usr@Linux$ ./test -flagfile=config.conf
// ip: 127.0.0.1, port: 8080, debug: false