
问题：Failed to create GLFW window  
解决：export LIBGL_ALWAYS_INDIRECT=0
```bash
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc_1.36/bin$ export LIBGL_ALWAYS_SOFTWARE=1
rt EGLxyy@kelijiuxiangquzhayu:~/workspace/renderdoc_1.36/bin$ export EGL_PLATFORM=surfaceless
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc_1.36/bin$ ./qrenderdoc
Failed to create GLFW window
Failed to create GLFW window
Failed to create GLFW window    
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc_1.36/bin$ unset EGL_PLATFORM
BGL_xyy@kelijiuxiangquzhayu:~/workspace/renderdoc_1.36/bin$ unset LIBGL_ALWAYS_SOFTWARE
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc_1.36/bin$ echo $DISPLAY
:0
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc_1.36/bin$ sudo apt update
sudo apt install -y libglfw3 libglfw3-dev libgl1-mesa-dev libx11-dev
sudo apt install -y libglfw3 libglfw3-dev libgl1-mesa-dev libx11-dev
Hit:1 http://security.ubuntu.com/ubuntu noble-security InRelease
Hit:2 http://archive.ubuntu.com/ubuntu noble InRelease
Hit:3 http://archive.ubuntu.com/ubuntu noble-updates InRelease
Hit:4 http://archive.ubuntu.com/ubuntu noble-backports InRelease
Hit:5 https://ppa.launchpadcontent.net/mozillateam/ppa/ubuntu noble InRelease
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
35 packages can be upgraded. Run 'apt list --upgradable' to see them.
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc_1.36/bin$ ./qrenderdoc
Failed to create GLFW window
Failed to create GLFW window
Failed to create GLFW window
Failed to create GLFW window
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc-triangle$ ./renderdoc-triangle
Failed to create GLFW window
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc-triangle$ sudo apt install -y mesa-utils
lxinfo | grep "OpenGL"
glxinfo | grep "OpenGL"
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
mesa-utils is already the newest version (9.0.0-2).
0 upgraded, 0 newly installed, 0 to remove and 35 not upgraded.
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc-triangle$ glxinfo | grep "OpenGL"
X Error of failed request:  BadValue (integer parameter out of range for operation)
  Major opcode of failed request:  148 (GLX)
  Minor opcode of failed request:  24 (X_GLXCreateNewContext)
  Value in failed request:  0x0
  Serial number of failed request:  28
  Current serial number in output stream:  29
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc-triangle$ glxinfo | grep OpenGL
X Error of failed request:  BadValue (integer parameter out of range for operation)
  Major opcode of failed request:  148 (GLX)
  Minor opcode of failed request:  24 (X_GLXCreateNewContext)
  Value in failed request:  0x0
  Serial number of failed request:  28
  Current serial number in output stream:  29
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc-triangle$ export LIBGL_ALWAYS_INDIRECT=0
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc-triangle$ glxinfo | grep "OpenGL version"
OpenGL version string: 4.1 (Compatibility Profile) Mesa 24.2.8-1ubuntu1~24.04.1
xyy@kelijiuxiangquzhayu:~/workspace/renderdoc-triangle$ ./renderdoc-triangle
```
