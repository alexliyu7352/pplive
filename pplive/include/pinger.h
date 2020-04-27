#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <istream>
#include <ostream>
#include <chrono>

#include "icmp_header.h"
#include "ipv4_header.h"

using boost::asio::steady_timer;
using boost::asio::ip::icmp;
namespace chrono = boost::asio::chrono;

namespace pplive {
class Pinger {
   public:
    Pinger(boost::asio::io_context& io_context, const char* destination)
        : resolver_(io_context),
          socket_(io_context, icmp::v4()),
          timer_(io_context),
          sequence_number_(0),
          spend_time_(0),
          num_replies_(0) {
        destination_ = *resolver_.resolve(icmp::v4(), destination, "").begin();

        start_send();
        start_receive();
    }

    long long spend_time() { return spend_time_; }

   private:
    /**
     * @brief 准备并发送数据 注册超时函数
     * 
     */
    void start_send() {
        std::string body("\"Hello!\" from Asio ping.");

        // Create an ICMP header for an echo request.
        icmp_header echo_request;
        echo_request.type(icmp_header::echo_request);
        echo_request.code(0);
        echo_request.identifier(get_identifier());
        echo_request.sequence_number(++sequence_number_);
        compute_checksum(echo_request, body.begin(), body.end());

        // Encode the request packet.
        boost::asio::streambuf request_buffer;
        std::ostream os(&request_buffer);
        os << echo_request << body;

        // Send the request.
        time_sent_ = steady_timer::clock_type::now();
        socket_.send_to(request_buffer.data(), destination_);

        // Wait up to five seconds for a reply.
        num_replies_ = 0;
        timer_.expires_at(time_sent_ + chrono::seconds(5));
        timer_.async_wait(
            [&](const boost::system::error_code& error) { handle_timeout(); });
    }

    void handle_timeout() {
        if (!spend_time_) {
            spend_time_ = -1;
        }
        socket_.close();
    }
    /**
     * @brief 清理reply_buffer_ 注册数据到达时候的回调函数
     * 
     */

    void start_receive() {
        // Discard any data already in the buffer.
        reply_buffer_.consume(reply_buffer_.size());

        // Wait for a reply. We prepare the buffer to receive up to 64KB.
        socket_.async_receive(
            reply_buffer_.prepare(65536),
            [&](boost::system::error_code const& error, std::size_t length) {
                handle_receive(length);
            });
    }
    /**
     * @brief 当收到数据时进行回调
     * 
     * @param length 收到的数据的长度
     */
    void handle_receive(std::size_t length) {
        // The actual number of bytes received is committed to the buffer so
        // that we can extract it using a std::istream object.
        reply_buffer_.commit(length);

        // std::cout << length << std::endl;

        // Decode the reply packet.
        std::istream is(&reply_buffer_);
        ipv4_header ipv4_hdr;
        icmp_header icmp_hdr;
        is >> ipv4_hdr >> icmp_hdr;

        // We can receive all ICMP packets received by the host, so we need to
        // filter out only the echo replies that match the our identifier and
        // expected sequence number.
        if (is && icmp_hdr.type() == icmp_header::echo_reply &&
            icmp_hdr.identifier() == get_identifier() &&
            icmp_hdr.sequence_number() == sequence_number_) {
            // If this is the first reply, interrupt the five second timeout.
            if (num_replies_++ == 0)
                timer_.cancel();

            // Print out some information about the reply packet.
            chrono::steady_clock::time_point now = chrono::steady_clock::now();
            chrono::steady_clock::duration elapsed = now - time_sent_;
            spend_time_ =
                chrono::duration_cast<chrono::nanoseconds>(elapsed).count();
        }
    }

    static unsigned short get_identifier() {
#if defined(ASIO_WINDOWS)
        return static_cast<unsigned short>(::GetCurrentProcessId());
#else
        return static_cast<unsigned short>(::getpid());
#endif
    }

    icmp::resolver resolver_; //处理器
    icmp::endpoint destination_; //目标地址
    icmp::socket socket_; //套接字
    steady_timer timer_; //计时器
    unsigned short sequence_number_; //序列号
    chrono::steady_clock::time_point time_sent_; //发信时间
    boost::asio::streambuf reply_buffer_; //回复buffer
    std::size_t num_replies_; //重试次数
    long long spend_time_; //花费时间
};
}