#include <iostream>
#include <string>
#include <type_traits>

template<typename KeyType, typename ValueType>
class KeyValuePair {
public:
    KeyValuePair(const KeyType& key, const ValueType& value) : key(key), value(value) {}

    size_t getKeySize() const {
        return getSize(key);
    }

    size_t getValueSize() const {
        return getSize(value);
    }

private:
    KeyType key;
    ValueType value;

    // Overloaded function for types other than std::string
    template<typename T>
    size_t getSize(const T& data, std::false_type) const {
        return sizeof(T);
    }

    // Overloaded function for std::string
    size_t getSize(const std::string& data, std::true_type) const {
        return data.size();
    }

    // Dispatcher function to select appropriate overload
    template<typename T>
    size_t getSize(const T& data) const {
        return getSize(data, std::is_same<T, std::string>());
    }
};

int main() {
    // Example usage with int key and string value
    KeyValuePair<int, std::string> pair1(123, "Hello");
    std::cout << "Key size: " << pair1.getKeySize() << std::endl; // Output: Size of int
    std::cout << "Value size: " << pair1.getValueSize() << std::endl; // Output: Size of string content

    // Example usage with string key and double value
    KeyValuePair<std::string, double> pair2("Key", 3.14);
    std::cout << "Key size: " << pair2.getKeySize() << std::endl; // Output: Size of string content
    std::cout << "Value size: " << pair2.getValueSize() << std::endl; // Output: Size of double

    return 0;
}