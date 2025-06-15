- ubuntu 使用 ip addr 查看ip

```
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 ...
    inet xxx.xxx.xx.xxx/24 ...
```
- ubuntu支持ssh
- sudo apt update
sudo apt install openssh-server
- 启动服务
- sudo systemctl start ssh
sudo systemctl enable ssh
- 确认服务状态 sudo systemctl status ssh
- vscode Ctrl+Shift+P → 打开命令面板  :Remote-SSH: Open SSH Configuration File...
- 打开C:\Users\<你的用户名>\.ssh\config

``` 
# 第一台 Ubuntu 虚拟机
Host ubuntu-vm
    HostName xxx.xxx.xx.xxx
    User op
    Port 22

# 第二台（如果有）
Host test-vm
    HostName xxx.xxx.xx.xxx
    User dev
    Port 22

```
- Ctrl+Shift+P 输入 Remote-SSH: Connect to Host...
- 输入密码，连接成功



---

密钥连接
- 本机 ssh-keygen
- 得到 私钥：C:\Users\你的用户名\.ssh\id_rsa

公钥：C:\Users\你的用户名\.ssh\id_rsa.pub 
- 把公钥的值复制到 ubuntu :
  mkdir -p ~/.ssh
nano ~/.ssh/authorized_keys
- 设置权限
chmod 700 ~/.ssh
chmod 600 ~/.ssh/authorized_keys
- 测试连接
- ssh op@xx.xx.xx.xx
- 连接成功或者失败
- vscode ctrl+shift+p 打开：  ssh用户配置 C:\Users\你的用户名\.ssh\config
Host ubuntu-vm
    HostName xx.xx.xx.xx
    User op
    IdentityFile C:\Users\你的用户名\.ssh\id_rsa
- ubuntu编辑密钥配置 （SSH 服务端配置不允许公钥登录）
- sudo nano /etc/ssh/sshd_config
改配置为：
PubkeyAuthentication yes
PasswordAuthentication yes
- 重启服务 sudo systemctl restart ssh
- 修正了问题
- 可能出现的问题：Permission denied (publickey)
- 检查用户名 ，公钥对的对不上，配置有咩有开启 PubkeyAuthentication 

