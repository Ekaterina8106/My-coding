#ifndef SERVER_H
#define SERVER_H

#ifdef __linux__
    #ifdef LIBRARY_EXPORT
        #define LIBRARY_API __attribute ((visibility ("default")))
    #else
        #define LIBRARY_API
    #endif
#else
    #ifdef LIBRARY_EXPORT
        #define LIBRARY_API __declspec(dllexport)
    #else
        #define LIBRARY_API __declspec(dllimport)
    #endif
#endif

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <regex>

#include "json.hpp"
#include "httplib.h"

using namespace std;
using json = nlohmann::json;
using namespace httplib;

#ifdef __cplusplus
extern "C"
{
    #endif

    /**
     * @brief Сервер для управления рецептами
     */
    class LIBRARY_API RecipeServer {
    private:
        json data;
        int nextId;

        /**
         * @brief Генерация ID
         * @param recipeName Имя рецепта
         * @return Сгенерированный ID
         * Генерирует ID для рецепта
         */
        string generateId(const string& recipeName);

    public:
        RecipeServer();
        ~RecipeServer();

        /**
         * @brief Загрузка данных
         * @param filename Имя файла
         * @return true при успехе
         * Производит загрузку данных из файла, перезапись
         */
        bool load(const string& filename);

        /**
         * @brief Добавление из файла
         * @param filename Имя файла
         * @return true при успехе
         * Производит загрузку из файла, добавление
         */
        bool appendFromFile(const string& filename);

        /**
         * @brief Вывод данных
         * @return JSON с рецептами
         * Печатает рецепты на экрвн
         */
        json print() const;

        /**
         * @brief Фильтрация
         * @param minCal Минимальная калорийность
         * @param maxCal Максимальная калорийность
         * @param minTime Минимальное время
         * @param maxTime Максимальное время
         * @return JSON с результатами
         * Итак понятно что делает
         */
        json filter(double minCal, double maxCal, double minTime, double maxTime) const;

        /**
         * @brief Добавление рецепта
         * @param newRecipe JSON рецепта
         * @return true при успехе
         * Добавляет рецепт через интерфейс
         */
        bool add(const json& newRecipe);

        /**
         * @brief Удаление рецепта
         * @param recipeName Имя рецепта
         * @return true при успехе
         * Удаляет рецепт, если таков имеется
         */
        bool remove(const string& recipeName);

         // Генерация рецепта
        json generate();

        /**
         * @brief Количество рецептов
         * @return Число рецептов
         */
        size_t count() const;

        /**
         * @brief Множественная генерация
         * @param count Количество
         * @return JSON с рецептами
         * Очевидно что... Генерирует не 1, а много рецептов
         */
        json generate_ex(int count);

        /**
         * @brief Поиск по ID
         * @param id Идентификатор
         * @return JSON рецепта
         * Ищет рецепт по ID
         */
        json findById(const string& id) const;
    };

    #ifdef __cplusplus
}
#endif

#endif //SERVER_H
