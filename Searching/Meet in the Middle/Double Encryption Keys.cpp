#include <iostream>
#include <map>

template <typename Text, typename Key, typename Encryptor, typename Decryptor>
std::pair<Key, Key> get_double_encryption_keys
    (const Text& plain, const Text& encrypted, const Key& min_key, const Key& max_key)
{
    Encryptor encrypt;
    Decryptor decrypt;

    std::map<Text, Key> half_encrypted;

    for (Key key = min_key; key <= max_key; key++) {
        half_encrypted[encrypt(plain, key)] = key;
    }

    for (Key key = min_key; key <= max_key; key++) {
        Text t = decrypt(encrypted, key);
        auto it = half_encrypted.find(t);
        if (it != half_encrypted.end()) {
            return {key, it->second};
        }
    }

    return {-1, -1};
}

struct CaesarCipher
{
    std::string operator()(const std::string& plain_text, int key)
    {
        std::string result;
        result.resize(plain_text.size());
        for (int i = 0; i < plain_text.size(); i++)
            result[i] = 'a' + ((plain_text[i] - 'a' + key + 26) % 26);
        return result;
    }
};

struct CaesarDecipher
{
    std::string operator()(const std::string& encrypted, int key) {
        return CaesarCipher()(encrypted, -key);
    }
};

void test(const std::string& plain_text, int key1, int key2)
{
    // Caesar Cipher is not the best example, because there exists
    //  multiple pair of keys that will give the same result, thus,
    //  you're not guaranteed to get the same pair of keys.
    
    CaesarCipher cc;
    std::string encrypted = cc(cc(plain_text, key1), key2);

    std::cout << "Plain Text: " << plain_text << std::endl;
    std::cout << "Keys: " << key1 << " and " << key2 << std::endl;
    std::cout << "Encrypted Text: " << encrypted << std::endl;

    auto get_keys = get_double_encryption_keys<std::string, int, CaesarCipher, CaesarDecipher>;

    auto result = get_keys(plain_text, encrypted, 0, 26);
    std::cout << "Result Keys: " << result.first << " and " << result.second;
    std::cout << std::endl << std::endl;
}

int main()
{
    test("hello world", 5, 13);
    test("hello world", 15, 20);
}