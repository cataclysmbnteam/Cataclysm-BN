#pragma once
#ifndef CATA_SRC_DECISION_TABLE_UTILS_H
#define CATA_SRC_DECISION_TABLE_UTILS_H

#include <map>

/**
 * @brief Creates a lookup lambda using a decision table.
 *
 * for example, it could be used to create a lambda that returns a color based on a stat value:
 * { ~ 25: magenta, 25 ~ 50: red, 50 ~ 75: yellow, 75 ~ 100: green }
 *
 * @param table keys are the lower bound, inclusive. passed as an rvalue reference to avoid unnecessary copying.
 * @param min the smallest value.
 *
 * @return A decision table lambda
 *
 * Example usage: https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGIAGxcpK4AMngMmAByPgBGmMQgAMyJpAAOqAqETgwe3r4BQemZjgJhEdEscQnJtpj2JQxCBEzEBLk%2BfoG19dlNLQRlUbHxSSkKza3t%2BV3j/YMVVaMAlLaoXsTI7BzmieHI3lgA1CaJbk7jxJisJ9gmGgCCO3sHmMenLEypN3ePDwSYLFSBn%2BbzcBAAnqlGKxXgBpTDg0iHCFQ5hsQ4ANTEXkw3weTC8REOWGQeEyAgA%2Bs0YvQION0CAQB8vqd4YjMdjcYlsMczP5zP5qfQkWgGOMOd5XgKWOElj8TAB2KwPQ6qw6XAjrBjHACsFiFUsSABFDvTGSxUAA3TAQA0rQ4yhgmHVGiCi8Vs3n%2BADWCKWxyVPzVwcOAHpQ4chFcNghDvxiMiEK8aMRxXUAYwCImmFmyYcYnhgMB4tntQQk4dfeCgyHVQSiYQ3iaDQA6LypKHECkxNYMdAQKtyxLK%2B611V4KiHCCNsBgE7Npg0zAtuLAcIQJb%2BxUjschxvz00EBkgVKXS3TghDndjxVGmu1idTmdz43Ixf0FuuDdegWVhFvW5X0IABaG4U3GLdAxVXc1Q1LVDhAm4lFFdATmvWtb3vEM4OIbVHTQrDMOHeUFTvB4fnCLMPnXSD0MPHM8GQQ561QQ5LU5Ck0FoEgm2JTBSXJBgqXfLkzkEJEzRAC5wmAG4IG3LDg23MwdSRcwzAAJUwVCzDMW9SEUtVtwVVTeTMABxS5GHU/TDNVbcdQ0NTdIATTqbiAHcbNIgzoIwvyAyNZyzAAWSYYtBCYdSr3lALJLQQlQRONwzKxSVDgADg0bKNBAMyktONiOK4kgICynKt1OZLD2PVxaDQsMI2AKynTio9GQSrNkuq9S0pxTLcvy7rCvYyVOM8UqssqtxqskuqGvDQ5msway2uPTqCpS3rOUOEy8vUzairGkriAgEzptm9qQHm4dGqWlqsPitYuqqwrtvShVJH23TDtGnFxu407Pouwq5r7erbsW8F3NQLy1o657Dve/qdR1b6zF%2B4qJtO1GQZSsH0Ahiw7uh2hPMeq6NuGrbdL615EkGg7qaO/6TogBm8ZqxkbuJxbLlQ%2BGUER5nkdeLhGZ%2B5m/swAHSvFzmCaJu6Pgi5oKfW4XXppsw6cOYCuC4MxEkkLh0cx47sYgfXDeNrgFaunnlfCzMovIgKcO1DQCPI0ifg4FZaE4HVeD8DgtFIVBOBmyxrFNNYNilI2eFIAhNH9lZvVGFtkhz3O8/8fROEkEO04jzheAUEAnNTsP/dIOBYCQNBAToeJyEoZvUlbhJgENsxSCwS1GMwDE8EwDyAHlUU4ZOaFof5U0oGJS4LZhiHBGfeFXlpwQnmJtH4mvk%2BbthBAnhhaA32uB4BQxgHEa/8EuBw8GtSvr8wVR%2BMJLZw8oupS60DwDEYgO8PBYFLgQYgeAWCbxWFQAwwAFCj3HlPRgm8ZCCBEGIdgUhMHyCUGoUuugggGCMCgaw1h9DAMrpAFYqBUgNHfhXdML9nAQFcFMPwQRQjhCGJUEYhQMhZAEFwvQRQREMHmMMBIQQ7CH16BMNongOh6HkWwxoSjpECNkbYJRYi5FaL4QsQRKwFDx02BIAOQcS7X0jhwQ4qgMr%2BGAv4SQS1kBMUNi2DGEBcCEB4jsO2vAa5aE3KQJMTAsAJA3KQTOyRs55ySckAugcODF1IKHcO9iK5VxTmnFYDdEAgGeqkQk7c3SoBbvQYgkQYScCcS4txHivFmB8bwbSAToEMiCPwLBohxB4L6QQlQ6hr4kNIB5UBqQ4GFw4MHTJpd7ET0JGUrMqBJyNNce44AnjDjeN8R4apJYglLBCQUjOWdknJLmRkrJvAcm2DyaE9OcyzC2OyeXfJtdwnWlTNkEAkggA%3D
 */
template <typename Key, typename Value>
auto decision_table(std::map<Key, Value> &&table, const Value &min)
{
    return [table = std::move(table), min](const Key &key) {
        // Find the first element with key greater than the input key
        const auto it = table.upper_bound(key);

        if (it == table.begin()) {
            // input key is smaller than the smallest key in the table
            return min;
        } else {
            // Otherwise, decrement the iterator to get the largest key
            // that is less than or equal to the input key, and return
            // the value associated with that key
            return std::prev(it)->second;
        }
    };
}

#endif // CATA_SRC_DECISION_TABLE_UTILS_H

