#ifndef ORDEREDMAP_H
#define ORDEREDMAP_H

#include <QList>
#include <QHash>

// Вспомогательный класс, который является простым аналогом QMap
// только с доступом в последовательности добавления элементов


template<typename Key, typename Value>
class OrderedMap {

public:
    // Конструкторы
    OrderedMap();
    OrderedMap(std::initializer_list<std::pair<Key, Value>> initList);

    void insert(const Key& key, const Value& value);
    Value value(const Key& key, const Value& defaultValue = Value()) const;
    bool contains(const Key& key) const;
    void remove(const Key& key);
    void clear();
    int size() const;
    bool isEmpty() const;
    QList<Key> keys() const;
    QList<Value> values() const;

    Value& operator[](const Key& key);
    // const Value& operator[](const Key& key) const;
    const Value& at(const Key& key) const; // Безопасный const-метод

    // Класс const_iterator
    class const_iterator {
    private:
        const OrderedMap* map;
        int index;

    public:
        const_iterator(const OrderedMap* m, int i) : map(m), index(i) {}

        // Оператор разыменования
        const std::pair<Key, Value> operator*() const {
            const Key& key = map->mapKeys[index];
            return std::make_pair(key, map->mapValues.value(key));
        }

        // Оператор доступа к члену
        const std::pair<Key, Value>* operator->() const {
            // Для упрощения возвращаем временный объект
            // В реальной реализации можно использовать указатель
            static std::pair<Key, Value> temp;
            temp = std::make_pair(map->mapKeys[index], map->mapValues.value(map->mapKeys[index]));
            return &temp;
        }

        // Префиксный инкремент
        const_iterator& operator++() {
            ++index;
            return *this;
        }

        // Постфиксный инкремент
        const_iterator operator++(int) {
            const_iterator temp = *this;
            ++index;
            return temp;
        }

        // Префиксный декремент
        const_iterator& operator--() {
            --index;
            return *this;
        }

        // Постфиксный декремент
        const_iterator operator--(int) {
            const_iterator temp = *this;
            --index;
            return temp;
        }

        // Операторы сравнения
        bool operator==(const const_iterator& other) const {
            return map == other.map && index == other.index;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

        // Операторы отношения (для совместимости с STL)
        bool operator<(const const_iterator& other) const {
            return index < other.index;
        }

        bool operator<=(const const_iterator& other) const {
            return index <= other.index;
        }

        bool operator>(const const_iterator& other) const {
            return index > other.index;
        }

        bool operator>=(const const_iterator& other) const {
            return index >= other.index;
        }
    };

    // Методы для итераторов
    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    const_iterator end() const {
        return const_iterator(this, mapKeys.size());
    }

    const_iterator cbegin() const {
        return const_iterator(this, 0);
    }

    const_iterator cend() const {
        return const_iterator(this, mapKeys.size());
    }

    // Поиск по ключу
    const_iterator find(const Key& key) const {
        int index = mapKeys.indexOf(key);
        if (index != -1) {
            return const_iterator(this, index);
        }
        return end();
    }

    // Первый и последний элементы
    const_iterator first() const {
        return isEmpty() ? end() : const_iterator(this, 0);
    }

    const_iterator last() const {
        return isEmpty() ? end() : const_iterator(this, mapKeys.size() - 1);
    }

private:
    QList<Key> mapKeys;
    QHash<Key, Value> mapValues;

};


// Явное инстанцирование для используемых типов
extern template class OrderedMap<QString, int>;
extern template class OrderedMap<QString, QString>;
extern template class OrderedMap<QString, double>;
extern template class OrderedMap<QString, QPair<QString, float> >;


#endif // ORDEREDMAP_H
