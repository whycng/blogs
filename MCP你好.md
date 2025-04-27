随手看看到底是个啥

模型上下文协议是一种开放标准，使开发人员能够在其数据源和 AI 驱动的工具之间构建安全的双向连接。架构非常简单：开发人员可以通过 MCP 服务器公开他们的数据，也可以构建连接到这些服务器的 AI 应用程序（MCP 客户端）。

Today, we're introducing three major components of the Model Context Protocol for developers: 今天，我们将为开发人员介绍模型上下文协议的三个主要组件：

- The Model Context Protocol [specification and SDKs](https://github.com/modelcontextprotocol) 模型上下文协议[规范和 SDK](https://github.com/modelcontextprotocol)
- Local MCP server support in the [Claude Desktop apps](https://claude.ai/download) [Claude Desktop 应用程序中](https://claude.ai/download)的本地 MCP 服务器支持
- An [open-source repository](https://github.com/modelcontextprotocol/servers) of MCP servers MCP 服务器的[开源存储库](https://github.com/modelcontextprotocol/servers)

MCP 是一种开放协议，它标准化了应用程序如何为 LLM 提供上下文。将 MCP 视为 AI 应用程序的 USB-C 端口。正如 USB-C 提供了一种将设备连接到各种外围设备和配件的标准化方式一样，MCP 也提供了一种将 AI 模型连接到不同数据源和工具的标准化方式。

添加图片注释，不超过 140 字（可选）

**ai解释**：这一段是在讲 Model Context Protocol（MCP）整体的“宿主–客户端–服务器”架构，以及各方角色是怎么配合来让 AI 应用透明地拿到本地／远程数据的。拆开来说，就是：

1. 核心思想：用一套统一的轻量级 RPC（远程过程调用）协议，把各种“数据源”封装成小服务（MCP Servers），Host 程序（MCP Hosts）通过一个 MCP Client 插件，跟这些小服务对话，拿到它们提供的能力（比如读文件、查数据库、调 Web API……），再把结果交给 LLM 做后续处理。

 

2. 角色划分

   • MCP Host：你的 AI 客户端／IDE 插件／Claude Desktop 这类程序，想给模型“喂”本地文件、数据库记录或 Web 服务的数据，就把这些请求都走 MCP。

   • MCP Client：Host 和每个 Server 之间的一对一通道实现，负责把 Host 的请求按 MCP 协议打包、发给对应的 Server，也把 Server 的回复送回 Host。

   • MCP Server：小而专的进程／服务，暴露一组预定义接口（Capability），按 MCP 协议接收请求、到它所管辖的数据源（本地文件、数据库、某个云 API……）拿数据、再把结果按协议返回。

   • Local Data Source：就是你电脑上的文件、数据库、后台服务，MCP Server 直接访问它们来读写数据。

   • Remote Service：互联网或内网里的其它系统（Rest API、GraphQL、RPC 服务……），由 MCP Server 充当“中间人”去调用。

3. 整体流程（图示概念）

   Host

​     ↓（MCP Client）

   Server A —→ Local Data Source A

​     ↓

   Server B —→ Local Data Source B

​     ↓

   Server C —→ Remote Service C

1. Host 要读本地文件，就让 MCP Client 把“read_file(path)”的请求发给某个 Server A。

2. Server A 看到请求，去读你机器上的那个文件，把内容封装成 MCP 回复返回给 Host。

3. 同理，如果 Host 想调一个外部 API，就走 Server C，Server C 再调用 Remote Service C，把结果回传。

4. 为什么要这样做

   – 解耦：Host（AI 模型运行环境）不用直接管理各种数据源的授权、路径、格式，只管用 MCP 调用即可。

   – 安全：本地数据的访问权限集中在各个小 Server 上，Host/模型本身无须拥有文件系统或数据库的全局权限。

   – 可扩展：新加一个数据源，只需写一个新的 MCP Server 插件，Host 发现到就能用，无需改 Host 代码。

总的来说，这段话就是在说明：MCP 并非单体库或单一进程，而是一套分布式的、以 Host–Client–Server 为核心、可以灵活接入多种本地/远程数据源的协议架构。 Host 通过 MCP Client 与一个或多个 MCP Server 建立 1:1 连接，各 Server 再分别对接本地文件、数据库或 Web API，最终把结果统一、透明地交给想用它们的 AI 工具。

看看官方教程咋说，摘两段：

### Core MCP Concepts 核心 MCP 概念

MCP servers can provide three main types of capabilities: MCP 服务器可以提供三种主要类型的功能：

1. **Resources**: File-like data that can be read by clients (like API responses or file contents) **资源** ：客户端可以读取的类似文件的数据（如 API 响应或文件内容）
2. **Tools**: Functions that can be called by the LLM (with user approval) **工具** ：LLM 可以调用的函数（经用户批准）
3. **Prompts**: Pre-written templates that help users accomplish specific tasks **提示** ：帮助用户完成特定任务的预先编写的模板

### **您必须会 蟒 ！还有 法学硕士！！！！！**

This quickstart assumes you have familiarity with: 本快速入门假定您熟悉：

- Python 蟒
- LLMs like Claude 像 Claude 这样的法学硕士

首先，让我们安装uv并设置我们的 Python 项目和环境

那uv是啥？deepseek说是**「Python 包管理工具」，gpt说是「本地→公网隧道工具」，”**但实际上是因为**Astral 团队开发了同名但功能完全不同的两个工具**，而它们的官网路径和用途截然不同**“**

### 1. **Python 包管理器** **uv**

- **官网**：https://astral.sh/uv
- **用途**：高性能 Python 依赖管理工具（替代 pip）。
- **安装命令**： powershell 复制 下载 irm https://astral.sh/uv/install.ps1 | iex

### 2. **公网隧道工具** **uv**

- **官网**：https://uv.astral.sh（注意子域名不同）
- **用途**：轻量级反向隧道客户端（类似 ngrok）。
- **功能**：暴露本地服务到公网（HTTP/TCP/UDP）。
- **安装命令**：可能与你提到的命令相同，但需确认具体脚本来源。

那很酷了，那我上官网看看莫，https://astral.sh/uv/，

uv 紫外线

An extremely fast Python package and project manager, written in Rust.

一个速度极快的 Python 包和项目经理，用 Rust 编写。

⚡️ 比pip[快 10-100 倍](https://github.com/astral-sh/uv/blob/main/BENCHMARKS.md)。

那很酷了

那我们为什么还要使用pip？？？

然后流程是这样:

## What’s happening under the hood 幕后发生了什么

When you ask a question: 当您提出问题时：

1. The client sends your question to Claude 客户将您的问题发送给 Claude
2. Claude analyzes the available tools and decides which one(s) to use Claude 分析可用的工具并决定使用哪一个
3. The client executes the chosen tool(s) through the MCP server 客户端通过 MCP 服务器执行所选工具
4. The results are sent back to Claude 结果将发送回给 Claude
5. Claude formulates a natural language response Claude 制定自然语言响应
6. The response is displayed to you! 响应将向您显示！

代码细节让ai总结下：

好的，我用更清晰的方式梳理整个流程，并明确每个组件的角色和调用关系。以下是一个分步的、角色化的解释：

### **1. 组件角色说明**

| 组件/角色          | 作用                                                         |
| ------------------ | ------------------------------------------------------------ |
| uv (Python版)      | Astral 开发的包管理工具，替代 pip，负责创建环境、安装依赖。  |
| MCP框架            | 一个协议化工具框架，通过装饰器和类型提示自动生成工具接口。   |
| FastMCP            | MCP 的快速实现类，简化服务器开发（示例中的 mcp = FastMCP("weather")）。 |
| NWS API            | 美国国家气象局提供的免费天气数据接口。                       |
| Claude for Desktop | 假设的MCP客户端工具，用于调用你编写的天气服务（文中提到测试用）。 |

### **2. 完整流程与调用关系**

### **阶段1：环境准备**

1. **安装** **uv**  irm https://astral.sh/uv/install.ps1 | iex

- 作用：安装 Python 包管理器 uv（类似 pip，但更快）。

1. **初始化项目**  uv init weather  # 创建项目目录 cd weather       # 进入目录 uv venv          # 创建虚拟环境 .\.venv\Scripts\activate  # 激活环境（Windows）

- 角色：uv 负责管理项目环境和依赖。

1. **安装依赖**  uv add mcp[cli] httpx

- 安装的包：  
- mcp[cli]：MCP框架的核心库。  
- httpx：异步HTTP客户端（用于请求NWS API）。

### **阶段2：编写服务逻辑**

**文件：****weather.py** 

```
from mcp.server.fastmcp import FastMCP
import httpx

# 初始化MCP服务器
mcp = FastMCP("weather")

# 定义工具1：获取天气警报
@mcp.tool()
async def get_alerts(state: str) -> str:
    url = f"https://api.weather.gov/alerts/active/area/{state}"
    data = await httpx.get(url)  # 调用NWS API
    return "处理后的警报信息"

# 定义工具2：获取天气预报
@mcp.tool()
async def get_forecast(lat: float, lon: float) -> str:
    url = f"https://api.weather.gov/points/{lat},{lon}"
    data = await httpx.get(url)  # 调用NWS API
    return "处理后的预报信息"

# 启动服务器
if __name__ == "__main__":
    mcp.run(transport='stdio')
```

**调用关系**：  

1. 用户通过客户端（如Claude）调用工具（如 get_alerts("CA")）。  
2. MCP框架解析请求，执行对应的 @mcp.tool 函数。  
3. 函数内部通过 httpx 请求 **NWS API** 并返回处理后的数据。  

### **阶段3：运行与测试**

1. **启动服务**  uv run weather.py

- 角色：uv 启动Python脚本，MCP服务开始监听请求。

1. **客户端调用** 

- 假设客户端是 Claude for Desktop，它会通过MCP协议发送请求（如请求加州的天气警报）。  
- 服务收到请求后，调用 get_alerts("CA")，返回格式化结果。

### **3. 数据流图示**

```
用户/客户端
  │
  ↓  (MCP协议请求，如get_alerts)
FastMCP服务器 (weather.py)
  │
  ↓  (调用NWS API)
NWS Weather API
  │
  ↓  (返回JSON数据)
FastMCP服务器 (处理数据)
  │
  ↓  (返回格式化结果)
用户/客户端
```

### **4. 关键点总结**

1. **uv** **的作用** 

- 仅用于环境管理（安装依赖、创建虚拟环境），不参与服务运行时逻辑。

1. **MCP框架的作用** 

- 提供工具协议化能力：通过 @mcp.tool 装饰器将函数暴露为可调用的接口。

1. **NWS API 的调用** 

- 由 httpx 在工具函数内部直接调用，与服务框架解耦。

1. **客户端交互** 

- 客户端通过MCP协议与 weather.py 通信，无需关心内部实现。

### **5. 类比理解**

- **uv** ≈ 更快的 pip（只管装包）。  
- **MCP** ≈ 一个自动生成API的框架（类似FastAPI的简化版）。  
- **NWS API** ≈ 第三方数据源（类似调用支付宝的天气接口）。  
- **客户端** ≈ 使用你服务的APP（如微信小程序调用你的后端）。  

这样是否更清晰了？如果有具体环节仍不清楚，可以针对性提问！

haod e,那么我们已经完全明白了这是怎么回事了，可是让为什么叫上下文协议呢？

旨在统一大型语言模型（LLM）与外部数据源和工具之间的“上下文”交换，而不仅仅是简单的 API 调用。MCP 定义了资源（Resources）、提示词（Prompts）、工具（Tools）、传输层（Transports）等核心组件，使得模型在处理任务时，能够持续管理并传递当前会话所需的上下文信息，而不仅仅是单次的函数调用[知乎专栏](https://zhuanlan.zhihu.com/p/27327515233?utm_source=chatgpt.com)[知乎专栏](https://zhuanlan.zhihu.com/p/29001189476?utm_source=chatgpt.com)。

## MCP 是什么？

-  **开放标准协议**：MCP 由 Anthropic 发布，取名“模型上下文协议”，旨在打破 AI 应用与数据源或工具之间的碎片化集成，实现类似 USB-C 接口的“即插即用”体验[博客园](https://www.cnblogs.com/shanren/p/18797313?utm_source=chatgpt.com)。 
-  **不止调用工具**：除了让模型能调用外部 API，MCP 还负责上下文的动态注入、管理与分发，确保模型和工具在多轮对话或复杂任务中共享相同的环境状态[腾讯云 - 产业智变 云启未来](https://cloud.tencent.com/developer/article/2505540?utm_source=chatgpt.com)。 

## “上下文协议” 名称含义

-  **Context = 上下文信息**：MCP 中的“Context”不仅包含一次调用所需的参数，还涵盖会话历史、工具配置、资源权限等多维度信息。它规定了如何把这些上下文打包、传输并由模型与工具端共同解析，以保持整个系统对话或任务执行的连贯性[知乎专栏](https://zhuanlan.zhihu.com/p/29001189476?utm_source=chatgpt.com)。 
-  **超越函数调用**：传统的 API 调用协议只关注请求—响应；而 MCP 则是一个“上下文层”，类似于网络中的 HTTP+Cookies，既要保证数据交互，又要维护状态与安全性[GitHub](https://github.com/liaokongVFX/MCP-Chinese-Getting-Started-Guide?utm_source=chatgpt.com)。 

## 为什么不叫“调用工具协议”？

-  **更宽的职责边界**：工具调用只是 MCP 的一个子功能。MCP 同时承担上下文注册、缓存、优先级调度和安全认证等职责，是一个真正的“中间件”层，而非仅仅是工具接口的封装[腾讯云 - 产业智变 云启未来](https://cloud.tencent.com/developer/article/2505540?utm_source=chatgpt.com)。 
-  **客户端-服务器架构**：MCP 定义了 Host（宿主应用）与 Server（工具提供方）之间的通信规范，支持多服务器并行连接，能在不同数据源之间切换上下文，更像是分布式系统中的“会话管理协议”，而非单纯的 API 触发机制[GitHub](https://github.com/liaokongVFX/MCP-Chinese-Getting-Started-Guide?utm_source=chatgpt.com)。 
-  **生态互操作性**：越来越多的第三方平台开始支持 MCP，使得开发者“一次集成、多处可用”，无需为每个工具单独编写适配层，这正是“上下文协议”名称所要强调的：标准化上下文交换，以实现跨系统互操作性[Vaayne's Tea House](https://vaayne.com/posts/2025/mcp-guide/?utm_source=chatgpt.com)。 

## 实际应用举例

-  **Claude Desktop 演示**：Anthropic 在发布时演示了通过 MCP 连接 GitHub，AI 模型能够创建仓库、提交 Pull Request，并在整个流程中动态维护会话状态（如用户权限、PR 历史等），凸显了上下文管理的重要性，而不仅是单次调用接口[The Verge](https://www.theverge.com/2024/11/25/24305774/anthropic-model-context-protocol-data-sources?utm_source=chatgpt.com)。 
-  **APISIX-MCP 案例**：在 APISIX-MCP 项目中，MCP 将自然语言指令转换为 API 管理操作，并在执行过程中携带认证信息、操作日志、上下文快照等，为运维场景提供了可靠的上下文感知能力，超越了传统的 REST 调用方式[Apache APISIX](https://apisix.apache.org/zh/blog/2025/04/01/embrace-intelligent-api-management-with-ai-and-mcp/?utm_source=chatgpt.com)。 

## 小结

将 MCP 称为“上下文协议”是因为它关注的是整个 AI 系统在多轮对话或复杂任务执行过程中的“上下文”管理，而不仅仅是一次性“调用工具”。通过定义统一的上下文交换格式和传输机制，MCP 为 AI 应用提供了一个可扩展、安全、可审计的中间层，支撑未来更智能、更自主的 Agent 化生态发展

\--- 

找个 [Example Servers - Model Context Protocol](https://modelcontextprotocol.io/examples) 看看

[servers/src/puppeteer at main · modelcontextprotocol/servers](https://github.com/modelcontextprotocol/servers/tree/main/src/puppeteer) 

来，看看代码 [servers/src/puppeteer/index.ts at main · modelcontextprotocol/servers](https://github.com/modelcontextprotocol/servers/blob/main/src/puppeteer/index.ts) 

对对对，**你贴的这段代码**，确实就是 mcp-servers/puppeteer 的**核心实现**！ 我给你拆解一下它到底是怎么运行的，能帮你快速掌握整个结构：

## 🧩 整体结构总结

### 1. 启动一个 MCP Server

-  用 @modelcontextprotocol/sdk/server 提供的 Server 类。 
-  用 StdioServerTransport —— 走的是**标准输入输出**（Stdin/Stdout）作为通信管道，不是 HTTP。 
-  等于是开了一个 MCP Server，模型（Agent）可以跟它用 MCP 标准协议聊天。 

```
ts
复制编辑
const server = new Server({...})
await server.connect(new StdioServerTransport());
```

### 2. 注册 MCP的标准接口

MCP标准有几种请求，比如：

-  ListResources：列出资源 
-  ReadResource：读取某个资源 
-  ListTools：列出工具 
-  CallTool：调用工具 

这个 puppeteer server 给它们都挂了自己的处理器（setRequestHandler）：

```
ts
复制编辑
server.setRequestHandler(ListToolsRequestSchema, async () => { tools: TOOLS })
server.setRequestHandler(CallToolRequestSchema, async (req) => handleToolCall(req.params.name, req.params.arguments))
server.setRequestHandler(ListResourcesRequestSchema, ...)
server.setRequestHandler(ReadResourceRequestSchema, ...)
```

### 3. 定义了浏览器控制的**工具集 (TOOLS)**

**一个 MCP Tool = 一个 JSON Schema描述的动作**，比如：

| MCP工具名            | 做什么       | 参数                              |
| -------------------- | ------------ | --------------------------------- |
| puppeteer_navigate   | 打开网址     | { url, launchOptions }            |
| puppeteer_screenshot | 截图         | { name, selector, width, height } |
| puppeteer_click      | 点击元素     | { selector }                      |
| puppeteer_fill       | 填输入框     | { selector, value }               |
| puppeteer_select     | 选下拉框     | { selector, value }               |
| puppeteer_hover      | 悬停元素     | { selector }                      |
| puppeteer_evaluate   | 执行 JS 脚本 | { script }                        |

这些都封装到 TOOLS: Tool[] 里了，供 MCP Server 返回给客户端（比如大模型）。

### 4. 处理指令：handleToolCall

**大模型发来指令，走到这里**：

-  根据指令名(name)和参数(args)去执行对应 Puppeteer 的操作。 
-  先确保有浏览器开着（ensureBrowser） 
-  然后比如：调用 page.goto(url)、page.screenshot()、page.click(selector)等等。 

核心分发逻辑长这样：

```
ts
复制编辑
switch (name) {
  case "puppeteer_navigate": ...
  case "puppeteer_screenshot": ...
  case "puppeteer_click": ...
  ...
}
```

每种 case 都返回 MCP标准的 CallToolResult：

-  content: [{type: "text", text: "xxx"}] 
-  isError: true/false 

### 5. 资源管理：截图和控制台日志

它还维护了一些**资源**，可以被读取，比如：

-  console://logs 浏览器里的 console 日志 
-  screenshot://xxx 保存的截图 

存储在：

-  consoleLogs 数组 
-  screenshots Map结构 

资源可以被 ListResources 和 ReadResource 请求访问。

## 🔥 总结它的**核心工作流**

1.  启动 MCP Server（用 stdio 通信） 
2.  大模型发来 "列出可用工具" 请求 → 返回 puppeteer 工具列表 
3.  大模型选择一个工具，比如 puppeteer_navigate 
4.  MCP Server 收到 CallTool 请求 → 调用 Puppeteer，控制浏览器执行动作 
5.  把执行结果通过 MCP 协议返回大模型 
6.  可以连续发多条指令，复用浏览器 session（上下文！） 

## 🚀 额外亮点

-  **安全性检测**：防止直接允许 --no-sandbox 之类不安全参数，除非你设置 allowDangerous: true 
-  **环境配置**：支持用环境变量 PUPPETEER_LAUNCH_OPTIONS 配置浏览器启动参数 
-  **自动日志捕获**：通过 override 浏览器的 console 方法，把页面的 console.log 自动收集到 MCP资源里 

## 你的问题直接回答：

>  怎么个事？ 

✅ 这是一个 MCP Server，用 Puppeteer 封装了一组浏览器自动化操作， ✅ 通过 MCP标准协议把它们暴露出去，让大模型可以远程指挥浏览器干活。 ✅ 上下文指的是维护了浏览器 session，不是一条指令干完就关掉，而是能连续指挥。

那我大概明白了，你们明白了吗？

reference:[For Server Developers - Model Context Protocol](https://modelcontextprotocol.io/quickstart/server) 
