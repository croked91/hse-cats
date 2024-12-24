#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <string>
#include <thread_safe_map.h>
#include <thread_safe_flag.h>
#include <thread_pool.h>
#include <addr_info_wrapper.h>
#include <utils.h>
#include <socket_wrapper.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

// TODO: если останется время - вынести в аргументы
const int max_retries = 10;
const std::vector<std::string> urls = {
    "http://algisothal.ru:8888/cat",
    "http://algisothal.ru:8889/cat",
    "http://algisothal.ru:8890/cat"
};
// TODO: если останется время - вынести в аргументы
const std::string base_filename = ".cats_temp/cat_";


int download_image(const std::string& request, 
  SocketWrapper& sock, 
  std::ofstream& file,
  ThreadSafeMap& images_meta
);
void prepare_and_orchestrate(
  const std::string& url, 
  const std::string& filename,
  ThreadSafeMap& images_meta,
  ThreadSafeFlag& has_errors_on_download  
);

void schedule_downloads(
  int images_count, 
  ThreadPool& pool, 
  ThreadSafeMap& images_meta, 
  ThreadSafeFlag& has_errors_on_download
);

#endif