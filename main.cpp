#include <thread_safe_map.h>
#include <thread_safe_flag.h>
#include <thread_pool.h>
#include <download.h>
#include <upload.h>
#include <collage_creator.h>

int main(int argc, char* argv[]) {
    std::string folderPath = ".cats_temp";
    size_t num_threads = std::thread::hardware_concurrency(); 
    int images_count = 12;
    ThreadSafeFlag has_errors_on_download;
    ThreadSafeMap globalMap;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--max_threads" && i + 1 < argc) {
            num_threads = std::stoi(argv[++i]); 
        } else if (arg == "--images_count" && i + 1 < argc) {
            images_count = std::stoi(argv[++i]);
        }
    }

    if (system("mkdir -p .cats_temp") != 0) {
        std::cerr << "Ошибка создания директории для файлов" << std::endl;
        return 1;
    }

    ThreadPool pool(num_threads);
    
    schedule_downloads(images_count, pool, globalMap, has_errors_on_download);

    pool.join();

    if (has_errors_on_download.get()) {
        std::cerr << "Произошли ошибки при загрузке файлов." << std::endl;
        return 1;
    }

    try {
        // TODO: добавить конфигурирование
        int canvasWidth = 600; 
        int canvasHeight = 800;     
        create_collage(folderPath, canvasWidth, canvasHeight, images_count);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }


    if (std::system("rm -rf .cats_temp") != 0) {
        std::cerr << "Ошибка при удалении директории " << folderPath << std::endl;
        return 1;
    }

    try {
        std::future<void> file_send_future = std::async(std::launch::async, [] {
            send_file_with_await();
        });

        file_send_future.get();
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Неизвестная ошибка в main" << std::endl;
    }

    return 0;
}
