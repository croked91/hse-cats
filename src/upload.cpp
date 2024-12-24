#include <upload.h>
#include <addr_info_wrapper.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <atomic>
#include <stdexcept>

future_awaiter::future_awaiter(std::future<void>& f) : fut(f) {}

bool future_awaiter::await_ready() {
    return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void future_awaiter::await_suspend(std::coroutine_handle<> h) {
    std::thread([this, h] {
        fut.wait();
        h.resume();
    }).detach();
}

void future_awaiter::await_resume() {
    fut.get();
}

void show_loader(download_progress& progress) {
    const std::string loader = "|/-\\";
    size_t idx = 0;

    while (!progress.is_download_complete.load()) {
        std::cout << "\rОтправка коллажа " << loader[idx % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        idx++;
    }

    std::cout << "\rОтправка коллажа - Завершено!" << std::endl;
}

std::future<void> send_file_async(download_progress& progress) {
    return std::async(std::launch::async, [&progress]() -> void {
        try {
            const std::string file_path = "collage.jpg";
            const std::string server_ip = "algisothal.ru";
            const std::string port = "8888";
            const std::string server_url = "/cat";
            const std::string boundary = "----BoundaryString12345";

            std::ifstream file(file_path, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file: " + file_path);
            }

            std::ostringstream file_content_stream;
            file_content_stream << file.rdbuf();
            const std::string file_data = file_content_stream.str();

            std::ostringstream body;
            body << "--" << boundary << "\r\n";
            body << "Content-Disposition: form-data; name=\"file\"; filename=\"collage.jpg\"\r\n";
            body << "Content-Type: image/jpeg\r\n\r\n";
            body << file_data << "\r\n";
            body << "--" << boundary << "--\r\n";

            const std::string body_data = body.str();

            std::ostringstream request;
            request << "POST " << server_url << " HTTP/1.1\r\n";
            request << "Host: " << server_ip << "\r\n";
            request << "Content-Type: multipart/form-data; boundary=" << boundary << "\r\n";
            request << "Content-Length: " << body_data.size() << "\r\n";
            request << "Connection: close\r\n";
            request << "\r\n";
            request << body_data;

            const std::string request_str = request.str();

            AddrInfoWrapper addr_info(server_ip, port);
            struct addrinfo* result = addr_info.get();

            int sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (sock < 0) {
                throw std::runtime_error("Ошибка при создании сокета");
            }

            if (connect(sock, result->ai_addr, result->ai_addrlen) < 0) {
                close(sock);
                throw std::runtime_error("Ошибка при подключении к серверу");
            }

            if (send(sock, request_str.c_str(), request_str.size(), 0) < 0) {
                close(sock);
                throw std::runtime_error("Ошибка при отправке запроса");
            }

            char response[4096] = {0};
            int bytes_received = recv(sock, response, sizeof(response) - 1, 0);
            if (bytes_received > 0) {
                std::cout << "\nОтвет сервера:\n" << response << std::endl;
            }

            close(sock);

            progress.is_download_complete.store(true);

        } catch (const std::exception& e) {
            std::cerr << "Поймали исключение: " << e.what() << std::endl;
        }
    });
}

task send_file_with_await() {
    try {
        download_progress progress;

        std::promise<void> loader_promise;
        std::future<void> loader_future = loader_promise.get_future();

        std::thread loader_thread(show_loader, std::ref(progress));
        loader_thread.detach();

        std::future<void> file_send = send_file_async(progress);
        co_await future_awaiter(file_send);

        loader_future.get();

        std::cout << "Файл отправлен" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при отправке файла: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Неизвестное исключение" << std::endl;
    }
}
