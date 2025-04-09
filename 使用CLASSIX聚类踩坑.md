
先是py3.11环境，numpy2.1，classix

能跑起来，但是classix无法使用Cython加速，显示没有Cython，实际上我是装了的

```
import classix
print(classix.cython_is_available()) #返回false
```

官方github上提到类似的错误 [ModuleNotFoundError: No module named 'numpy' · Issue #12 · nla-group/classix](https://github.com/nla-group/classix/issues/12)  提到也许是构建的问题

```
[build-system]
requires = ["setuptools", "wheel", "Cython ~= 0.29.32", "numpy ~= 1.22.4"]
```

我使用conda新建了环境，仍然是同样的错误，我怀疑是conda包依赖问题，于是我选择使用pyenv(搭配pdm)新建环境测试，显示启用Cython了，但是出现新的错误

```
model = CLASSIX(radius=radius )
model.fit(data)
```

这样报错：

```
ValueError: Buffer dtype mismatch, expected 'long long' but got 'long'
```

gpt说是Cython的问题，尝试numpy降级，没用，尝试设置  

` model = CLASSIX(radius=radius,group_merging='density')`,加上group_merging='density' ，就成功了

