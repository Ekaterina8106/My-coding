#include "../../lib/inc/server.h"
#include "httplib.h"
#include <iostream>
#include <csignal>

using namespace httplib;
using namespace std;

int main() {
    Server svr;
    RecipeServer recipeServer; // Правильное имя класса и переменной

    // Обработчики запросов с захватом объектов
    svr.Post("/load", [&](const Request& req, Response& res) {
        cout << "[LOAD] Загрузка из: " << req.body << endl;
        res.set_content(recipeServer.load(req.body) ? "1" : "0", "text/plain");
    });

    svr.Post("/append", [&](const Request& req, Response& res) {
        cout << "[APPEND] Добавление данных из: " << req.body << endl;
        res.set_content(recipeServer.appendFromFile(req.body) ? "1" : "0", "text/plain");
    });

    svr.Get("/print", [&](const Request&, Response& res) {
        cout << "[PRINT] Запрос всех рецептов" << endl;
        res.set_content(recipeServer.print().dump(), "application/json");
    });

    svr.Get(R"(/filter)", [&](const Request& req, Response& res) {
    try {
        double minCal = stod(req.get_param_value("minCal"));
        double maxCal = stod(req.get_param_value("maxCal"));
        double minTime = stod(req.get_param_value("minTime"));
        double maxTime = stod(req.get_param_value("maxTime"));

        cout << "[FILTER] Параметры: "
             << minCal << "-" << maxCal << " калорий, "
             << minTime << "-" << maxTime << " минут" << endl;

        // Получаем JSON с результатами фильтрации
        json result = recipeServer.filter(minCal, maxCal, minTime, maxTime);
        res.set_content(result.dump(), "application/json"); // Отправляем JSON клиенту
    } catch (...) {
        res.status = 400;
        res.set_content(json::array().dump(), "application/json");
    }
    });

    svr.Post("/add", [&](const Request& req, Response& res) {
        cout << "[ADD] Добавление рецепта" << endl;
        try {
            json newRecipe = json::parse(req.body);
            res.set_content(recipeServer.add(newRecipe) ? "1" : "0", "text/plain");
        } catch (...) {
            res.status = 400;
            res.set_content("Ошибка данных", "text/plain");
        }
    });

    svr.Post("/delete", [&](const Request& req, Response& res) {
        cout << "[DELETE] Удаление: " << req.body << endl;
        res.set_content(recipeServer.remove(req.body) ? "1" : "0", "text/plain");
    });

    svr.Post("/generate", [&](const Request&, Response& res) {
        cout << "[GENERATE] Создание одного рецепта" << endl;
        res.set_content(recipeServer.generate().dump(), "application/json");
    });

    svr.Post("/generate_ex", [&](const Request& req, Response& res) {
        try {
            int count = stoi(req.body);
            cout << "[GENERATE] Запрос на " << count << " рецептов" << endl;
            res.set_content(recipeServer.generate_ex(count).dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content("Неверный формат", "text/plain");
        }
    });

    svr.Post("/find", [&](const Request& req, Response& res) {
        cout << "[FIND] Поиск по ID: " << req.body << endl;
        json result = recipeServer.findById(req.body);
        if (!result.empty()) {
            res.set_content(result.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content("Not found", "text/plain");
        }
    });

    svr.Get("/shutdown", [&](const Request&, Response& res) {
        cout << "[SHUTDOWN] Остановка сервера" << endl;
        svr.stop();
        res.set_content("Сервер остановлен", "text/plain");
    });

    cout << "[START] Сервер запущен на http://localhost:8080" << endl;
    if (!svr.listen("0.0.0.0", 8080)) {
        cerr << "[ERROR] Не удалось запустить сервер!" << endl;
        return 1;
    }

    return 0;
}
