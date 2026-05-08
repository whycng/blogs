# QQ Bot 数字生命体 — 改造全记录

> 创建: 2026-05-08
> 用途: 记录从"被 @ 才回复"的被动 Bot → 拥有自主行为能力的群聊 AI 的完整改造过程
> 涉及: openclaw-onebot 插件 / bot/ 人格系统 / OpenClaw cron

---

## 目录

1. [第一阶段：发图 Bug 修复](#第一阶段发图-bug-修复)
2. [第二阶段：人格系统搭建](#第二阶段人格系统搭建)
3. [第三阶段：Cron Job 故障修复](#第三阶段cron-job-故障修复)
4. [第四阶段：命令处理链路改造](#第四阶段命令处理链路改造)
5. [最终架构总览](#最终架构总览)
6. [关键技术决策](#关键技术决策)
7. [踩坑清单](#踩坑清单)

---

## 第一阶段：发图 Bug 修复

### 问题描述

QQ Bot 发送图片时，日志显示 `retcode=0` `message_id=xxx` 但图片发不出来。

### 环境拓扑

```
Lagrange (QQ 客户端) → 跑在 Windows 宿主机
OpenClaw + OneBot 插件 → 跑在 WSL2 (Linux)
WebSocket 连接
```

### 根因

`connection.js` 中发送图片时，传的是 WSL2 的绝对路径，如 `/home/xyy/.../oil_new_gold.png`。Lagrange 在 Windows 上跑，不认识这个 Linux 路径。

### 修复

修改 `openclaw-onebot/dist/connection.js`：

- 检测 OneBot 是否在同一台机器（回环连接 vs 跨机器）
- **跨机器（WSL2 → Windows）**：文件读取后转成 `base64://` 格式发送
- **本机回环**：保留原有裸路径或 URL 逻辑

### 涉及文件

```
/home/xyy/.openclaw/extensions/openclaw-onebot/dist/connection.js
```

---

## 第二阶段：人格系统搭建

### 设计目标

从被动响应升级为主动行为，具备：
- 自主发言（定时器触发）
- 随机插嘴（关键词命中）
- 人格状态机（mood 切换）
- 群聊记忆（话题/用户追踪）
- 开关控制（`/auto on/off`）
- 夜间模式（1:00-5:00 自动切换）
- 低打扰非客服感

### 系统架构

```
workspace/bot/
├── persona/
│   ├── state.json              ← 运行时人格状态
│   ├── moods.json               ← 7 种 mood 定义
│   └── state-manager.js        ← 状态读写/能量衰减/夜间模式
├── behavior.js                  ← 🧠 行为决策引擎
├── memory.js                    ← 话题/用户追踪
├── memory/                      ← 持久化记忆目录
├── auto-speak.mjs               ← 自主发言脚本（对接 onebot）
├── brain-prompt.md              ← Agent 人格上下文注入
├── cron-bootstrap.mjs           ← 定时任务安装指南
└── persona-command.mjs          ← 群命令处理器
```

### 技术细节

#### state.json 关键字段

| 字段 | 类型 | 说明 |
|------|------|------|
| `mood` | string | 当前人格状态 |
| `energy` | float 0-1 | 能量值，越高越活跃 |
| `talkative` | float 0-1 | 话痨度 |
| `auto_enabled` | bool | 自主发言开关 |
| `respond_all` | bool | 接管模式（回复全部消息） |
| `night_mode` | bool | 夜间模式 |
| `last_active_time` | timestamp | 最后活跃时间 |
| `cooldown_until` | timestamp | 发言冷却截止时间 |
| `speak_count_today` | int | 今日发言次数 |

#### Mood 类型

| mood | 风格 |
|------|------|
| `normal` | 理性、轻松、日常 |
| `sleepy` | 简略、迷糊、句号多 |
| `crazy` | 跳跃、发散、emoji 轰炸 |
| `observer` | 一针见血观察结论 |
| `economic_doom` | 悲观经济学锐评 |
| `tsundere` | 教科书式傲娇 |
| `silent` | 不说话 |

#### Cron Job 配置

```json
// 人格能量 tick — 每 5 分钟
{
  "id": "89e42521-07a9-4372-8819-786d66edb4bd",
  "name": "persona-tick",
  "everyMs": 300000,
  "payload": {
    "kind": "agentTurn",
    "message": "【人格 tick】执行能量更新...",
    "timeoutSeconds": 60
  },
  "delivery": { "mode": "none" }
}

// 自主发言检查 — 每 25 分钟
{
  "id": "582c6dd1-1ba9-4b6a-a604-66a26be19bb1",
  "name": "auto-speak-check",
  "everyMs": 1500000,
  "payload": {
    "kind": "agentTurn",
    "message": "【自主发言检查】...",
    "timeoutSeconds": 120
  },
  "delivery": { "mode": "none" }
}
```

### 涉及文件（全部 workspace/bot/）

```
bot/persona/state.json
bot/persona/moods.json
bot/persona/state-manager.js
bot/behavior.js
bot/memory.js
bot/auto-speak.mjs
bot/brain-prompt.md
bot/cron-bootstrap.mjs
bot/persona-command.mjs
```

---

## 第三阶段：Cron Job 故障修复

### 问题 1：persona-tick 投递失败

**现象**：cron 执行成功，但 delivery 阶段报错：
```
OneBot requires --to <user_id|group_id>
```

**根因**：`delivery: { mode: "announce" }` 且无 `to` 目标。cron 尝试 announce 到 OneBot 的 "last" channel，但 OneBot 必须指定群号/用户号。

**修复**：`delivery` → `{ "mode": "none" }`。persona-tick 只是内部状态更新，不需要发消息。

### 问题 2：两个 cron job 均超时

**现象**：
```
persona-tick:   timeoutSeconds=30, durationMs=32s → 超时
auto-speak-check: timeoutSeconds=60, durationMs=68s → 超时
```

**根因**：agentTurn 的 timeoutSeconds 设太短，模型处理 + 工具调用超出时限。

**修复**：
- persona-tick: `30s → 60s`
- auto-speak-check: `60s → 120s`
- 同步精简了 prompt 文本减少模型耗时

### cron job 修复命令

```bash
openclaw cron update --id 89e42521-... --patch '{"delivery":{"mode":"none"},"payload":{"timeoutSeconds":60}}'
openclaw cron update --id 582c6dd1-... --patch '{"payload":{"timeoutSeconds":120}}'
```

---

## 第四阶段：命令处理链路改造

### 背景

最初命令处理完全依赖 Agent（LLM）主动识别群消息中的命令并执行 `persona-command.mjs`。但 LLM 行为不稳定，经常选择性忽略命令。

### 改造方案

**在插件层（process-inbound.js）做前置命令拦截**，消息到 Agent 之前就检查是否是人格命令，是则直接执行并回复，不走 LLM。

### 改动涉及的 Bug 链（踩坑最多的一段）

#### 🐛 Bug 1：behavior.js import 路径错误

**文件**：`workspace/bot/behavior.js` 第 12 行

**现象**：@Bot 时日志报错 `[tools] read failed: ENOENT: no such file or directory, access '.../bot/state-manager.js'`

**根因**：
```js
// ❌ 错误
import { ... } from "./state-manager.js";
// 文件实际在 ./persona/state-manager.js
```

**修复**：
```js
// ✅ 正确
import { ... } from "./persona/state-manager.js";
```

---
*(以下 Bug 发生在 process-inbound.js 的命令拦截代码中)*

#### 🐛 Bug 2：process.cwd() 不等于 workspace

**现象**：gateway 重启后，拦截器报错：
```
Error: Cannot find module '/home/xyy/bot/persona-command.mjs'
```

**根因**：插件运行时的 `process.cwd()` 是 `/home/xyy`，不是 workspace 的 `/home/xyy/.openclaw/workspace`。

**修复**：使用 `fileURLToPath(import.meta.url)` 定位插件自身路径，然后 resolve 到 workspace：
```js
const thisFile = fileURLToPath(import.meta.url);
const workspaceDir = resolve(dirname(thisFile), "../../../../workspace");
```

#### 🐛 Bug 3：getConfig 未定义

**现象**：gateway 启动时报错：
```
Cannot access 'getConfig' before initialization
```

**根因**：在命令拦截代码块中引用了 `getConfig`，但该变量在原始文件的后面位置才定义（`const getConfig = () => getOneBotConfig(api)`）。

**修复**：在拦截代码块内定义局部 `cmdConfig`：
```js
const cmdConfig = () => getOneBotConfig(api);
```

#### 🐛 Bug 4：execSync 找不到 /bin/sh

**现象**：执行命令时报错：
```
spawnSync /bin/sh ENOENT
```

**根因**：`execSync` 默认 spawn `/bin/sh -c` 来执行命令，但插件运行环境中 `/bin/sh` 路径异常（指向 dash 但 spawn 失败）。

**修复**：改用 `spawnSync` 指定 `/usr/bin/node`，避免 shell 介入：
```js
const result = spawnSync("/usr/bin/node", [scriptPath, ...args], {
    cwd: workspaceDir,
    timeout: 10000,
    encoding: "utf-8",
});
```

#### 🐛 Bug 5：respond_all 检测也用了错误的路径

**现象**：`respond_all: true` 已设置，但非 @ 消息仍被忽略（日志无显示）。

**根因**：`process-inbound.js` 中原有的 respond_all 检测代码同样使用 `process.cwd()` 读取 state.json，路径同样错误，catch 静默吞异常。

**修复**：改为与命令拦截器相同的路径解析方式：
```js
const stateFile = resolve(dirname(fileURLToPath(import.meta.url)),
    "../../../../workspace/bot/persona/state.json");
```

#### 🐛 Bug 6：Agent 自身也拒绝回复非 @ 消息

**现象**：respond_all 绕过 @ 检查后消息进入了 Agent，但 Agent 选择不回复。

**根因**：`AGENTS.md` 和 `brain-prompt.md` 都写着"被 @ 才回复"，Agent 遵守了指令。

**修复**：更新两份 prompt 文件，增加 `respond_all` 的判断逻辑：
- `brain-prompt.md`：先读 state.json 检查 respond_all
- `AGENTS.md`：行为原则增加 respond_all 优先判断

### 涉及文件（全部改动）

```
extensions/openclaw-onebot/dist/handlers/process-inbound.js  ← 多次修改
workspace/bot/behavior.js                                     ← 1次修改
workspace/bot/brain-prompt.md                                  ← 1次修改
workspace/AGENTS.md                                            ← 1次修改
```

---

## 最终架构总览

```
                    ┌─────────────────────┐
                    │        QQ 群         │
                    │  用户发消息 @Bot     │
                    └────────┬────────────┘
                             │
                    ┌────────▼────────┐
                    │  Lagrange 客户端 │  (Windows 宿主机)
                    └────────┬────────┘
                             │ WebSocket
                    ┌────────▼────────┐
                    │ OneBot 插件     │  (WSL2 / OpenClaw 插件)
                    └────────┬────────┘
                             │
              ┌──────────────┼──────────────┐
              ▼              ▼              ▼
     ┌────────────┐  ┌────────────┐  ┌────────────┐
     │ 命令拦截    │  │ respond_all│  │ 关键词匹配  │
     │ (代码级)    │  │ 检测       │  │ + @检测     │
     └──────┬─────┘  └─────┬──────┘  └──────┬─────┘
            │              │                │
      匹配命令          respond_all=   都不匹配 →
      直接 exec            true         忽略/过滤
      persona-cmd          ↓
            │         所有消息放行
            ▼              ▼
            └──────┬──────┘
                   ▼
          ┌────────────────┐
          │  Agent (LLM)   │
          │  main 会话     │
          │                │
          │  brain-prompt  │──→ 读取 state.json
          │  AGENTS.md     │──→ 判断回复策略
          └────────┬───────┘
                   │
                   ▼
          ┌────────────────┐
          │  OneBot 发送   │──→ 群消息
          └────────────────┘

          ┌────────────────┐
          │  Cron 定时任务  │──→ persona-tick (5min)
          │                 │──→ auto-speak-check (25min)
          └────────────────┘
```

---

## 关键技术决策

### 1. 命令处理：Agent 层 → 插件层

- ❌ 最初方案：Agent（LLM）识别命令并执行 → 不稳定
- ✅ 最终方案：插件 `process-inbound.js` 代码级拦截 → 100% 可靠

### 2. 图片发送：裸路径 → base64://

- WSL2 ↔ Windows 跨机器场景，不能发 Linux 路径
- 自动检测并转换成本地 base64 数据

### 3. Cron Job：systemEvent → agentTurn

- systemEvent 格式在会话挂起时会卡住状态
- agentTurn + isolated session 更稳定

### 4. 路径解析：process.cwd() → import.meta.url

- `process.cwd()` 在插件上下文不可靠
- `fileURLToPath(import.meta.url)` + `resolve()` 更健壮

---

## 踩坑清单

| # | 问题 | 症状 | 根因 | 修复方式 |
|---|------|------|------|----------|
| 1 | 发图失败 | retcode=0 但图不显示 | WSL2↔Windows 路径不通 | connection.js 加 base64:// 转换 |
| 2 | persona-tick 报错 | OneBot requires --to | delivery announce 无目标 | 改为 delivery: none |
| 3 | Cron 超时 | 执行时间 > timeoutSeconds | timeout 设太短 | 提高至 60s/120s |
| 4 | behavior.js import 错误 | ENOENT state-manager.js | import 路径少一层 persona/ | 改为 ./persona/state-manager.js |
| 5 | 命令拦截路径错误 | MODULE_NOT_FOUND | process.cwd() 不是 workspace | 改用 import.meta.url 定位 |
| 6 | getConfig 未定义 | Cannot access before init | 变量作用域引用错误 | 定义局部 cmdConfig |
| 7 | spawnSync /bin/sh ENOENT | shell 找不到 | execSync 依赖 /bin/sh | 改用 spawnSync 直接调 node |
| 8 | respond_all 不生效 | 非 @ 消息仍被忽略 | state.json 路径也错了 | 同一路径修复 |
| 9 | Agent 不回复非 @ 消息 | respond_all 绕过插件但 Agent 沉默 | prompt 说"被 @ 才回复" | 更新 brain-prompt.md + AGENTS.md |

---

## 当前状态（2026-05-08 08:55）

### ✅ 正常工作

- @Bot → 100% 回复
- `/status` `/auto on/off` `/mood` `/respond_all on/off` → 命令拦截器执行
- respond_all 开启时 → 每条群消息都回复
- persona-tick cron → 每 5 分钟更新 energy/night_mode
- auto-speak-check cron → 每 25 分钟评估并可能自主发言
- 发图 → base64:// 跨平台正常

### ⚠️ 注意

- 所有插件改动都在 dist 目录，直改源码重部署会覆盖
- cron job 是 agentTurn 格式，需注意 timeout
- `process.cwd()` 相关的问题已修复但其他地方若复用需警惕
