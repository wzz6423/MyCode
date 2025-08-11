# 如果你不想使用 shell 脚本而更倾向于使用 python 脚本的话, 考虑使用本脚本
import os
import subprocess
import shutil

def get_depends(executable_path, dest_dir):
    """
    获取可执行文件的依赖库并复制到指定目录
    
    参数:
        executable_path: 可执行文件的路径
        dest_dir: 依赖库存放的目标目录
    """
    try:
        # 确保目标目录存在
        os.makedirs(dest_dir, exist_ok=True)
        
        # 执行ldd命令获取依赖库
        result = subprocess.run(
            ['ldd', executable_path],
            capture_output=True,
            text=True,
            check=True
        )
        
        # 解析输出，提取依赖库路径
        depends = []
        for line in result.stdout.splitlines():
            parts = line.strip().split()
            # 寻找包含路径的条目（通常是第三个字段）
            for part in parts:
                if part.startswith('/'):
                    depends.append(part)
                    break
        
        # 复制依赖库到目标目录
        for dep in depends:
            if os.path.exists(dep):
                # 使用shutil.copy2保留元数据，类似cp -L处理符号链接
                shutil.copy2(dep, dest_dir, follow_symlinks=True)
                print(f"已复制: {dep} 到 {dest_dir}")
    
    except subprocess.CalledProcessError as e:
        print(f"执行ldd命令失败: {e}")
    except Exception as e:
        print(f"处理依赖时出错: {e}")

def main():
    # 处理各个服务器程序的依赖
    servers = [
        ("./gateway/build/gateway_server", "./gateway/depends"),
        ("./file/build/file_server", "./file/depends"),
        ("./crony/build/crony_server", "./crony/depends"),
        ("./message/build/message_server", "./message/depends"),
        ("./speech/build/speech_server", "./speech/depends"),
        ("./transmite/build/transmite_server", "./transmite/depends"),
        ("./user/build/user_server", "./user/depends")
    ]
    
    for exe, dep_dir in servers:
        print(f"处理 {exe} 的依赖...")
        get_depends(exe, dep_dir)
    
    # 复制特定的libcurl库
    libcurl_path = "/usr/lib/x86_64-linux-gnu/libcurl.so"
    user_dep_dir = "./user/depends"
    if os.path.exists(libcurl_path):
        os.makedirs(user_dep_dir, exist_ok=True)
        shutil.copy2(libcurl_path, user_dep_dir, follow_symlinks=True)
        print(f"已复制: {libcurl_path} 到 {user_dep_dir}")
    
    # 复制nc命令到各个build目录
    nc_path = "/bin/nc"
    build_dirs = [
        "./gateway/build",
        "./file/build",
        "./crony/build",
        "./message/build",
        "./speech/build",
        "./transmite/build",
        "./user/build"
    ]
    
    if os.path.exists(nc_path):
        for build_dir in build_dirs:
            os.makedirs(build_dir, exist_ok=True)
            shutil.copy2(nc_path, build_dir, follow_symlinks=True)
            print(f"已复制nc到 {build_dir}")
    
    # 处理nc命令的依赖
    for _, dep_dir in servers:
        print(f"处理nc的依赖到 {dep_dir}...")
        get_depends(nc_path, dep_dir)

if __name__ == "__main__":
    main()
