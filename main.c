#include <iostream>
#include <pthread.h>
#include <unistd.h>  // For sleep function

// Struct dùng chung giữa hai thread
struct SharedData {
    int x, y, z;
    int ready;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

void* thread_con(void* arg) {
    SharedData* data = (SharedData*)arg;

    // Thread con chờ đến khi ready == 1
    pthread_mutex_lock(&data->lock);
    while (data->ready == 0) {
        pthread_cond_wait(&data->cond, &data->lock);
    }

    // Tính z = x + y
    data->z = data->x + data->y;
    std::cout << "Thread con: Đã tính x + y = " << data->z << std::endl;

    // Gán ready = 0
    data->ready = 0;

    // Báo hiệu cho thread mẹ
    pthread_cond_signal(&data->cond);
    pthread_mutex_unlock(&data->lock);

    pthread_exit(nullptr);
}

int main() {
    pthread_t con_thread;
    SharedData data = {0, 0, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

    // Tạo thread con
    if (pthread_create(&con_thread, nullptr, thread_con, &data)) {
        std::cerr << "Không thể tạo thread con\n";
        return 1;
    }

    // Thread mẹ đọc giá trị x và y
    pthread_mutex_lock(&data.lock);
    std::cout << "Thread mẹ: Nhập giá trị x: ";
    std::cin >> data.x;
    std::cout << "Thread mẹ: Nhập giá trị y: ";
    std::cin >> data.y;

    // Gán ready = 1
    data.ready = 1;

    // Báo hiệu cho thread con
    pthread_cond_signal(&data.cond);

    // Chờ đến khi ready == 0
    while (data.ready == 1) {
        pthread_cond_wait(&data.cond, &data.lock);
    }

    // In giá trị z
    std::cout << "Thread mẹ: Giá trị z = " << data.z << std::endl;
    pthread_mutex_unlock(&data.lock);

    // Chờ thread con kết thúc
    pthread_join(con_thread, nullptr);

    return 0;
}
