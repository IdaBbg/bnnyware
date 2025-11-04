#include "Coms.hpp"


#include <memory>
#include <atomic>

class SocketReader {
public:
    static bool ReadExactBytes(SOCKET socket, void* buffer, size_t size) {
        char* output = static_cast<char*>(buffer);
        size_t totalRead = 0;

        while (totalRead < size) {
            int bytesReceived = recv(socket, output + totalRead,
                static_cast<int>(size - totalRead), 0);

            if (bytesReceived <= 0) {
                return false;
            }

            totalRead += static_cast<size_t>(bytesReceived);
        }
        return true;
    }
};

class TcpServer {
private:
    std::atomic<bool> running_{ false };
    SOCKET listen_socket_{ INVALID_SOCKET };
    static constexpr size_t MAX_SCRIPT_SIZE = 8 * 1024 * 1024;

    void HandleClient(SOCKET client_socket) {
        auto socket_cleanup = [&]() { closesocket(client_socket); };

        uint32_t network_length = 0;
        if (!SocketReader::ReadExactBytes(client_socket, &network_length, sizeof(network_length))) {
            return;
        }

        uint32_t script_length = ntohl(network_length);
        if (script_length == 0 || script_length > MAX_SCRIPT_SIZE) {
            return;
        }

        std::vector<char> script_buffer(script_length);
        if (!SocketReader::ReadExactBytes(client_socket, script_buffer.data(), script_length)) {
            return;
        }

        std::string script_content(script_buffer.data(), script_buffer.size());
        SC::RequestExecution(script_content);
    }

    void ServerLoop() {
        while (running_) {
            SOCKET client_socket = accept(listen_socket_, nullptr, nullptr);
            if (client_socket == INVALID_SOCKET) {
                if (running_) {
                    Sleep(50);
                }
                continue;
            }

            std::thread([this, client_socket]() {
                HandleClient(client_socket);
                closesocket(client_socket);
                }).detach();
        }
    }

public:
    ~TcpServer() {
        Stop();
    }

    bool Initialize() {
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            return false;
        }

        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        addrinfo* address_info = nullptr;
        if (getaddrinfo("127.0.0.1", "5050", &hints, &address_info) != 0) {
            WSACleanup();
            return false;
        }

        std::unique_ptr<addrinfo, void(*)(addrinfo*)> addr_cleanup(address_info, freeaddrinfo);

        listen_socket_ = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
        if (listen_socket_ == INVALID_SOCKET) {
            return false;
        }

        BOOL option = TRUE;
        setsockopt(listen_socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&option), sizeof(option));

        if (bind(listen_socket_, address_info->ai_addr, static_cast<int>(address_info->ai_addrlen)) == SOCKET_ERROR) {
            closesocket(listen_socket_);
            return false;
        }

        if (listen(listen_socket_, SOMAXCONN) == SOCKET_ERROR) {
            closesocket(listen_socket_);
            return false;
        }

        running_ = true;
        return true;
    }

    void Start() {
        if (listen_socket_ != INVALID_SOCKET) {
            ServerLoop();
        }
    }

    void Stop() {
        running_ = false;
        if (listen_socket_ != INVALID_SOCKET) {
            closesocket(listen_socket_);
            listen_socket_ = INVALID_SOCKET;
        }
        WSACleanup();
    }
};

namespace Coms {
    void Init() {
        auto server = std::make_unique<TcpServer>();
        if (server->Initialize()) {
            std::thread([server = std::move(server)]() mutable {
                server->Start();
                }).detach();
        }
    }
}