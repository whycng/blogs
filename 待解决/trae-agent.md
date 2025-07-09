https://github.com/bytedance/trae-agent


 

# 🧾 Trae Agent 安装与使用流程文档（含问题排查记录）

> 本文记录了在 Ubuntu/Linux 下使用 Python venv 安装并运行 [ByteDance 的 Trae Agent 项目](https://github.com/bytedance/trae-agent) 的全过程，并在末尾分析了遇到的 API 路径兼容性问题。

---

## 📌 一、环境准备

### 1. 安装依赖（Ubuntu 环境为例）

```bash
sudo apt update
sudo apt install git python3 python3-venv python3-pip -y
```

### 2. 克隆项目

```bash
git clone https://github.com/bytedance/trae-agent.git
cd trae-agent
```

---

## 🛠 二、创建虚拟环境并安装依赖

```bash
python3 -m venv venv
source venv/bin/activate  # 激活虚拟环境

pip install -U pip
pip install -e .  # 安装 trae-agent 自身（editable 模式）
```

---

## ⚙️ 三、配置 API Key 和模型信息

创建或编辑 `trae_config.json` 文件（可放在项目根目录）：

```json
{
  "default_provider": "openai",
  "max_steps": 20,
  "enable_lakeview": true,
  "model_providers": {
    "openai": {
      "api_key": "sk-xxxxxBb2f",
      "base_url": "https://apic.ohmygpt.com/v1",
      "model": "gpt-4.1",
      "max_tokens": 128000,
      "temperature": 0.5,
      "top_p": 1,
      "max_retries": 10
    }
  }
}
```

⚠️ 注意：

* `base_url` 为第三方代理地址（如 ohmygpt）时需注意兼容性问题。
* 如用 OpenAI 官方 API，应使用 `https://api.openai.com/v1`。

---

## 🚀 四、运行 Trae Agent 执行任务

```bash
trae-cli run "Create a hello world Python script"
```
### 缺乏依赖
pip install -e . 
### 调试模式
尝试运行时加上调试信息：

<BASH>
# 设置调试环境变量
export TRAE_DEBUG=1
export OPENAI_LOG=debug
# 重新运行
trae-cli run "Create a hello world Python script"

可选参数（部分）：

```bash
-p / --provider          # 指定 LLM 提供方
-m / --model             # 模型名称
-k / --api-key           # 指定 API 密钥
-w / --working-dir       # 设置工作目录
--config-file            # 指定配置文件路径
```

输出类似：

```
╭───────────────────── Task Details ─────────────────────╮
│ Task: Create a hello world Python script              │
│ Provider: openai                                      │
│ Model: gpt-4.1                                        │
╰────────────────────────────────────────────────────────╯
```

如果任务卡住，可以 Ctrl+C 终止，此时系统会提示：

```bash
Partial trajectory saved to: trajectory_20250709_xxxxxx.json
```

---

## 🧪 五、验证 API 通信（手动 curl）

```bash
curl https://apic.ohmygpt.com/v1/chat/completions \
  -H "Authorization: Bearer sk-xxxxxBb2f" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "gpt-4.1",
    "messages": [{"role": "user", "content": "Say hello world in Python."}]
  }'
```

✅ 响应正常说明 API Key 和代理服务是通的。

---

## ⚠️ 六、问题回顾与排查：`/v1/responses` 404 错误

### ❗ 问题表现：

Trae Agent 启动任务时报错：

```bash
POST https://apic.ohmygpt.com/v1/responses → 404 Not Found
```

### 🔍 问题原因：

* `https://apic.ohmygpt.com/v1` 只支持 **OpenAI 官方标准 API 路径**。
* `/v1/responses` 不是 OpenAI 官方 API 支持的路径。
* 这是 Trae Agent 框架内部构造的私有路径，假设你使用的是原生 OpenAI 服务。

### ✅ 解决方案：

| 方法      | 描述                                                 |
| ------- | -------------------------------------------------- |
| ✅ 方法 1  | 修改 `base_url` 为 `https://api.openai.com/v1`（官方服务）  |
| ✅ 方法 2  | 修改 Trae Agent 源码，改为调用 `/v1/chat/completions` 等标准路径 |
| 🛠 方法 3 | 修改代理服务代码，mock 提供 `/responses`（不推荐）                 |

---

## 🧩 附加操作

### 查看任务轨迹文件：

```bash
ls trajectory_*.json
cat trajectory_20250709_xxxxxx.json
```

示例结构：

```json
{
  "task": "Create a hello world Python script",
  "provider": "openai",
  "model": "gpt-4.1",
  "llm_interactions": [],
  "agent_steps": [],
  "success": false
}
```

---

## ✅ 当前状态总结

| 步骤            | 状态                              |
| ------------- | ------------------------------- |
| 安装依赖、环境配置     | ✅ 成功                            |
| curl 调用 LLM   | ✅ 成功                            |
| Trae Agent 启动 | ⚠️ 成功启动但未执行，因 `/responses` 报错   |
| 问题原因          | ❌ ohmygpt 不支持非标准路径 `/responses` |
| 修复方向          | ✅ 改用官方 API 或 patch 源码           |

 
