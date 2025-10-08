#!/bin/bash

# Ubuntu 24.04 Vim 自动化配置脚本（全新安装）
# 支持 C++、Java、Go、Python 等多语言开发环境
# 自动清除过去的安装配置

# 清除过去的安装
clean_previous_install() {
    echo -e "\e[31m[步骤0/8] 清除过去的安装配置...\e[0m"
    
    # 备份用户自定义配置（如果有）
    if [ -f "$HOME/.vimrc" ]; then
        echo "备份旧配置: ~/.vimrc -> ~/.vimrc.bak"
        cp "$HOME/.vimrc" "$HOME/.vimrc.bak"
    fi
    
    # 彻底清除 Vim 相关文件和目录
    rm -rf "$HOME/.vim" 2>/dev/null
    rm -f "$HOME/.vimrc" 2>/dev/null
    rm -f "$HOME/.viminfo" 2>/dev/null
    rm -rf "$HOME/.vim/plugged" 2>/dev/null
    rm -rf "$HOME/.config/coc" 2>/dev/null
    rm -rf "$HOME/.coc" 2>/dev/null
    rm -rf "$HOME/.cache/vim" 2>/dev/null
    rm -rf "$HOME/.npm-global" 2>/dev/null
    # 清除vimplus残留（若存在）
    rm -rf "$HOME/.vimplus" 2>/dev/null
    
    # 清除 Coc 相关扩展
    find "$HOME/.config" -maxdepth 1 -name 'coc-*' -exec rm -rf {} + 2>/dev/null
    
    # 重置虚拟环境
    rm -rf "$HOME/.venv" 2>/dev/null
    
    echo "过去的 Vim 配置已被彻底清除"
    sleep 2
}

# 函数：安全退出
safe_exit() {
    echo -e "\e[31m[错误] $1\e[0m"
    echo "脚本执行中断。请查看输出信息并手动完成后续设置。"
    exit 1
}

# 检查用户权限
if [ "$(id -u)" -eq 0 ]; then
    safe_exit "请不要使用 root 权限运行此脚本。请以普通用户身份运行。"
fi

# 设置目录变量
VIM_DIR="$HOME/.vim"
VIMRC="$HOME/.vimrc"
VIM_PLUGINS="$VIM_DIR/plugged"

# ------------------------- 主脚本开始 -------------------------

# 步骤0: 清除过去的安装
clean_previous_install

# 步骤1: 安装必要依赖
echo -e "\e[32m[步骤1/8] 安装系统依赖...\e[0m"
sudo apt update || echo "警告: 系统更新失败，继续执行..."

# 安装必要软件包（包含git，为后续vimplus克隆做准备）
sudo apt install -y --no-install-recommends \
    vim git cmake build-essential \
    python3-dev python3-pip python3-venv \
    nodejs npm openjdk-17-jdk golang clang clangd \
    ca-certificates curl || echo "警告: 部分依赖安装可能不完全"

# 设置npm全局安装权限
mkdir -p "$HOME/.npm-global"
npm config set prefix "$HOME/.npm-global" 2>/dev/null

# 添加npm到PATH
echo 'export PATH="$HOME/.npm-global/bin:$PATH"' >> "$HOME/.bashrc"
source "$HOME/.bashrc" >/dev/null 2>&1

# 步骤2: 安装Vim插件管理器
echo -e "\e[32m[步骤2/8] 安装vim-plug...\e[0m"
mkdir -p "$VIM_DIR/autoload"
curl -fLo "$VIM_DIR/autoload/plug.vim" --create-dirs \
    https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim || safe_exit "vim-plug下载失败"

# 步骤3: 创建Vim基础配置
echo -e "\e[32m[步骤3/8] 配置Vim基础设置...\e[0m"
cat > "$VIMRC" << 'EOL'
" ===== 基础设置 =====
set nocompatible
filetype plugin indent on
syntax enable
set encoding=utf-8
set number relativenumber
set tabstop=4
set shiftwidth=4
set expandtab
set smartindent
set cursorline
set mouse=a
set clipboard=unnamedplus
set termguicolors
set hidden
set nobackup
set nowritebackup
set updatetime=300
set signcolumn=yes

" ===== 插件管理 =====
call plug#begin('~/.vim/plugged')

" 外观
Plug 'gruvbox-community/gruvbox'              " 主题
Plug 'vim-airline/vim-airline'                " 状态栏
Plug 'vim-airline/vim-airline-themes'         " 状态栏主题
Plug 'Yggdroot/indentLine'                   " 缩进参考线

" 功能增强
Plug 'preservim/nerdtree'                    " 文件浏览器
Plug 'junegunn/fzf', { 'do': { -> fzf#install() } } " 模糊搜索
Plug 'junegunn/fzf.vim'
Plug 'tpope/vim-fugitive'                    " Git集成
Plug 'airblade/vim-gitgutter'                " Git变更标记
Plug 'tpope/vim-commentary'                  " 注释工具
Plug 'jiangmiao/auto-pairs'                  " 自动括号
Plug 'preservim/tagbar'                      " 代码结构视图

" 语言支持
Plug 'sheerun/vim-polyglot'                  " 多语言语法支持
Plug 'neoclide/coc.nvim', {'branch': 'release'} " LSP支持
Plug 'fatih/vim-go', { 'do': ':GoUpdateBinaries' } " Go支持

call plug#end()

" ===== 主题和外观配置 =====
colorscheme gruvbox
set background=dark
let g:airline_theme='gruvbox'
let g:indentLine_setColors = 0
let g:indentLine_char = '|'

" ===== 快捷键映射 =====
let mapleader = ","
nmap <silent> <leader>d :NERDTreeToggle<CR>
nmap <silent> <C-p> :Files<CR>
nmap <silent> <leader>t :TagbarToggle<CR>
nmap <silent> <leader>f :CocCommand eslint.executeAutofix<CR>
nmap <silent> <leader>r :source $MYVIMRC<CR>

" 窗口导航
nmap <silent> <C-h> <C-w>h
nmap <silent> <C-j> <C-w>j
nmap <silent> <C-k> <C-w>k
nmap <silent> <C-l> <C-w>l

" ===== Coc (LSP) 配置 =====
nmap <silent> gd <Plug>(coc-definition)
nmap <silent> gr <Plug>(coc-references)
nmap <silent> gi <Plug>(coc-implementation)
nmap <silent> <leader>rn <Plug>(coc-rename)
inoremap <expr> <Tab> pumvisible() ? "\<C-n>" : "\<Tab>"
inoremap <expr> <S-Tab> pumvisible() ? "\<C-p>" : "\<S-Tab>"

" 诊断导航
nmap <silent> [g <Plug>(coc-diagnostic-prev)
nmap <silent> ]g <Plug>(coc-diagnostic-next)

" 语言服务器扩展
let g:coc_global_extensions = [
    \ 'coc-clangd',
    \ 'coc-java',
    \ 'coc-pyright',
    \ 'coc-go',
    \ 'coc-json',
    \ 'coc-yaml',
    \ 'coc-tsserver',
    \ 'coc-html',
    \ 'coc-css',
    \ 'coc-eslint',
    \ 'coc-marketplace'
    \ ]

" ===== 文件类型设置 =====
" Python
autocmd FileType python setlocal commentstring=#\ %s expandtab tabstop=4 shiftwidth=4
let g:python3_host_prog = '/usr/bin/python3'

" Java
autocmd FileType java setlocal commentstring=//\ %s expandtab tabstop=2 shiftwidth=2

" Go
let g:go_fmt_command = "goimports"
let g:go_autodetect_gopath = 1
autocmd BufWritePre *.go :call CocAction('format')

" C++
autocmd FileType cpp setlocal commentstring=//\ %s
autocmd FileType cpp setlocal tabstop=2 shiftwidth=2

" JavaScript/TypeScript
autocmd FileType javascript,typescript setlocal tabstop=2 shiftwidth=2

" JSON
autocmd FileType json setlocal conceallevel=0

" ===== 保存时自动格式化 =====
command! -nargs=0 Prettier :CocCommand prettier.formatFile
autocmd BufWritePre *.js,*.jsx,*.mjs,*.ts,*.tsx,*.css,*.less,*.scss,*.json,*.graphql,*.html,*.yaml,*.yml :call CocAction('runCommand', 'prettier.formatFile')

" ===== 文档查看功能 =====
function! s:show_documentation()
  if (index(['vim','help'], &filetype) >= 0)
    execute 'h '.expand('<cword>')
  elseif (coc#rpc#ready())
    call CocActionAsync('doHover')
  else
    execute '!' . &keywordprg . " " . expand('<cword>')
  endif
endfunction
nnoremap <silent> K :call <SID>show_documentation()<CR>

" ===== 首次运行自动安装插件 =====
autocmd VimEnter * if len(filter(values(g:plugs), '!isdirectory(v:val.dir)'))
      \| PlugInstall --sync | source $MYVIMRC
      \| endif
EOL

# 步骤4: 安装Vim基础插件
echo -e "\e[32m[步骤4/8] 安装Vim基础插件...\e[0m"
vim -E -s -c "source $VIMRC" -c "PlugInstall" -c "qa" >/dev/null 2>&1 || echo "警告: 基础插件安装可能不完全，后续可手动执行:PlugInstall"

# 步骤5: 配置语言服务器核心
echo -e "\e[32m[步骤5/8] 配置语言服务器核心组件...\e[0m"
if [ -d "$VIM_PLUGINS/coc.nvim" ]; then
    cd "$VIM_PLUGINS/coc.nvim"
    npm install --no-audit --quiet >/dev/null 2>&1 || echo "警告: Coc.nvim核心安装可能不完整，后续可手动进入~/.vim/plugged/coc.nvim执行npm install"
    cd - >/dev/null
fi

# 步骤6: 安装开发辅助工具
echo -e "\e[32m[步骤6/8] 安装多语言开发辅助工具...\e[0m"

# Python工具链
python3 -m venv ~/.venv >/dev/null 2>&1
source ~/.venv/bin/activate
pip install --quiet --upgrade pip >/dev/null 2>&1
pip install --quiet jedi pylint flake8 autopep8 >/dev/null 2>&1 || echo "警告: Python开发工具安装失败，后续可手动激活venv并重新安装"
deactivate

# Go工具链
command -v go >/dev/null && {
    go install golang.org/x/tools/gopls@latest >/dev/null 2>&1
    go install golang.org/x/tools/cmd/goimports@latest >/dev/null 2>&1
} || echo "警告: Go工具链未检测到，跳过Go相关工具安装"

# NodeJS工具链
command -v npm >/dev/null && {
    npm install --silent -g typescript eslint prettier >/dev/null 2>&1 || echo "警告: NodeJS开发工具安装失败，后续可手动执行npm install -g typescript eslint prettier"
} || echo "警告: npm未检测到，跳过NodeJS相关工具安装"

# 步骤7: 安装vimplus增强套件（新增步骤）
echo -e "\e[32m[步骤7/8] 安装vimplus增强套件...\e[0m"
# 克隆vimplus仓库到用户目录
if command -v git &> /dev/null; then
    echo "正在从Gitee克隆vimplus仓库..."
    git clone https://gitee.com/wzz6423/vimplus.git ~/.vimplus || safe_exit "vimplus仓库克隆失败，请检查网络连接或Gitee仓库地址"
else
    safe_exit "git命令未找到，无法克隆vimplus仓库（可能是步骤1依赖安装失败）"
fi

# 执行vimplus安装脚本
echo "进入vimplus目录并执行安装脚本..."
cd ~/.vimplus || safe_exit "无法进入~/.vimplus目录，克隆操作可能未完成"
# 为安装脚本添加执行权限
chmod +x install.sh || echo "警告: 无法为install.sh添加执行权限，尝试直接运行"
# 执行安装脚本（vimplus安装过程可能需要交互，此处保持默认执行）
./install.sh || echo "警告: vimplus安装脚本执行异常，后续可手动进入~/.vimplus目录执行./install.sh完成安装"
cd - >/dev/null

# 步骤8: 生成最终配置文件并收尾
echo -e "\e[32m[步骤8/8] 生成最终配置文件并完成设置...\e[0m"
mkdir -p "$HOME/.vim"
# 生成Coc.nvim全局配置
cat > "$HOME/.vim/coc-settings.json" << 'EOL'
{
  "languageserver": {
    "ccls": {
      "command": "ccls",
      "filetypes": ["c", "cc", "cpp", "c++", "objc", "objcpp"],
      "rootPatterns": [".ccls", "compile_commands.json", ".git/", ".hg/"]
    },
    "golang": {
      "command": "gopls",
      "rootPatterns": ["go.mod", ".git/"],
      "filetypes": ["go"]
    }
  },
  "coc.preferences.formatOnSaveFiletypes": ["*"],
  "diagnostic.enable": true,
  "diagnostic.displayByAle": false
}
EOL

# 完成信息汇总
echo -e "\n\e[42m\e[30m Vim 完整配置（含vimplus）成功完成！ \e[0m\e[49m"
echo ""
echo "已集成的核心功能:"
echo "  ✓ 现代化界面主题（Gruvbox + vim-airline）"
echo "  ✓ 智能代码补全与LSP支持（Coc.nvim）"
echo "  ✓ 多语言开发环境（C++/Java/Go/Python/JS/TS）"
echo "  ✓ Git版本控制深度集成（fugitive + gitgutter）"
echo "  ✓ 自动代码格式化与诊断"
echo "  ✓ vimplus增强套件（含额外插件与优化）"
echo ""
echo "常用快捷键指南（基础+vimplus补充）:"
echo "  基础快捷键:"
echo "    ,d        - 打开/关闭文件浏览器（NERDTree）"
echo "    Ctrl+P    - 文件模糊搜索（fzf）"
echo "    ,t        - 查看代码结构（Tagbar）"
echo "    gd        - 跳转到定义（Coc.nvim）"
echo "    gr        - 查找引用（Coc.nvim）"
echo "    ,f        - 修复当前文件（ESLint）"
echo "    ,r        - 重新加载Vim配置"
echo "  vimplus补充快捷键（具体以vimplus文档为准）:"
echo "    F2        - 重命名当前文件"
echo "    F3        - 搜索当前单词"
echo "    F4        - 切换头文件/源文件（C/C++）"
echo "    F5        - 编译当前项目"
echo ""
echo "后续操作建议:"
echo "  1. 关闭当前终端，打开新终端使所有环境变量生效"
echo "  2. 首次运行 'vim' 时，等待剩余插件自动安装完成"
echo "  3. 若vimplus功能异常，手动执行以下命令修复:"
echo "      cd ~/.vimplus && ./install.sh"
echo "  4. 安装额外语言支持（示例）:"
echo "      :CocInstall coc-python  # 增强Python支持"
echo "      :CocInstall coc-rust-analyzer  # Rust支持"
echo ""
echo "配置检查命令:"
echo "  :checkhealth  - 检查Vim整体环境状态"
echo "  :CocInfo      - 查看Coc.nvim语言服务器运行信息"
echo "  :VimplusHelp  - 查看vimplus详细帮助文档"