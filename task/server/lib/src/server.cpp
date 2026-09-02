#include "../inc/server.h"
#include <fstream>
#include <random>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

RecipeServer::RecipeServer() : nextId(1) {
    data = json::array();
}

RecipeServer::~RecipeServer() {}

// Генерация ID
string RecipeServer::generateId(const string& recipeName) {
    // Форматируем номер (3 цифры с ведущими нулями)
    string idNum = to_string(nextId);
    if (idNum.length() > 3) {
        idNum = idNum.substr(idNum.length() - 3, 3);
    } else {
        idNum = string(3 - idNum.length(), '0') + idNum;
    }

    // Берем первые 3 символа названия (только буквы, причем английский...)
    string prefix;
    for (char c : recipeName) {
        if (isalnum(c)) prefix += c;
        if (prefix.length() >= 3) break;
    }

    // Дополняем до 3 символов при необходимости
    if (prefix.length() < 3) {
        prefix.append(3 - prefix.length(), 'X');
    }

    nextId++;
    return "REC" + idNum + prefix;
}

bool RecipeServer::load(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    try {
        file >> data;

        // Генерируем ID для всех загруженных рецептов
        for (auto& recipe : data) {
            string name = recipe["Name"];
            recipe["ID"] = generateId(name);
        }

        return true;
    } catch (const exception& e) {
        cerr << "JSON parse error: " << e.what() << endl;
        return false;
    }
}

bool RecipeServer::appendFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    try {
        json newData;
        file >> newData;

        // Генерируем ID для новых рецептов
        for (auto& recipe : newData) {
            string name = recipe["Name"];
            recipe["ID"] = generateId(name);
        }

        if (data.is_array() && newData.is_array()) {
            for (const auto& recipe : newData) {
                data.push_back(recipe);
            }
            return true;
        }
        return false;
    } catch (const exception& e) {
        cerr << "JSON parse error: " << e.what() << endl;
        return false;
    }
}

// Вывод данных в консоль
json RecipeServer::print() const {
    //cout << data.dump(4) << endl; /* преобразует объект в строку, 4 - количество пробелов */ // кстати если оставить эту строчку, то рецепты в сервере будут появляться
    return data;
}

// Фильтрация по калорийности и времени
json RecipeServer::filter(double minCal, double maxCal, double minTime, double maxTime) const {
    json result = json::array();
    for (const auto& recipe : data) {
        try {
            double cal = recipe["Cal"];
            double time = recipe["Time"];

            if (cal >= minCal && cal <= maxCal && time >= minTime && time <= maxTime) {
                result.push_back(recipe);
            }
        }
        catch (...) {
            continue;
        }
    }

    // Сохраняем результат в файл (опционально)
    ofstream output("filtered_recipes.json");
    output << result.dump(4);
    cout << "Фильтрация завершена. Результат сохранен" << endl;

    return result; // Возвращаем JSON для отправки клиенту
}

bool RecipeServer::add(const json& newRecipe) {
    // Создаем копию, чтобы добавить ID
    json recipe = newRecipe;
    string name = recipe["Name"];
    recipe["ID"] = generateId(name);

    data.push_back(recipe);
    return true;
}

// Удаляем рецепт по имени
bool RecipeServer::remove(const string& recipeName) {
    size_t initialSize = data.size();
    auto it = remove_if(data.begin(), data.end(), [&recipeName](const json& recipe) {
        /* перемещает неподходящие элементы в начало, элементы которые надо удалить
         *    идут после it, размер при этом сохраняется*/
        try {
            return recipe["Name"] == recipeName;
        } catch (...) {
            return false;
        }
    });

    data.erase(it, data.end());

    if (data.size() < initialSize) {
        cout << "[DELETE] Рецепт \"" << recipeName << "\" удалён" << endl;
        return true;
    } else {
        cout << "[WARNING] Рецепт \"" << recipeName << "\" не найден" << endl;
        return false;
    }
}

// Генерация случайного рецепта
json RecipeServer::generate() {
    static const vector<string> recipeNames = {
        "Spagetti Bolonieze", "Cesar Salad", "Steak",
        "fastnika", "Chocolate cake", "MSU"
    };

    static const vector<string> ingredients = {
        "Мука", "Сахар", "Соль", "Перец", "Масло", "Яйцо", "Молоко",
        "Курочка", "Говядина", "Помидор", "Мехмат", "ВМК"
    };

    random_device rd; // не спрашивайте, работает значит не трогаем. Случайные числа делает
    mt19937 gen(rd());
    uniform_int_distribution<> nameDist(0, recipeNames.size() - 1); // выбираю случайное имя из списка выше
    uniform_int_distribution<> timeDist(10, 120); // случайное время от 10 до 120
    uniform_int_distribution<> calDist(100, 1000);
    uniform_int_distribution<> ingCountDist(2, 8); // число ингридиентов
    uniform_int_distribution<> ingDist(0, ingredients.size() - 1);
    uniform_real_distribution<> weightDist(10, 500);

    json newRecipe;
    newRecipe["Name"] = recipeNames[nameDist(gen)]; // выдали случайное имя рецепту
    newRecipe["Time"] = timeDist(gen);
    newRecipe["Cal"] = calDist(gen);
    string name = newRecipe["Name"];
    newRecipe["ID"] = generateId(name);

    int ingCount = ingCountDist(gen); // gen - генерирует случайное число от 2 до 8
    for (int i = 0; i < ingCount; ++i) {
        json ingredient;
        ingredient["Name"] = ingredients[ingDist(gen)];
        ingredient["Value"] = weightDist(gen);
        newRecipe["Ingredient"].push_back(ingredient);
    }

    data.push_back(newRecipe);
    cout << "[GENERATE] Создан рецепт:" << newRecipe["Name"]  << endl;
    //cout << "[GENERATE] Создан рецепт:\n" << newRecipe.dump(4)  << endl;
    //cout << newRecipe.dump(4) << endl;
    return newRecipe;
}

// Количество рецептиков
size_t RecipeServer::count() const {
    return data.is_array() ? data.size() : 0;
}

json RecipeServer::generate_ex(int count) {
    json result;// = json::array();
    for (int i = 0; i < count; ++i) {
        result.push_back(generate());
    }
    //data.insert(data.end(), result.begin(), result.end());
    return result;
}

json RecipeServer::findById(const string& id) const {
    for (const auto& recipe : data) {
        try {
            if (recipe["ID"] == id) {
                return recipe;
            }
        } catch (...) {
            // Игнорируем ошибки доступа
        }
    }
    return json();  // Возвращаем пустой JSON если не найдено
}

