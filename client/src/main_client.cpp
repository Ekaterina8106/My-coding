#include "../inc/client.h"
#include <iostream>
#include <limits>
#include <regex>
#include <fstream> // Добавляем для работы с файлами


using namespace std;

/**
 * @brief вывод интерфейса в консоль
 */

void displayMenu() {
    cout << "\n=== Управление Рецептами ===" << endl;
    cout << "1. Загрузить данные из файла" << endl;
    cout << "2. Распечатать все рецепты" << endl;
    cout << "3. Фильтруем по времени и калориям" << endl;
    cout << "4. Добавить новый рецепт" << endl;
    cout << "5. Удалить рецепт" << endl;
    cout << "6. Сделать рандомный рецепт" << endl;
    cout << "7. Сгенерировать несколько рецептов" << endl;
    cout << "8. Найти рецепт по ID" << endl;
    cout << "0. Выход" << endl;
    cout << "Что выбираем?  ";
}

/**
 * @brief Ввод нового рецепта с клавиатуры
 * @return JSON с данными рецепта
 */

json inputRecipe() { /* Функция добавления рецепта из файла CookingRecipe
    перекочевала сюда. Теперь она в клиенте, многие остальные функции в сервере */
    json newRecipe;
    string name;
    double time, cal;
    int ingredientCount;

    cout << "Имя для нового рецепта: ";
    getline(cin, name);
    newRecipe["Name"] = name;

    cout << "Время приготовления (в минутах): ";
    cin >> time;
    newRecipe["Time"] = time;

    cout << "Количество калорий: ";
    cin >> cal;
    newRecipe["Cal"] = cal;

    cout << "Количество ингредиентов: ";
    cin >> ingredientCount;
    cin.ignore();

    for (int i = 0; i < ingredientCount; i++) {
        json ingredient;
        string ingName;
        double value;

        cout << "Ингредиент " << i+1 << " название: ";
        getline(cin, ingName);

        cout << "Ингредиент " << i+1 << " вес (в граммах): ";
        cin >> value;
        cin.ignore();

        ingredient["Name"] = ingName;
        ingredient["Value"] = value;
        newRecipe["Ingredient"].push_back(ingredient);
    }

    return newRecipe;
}

int main(int argc, char* argv[]) {
    RecipeClient client("localhost", 8080);
    int choice;
    string filename;
    string recipeName;

    // Сохраняем оригинальный буфер cin
    streambuf* orig_cin = cin.rdbuf();

    // Если передан аргумент, читаем из файла
    ifstream test_file;
    if (argc > 1) {
        test_file.open(argv[1]);
        if (test_file.is_open()) {
            // Перенаправляем стандартный ввод на файл
            cin.rdbuf(test_file.rdbuf());
        } else {
            cerr << "Error opening test file: " << argv[1] << endl;
            return 1;
        }
    }

    do {
        if (argc == 1) { // Показываем меню только в интерактивном режиме
            displayMenu();
        }

        cin >> choice;
        if (cin.fail()) break; // Выход при ошибке чтения

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                if (argc == 1) {
                    cout << "1. Загрузить новые данные (перезаписать)" << endl;
                    cout << "2. Добавить данные из файла" << endl;
                    cout << "Выберите действие: ";
                }

                int var = 0;
                cin >> var;
                cin.ignore();

                if (argc == 1) {
                    cout << "Введите имя файла: ";
                }
                getline(cin, filename);

                if (var == 1) {
                    if (client.load(filename)) {
                        if (argc == 1) cout << "Данные успешно загружены!" << endl;
                    } else {
                        if (argc == 1) cerr << "Ошибка загрузки данных" << endl;
                    }
                } else if (var == 2) {
                    if (client.appendFromFile(filename)) {
                        if (argc == 1) cout << "Данные успешно добавлены!" << endl;
                    } else {
                        if (argc == 1) cerr << "Ошибка добавления данных" << endl;
                    }
                } else {
                    if (argc == 1) cerr << "Неверный выбор" << endl;
                }
                break;
            }

            case 2: // Вывод данных
                client.print();
                break;

            case 3: { // Фильтрация по калорийности и времени
                double minCal, maxCal, minTime, maxTime;
                if (argc == 1) {
                    cout << "Минимум калорий: ";
                    cin >> minCal;
                    cout << "Максимум калорий: ";
                    cin >> maxCal;
                    cout << "Минимальное время: ";
                    cin >> minTime;
                    cout << "Максимальное время: ";
                    cin >> maxTime;
                    cin.ignore();
                } else {
                    cin >> minCal >> maxCal >> minTime >> maxTime;
                }

                json result = client.filter(minCal, maxCal, minTime, maxTime);
                if (argc == 1) {
                    cout << "Отфильтрованные рецепты:" << endl;
                    cout << result.dump(4) << endl;
                }
                break;
            }

            case 4: { // Добавление рецепта
                json newRecipe = inputRecipe();
                if (client.add(newRecipe)) {
                    if (argc == 1) cout << "Рецепт успешно добавлен!" << endl;
                }
                break;
            }

            case 5: {
                if (argc == 1) cout << "Имя рецепта для удаления: ";
                getline(cin, recipeName);
                if (client.remove(recipeName)) {
                    if (argc == 1) cout << "Рецепт '" << recipeName << "' успешно удалён!" << endl;
                } else {
                    if (argc == 1) cout << "Рецепт '" << recipeName << "' не найден!" << endl;
                }
                break;
            }

            case 6: {
                json randomRecipe = client.generate();
                if (argc == 1) {
                    cout << "Сгенерированный рецепт:\n"
                    << randomRecipe.dump(4) << endl;
                }
                break;
            }

            case 7: { // Генерируем несколько рецептов
                int count;
                if (argc == 1) {
                    cout << "Введите количество рецептов: ";
                    cin >> count;
                    cin.ignore();
                } else {
                    cin >> count;
                }

                json result = client.generate_ex(count);
                if (argc == 1) {
                    cout << "Сгенерировано " << count << " рецептов:" << endl;
                    cout << result.dump(4) << endl;
                }
                break;
            }

            case 8: {
                if (argc == 1) cout << "Введите ID рецепта: ";
                string id;
                getline(cin, id);

                // Проверка формата регулярным выражением
                regex id_pattern(R"(REC\d{3}[\wа-яА-Я]{3})");
                if (!regex_match(id, id_pattern)) {
                    if (argc == 1) cout << "Неверный формат ID! Пример: REC001Спа" << endl;
                } else {
                    try {
                        json result = client.findById(id);
                        if (!result.empty()) {
                            if (argc == 1) {
                                cout << "Рецепт найден:\n" << result.dump(4) << endl;
                            }
                        } else {
                            if (argc == 1) cout << "Рецепт с ID '" << id << "' не найден!" << endl;
                        }
                    } catch (const exception& e) {
                        if (argc == 1) cerr << "Ошибка соединения: " << e.what() << endl;
                    }
                }
                break;
            }

            case 0: {
                client.shutdownServer();
                if (argc == 1) cout << "Сервер остановлен. Пока!" << endl;
                break;
            }

            default:
                if (argc == 1) cout << "ТЫ НЕ ПРОЙДЕШЬ!" << endl;
        }
    } while (choice != 0 && !cin.eof());

    // Восстанавливаем стандартный ввод
    if (argc > 1) {
        cin.rdbuf(orig_cin);
        if (test_file.is_open()) {
            test_file.close();
        }
    }

    client.shutdownServer();
    return 0;
}
