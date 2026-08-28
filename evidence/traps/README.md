# Traps 实验验证记录

## 实现内容

- 阅读 user/call.asm，完成 RISC-V 参数寄存器、内联、返回地址、大小端和可变参数相关问答。
- 通过 s0 帧指针遍历内核栈，实现 backtrace，并接入 sys_sleep 和 panic。
- 新增 sigalarm 与 sigreturn 系统调用。
- 在用户时钟中断到期时保存完整 trapframe，并把执行入口切换到用户处理函数。
- 恢复被中断现场，并通过状态标志防止 alarm 处理函数重入。

## 自动验证

在 Ubuntu 20.04 WSL 2 中执行：

make clean
make grade

首次完整评分结果为 Score: 85/85：

- answers-traps.txt: OK
- backtrace test: OK
- alarmtest: test0: OK
- alarmtest: test1: OK
- alarmtest: test2: OK
- usertests: OK
- time: OK

完整输出保存在 grade.txt。

## 手动演示

进入 xv6 后依次执行 bttest 和 alarmtest。前者应打印三层内核调用地址，后者应依次显示 test0 passed、test1 passed 和 test2 passed。
