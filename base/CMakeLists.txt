cmake_minimum_required(VERSION 2.8.10)

set (INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
                    /usr/local/include
					)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib
            /usr/local/lib
	    /usr/lib/x86_64-linux-gnu)

add_compile_options(-std=c++11 -g)

include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

add_executable(mutex mutex.cpp test/mutex_test.cpp)
add_executable(thread mutex.cpp thread.cpp test/thread_test.cpp)
add_executable(condition mutex.cpp thread.cpp condition.cpp test/condition_test.cpp)
add_executable(threadpool mutex.cpp thread.cpp condition.cpp threadpool.cpp test/threadpool_test.cpp)
add_executable(singleton test/singleton_test.cpp)
add_executable(fixedbuffer test/fixedbuffer_test.cpp)
add_executable(writefile writefile.cpp test/writefile_test.cpp)
add_executable(logfile writefile.cpp logfile.cpp test/logfile_test.cpp)
add_executable(asynclog mutex.cpp thread.cpp condition.cpp writefile.cpp logfile.cpp asynclog.cpp test/asynclog_test.cpp)
add_executable(log mutex.cpp thread.cpp condition.cpp writefile.cpp logfile.cpp asynclog.cpp log.cpp test/log_test.cpp)

target_link_libraries(mutex -lpthread)
target_link_libraries(thread -lpthread)
target_link_libraries(condition -lpthread)
target_link_libraries(threadpool -lpthread)
target_link_libraries(singleton -lpthread)
target_link_libraries(asynclog -lpthread)
target_link_libraries(log -lpthread)