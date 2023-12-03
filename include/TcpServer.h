/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 18:37:53
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-03 13:33:11
 * @FilePath: /桌面/myModuo/include/TcpServer.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

/**
 * 用户使用muduo编写服务器程序
 */
#include "Acceptor.h"
#include "Buffer.h"
#include "Callback.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "NonCopyable.h"
#include "TcpConnection.h"
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
// TcpServer.h
/**
 * Tcp Server, 支持单线程和thread-poll模型.
 * 接口类, 因此不要暴露太多细节.
 */
// 对外的服务器编程使用的类
class TcpServer : NonCopyable {
public:
  //*线程初始化回调函数
  using ThreadInitCallback = std::function<void(EventLoop *)>;

  enum Option {
    kNoReusePort, //*不允许重用端口
    kReusePort,   //*允许重用端口
  };

  TcpServer(EventLoop *loop, const InetAddress &listenAddr,
            const std::string &nameArg, Option option = kNoReusePort);
  ~TcpServer();

  void setThreadInitcallback(const ThreadInitCallback &cb) {
    threadInitCallback_ = cb;
  }
  /**
   * 设置连接回调.
   * 非线程安全.
   */
  void setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
  }
  /**
   * 设置消息回调.
   * 非线程安全.
   */
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  /**
   * 设置写完成回调.
   * 非线程安全.
   */
  void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
    writeCompleteCallback_ = cb;
  }

  // 设置底层subloop的个数
  void setThreadNum(int numThreads);

  // *开启服务器监听
  /**
   * 如果没有监听, 就启动服务器(监听).
   * 多次调用没有副作用.
   * 线程安全.
   */
  void start();

private:
  /**
   * 新建一个TcpConnection对象, 用于连接管理.
   * @details 新建的TcpConnection对象会加入内部ConnectionMap.
   * @param sockfd accept返回的连接fd (accepted socket fd)
   * @param peerAddr 对端ip地址信息
   * @note 必须在所属loop线程运行
   */
  void newConnection(int sockfd, const InetAddress &peerAddr);
  void removeConnection(const TcpConnectionPtr &conn);
  void removeConnectionInLoop(const TcpConnectionPtr &conn);

  using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

  EventLoop *loop_; // baseLoop 用户定义的loop

  const std::string ipPort_; // ip:port
  const std::string name_;   // 用户自定义的名字

  std::unique_ptr<Acceptor> acceptor_; // 运行在mainLoop，任务就是监听新连接事件

  std::shared_ptr<EventLoopThreadPool>
      threadPool_; // one loop per thread,线程池

  ConnectionCallback connectionCallback_;       // 有新连接时的回调
  MessageCallback messageCallback_;             // 有读写消息时的回调
  WriteCompleteCallback writeCompleteCallback_; // 消息发送完成以后的回调

  ThreadInitCallback threadInitCallback_; // loop线程初始化的回调

  std::atomic_int started_; // 是否开启监听

  int nextConnId_;            // 连接id
  ConnectionMap connections_; // 保存所有的连接
};