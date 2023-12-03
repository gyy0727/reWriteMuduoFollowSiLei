/*
 * @Author: Gyy0727 3155833132@qq.com
 * @Date: 2023-12-02 18:58:04
 * @LastEditors: Gyy0727 3155833132@qq.com
 * @LastEditTime: 2023-12-03 17:09:37
 * @FilePath: /桌面/myModuo/src/TcpConnection.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/TcpConnection.h"
static EventLoop *CheckLoopNotNull(EventLoop *loop) {
  if (loop == nullptr) {
    LOG_FATAL("%s:%s:%d TcpConnection Loop is null! \n", __FILE__, __FUNCTION__,
              __LINE__);
  }
  return loop;
}
TcpConnection::TcpConnection(EventLoop *loop, const std::string &name,
                             int sockfd, const InetAddress &localAddr,
                             const InetAddress &peerAddr)
    : loop_(loop), name_(name), state_(kConnecting), reading_(true),
      socket_(new Socket(sockfd)), channel_(new Channel(loop, sockfd)),
      localAddr_(localAddr), peerAddr_(peerAddr),
      highWaterMark_(64 * 1024 * 1024) {
  channel_->setReadCallback(
      std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
  channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));

  LOG_INFO("TcpConnection::ctor[%s] at fd=%d\n", name_.c_str(), sockfd);
  socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
  LOG_INFO("TcpConnection::dtor[%s] at fd=%d state=%d \n", name_.c_str(),
           channel_->sockfd(), (int)state_);
}

// 发送数据
void TcpConnection::send(const std::string &buf) {
  if (state_ == kConnected) {
    if (loop_->isInLoopThread()) {
      sendInLoop(buf.c_str(), buf.size());
    } else {
      loop_->runInLoop(
          std::bind(&TcpConnection::sendInLoop, this, buf.c_str(), buf.size()));
    }
  }
}

// 关闭连接
void TcpConnection::shutdown() {
  if (state_ == kConnected) {
    setState(kDisconnecting);
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
  }
}

// 连接建立
void TcpConnection::connectEstablished() {
  setState(kConnected);
  channel_->tie(shared_from_this());
  channel_->enableReading(); // 向poller注册channel的epollin事件

  // 新连接建立，执行回调
  connectionCallback_(shared_from_this());
}
// 连接销毁
void TcpConnection::connectDestroyed() {
  if (state_ == kConnected) {
    setState(kDisconnected);
    channel_->disableAll(); // 把channel的所有感兴趣的事件，从poller中del掉
    connectionCallback_(shared_from_this());
  }
  channel_->remove(); // 把channel从poller中删除掉
}

void TcpConnection::handleRead(TimeStamp receiveTime) {}
void TcpConnection::handleWrite() {
  if (channel_->isWriting())
    {
        int savedErrno = 0;
        ssize_t n = outputBuffer_.writeFd(channel_->sockfd(), &savedErrno);
        if (n > 0)
        {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0)
            {
                channel_->disableWriting();
                if (writeCompleteCallback_)
                {
                    // 唤醒loop_对应的thread线程，执行回调
                    loop_->queueInLoop(
                        std::bind(writeCompleteCallback_, shared_from_this())
                    );
                }
                if (state_ == kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR("TcpConnection::handleWrite");
        }
    }
    else
    {
        LOG_ERROR("TcpConnection fd=%d is down, no more writing \n", channel_->sockfd());
    }
}
// poller => channel::closeCallback => TcpConnection::handleClose
void TcpConnection::handleClose()
{
    LOG_INFO("TcpConnection::handleClose fd=%d state=%d \n", channel_->sockfd(), (int)state_);
    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr connPtr(shared_from_this());
    connectionCallback_(connPtr); // 执行连接关闭的回调
    closeCallback_(connPtr); // 关闭连接的回调  执行的是TcpServer::removeConnection回调方法
}

void TcpConnection::handleError()
{
    int optval;
    socklen_t optlen = sizeof optval;
    int err = 0;
    if (::getsockopt(channel_->sockfd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        err = errno;
    }
    else
    {
        err = optval;
    }
    LOG_ERROR("TcpConnection::handleError name:%s - SO_ERROR:%d \n", name_.c_str(), err);
}
/**
 * 发送数据  应用写的快， 而内核发送数据慢， 需要把待发送数据写入缓冲区，
 * 而且设置了水位回调
 */
/**
 * 在所属loop线程中, 发送data[len]
 * @param data 要发送的缓冲区首地址
 * @param len　要发送的缓冲区大小(bytes)
 * @details 发生write错误, 如果发送缓冲区未满,　对端已发FIN/RST分节
 * 表明tcp连接发生致命错误(faultError为true)
 */
void TcpConnection::sendInLoop(const void *data, size_t len) {
  ssize_t nwrote = 0;      // 已发送数据
  size_t remaining = len;  // 未发送数据
  bool faultError = false; // 是否发生错误

  // 之前调用过该connection的shutdown，不能再进行发送了
  // 如果已经断开连接(kDisconnected), 就无需发送, 打印log(LOG_WARN)
  if (state_ == kDisconnected) {
    LOG_ERROR("disconnected, give up writing!");
    return;
  }
  // write一次, 往对端发送数据, 后面再看是否发生错误, 是否需要高水位回调
  // if no thing output queue, try writing directly
  // 表示channel_第一次开始写数据，而且缓冲区没有待发送数据
  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
    nwrote = ::write(channel_->sockfd(), data, len);
    if (nwrote >= 0) {
      remaining = len - nwrote;
      if (remaining == 0 && writeCompleteCallback_) {
        // 既然在这里数据全部发送完成，就不用再给channel设置epollout事件了
        loop_->queueInLoop(
            std::bind(writeCompleteCallback_, shared_from_this()));
      } else // nwrote < 0
      {
        nwrote = 0;
        // EWOULDBLOCK表示操作非阻塞的 I/O
        // 操作时，由于当前操作会被阻塞，而操作未能成功完成
        if (errno != EWOULDBLOCK) {
          LOG_ERROR("TcpConnection::sendInLoop");
          if (errno == EPIPE || errno == ECONNRESET) // SIGPIPE  RESET
          {
            faultError = true;
          }

        } // 说明当前这一次write，并没有把数据全部发送出去，剩余的数据需要保存到缓冲区当中，然后给channel
        // 注册epollout事件，poller发现tcp的发送缓冲区有空间，会通知相应的sock-channel，调用writeCallback_回调方法
        // 也就是调用TcpConnection::handleWrite方法，把发送缓冲区中的数据全部发送完成
        if (!faultError && remaining > 0) {
          // 目前发送缓冲区剩余的待发送数据的长度
          size_t oldLen = outputBuffer_.readableBytes();
          if (oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_ &&
              highWaterMarkCallback_) {
            loop_->queueInLoop(std::bind(highWaterMarkCallback_,
                                         shared_from_this(),
                                         oldLen + remaining));
          }
          outputBuffer_.append((char *)data + nwrote, remaining);
          if (!channel_->isWriting()) {
            channel_
                ->enableWriting(); // 这里一定要注册channel的写事件，否则poller不会给channel通知epollout
          }
        }
      }
    }
  }
}

void TcpConnection::shutdownInLoop() {
  if (!channel_->isWriting()) // 说明outputBuffer中的数据已经全部发送完成
  {
    socket_->shutdownWrite(); // 关闭写端
  }
}
