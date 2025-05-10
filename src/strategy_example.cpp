#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <thread>
#include <string>
#include <chrono>
#include <random>

// Интерфейс стратегии сортировки
class SortingStrategy {
public:
    virtual ~SortingStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getComplexity() const = 0;
};

// Реализация быстрой сортировки
class QuickSortStrategy : public SortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Применяется " << getName() << " (сложность: O(" << getComplexity() << "))" << std::endl;
        std::sort(data.begin(), data.end());
    }
    
    std::string getName() const override {
        return "Быстрая сортировка";
    }
    
    std::string getComplexity() const override {
        return "n log n";
    }
};

// Реализация сортировки пузырьком
class BubbleSortStrategy : public SortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Применяется " << getName() << " (сложность: O(" << getComplexity() << "))" << std::endl;
        
        for (size_t i = 0; i < data.size(); ++i) {
            for (size_t j = 0; j < data.size() - i - 1; ++j) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }
    
    std::string getName() const override {
        return "Сортировка пузырьком";
    }
    
    std::string getComplexity() const override {
        return "n²";
    }
};

// Контекст, использующий стратегию
class ArraySorter {
private:
    std::unique_ptr<SortingStrategy> strategy_;
    std::vector<int> data_;
    
public:
    ArraySorter(std::vector<int> data) : data_(std::move(data)) {}
    
    void setStrategy(std::unique_ptr<SortingStrategy> strategy) {
        strategy_ = std::move(strategy);
    }
    
    void sort() {
        if (!strategy_) {
            std::cout << "Стратегия сортировки не установлена!" << std::endl;
            return;
        }
        
        std::cout << "До сортировки: ";
        printArray();
        
        strategy_->sort(data_);
        
        std::cout << "После сортировки: ";
        printArray();
    }
    
    void printArray() const {
        for (const auto& item : data_) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
};

// Функция для генерации случайного массива
std::vector<int> generateRandomArray(size_t size, int min, int max) {
    std::vector<int> result(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    
    for (size_t i = 0; i < size; ++i) {
        result[i] = dis(gen);
    }
    
    return result;
}

// Функция для выполнения сортировки в отдельном потоке
void sortInThread(int threadId, std::vector<int> data, std::unique_ptr<SortingStrategy> strategy) {
    std::cout << "Поток " << threadId << " начал работу" << std::endl;
    
    ArraySorter sorter(std::move(data));
    sorter.setStrategy(std::move(strategy));
    sorter.sort();
    
    std::cout << "Поток " << threadId << " завершил работу" << std::endl;
}

int main() {
    // Генерируем случайные массивы для сортировки
    std::vector<int> array1 = generateRandomArray(10, 1, 100);
    std::vector<int> array2 = generateRandomArray(10, 1, 100);
    
    // Создаем стратегии сортировки
    auto quickSort = std::make_unique<QuickSortStrategy>();
    auto bubbleSort = std::make_unique<BubbleSortStrategy>();
    
    // Запускаем сортировку в разных потоках
    std::thread thread1(sortInThread, 1, array1, std::move(quickSort));
    std::thread thread2(sortInThread, 2, array2, std::move(bubbleSort));
    
    // Ожидаем завершения потоков
    thread1.join();
    thread2.join();
    
    return 0;
}
