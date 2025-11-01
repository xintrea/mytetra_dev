#include "OrderedMap.h"


template<typename Key, typename Value>
OrderedMap<Key, Value>::OrderedMap() = default;

template<typename Key, typename Value>
OrderedMap<Key, Value>::OrderedMap(std::initializer_list<std::pair<Key, Value>> initList) {
    for (const auto& pair : initList) {
        insert(pair.first, pair.second);
    }
}

// Вставка элемента
template<typename Key, typename Value>
void OrderedMap<Key, Value>::insert(const Key& key, const Value& value) {
    if (!mapValues.contains(key)) {
        mapKeys.append(key);
    }
    mapValues[key] = value;
}

// Получение значения по ключу
template<typename Key, typename Value>
Value OrderedMap<Key, Value>::value(const Key& key, const Value& defaultValue) const {
    return mapValues.value(key, defaultValue);
}

// Проверка наличия ключа
template<typename Key, typename Value>
bool OrderedMap<Key, Value>::contains(const Key& key) const {
    return mapValues.contains(key);
}

// Удаление элемента
template<typename Key, typename Value>
void OrderedMap<Key, Value>::remove(const Key& key) {
    if (mapValues.contains(key)) {
        mapValues.remove(key);
        mapKeys.removeAll(key);
    }
}

// Очистка
template<typename Key, typename Value>
void OrderedMap<Key, Value>::clear() {
    mapKeys.clear();
    mapValues.clear();
}

// Размер
template<typename Key, typename Value>
int OrderedMap<Key, Value>::size() const {
    return mapKeys.size();
}

// Проверка что объект пустой (не содержит элементов)
template<typename Key, typename Value>
bool OrderedMap<Key, Value>::isEmpty() const {
    return mapKeys.isEmpty();
}

// Получение списка ключей в порядке добавления
template<typename Key, typename Value>
QList<Key> OrderedMap<Key, Value>::keys() const {
    return mapKeys;
}

// Получение списка значений в порядке добавления
template<typename Key, typename Value>
QList<Value> OrderedMap<Key, Value>::values() const {
    QList<Value> result;
    for (const Key& key : mapKeys) {
        result.append(mapValues.value(key));
    }
    return result;
}

// Оператор доступа
template<typename Key, typename Value>
Value& OrderedMap<Key, Value>::operator[](const Key& key) {
    if (!mapValues.contains(key)) {
        mapKeys.append(key);
    }
    return mapValues[key];
}

// Const оператор доступа
/*
template<typename Key, typename Value>
const Value& OrderedMap<Key, Value>::operator[](const Key& key) const {
    return mapValues[key];
}
*/

template<typename Key, typename Value>
const Value& OrderedMap<Key, Value>::at(const Key& key) const {
    auto it = mapValues.find(key);
    if (it == mapValues.end()) {
        throw std::out_of_range("Key not found in OrderedMap");
    }
    return it.value();
}

// Явное инстанцирование для конкретных типов
template class OrderedMap<QString, int>;
template class OrderedMap<QString, QString>;
template class OrderedMap<QString, double>;
template class OrderedMap<QString, QPair<QString, float> >;

