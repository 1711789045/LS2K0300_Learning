# 更新程序说明

当您需要修改main.cpp内容后，请按照以下步骤更新程序：

## 更新步骤

### 1. 修改main.cpp
使用您喜欢的编辑器修改main.cpp文件：
```bash
# 使用vim编辑
vim main.cpp

# 或者使用其他编辑器
nano main.cpp
```

### 2. 重新编译程序
运行build.sh脚本重新编译：
```bash
./build.sh
```

### 3. 传输到开发板
编译成功后，程序会自动传输到开发板的/tmp目录下。

### 4. 更新开发板上的程序
在开发板上执行以下命令：
```bash
# 停止当前服务
systemctl stop auto_menu.service

# 替换程序文件
cp /tmp/E10_auto_menu_demo_new /home/root/E10_auto_menu_demo/E10_auto_menu_demo

# 启动服务
systemctl start auto_menu.service

# 检查服务状态
systemctl status auto_menu.service
```

### 5. 验证更新
检查程序是否正常运行：
```bash
# 检查进程
ps aux | grep E10_auto_menu_demo | grep -v grep

# 检查服务日志
journalctl -u auto_menu.service -n 10
```

## 一键更新脚本
您也可以使用以下一键更新脚本：

```bash
#!/bin/bash
# 一键更新脚本
cd /home/root/E10_auto_menu_demo
./update.sh
```

## 注意事项
1. 确保修改后的代码语法正确
2. 编译过程中可能会遇到错误，请根据错误信息修复
3. 如果更新后程序无法启动，可以检查服务日志
4. 如果需要回滚，可以重新编译并传输之前的版本

## 快速命令总结
```bash
# 1. 修改代码
vim main.cpp

# 2. 编译
./build.sh

# 3. 更新开发板
ssh root@172.20.10.7 "
    systemctl stop auto_menu.service
    cp /tmp/E10_auto_menu_demo_new /home/root/E10_auto_menu_demo/E10_auto_menu_demo
    systemctl start auto_menu.service
"
```
