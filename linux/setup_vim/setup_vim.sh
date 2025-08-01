#!/bin/bash

# Ubuntu 24.04 Vim 自动化配置脚本（全新安装）
# 支持 C++、Java、Go、Python 等多语言开发环境
# 自动清除过去的安装配置

# 清除过去的安装
clean_previous_install() {
    echo -e "\e[31m[步骤0/7] 清除过去的安装配置...\e[0m"
    
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
echo -e "\e[32m[步骤1/7] 安装系统依赖...\e[0m"
sudo apt update || echo "警告: 系统更新失败，继续执行..."

# 安装必要软件包
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
echo -e "\e[32m[步骤2/7] 安装vim-plug...\e[0m"
mkdir -p "$VIM_DIR/autoload"
curl -fLo "$VIM_DIR/autoload/plug.vim" --create-dirs \
    https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim || safe_exit "vim-plug下载失败"

# 步骤3: 创建Vim配置
echo -e "\e[32m[步骤3/7] 配置Vim...\e[0m"
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

# 步骤4: 安装Vim插件
echo -e "\e[32m[步骤4/7] 安装Vim插件...\e[0m"
vim -E -s -c "source $VIMRC" -c "PlugInstall" -c "qa" >/dev/null 2>&1 || echo "警告: 插件安装可能不完全"

# 步骤5: 安装语言服务器
echo -e "\e[32m[步骤5/7] 配置语言服务器...\e[0m"
if [ -d "$VIM_PLUGINS/coc.nvim" ]; then
    cd "$VIM_PLUGINS/coc.nvim"
    npm install --no-audit --quiet >/dev/null 2>&1 || echo "警告: Coc安装可能不完整"
    cd - >/dev/null
fi

# 步骤6: 安装开发工具
echo -e "\e[32m[步骤6/7] 安装开发工具...\e[0m"

# Python工具
python3 -m venv ~/.venv >/dev/null 2>&1
source ~/.venv/bin/activate
pip install --quiet --upgrade pip >/dev/null 2>&1
pip install --quiet jedi pylint flake8 autopep8 >/dev/null 2>&1 || echo "警告: Python工具安装失败"
deactivate

# Go工具
command -v go >/dev/null && {
    go install golang.org/x/tools/gopls@latest >/dev/null 2>&1
    go install golang.org/x/tools/cmd/goimports@latest >/dev/null 2>&1
}

# NodeJS工具
command -v npm >/dev/null && {
    npm install --silent -g typescript eslint prettier >/dev/null 2>&1 || echo "警告: NodeJS工具安装失败"
}

# 步骤7: 创建项目配置文件
echo -e "\e[32m[步骤7/7] 完成配置...\e[0m"
mkdir -p "$HOME/.vim"
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

# 完成信息
echo -e "\n\e[42m\e[30m Vim 配置成功完成！ \e[0m\e[49m"
echo ""
echo "已安装的功能:"
echo "  ✓ 现代化的界面主题 (Gruvbox)"
echo "  ✓ 智能代码补全 (Coc.nvim)"
echo "  ✓ 多语言支持 (C++/Java/Go/Python/JS/TS)"
echo "  ✓ Git版本控制集成"
echo "  ✓ 自动代码格式化"
echo ""
echo "快捷键指南:"
echo "  ,d        - 打开/关闭文件浏览器"
echo "  Ctrl+P    - 文件模糊搜索"
echo "  ,t        - 查看代码结构"
echo "  gd        - 跳转到定义"
echo "  gr        - 查找引用"
echo "  ,f        - 修复当前文件"
echo "  ,r        - 重新加载配置"
echo ""
echo "下一步:"
echo "  1. 打开一个新终端使所有更改生效"
echo "  2. 运行 'vim' 并等待插件完全安装"
echo "  3. 要添加特定语言支持:"
echo "      :CocInstall coc-java      # Java支持"
echo "      :CocInstall coc-pyright    # Python支持"
echo ""
echo "使用 :checkhealth 检查Vim状态"
echo "使用 :CocInfo 查看语言服务器信息"
