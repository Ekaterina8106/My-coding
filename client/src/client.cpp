#include "../inc/client.h"
#include "../../ext/httplib/httplib.h"
#include <iostream>

using namespace std;

/**
 * @brief Конструктор клиента
 * @param address Адрес сервера (по умолчанию localhost)
 * @param port Порт сервера (по умолчанию 8080)
 */
RecipeClient::RecipeClient(const string& address, int port)
: serverAddress(address), port(port) {}

/**
 * @brief Загрузка рецептов из файла на сервер
 * @param filename Имя файла для загрузки
 * @return true если успешно, false при ошибке
 */
bool RecipeClient::load(const string& filename) {
    httplib::Client cli(serverAddress, port);

    // Отправляем POST запрос с именем файла
    auto res = cli.Post("/load", filename, "text/plain");

    // Проверяем ответ сервера (1 - успех, 0 - ошибка)
    if (!res || res->status != 200) {
        cerr << "Ошибка соединения с сервером" << endl;
        return false;
    }
    return res->body == "1";
}

/**
 * @brief Печать всех рецептов
 */
void RecipeClient::print() const {
    httplib::Client cli(serverAddress, port);
    auto res = cli.Get("/print");

    if (res && res->status == 200) {
        try {
            json data = json::parse(res->body);
            if (data.is_array()) {
                cout << "Все рецепты (" << data.size() << " шт.):\n";
                cout << data.dump(4) << endl; // Форматирование с отступами
            } else {
                cout << "Данные повреждены!\n";
            }
        } catch (const exception& e) {
            cerr << "Ошибка парсинга JSON: " << e.what() << endl;
        }
    } else {
        cerr << "Ошибка соединения с сервером\n";
    }
}

/**
 * @brief Фильтрация рецептов
 * @return JSON с отфильтрованными рецептами
 */
json RecipeClient::filter(double minCal, double maxCal, double minTime, double maxTime) const {
    httplib::Client cli(serverAddress, port);

    // Формируем URL с параметрами
    string path = "/filter?minCal=" + to_string(minCal) +
    "&maxCal=" + to_string(maxCal) +
    "&minTime=" + to_string(minTime) +
    "&maxTime=" + to_string(maxTime);

    auto res = cli.Get(path.c_str());

    if (res && res->status == 200) {
        return json::parse(res->body);
    }
    return json::array(); // Возвращаем пустой массив при ошибке
}

/**
 * @brief Добавление нового рецепта
 * @param newRecipe JSON с данными рецепта
 * @return true если успешно
 */
bool RecipeClient::add(const json& newRecipe) {
    httplib::Client cli(serverAddress, port);
    auto res = cli.Post("/add", newRecipe.dump(), "application/json");

    if (!res || res->status != 200) {
        cerr << "Ошибка добавления рецепта" << endl;
        return false;
    }
    return res->body == "1";
}

/**
 * @brief Удаление рецепта по имени
 * @param recipeName Название рецепта
 * @return true если успешно
 */
bool RecipeClient::remove(const string& recipeName) {
    httplib::Client cli(serverAddress, port);
    auto res = cli.Post("/delete", recipeName, "text/plain");

    if (!res || res->status != 200) {
        cerr << "Ошибка соединения с сервером" << endl;
        return false;
    }

    return (res->body == "1"); // Возвращает true, если рецепт был удалён
}

/**
 * @brief Генерация случайного рецепта
 * @return JSON с сгенерированным рецептом
 */
json RecipeClient::generate() {
    httplib::Client cli(serverAddress, port);
    auto res = cli.Post("/generate", "", "text/plain");

    if (res && res->status == 200) {
        return json::parse(res->body);
    }
    return json::object();
}

/**
 * @brief
 * @return
 */
bool RecipeClient::appendFromFile(const string& filename) {
    httplib::Client cli(serverAddress, port);
    auto res = cli.Post("/append", filename, "text/plain");

    if (!res || res->status != 200) {
        cerr << "Ошибка соединения с сервером" << endl;
        return false;
    }
    return res->body == "1";
}

json RecipeClient::generate_ex(int count) {
    httplib::Client cli(serverAddress, port);
    auto res = cli.Post("/generate_ex", to_string(count), "text/plain");

    if (res && res->status == 200) {
        return json::parse(res->body);
    }
    return json::array();
}

bool RecipeClient::shutdownServer() {
    httplib::Client cli(serverAddress, port);
    auto res = cli.Get("/shutdown");
    return (res && res->status == 200);
}

json RecipeClient::findById(const string& id) const {
    try {
        httplib::Client cli(serverAddress, port);
        auto res = cli.Post("/find", id, "text/plain");

        if (res) {
            if (res->status == 200) {
                return json::parse(res->body);
            } else {
                cerr << "Ошибка сервера: " << res->status << endl;
            }
        } else {
            cerr << "Ошибка соединения с сервером: "
            << httplib::to_string(res.error()) << endl;
        }
    } catch (const exception& e) {
        cerr << "Исключение при поиске: " << e.what() << endl;
    }
    return json();
}
