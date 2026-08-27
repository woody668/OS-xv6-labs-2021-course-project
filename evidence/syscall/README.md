# System calls 实验记录

## 实验内容

本实验在 xv6 内核中增加 `trace` 和 `sysinfo` 两个系统调用。`trace` 使用进程级位掩码选择需要输出的系统调用，并由 `fork` 继承给子进程；`sysinfo` 统计空闲物理内存和已使用进程槽位，并通过 `copyout` 安全返回用户空间。

## 自动评分

执行 `make grade` 后，以下测试全部通过：

- `trace 32 grep`
- `trace all grep`
- `trace nothing`
- `trace children`
- `sysinfotest`
- `time`

最终结果为 `Score: 35/35`，完整输出保存在 `grade.txt`。

## 回归测试

使用全新 `fs.img` 运行完整 `usertests`，最终输出 `ALL TESTS PASSED`。测试中出现的预期用户态非法访问陷阱由对应测试主动触发，各测试最终均显示 `OK`。

## 手动验证

计划在 QEMU 启动的 xv6 Shell 中运行：

```text
trace 32 grep hello README
trace 2147483647 grep hello README
sysinfotest
```

内部预检确认，第一条命令只输出 `read`，第二条命令输出多种系统调用，第三条命令输出 `sysinfotest: OK`。正式手动运行截图由本人在 WSL 环境中完成并保存在课程项目的 `素材/2` 目录中。
