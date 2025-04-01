
本质上是目标带extenison指向，E:\app\vscode\VSCode-win32-x64-1.98.2\Code.exe  --extensions-dir "E:\app\TOOLS\vscodeExtensionsSave\extensions"，
不过我好奇如果新下载的插件会放在哪里呢？或者说更新vscode之后，是否还需要重新指定一遍... 实际上软链指向新版本就可以

---

以下是详细步骤指南，帮助你将VSCode插件安装路径从C盘迁移到其他磁盘（如D盘），并确保后续插件安装到新位置：

---

### **步骤1：关闭VSCode**
确保所有VSCode窗口已完全关闭，避免文件占用导致操作失败。

---

### **步骤2：剪切原插件文件夹**
1. 打开资源管理器，进入C盘用户目录：
   ```
   C:\Users\你的用户名\.vscode\extensions
   ```
   （将“你的用户名”替换为你的实际电脑用户名，如`Administrator`等）

2. **剪切**整个`extensions`文件夹（按 `Ctrl+X`）。

---

### **步骤3：粘贴到新位置**
1. 在目标磁盘（如D盘）创建文件夹（例如`D:\VSCode\extensions`）。
2. 将剪切的`extensions`文件夹**粘贴**到新路径（按 `Ctrl+V`）。

---

### **步骤4：修改VSCode快捷方式**
1. **右键桌面VSCode快捷方式** → 选择 **属性**。
2. 在 **“目标”** 栏中，修改内容为：
   ```
   "原始Code.exe路径" --extensions-dir "D:\VSCode\extensions"
   ```
   - 示例（根据你的实际安装路径调整）：
     ```
     "D:\VSCode\Microsoft VS Code\Code.exe" --extensions-dir "D:\VSCode\extensions"
     ```
3. 点击 **应用** → **确定** 保存。

---

### **步骤5：验证操作**
1. 通过修改后的快捷方式启动VSCode。
2. 检查插件是否正常加载：
   - 按下 `Ctrl+Shift+P`，输入 `Extensions: Show Installed Extensions`，查看已安装插件列表。
3. 确认新插件安装路径：
   - 安装一个新插件（如“Chinese”），检查是否出现在 `D:\VSCode\extensions` 下。

---

### **注意事项**
1. **C盘残留空文件夹**：  
   迁移后，C盘原路径可能仍存在空的`extensions`文件夹，这是正常的系统缓存，可忽略或手动删除（无影响）。

2. **便携版用户**：  
   如果使用VSCode便携版（Portable Mode），插件默认在`data/extensions`文件夹内，无需额外配置。

3. **全局生效**：  
   若需所有VSCode实例都使用新路径，需通过命令行启动时添加参数，或修改系统环境变量（不推荐普通用户操作）。

---

### **恢复默认路径（如需还原）**
删除快捷方式中的`--extensions-dir`参数，VSCode会自动恢复使用C盘默认路径。

---

通过以上步骤，你已成功将插件存储路径迁移到其他磁盘，避免占用C盘空间。如有其他问题，欢迎追问！
