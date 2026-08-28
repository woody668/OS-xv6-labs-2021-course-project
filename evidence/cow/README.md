# Copy-on-Write Fork 验证记录

## 实现内容

- fork 时共享用户物理页，并把原可写页标记为 COW。
- 使用引用计数维护共享物理页生命周期。
- 在用户写缺页和内核 copyout 中完成 COW 拆分。
- 对单引用 COW 页直接恢复写权限，避免不必要复制。

## 自动验证

执行 make clean 和 make grade，最终结果为 Score: 110/110。cowtest 的 simple、three、file 以及 usertests 的 copyin、copyout 和全部回归均通过。

完整评分输出保存在 grade.txt。

## 手动演示

进入 xv6 后运行 cowtest，结果应以 ALL COW TESTS PASSED 结束。
