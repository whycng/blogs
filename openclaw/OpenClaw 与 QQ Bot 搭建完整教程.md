# OpenClaw 与 QQ Bot 搭建完整教程

本文旨在记录如何在 WSL2 Ubuntu 环境下搭建 OpenClaw 并与 QQ Bot 进行打通。

## 前置环境

- 操作系统：Windows 11（使用 WSL2 运行 Ubuntu 24）
- 开发工具：VS Code（通过远程连接到 WSL）
- 代理软件：需开启局域网连接模式（允许其他计算机通过代理访问网络）

## 第一部分：安装 OpenClaw

### 1. 下载并安装 OpenClaw

访问官方文档：https://docs.openclaw.ai/zh-CN/install

在终端执行一键安装命令：
```bash
curl -fsSL https://openclaw.ai/install.sh | bash
```

### 2. 配置网络代理

编辑 `~/.bashrc` 文件（该文件位于用户主目录下）：
```bash
nano ~/.bashrc
```

在文件末尾添加以下代理配置（根据你的代理软件实际端口修改）：
```bash
export http_proxy=http://127.0.0.1:7890
export https_proxy=http://127.0.0.1:7890
export all_proxy=http://127.0.0.1:7890
```

保存后，执行以下命令使配置生效：
```bash
source ~/.bashrc
```

### 3. 处理 Node.js 安装问题

在安装过程中，Node.js 可能因网络问题导致安装失败。有以下两种解决方案：

**方案 A：使用 NVM 安装 Node.js**
```bash
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
nvm install node
```

**方案 B：配置 Node.js 国内镜像源**

在 `~/.npmrc` 文件中添加国内镜像源：
```bash
registry=https://registry.npmmirror.com
```

如果仍然无法安装，可以给 npm 的系统配置文件添加代理设置。编辑 `/etc/npm/npmrc` 或全局 npm 配置文件，添加 proxy 和 https-proxy 配置。

### 4. 初始化 OpenClaw 配置

安装完成后，OpenClaw 会提示你进行初始化配置。根据以下指引完成：

**选择大模型提供商：**
- 选择 `custom`（自定义）或 `deepseek`
- 提供你的 API Key
- 输入模型名称（例如：`deepseek-v4-flash`）

**跳过插件和技能安装：**
- 在提示中遇到 Skills、Plugins 等选项时，一律选择 `skip`
- 暂时不安装任何额外的插件，避免配置复杂化

系统会进行一段时间的初始化和下载，请耐心等待。

### 5. 启动 OpenClaw 网页界面

首先，查看 OpenClaw 的运行状态：
```bash
openclaw gateway status
```

启动 Gateway 服务（可以看到实时日志）：
```bash
openclaw gateway run
```

在另一个终端窗口执行：
```bash
openclaw dashboard
```

系统会自动将访问链接复制到剪贴板，在你的本机浏览器中打开该链接即可进入 OpenClaw 的 Web UI。

> **提示**：如果没有自动复制链接，可能是 WSL 与本机剪贴板未互通。需要在系统设置中启用相关功能或安装剪贴板同步工具。

## 第二部分：配置 OpenClaw 人设

### 1. 完善 Agent 人设

打开 Web UI 后，你可以在界面中进行简单的对话测试。此时 OpenClaw 会提示你完善 Agent（代理）的人设配置。

人设文件位于：`~/.openclaw/workspace/` 目录下

**核心人设文件说明：**

| 文件名 | 说明 |
|--------|------|
| `SOUL.md` | Agent 的灵魂和核心价值观定义 |
| `AGENT.md` | Agent 的具体行为规则和能力定义 |
| `IDENTITY.md` | Agent 的身份和角色设定 |
| `bootstrap.md` | 启动时的初始化配置（初学者可以删除） |

### 2. 参考优秀实例

如果不知道如何编写这些配置文件，可以参考其他开发者的配置：
- 访问 GitHub: https://github.com/search?q=awesome-openclaw-agent
- 浏览别人的 Agent 人设写法，学习基本结构

### 3. 快速配置方案（初学者推荐）

为了快速开始，可以采用以下简化方案：
1. 删除 `bootstrap.md` 文件
2. 对于 `SOUL.md`、`AGENT.md`、`IDENTITY.md`，直接在 Web UI 中与 AI 对话，让它帮你快速完成初始化内容
3. 根据后续测试效果进行微调

### 4. 应用配置更改

完成人设修改后，需要重启 Gateway 使配置生效。在运行 Gateway 的终端执行：
```bash
# 先停止现有的 Gateway
Ctrl+C

# 或者在另一个终端执行
openclaw gateway stop
```## 第三部分：验证并配置模型

### 1. 检查配置文件

停止 Gateway 后，检查并验证 OpenClaw 的配置文件：`~/.openclaw/openclaw.json`

该文件应包含以下两个关键配置部分：

**Agent 配置部分：**
```json
"agents": {
  "defaults": {
    "workspace": "/home/xyy/.openclaw/workspace",
    "model": {
      "primary": "custom-api-ohmygpt-com/deepseek-v4-flash"
    },
    "models": {
      "custom-api-ohmygpt-com/deepseek-v4-flash": {}
    }
  }
}
```

**模型提供商配置部分：**
```json
"models": {
  "mode": "merge",
  "providers": {
    "custom-api-ohmygpt-com": {
      "baseUrl": "https://api.ohmygpt.com/v1",
      "api": "openai-completions",
      "apiKey": "sk-xxxxx",
      "models": [
        {
          "id": "deepseek-v4-flash",
          "name": "deepseek-v4-flash (Custom Provider)",
          "contextWindow": 1000000,
          "maxTokens": 384000,
          "input": [
            "text"
          ],
          "cost": {
            "input": 0,
            "output": 0,
            "cacheRead": 0,
            "cacheWrite": 0
          },
          "reasoning": false
        }
      ]
    }
  }
}
```

### 2. 关键配置参数说明

| 参数 | 说明 | 修改建议 |
|------|------|--------|
| `baseUrl` | API 服务的基础 URL | 根据你使用的 API 提供商修改 |
| `apiKey` | 你的 API 密钥 | 替换为真实的 API Key（注意保管好这个值） |
| `contextWindow` | 模型支持的上下文窗口大小 | 根据模型实际能力修改，单位为 token |
| `maxTokens` | 单次请求的最大输出 token 数 | 根据模型限制和需求调整 |

### 3. 重启 OpenClaw 服务

配置完成后，重新启动 Gateway：
```bash
openclaw gateway run
```

返回 Web UI 刷新页面（无需再次运行 `openclaw dashboard`），等待服务恢复到正常状态即可继续使用。

## 第四部分：集成 QQ Bot

### 1. 官方 QQ Bot 的局限性

OpenClaw 官方提供了 QQ Bot 插件，但目前功能有限：

⚠️ **重要限制：**
- 官方机器人**暂不支持群聊**
- 仅支持私聊功能
- 功能和稳定性持续改进中

详细讨论可参考：
- 官方网页讨论
- QQ Bot 官方的 QQ 频道讨论

相关安装教程：https://github.com/BytePioneer-AI/openclaw-china

### 2. 使用第三方 QQ Bot 方案

由于官方功能限制，许多用户选择使用第三方解决方案来实现群聊功能。虽然有一定的封号风险，但这是实现完整功能的一个有效途径。

**推荐的第三方方案：**

| 方案 | 项目链接 | 说明 |
|------|--------|------|
| **NapCat**（推荐） | https://napneko.github.io/ | 轻量级、功能完整 |
| AstroBot | https://docs.astrbot.app/platform/aiocqhttp.html | 功能丰富、文档完善 |

本教程采用 NapCat 方案，使用 `openclaw-onebot` 插件进行集成。

### 3. 核心原理简介

集成流程：
1. **NapCat** 接管 QQ 账号，运行一个 WebSocket 服务端
2. **OpenClaw** 通过 `openclaw-onebot` 插件，配置转发连接到 NapCat 的 WS 服务
3. 当 QQ 接收到消息时，NapCat 将其转发给 OpenClaw
4. OpenClaw 使用 Agent 人设进行回复，响应内容返回给 QQ

### 4. 安装 OpenClaw OneBot 插件

执行以下命令安装插件（注意必须包含 `--dangerously-force-unsafe-install` 参数）：

```bash
openclaw plugins install @kirigaya/openclaw-onebot --dangerously-force-unsafe-install
```

> **重要**：`--dangerously-force-unsafe-install` 参数是必须的，即使是官方 QQ Bot 插件也存在同样的要求。

**参考资源：**
- 插件项目：https://github.com/LSTM-Kirigaya/openclaw-onebot
- 详细教程：https://kirigaya.cn/blog/article?seq=368

### 5. 配置 OpenClaw OneBot 插件

插件安装完成后，执行配置命令：

```bash
openclaw onebot setup
```

按照提示进行配置：

| 配置项 | 值 | 说明 |
|-------|-----|------|
| 连接类型 | `forward`（正向代理） | OpenClaw 作为客户端连接到 NapCat 服务器 |
| 服务器地址 | `127.0.0.1` | NapCat 运行的本地地址 |
| 服务器端口 | `3001` | NapCat WS 服务的端口号 |
| Access Token | `qqbotuse` | 访问令牌（需与 NapCat 端设置一致） |

配置完成后，重启 OpenClaw Gateway：
```bash
openclaw gateway restart
```

### 6. 下载并配置 NapCat

**下载 NapCat：**
访问 https://napneko.github.io/ 下载桌面端应用

**启动和登录：**
1. 运行 NapCat 桌面应用
2. 使用 QQ 二维码扫码登录

**获取 WebUI 访问信息：**
查看文件：`...\NapCat.Shell\config\webui.json`（Windows 用户）
或者 Linux 用户查看相应的配置目录

从配置文件中获取：
- 本地访问端口
- WebUI 访问 Token

### 7. 在 NapCat 中配置 WebSocket 服务

打开 NapCat 的 WebUI（通过浏览器访问本地端口），进行以下配置：

**设置 WebSocket 服务端：**

| 配置项 | 值 | 说明 |
|-------|-----|------|
| 监听地址 | `0.0.0.0` | 允许来自所有网络接口的连接 |
| 监听端口 | `3001` | 与 OpenClaw 配置的端口保持一致 |
| Access Token | `qqbotuse` | 与 OpenClaw 配置的 Token 保持一致 |

配置完成后启用 WebSocket 服务。

### 8. 验证连接状态

完成以上配置后，验证两边的连接：

**检查 NapCat 日志：**
```
WebSocket 服务监听成功：0.0.0.0:3001
```

**检查 OpenClaw Gateway 日志：**
```
[plugins] OneBot 插件启动成功
Forward 代理连接成功
```

### 9. 测试 QQ Bot 功能

现在可以对集成好的 QQ 机器人进行测试：

1. 使用另一个 QQ 账号给你的 OpenClaw 控制的 QQ 账号发送消息
2. 观察 OpenClaw Gateway 的日志输出，你会看到：
   - 插件系统开始处理消息
   - Agent 人设开始工作
   - 系统提示词被加载和执行
3. QQ 的消息会显示一个**咖啡标志**（⏳ 等待标记），表示机器人正在处理
4. 稍等片刻，你的 OpenClaw Agent 会给出回复

### 常见问题

**Q: OneBot 插件安装时总是失败？**
A: 确保使用了 `--dangerously-force-unsafe-install` 参数。

**Q: NapCat 的 QQ 号显示离线？**
A: 检查 WebSocket 服务是否已启动，以及防火墙设置是否允许 3001 端口的通信。可以使用powershell测试3001端口的tcp连通性 

**Q: OpenClaw 收不到 QQ 消息？**
A: 检查 Gateway 日志，确认 OneBot 插件已成功连接到 NapCat，以及访问令牌是否匹配。

**Q: 消息在 QQ 上显示咖啡标志但一直不回复？**
A: 检查 OpenClaw 的 Agent 配置和模型 API 是否正常工作，查看 Web UI 中的对话历史。

### 其余注意事项

如果你先安装了官方qq bot，再安装ontbot插件，两者工作时会发生冲突，因为他们对某些项使用了同样的命名定义。

onebot插件设置的一定是forward，napcat一定设置的是ws的服务端。并且两者的accseetoken都是相同的，注意，这个accesstoken不是napcat网页管理端的token，这个token是其目录下config\webui.json使用的

onebot是127.0.0.1，而napcat是0.0.0.0

openclaw安装完之后先跑起来对话一次，确认没问题，可以在gateway看它经过了哪些阶段，这对后续调试qq bot有用（虽然按理说应该一次成功，但保不准哪里又会出幺蛾子，像我一样的萌新瞎测(*Φ皿Φ*)）





 




