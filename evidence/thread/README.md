# Multithreading 验证记录

## 实现内容

- 保存和恢复用户线程的 ra、sp 与 s0-s11，实现协作式上下文切换。
- 使用每桶互斥锁保证哈希表正确性并保留并行插入能力。
- 使用互斥锁、条件变量和轮次实现可重复使用的 barrier。

## 自动验证

执行 make grade，uthread、answers-thread.txt、ph_safe、ph_fast、barrier 和 time 全部通过，最终结果为 Score: 60/60。

完整评分输出保存在 grade.txt。

## 手动验证

uthread 的三个线程均运行 100 次并正常退出。ph 2 的缺失键数量为 0，插入吞吐高于 ph 1。barrier 2 输出 OK; passed。
