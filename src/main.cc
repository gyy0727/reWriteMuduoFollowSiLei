/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 13:29:48
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2024-03-24 14:58:18
 * @FilePath: /myModuo/src/main.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-11-21 13:29:48
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2024-03-24 14:53:22
 * @FilePath: /桌面/myModuo/src/main.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/TcpServer.h"
#include "../include//Logger.h"

#include <functional>
#include <string>

class EchoServer {
public:
  EchoServer(EventLoop *loop, const InetAddress &addr, const std::string &name)
      : loop_(loop),server_(loop, addr, name) {
    // 注册回调函数
    server_.setConnectionCallback(
        std::bind(&EchoServer::onConnection, this, std::placeholders::_1));

    server_.setMessageCallback(
        std::bind(&EchoServer::onMessage, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));

    // 设置合适的loop线程数量 loopthread
    server_.setThreadNum(3);
  }
  void start() { server_.start(); }

private:
  // 连接建立或者断开的回调
  void onConnection(const TcpConnectionPtr &conn) {
    if (conn->connected()) {
      std::cout << "conn up " << std::endl;
    } else {
     std::cout << "conn down " << std::endl;
    }
  }

  // 可读写事件回调
  void onMessage(const TcpConnectionPtr &conn, Buffer *buf, TimeStamp time) {
    std::string msg = buf->retrieveAllAsString();
    conn->send(msg);
    conn->shutdown(); // 写端   EPOLLHUP =》 closeCallback_
  }

  EventLoop *loop_;
  TcpServer server_;
};

int main() {
  EventLoop loop;
  InetAddress addr(8000);
  EchoServer server(
      &loop, addr,
      "EchoServer-01"); // Acceptor non-blocking listenfd  create bind
  server
      .start(); // listen  loopthread  listenfd => acceptChannel => mainLoop =>
  loop.loop();  // 启动mainLoop的底层Poller

  return 0;
}