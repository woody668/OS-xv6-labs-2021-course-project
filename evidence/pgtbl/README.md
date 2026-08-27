# Page tables 实验记录

## 实验内容

本实验完成三个任务：为每个进程建立用户只读 `USYSCALL` 映射，使 `ugetpid` 无需进入内核即可读取PID；实现 `vmprint` 递归打印Sv39三级页表；实现 `pgaccess` 检查并清除用户页面的 `PTE_A`，通过位掩码返回访问状态。

## 自动评分

执行 `make grade` 后，以下项目全部通过：

- `pgtbltest: ugetpid`
- `pgtbltest: pgaccess`
- `pte printout`
- `answers-pgtbl.txt`
- `usertests: all tests`
- `time`

最终结果为 `Score: 46/46`。评分脚本包含完整 `usertests`，结果为 `ALL TESTS PASSED`。

## 手动验证

启动 `make qemu` 时，PID 1完成 `exec` 后自动输出完整三级页表。在xv6 Shell中运行：

```text
pgtbltest
```

内部预检结果为 `ugetpid_test: OK`、`pgaccess_test: OK` 和 `pgtbltest: all tests succeeded`。正式自动评分和手动运行截图由本人在WSL环境中完成并保存在课程项目的 `素材/3` 目录中。
