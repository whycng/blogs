好的！我来为你整理一个**完整的 VCPToolBox + VCPChat 部署教程**，你可以保存下来作为参考。

---

# VCPToolBox + VCPChat 完整部署教程

## 目录
1. [项目简介](#1-项目简介)
2. [环境准备](#2-环境准备)
3. [后端部署](#3-后端部署)
4. [前端配置](#4-前端配置)
5. [常见问题排查](#5-常见问题排查)
6. [进阶配置](#6-进阶配置)

---

## 1. 项目简介

### 项目结构
- **VCPToolBox**: 后端服务，提供 AI 对话、插件管理、WebSocket 等功能
- **VCPChat**: 前端 Electron 应用，提供用户界面

### 技术栈
- 后端：Node.js + Express + Docker
- 前端：Electron + WebSocket
- AI：支持 OpenAI/ohmygpt 等 API

---

## 2. 环境准备

### 必需软件
- **Windows 11** + **Docker Desktop**
- **Node.js** (用于前端开发)
- **Git** (可选，用于代码管理)

### 安装 Docker Desktop
1. 下载并安装 [Docker Desktop for Windows](https://www.docker.com/products/docker-desktop/)
2. 启动 Docker Desktop，等待状态变为 "Running"
3. 确保 WSL2 集成已启用（Settings → Resources → WSL Integration）

---

## 3. 后端部署

### 3.1 下载项目
```bash
# 克隆或下载 VCPToolBox 项目到本地
# 例如：E:\Proj\projGithub\VCPToolBox
```

### 3.2 配置环境变量
在 `VCPToolBox` 目录下创建 `.env` 文件：

```env
# 服务相关配置
API_Key=sk-TBxx75
API_URL=https://apic.ohmygpt.com
PORT=6005
Key=key123
Image_Key=none
VCP_Key=vcpkey123

# 调试与管理
DebugMode=False
ShowVCP=False
AdminUsername=admin
AdminPassword=123456

# 回调地址与 HTTP 访问 IP
CALLBACK_BASE_URL=http://localhost:6005/plugin-callback
VarHttpUrl=http://localhost:6005

# 插件白名单
WhitelistImageModel=gemini-2.0-flash-exp-image-generation
WhitelistEmbeddingModel=gemini-embedding-exp-03-07

# Agent 与提示词
AgentNova=Nova.txt
AgentCoco=ThemeMaidCoco.txt
TarSysPrompt="..."
VarToolList="..."
VarVCPGuide="..."
VarDailyNoteGuide="..."
VarFileTool="..."

# 系统信息变量
VarTimeNow="今天是{{Date}},{{Today}},{{Festival}}。现在是{{Time}}。"
VarSystemInfo="Windows 11"
VarCity=Beijing
VarUser=xyy
VarHome=Local development environment
VarHttpsUrl=https://your-domain.com/
VarDdnsUrl=http://your-ddns-provider.com

# 插件 API Keys（设为 none 防止报错）
WeatherKey=none
TavilyKey=none
SILICONFLOW_API_KEY=none
BILIBILI_COOKIE=none

# 多模态图像模型设置
ImageModel=gemini-2.5-flash-preview-05-20
ImagePrompt="..."
ImageInsertPrompt="..."
ImageModelOutputMaxTokens=50000
```

### 3.3 启动后端服务
在 Windows PowerShell 或 CMD 中执行：

```powershell
cd E:\Proj\projGithub\VCPToolBox
docker compose up --build -d
```

### 3.4 验证后端服务
```powershell
# 检查容器状态
docker ps

# 测试 API 接口
curl -X POST http://localhost:6005/v1/chat/completions -H "Authorization: Bearer key123" -H "Content-Type: application/json" -d "{\"model\":\"gemini-2.5-flash-preview-05-20\",\"messages\":[{\"role\":\"user\",\"content\":\"你好\"}]}"
```

### 3.5 访问管理后台
- 地址：http://localhost:6005/AdminPanel
- 用户名：admin
- 密码：123456

---

## 4. 前端配置

### 4.1 下载前端项目
```bash
# 下载 VCPChat 项目到本地
# 例如：E:\Proj\projGithub\VCPChat
```

### 4.2 安装依赖
```bash
cd E:\Proj\projGithub\VCPChat
npm install
```

### 4.3 配置前端设置
启动前端后，在全局设置中配置：

| 配置项 | 填写内容 |
|--------|----------|
| VCP服务器URL | `http://localhost:6005/v1/chat/completions` |
| VCP API Key | `key123` |
| VCPWebSocket服务器URL | `ws://localhost:6005` |
| VCPWebSocket鉴权Key | `vcpkey123` |
| 网络笔记路径 | `E:\Proj\projGithub\VCPNotes` |

### 4.4 启动前端
```bash
npm start
```

---

## 5. 常见问题排查

### 5.1 端口问题
**问题**：端口被占用或无法访问
**解决**：
```powershell
# 检查端口占用
netstat -ano | findstr 6005

# 更换端口（需要同时修改 .env 和前端配置）
# .env: PORT=6006
# 前端: VCP服务器URL = http://localhost:6006/v1/chat/completions
```

### 5.2 API 路径拼接错误
**问题**：出现 `/v1/chat/completions/v1/chat/completions` 重复路径
**解决**：
- 确保 `.env` 中 `API_URL=https://apic.ohmygpt.com`（不要带 `/v1/chat/completions`）
- 前端 VCP服务器URL 填完整路径：`http://localhost:6005/v1/chat/completions`

### 5.3 401 鉴权失败
**问题**：Bearer token required
**解决**：
- 确保前端 VCP API Key 和后端 `.env` 中的 `Key` 一致
- 检查请求头是否包含 `Authorization: Bearer key123`

### 5.4 404 模型接口错误
**问题**：`/v1/models` 返回 404
**解决**：
- 这是正常现象，因为上游 API 不支持 `/v1/models`
- 不影响主流程，可以忽略
- 如需解决，可在后端添加假数据兜底

### 5.5 Docker 环境变量不生效
**问题**：修改 `.env` 后配置未更新
**解决**：
```powershell
# 重启容器
docker compose down
docker compose up --build -d
```

---

## 6. 进阶配置

### 6.1 插件配置
在管理后台可以配置各种插件：
- 天气插件：需要配置 `WeatherKey`
- 图片生成：需要配置相关 API Key
- 日记管理：配置存储路径

### 6.2 日志调试
```env
# 启用调试模式
DebugMode=True
ShowVCP=True
```

### 6.3 自定义 Agent
- 在 `Agent/` 目录下添加 `.txt` 文件
- 在 `.env` 中配置 `AgentXXX=文件名.txt`

### 6.4 生产环境部署
- 修改 `VarHttpUrl` 为实际域名
- 配置 HTTPS 证书
- 设置防火墙规则
- 使用反向代理（如 Nginx）

---

## 7. 故障排除流程

### 7.1 系统化排查步骤
1. **检查服务状态**：`docker ps`
2. **查看容器日志**：`docker logs vcptoolbox`
3. **测试网络连通**：`curl http://localhost:6005/`
4. **验证配置**：检查 `.env` 和前端设置
5. **查看错误日志**：管理后台 → 服务器日志

### 7.2 常用调试命令
```powershell
# 查看容器状态
docker ps

# 查看容器日志
docker logs vcptoolbox

# 进入容器调试
docker exec -it vcptoolbox sh

# 重启服务
docker compose restart

# 完全重建
docker compose down
docker compose up --build -d
```

---

## 8. 总结

### 关键配置要点
1. **API_URL** 只填主机，不带路径
2. **前端 VCP服务器URL** 填完整路径
3. **Key 和 VCP_Key** 前后端保持一致
4. **每次修改配置后重启容器**

### 成功标志
- 后端：`docker ps` 显示容器运行，端口映射正常
- 前端：能正常启动，WebSocket 连接成功
- 对话：能正常发送消息并收到回复

---

**这个教程涵盖了从环境准备到故障排除的完整流程，你可以保存下来作为参考。遇到问题时，按照故障排除流程逐步排查即可！**
