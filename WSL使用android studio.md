
安装好之后，使用xeyes测试是否具备图形化展示的能力
装android studio，进入模拟器目录执行模拟器判断错误在哪

https://stackoverflow.com/questions/67346232/android-emulator-issues-in-new-versions-the-emulator-process-has-terminated 
https://askubuntu.com/questions/893922/ubuntu-16-04-gives-x-error-of-failed-request-badvalue-integer-parameter-out-o

ai胡言乱语说是WSL2-ubuntu不支持虚拟机套虚拟机，实际上是内存不够大，本机win11找到 .wslconfig，在[wsl2]下面加上 memory=8GB ，然后回去执行模拟器就能进去了

---

错误里提示缺少的库：
sudo apt update

sudo apt install libnss3 

再就是警告

"Emulator is running using nested virtualization. This is not recommended..."
(模拟器正在使用嵌套虚拟化运行。这不被推荐...)

"WARNING | Your GPU drivers may have a bug. Switching to software rendering."
(警告 | 您的 GPU 驱动可能有 bug。正在切换到软件渲染。)

对于说为什么gpu不支持，按理说不应该，因为 nvm-si是能看到我gpu的，然后win11本机也有cuda环境

然后ai说对于usb要连接手机来调试的话，要usb转发到wsl，给出方案 
winget install --interactive --exact dorssel.usbipd-win

usbipd list 

usbipd attach --wsl --busid <BUSID> 

然后wsl测试 adb devices 

实际上，我wsl直接抢占了usb连上了真机，上面的步骤似乎就没用
