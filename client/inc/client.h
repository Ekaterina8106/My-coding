#ifndef CLIENT_H
#define CLIENT_H

#include "../../ext/nlohmann/json.hpp"
#include <string>

using namespace std;
using json = nlohmann::json;

/**
 * @brief Клиент для взаимодействия с сервером рецептов
 */
class RecipeClient {
private:
    string serverAddress; // Адрес сервера
    int port; // Порт сервера

public:
    /**
     * @brief Конструктор клиента
     * @param address Адрес сервера
     * @param port Порт сервера
     */
    RecipeClient(const string& address = "localhost", int port = 8080);

    /**
     * @brief Загрузка рецептов из файла
     * @param filename Имя файла
     * @return true при успехе
     */
    bool load(const string& filename);

    // Вывод всех рецептов
    void print() const;

    /**
     * @brief Фильтрация рецептов
     * @param minCal Минимальная калорийность
     * @param maxCal Максимальная калорийность
     * @param minTime Минимальное время
     * @param maxTime Максимальное время
     * @return JSON с результатами фильтрации
     */
    json filter(double minCal, double maxCal, double minTime, double maxTime) const;

    /**
     * @brief Добавление рецепта
     * @param newRecipe JSON рецепта
     * @return true при успехе
     */
    bool add(const json& newRecipe);

    /**
     * @brief Удаление рецепта
     * @param recipeName Имя рецепта
     * @return true при успехе
     */
    bool remove(const string& recipeName);

    /// Генерация случайного рецепта
    json generate();

    /**
     * @brief Добавление из файла
     * @param filename Имя файла
     * @return true при успехе
     */
    bool appendFromFile(const string& filename);

    /**
     * @brief Множественная генерация
     * @param count Количество рецептов
     * @return JSON с рецептами
     */
    json generate_ex(int count);

    // Остановка сервера
    bool shutdownServer();

    /**
     * @brief Поиск по ID
     * @param id Идентификатор рецепта
     * @return JSON рецепта
     */
    json findById(const string& id) const;
};

#endif
