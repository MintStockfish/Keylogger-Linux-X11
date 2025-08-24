#include "app/application.hpp"
#include <iostream>

int main() {
    try {
        Application app;
        app.run();
    } catch (const std::runtime_error& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Произошла неизвестная критическая ошибка." << std::endl;
        return 1;
    }
    
    return 0;
}