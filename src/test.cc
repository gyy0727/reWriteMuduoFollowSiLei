/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2024-03-22 15:17:46
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2024-03-24 15:00:16
 * @FilePath: /sylar/home/muqiu0614/桌面/myModuo/src/test.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
// #define BOOST_BIND_GLOBAL_PLACEHOLDERS 
// #include <muduo/net/TcpServer.h>
// #include <muduo/base/Logging.h>
// #include <boost/bind.hpp>
// #include <muduo/net/EventLoop.h>
// #include <boost/bind/bind.hpp>
// #include <boost/bind/placeholders.hpp> 
// // 定义此宏
// using namespace boost::placeholders;
// // 使用muduo开发回显服务器
// class EchoServer
// {
//  public:
//   EchoServer(muduo::net::EventLoop* loop,
//              const muduo::net::InetAddress& listenAddr);

//   void start(); 

//  private:
//   void onConnection(const muduo::net::TcpConnectionPtr& conn);

//   void onMessage(const muduo::net::TcpConnectionPtr& conn,
//                  muduo::net::Buffer* buf,
//                  muduo::Timestamp time);

//   muduo::net::TcpServer server_;
// };

// EchoServer::EchoServer(muduo::net::EventLoop* loop,
//                        const muduo::net::InetAddress& listenAddr)
//   : server_(loop, listenAddr, "EchoServer")
// {
//   server_.setConnectionCallback(
//       boost::bind(&EchoServer::onConnection, this, _1));
//   server_.setMessageCallback(
//       boost::bind(&EchoServer::onMessage, this, _1, _2, _3));
// }

// void EchoServer::start()
// {
//   server_.start();
// }

// void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
// {
//   LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
//            << conn->localAddress().toIpPort() << " is "
//            << (conn->connected() ? "UP" : "DOWN");
// }

// void EchoServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
//                            muduo::net::Buffer* buf,
//                            muduo::Timestamp time)
// {
//   // 接收到所有的消息，然后回显
//   muduo::string msg(buf->retrieveAllAsString());
//   LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
//            << "data received at " << time.toString();
//   conn->send(msg);
// }


// int main()
// {
//   LOG_INFO << "pid = " << getpid();
//   muduo::net::EventLoop loop;
//   muduo::net::InetAddress listenAddr(8888);
//   EchoServer server(&loop, listenAddr);
//   server.start();
//   loop.loop();
// }
