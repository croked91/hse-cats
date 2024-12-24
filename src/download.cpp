#include <download.h>

// TODO: если останется время, сдлеать классом все это
int download_image(
    const std::string& request, 
    SocketWrapper& sock, 
    std::ofstream& file,
    ThreadSafeMap& images_meta
) {
    send(sock.get(), request.c_str(), request.size(), 0);

    char buffer[4096];
    bool header_end = false;
    std::string response;

    while (true) {
        ssize_t bytes_received = recv(sock.get(), buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }

        response.append(buffer, bytes_received);
        const std::string content_length = extractContentLength(response);
            
        if (!header_end) {
            size_t pos = response.find("\r\n\r\n");

            if (!content_length.empty()) {
                size_t body_start = pos + 4;
                size_t body_length = response.size() - body_start;
                std::string body_data = response.substr(body_start, std::min(body_length, static_cast<size_t>(256)));

                if (!images_meta.insert_if_not_contains(content_length, body_data)) {
                    response.clear();
                    return 1;
                }
            }

            if (pos != std::string::npos) {
                header_end = true;
                file.write(response.data() + pos + 4, response.size() - pos - 4);
                response.clear();
            }
        } else {
            file.write(buffer, bytes_received);
        }
    }
    return 0;
}


void prepare_and_orchestrate(
    const std::string& url, 
    const std::string& filename,
    ThreadSafeMap& images_meta,
    ThreadSafeFlag& has_errors_on_download  
) {
    std::string host;
    std::string path;
    int port = 80;

    size_t pos = url.find("://");
    if (pos == std::string::npos) {
        std::cerr << "Неверный URL: отсутствует схема (http://)" << std::endl;
        return;
    }

    pos += 3;

    size_t port_pos = url.find(':', pos);
    size_t path_start = url.find('/', pos);

    if (port_pos != std::string::npos && (path_start == std::string::npos || port_pos < path_start)) {
        host = url.substr(pos, port_pos - pos);
        port = std::stoi(url.substr(port_pos + 1, (path_start == std::string::npos ? url.length() : path_start) - port_pos - 1));
    } else {
        host = url.substr(pos, (path_start == std::string::npos ? url.length() : path_start) - pos);
    }

    if (path_start != std::string::npos) {
        path = url.substr(path_start);
    } else {
        path = "/";
    }

    try {
        AddrInfoWrapper addrInfo(host, std::to_string(port));
        SocketWrapper socket(addrInfo.get());
        socket.connect(addrInfo.get());

        std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";

        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла для записи" << std::endl;
            return;
        }

        int res;
        for (int retries = 1; retries <= max_retries; ++retries) {
            std::cout << "Скачиваем файл: " << filename << std::endl;
            res = download_image(request, socket, file, images_meta);
            if (res != 0 && retries < max_retries) {
                std::cerr << "Файл " << filename << " дублируется. Пробуем ещё. Попытка " << retries << " из " << max_retries << "." << std::endl;
                continue;
            }

            if (res != 0) {
                std::cerr << "Файл " << filename << " не удалось скачать за " << retries << " попыток. Попробуйте почистить карму и запустить программу еще раз" << std::endl;
                has_errors_on_download.set(true);
                break;
            }

            std::cout << "Файл " << filename << " успешно загружен." << std::endl;
            break;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


void schedule_downloads(
    int images_count, 
    ThreadPool& pool, 
    ThreadSafeMap& images_meta, 
    ThreadSafeFlag& has_errors_on_download
) {
    for (int i = 0; i < images_count; ++i) {
        const std::string& url = urls[i % urls.size()];
        std::string filename = base_filename + std::to_string(i) + ".jpeg";
        pool.enqueue([url, filename, &images_meta, &has_errors_on_download] { 
            prepare_and_orchestrate(url, filename, images_meta, has_errors_on_download); 
        });
    }
}