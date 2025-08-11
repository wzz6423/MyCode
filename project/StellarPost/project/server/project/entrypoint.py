# python3 wait_and_exec.py -h 127.0.0.1 -p 8080,3306 -c "echo '所有端口已就绪'"
import socket
import time
import argparse
import subprocess

def wait_for(ip, port):
    """等待指定IP的端口可用"""
    while True:
        try:
            # 创建socket连接尝试
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.settimeout(1)  # 1秒超时
                result = s.connect_ex((ip, port))
                if result == 0:
                    print(f"端口 {ip}:{port} 已可用")
                    return
        except Exception as e:
            print(f"检测 {ip}:{port} 时发生错误: {e}")
        
        # 连接失败，等待1秒后重试
        time.sleep(1)

def main():
    # 解析命令行参数
    parser = argparse.ArgumentParser(description='等待指定IP的多个端口可用后执行命令')
    parser.add_argument('-h', required=True, help='目标IP地址')
    parser.add_argument('-p', required=True, help='端口列表，用逗号分隔')
    parser.add_argument('-c', required=True, help='要执行的命令')
    
    args = parser.parse_args()
    
    # 解析端口列表
    ports = []
    for port_str in args.p.split(','):
        try:
            port = int(port_str.strip())
            if 1 <= port <= 65535:
                ports.append(port)
            else:
                print(f"无效的端口号: {port_str}，将被忽略")
        except ValueError:
            print(f"无法将 {port_str} 转换为端口号，将被忽略")
    
    if not ports:
        print("没有有效的端口号，程序退出")
        return
    
    # 等待所有端口可用
    for port in ports:
        print(f"等待 {args.h}:{port} 可用...")
        wait_for(args.h, port)
    
    # 执行命令
    print(f"所有端口已可用，执行命令: {args.c}")
    try:
        # 使用shell=True来支持复杂命令和管道
        subprocess.run(args.c, shell=True, check=True, executable='/bin/bash')
    except subprocess.CalledProcessError as e:
        print(f"命令执行失败: {e}")
        exit(1)

if __name__ == "__main__":
    main()
