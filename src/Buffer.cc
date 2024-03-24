/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-12-02 18:56:07
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2024-03-22 15:31:36
 * @FilePath: /sylar/home/muqiu0614/桌面/myModuo/src/Buffer.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/Buffer.h"
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>
/**
 * 从fd读取数据到内部缓冲区, 将系统调用错误保存至savedErrno
 * @param 要读取的fd, 通常是代表连接的conn fd
 * @param savedErrno[out] 保存的错误号
 * @return 读取数据结果. < 0, 发生错误; >= 成功, 读取到的字节数
 */
ssize_t Buffer::readFd(int fd, int *saveErrno) {
  char extrabuf[65536] = {0}; // 栈上的内存空间  64K
  struct iovec vec[2];

  const size_t writable =
      writableBytes(); // 这是Buffer底层缓冲区剩余的可写空间大小
  vec[0].iov_base = begin() + writerIndex_; //可写数据首地址
  vec[0].iov_len = writable;

  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;

  const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
  const ssize_t n = ::readv(fd, vec, iovcnt);
  if (n < 0) {
    // ::readv系统调用错误
    *saveErrno = errno;
  } else if (static_cast<size_t>(n) <= writable) // Buffer的可写缓冲区已经够存储读出来的数据了
  {
    writerIndex_ += n;
  } else // extrabuf里面也写入了数据
  { // *读取的数据超过现有内部buffer_的writable空间大小时, 启用备用的extrabuf
    // *64KB空间, 并将这些数据添加到内部buffer_的末尾
    // *过程可能会合并多余prependable空间或resize buffer_大小,
    // *以腾出足够writable空间存放数据
    writerIndex_ = buffer_.size();
    append(extrabuf, n - writable); // writerIndex_开始写 n - writable大小的数据
  }

  return n;
}

ssize_t Buffer::writeFd(int fd, int *saveErrno) {
  ssize_t n = ::write(fd, peek(), readableBytes());
  if (n < 0) {
    *saveErrno = errno;
  }
  return n;
}