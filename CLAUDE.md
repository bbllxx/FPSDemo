# FPSDemo - Claude Code 项目规则

UE5.4 FPS 僵尸游戏项目，已实现僵尸系统 C++ 代码。

---

## 工作原则

### 不偷懒原则
在用户询问"怎么做"或"需要做什么"时，**先判断我是否有能力/权限直接执行**，而不是直接说"你需要先XXX"把任务推回给用户。如果能执行就自己执行，如果不能才说明原因和替代方案。

### 同意阈值
- **可直接执行：** 读操作、搜索、导航、查询等不会修改文件的操作，包括但不限于：
  - 文件查看：`cat`, `head`, `tail`, `less`, `more`, `stat`, `ls`, `pwd`, `tree`, `wc`
  - 内容搜索：`grep`, `rg`, `find`, `which`, `type`, `where`
  - Git 只读：`git status`, `git log`, `git diff`, `git show`, `git branch`, `git remote -v`, `git fetch`, `git ls-files`, `git describe`, `git rev-parse`
  - 系统信息：`ps`, `top`, `df`, `du`, `free`, `uname`, `env`, `whoami`, `netstat`, `ss`, `curl -I`, `ping`, `nslookup`, `traceroute`
  - 包管理查询：`npm list`, `pip list`, `cargo tree`, `docker ps`, `docker images`
  - 目录切换：`cd`（切换工作目录，不修改文件）
  - **MCP 工具：** `WebSearch`（联网搜索）、`Grep`（代码搜索）、`Glob`（文件查找）、`WebFetch`（获取网页内容）、`ReadMcpResourceTool`（读取 MCP 资源）、`Read`（读取本地文件）
  - **UE 源码查询：** 查看 UE 引擎源码（`E:\UE_5.4`）为读操作，无需授权
- **记忆与计划文件：** 对 `.claude/` 目录下的记忆文件（memory/）及 plans 文件进行读写操作时，不必询问，直接执行

---

## 回复风格

- **所有回答必须使用中文（简体）**，不得使用英文
- 多元选择时，使用 `AskUserQuestion` 工具展示是/否选项，而不是开放式的提问

---

## 代码注释规范

- **所有代码注释必须使用中文**
- 修改代码时，将原有的英文注释翻译为中文

---

## Git 提交规范

**提交范围：**
- 包含：`Source/` 目录下所有文件
- 包含：项目根目录下的非文件夹文件（如 `.gitignore`, `.mcp.json`, `.uproject`, `.sln`, `.vsconfig` 等）
- 排除：`.vscode/`, `Backup/`, `Config/`, `Content/`, `Plugins/` 等文件夹

---

## 搜索与 Skill

- **搜索：** 使用 `open-websearch search --engine <引擎> "<关键词>"` CLI 执行，不用 MCP WebSearch 工具
- **查找 Skill：** 先用 `find-skills` skill 搜索，本地找不到再上网搜索

---

## 项目信息

- **UE5 源码路径：** `E:\UE_5.4\Engine\Source\Runtime\Engine`

---

## 编译命令

**UE5 项目编译（使用 UnrealBuildTool）：**
```bash
"E:/UE_5.4/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe" FPSDemoEditor Win64 Development -Project="$PWD/FPSDemo.uproject" -WaitMutex -NoHotReload
```

> 此命令直接编译 C++ 代码为 DLL，不依赖 Visual Studio IDE。
