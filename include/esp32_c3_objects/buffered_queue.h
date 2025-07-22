#ifndef ESP32_C3_UTILS_BUFFERED_QUEUE_H
#define ESP32_C3_UTILS_BUFFERED_QUEUE_H

#pragma once

#include "queue.h"
#include <memory>

namespace esp32_c3::objects
{
    /**
     * @brief Потокобезопасная буферизированная очередь фиксированного размера
     * @tparam T Тип элементов очереди
     * @tparam BufferSize Максимальный размер буфера
     */
    template <typename T, size_t BufferSize>
    class BufferedQueue
    {
    public:
        /// @brief Тег для логирования
        static constexpr auto TAG = "BufferedQueue";

        /**
         * @brief Конструктор
         * @param queueLength Максимальное количество элементов в очереди
         */
        explicit BufferedQueue(UBaseType_t queueLength) noexcept;

        /**
         * @brief Проверка валидности очереди
         * @return true если все компоненты инициализированы успешно
         */
        [[nodiscard]] bool isValid() const noexcept;

        /**
         * @brief Отправить элемент в очередь
         * @param item Элемент для отправки
         * @param ticksToWait Время ожидания
         * @return true если успешно
         */
        bool send(const T& item, TickType_t ticksToWait = portMAX_DELAY) noexcept;

        /**
         * @brief Получить элемент из очереди
         * @param item Ссылка для сохранения элемента
         * @param ticksToWait Время ожидания
         * @return true если успешно
         */
        bool receive(T& item, TickType_t ticksToWait = portMAX_DELAY) noexcept;

        /// @brief Количество свободных мест в очереди
        [[nodiscard]] size_t available() const noexcept;

        /// @brief Количество ожидающих элементов
        [[nodiscard]] size_t waiting() const noexcept;

    private:
        struct QueueItem
        {
            size_t index; ///< Индекс элемента в буфере
        };

        bool getFreeIndex(size_t& index, TickType_t ticksToWait) const noexcept;
        void returnFreeIndex(size_t index) const noexcept;

        Queue<QueueItem> mQueue;      ///< Основная очередь
        Queue<size_t> mFreeIndices;   ///< Очередь свободных индексов
        std::unique_ptr<T[]> mBuffer; ///< Буфер данных
        bool mInitialized = false;    ///< Флаг успешной инициализации
    };
} // namespace esp32_c3::objects

#endif //ESP32_C3_UTILS_BUFFERED_QUEUE_H
