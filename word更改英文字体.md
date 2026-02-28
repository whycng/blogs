不用一个一个改，用**查找替换+通配符**或**VBA宏**，就能**只改英文、不动中文/数字/封面/引用**，下面给你最稳妥、最快的两种方法（按推荐顺序）。

---

### 方法一：查找替换（零代码，最推荐）
1. 按 `Ctrl+H` 打开「查找和替换」
2. 点「更多」→ 勾选「使用通配符」
3. 查找内容输入：`[A-Za-z]`（只匹配英文字母）
4. 光标放在「替换为」框 → 点「格式」→「字体」
   - 西文字体：选 **Times New Roman**
   - 中文字体：**留空/不选**（关键！不影响中文）
   - 字号、加粗、颜色等：**保持原样**
5. 点「全部替换」→ 完成

✅ 效果：**只改所有英文**，中文、数字、封面、引用编号、公式里的数字/符号**完全不动**。

---

### 方法二：VBA宏（更稳、覆盖全文/表格/文本框）
适合长文档、有表格/文本框的场景，一键执行：
1. 按 `Alt+F11` 打开VBA编辑器
2. 插入 → 模块 → 粘贴以下代码：
```vba
Sub SetEnglishToTimesNewRoman()
    Dim rng As Range
    Application.ScreenUpdating = False
    For Each rng In ActiveDocument.StoryRanges
        With rng.Find
            .ClearFormatting
            .Replacement.ClearFormatting
            .Text = "[A-Za-z]"
            .MatchWildcards = True
            .Replacement.Font.NameAscii = "Times New Roman"
            .Execute Replace:=wdReplaceAll
        End With
    Next rng
    Application.ScreenUpdating = True
    MsgBox "所有英文已设为 Times New Roman", vbInformation
End Sub
```
3. 按 `F5` 运行 → 完成

✅ 效果：**遍历正文、表格、文本框、页眉页脚**，只改英文，其他完全不动。

---

### 方法三：样式法（适合规范排版、长期复用）
如果你的论文用「正文」「标题」等样式：
1. 开始 → 样式 → 右键「正文」→ 修改
2. 格式 → 字体
   - 中文字体：宋体/黑体（不变）
   - 西文字体：**Times New Roman**
3. 勾选「自动更新」→ 确定

✅ 效果：**全文自动区分中英**，后续输入英文自动用Times New Roman，一劳永逸。

---

### 你现在直接用哪个？
- 只想快速改一遍 → 用**方法一（查找替换）**，10秒搞定
- 文档复杂、有表格/文本框 → 用**方法二（VBA）**
- 想以后排版更规范 → 用**方法三（样式）**

需要我把上面的**VBA代码**直接给你复制即用版，或者再给你**只改英文+数字**的版本吗？
