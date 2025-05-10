#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <mutex>
#include <atomic>

// Базовый класс с шаблонным методом
class DataProcessingTemplate {
protected:
    std::string dataSource_;
    std::vector<std::string> processedData_;
    std::mutex mutex_;
    std::atomic<int> processedItems_{0};
    
    // Шаги алгоритма, которые могут быть переопределены в подклассах
    virtual void initialize() {
        std::cout << "Инициализация обработки данных..." << std::endl;
    }
    
    virtual void loadData() = 0;
    
    virtual void preProcess() {
        std::cout << "Предварительная обработка данных..." << std::endl;
    }
    
    virtual void processData() {
        std::cout << "Параллельная обработка данных:" << std::endl;
        
        std::vector<std::thread> threads;
        const int threadCount = 4;
        
        for (int i = 0; i < threadCount; ++i) {
            threads.emplace_back([this, i]() {
                // Имитация обработки данных
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    processedItems_++;
                    std::cout << "  Поток " << i << " обработал элемент данных" << std::endl;
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    virtual void postProcess() {
        std::cout << "Пост-обработка данных..." << std::endl;
    }
    
    virtual void saveResults() = 0;
    
    virtual void cleanup() {
        std::cout << "Очистка ресурсов..." << std::endl;
    }
    
public:
    DataProcessingTemplate(const std::string& dataSource) 
        : dataSource_(dataSource) {}
    
    virtual ~DataProcessingTemplate() = default;
    
    // Шаблонный метод, определяющий скелет алгоритма
    void process() {
        std::cout << "Начало обработки данных из источника: " << dataSource_ << std::endl;
        
        initialize();
        loadData();
        preProcess();
        processData();
        postProcess();
        saveResults();
        cleanup();
        
        std::cout << "Обработка завершена. Обработано элементов: " << processedItems_ << std::endl;
    }
};

// Конкретная реализация для CSV файлов
class CSVProcessor : public DataProcessingTemplate {
protected:
    void loadData() override {
        std::cout << "Загрузка данных из CSV файла: " << dataSource_ << std::endl;
        // Имитация загрузки данных
        processedData_ = {"строка1", "строка2", "строка3", "строка4"};
    }
    
    void saveResults() override {
        std::cout << "Сохранение результатов в CSV файл" << std::endl;
        for (const auto& item : processedData_) {
            std::cout << "  Сохранено: " << item << std::endl;
        }
    }
    
public:
    CSVProcessor(const std::string& filename) 
        : DataProcessingTemplate(filename) {}
};

// Конкретная реализация для JSON файлов
class JSONProcessor : public DataProcessingTemplate {
protected:
    void loadData() override {
        std::cout << "Загрузка данных из JSON файла: " << dataSource_ << std::endl;
        // Имитация загрузки данных
        processedData_ = {"объект1", "объект2", "объект3", "объект4"};
    }
    
    void saveResults() override {
        std::cout << "Сохранение результатов в JSON файл" << std::endl;
        for (const auto& item : processedData_) {
            std::cout << "  Сохранено: " << item << std::endl;
        }
    }
    
public:
    JSONProcessor(const std::string& filename) 
        : DataProcessingTemplate(filename) {}
};

int main() {
    // Обработка CSV файла
    std::cout << "=== Обработка CSV файла ===" << std::endl;
    CSVProcessor csvProc("data.csv");
    csvProc.process();
    
    std::cout << "\n=== Обработка JSON файла ===" << std::endl;
    JSONProcessor jsonProc("data.json");
    jsonProc.process();
    
    return 0;
}
