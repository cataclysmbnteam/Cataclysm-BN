#pragma once
#ifndef CATA_SRC_FSTREAM_UTILS_H
#define CATA_SRC_FSTREAM_UTILS_H

#include <iosfwd>
#include <string>
#include <memory>
#include <functional>

#if defined (_WIN32) && !defined (_MSC_VER)
namespace __gnu_cxx
{
template<typename _CharT, typename _Traits >
class stdio_filebuf;
}
#endif

class JsonIn;
class JsonOut;

enum class cata_ios_mode : int {
    none = 0,
    app = 1 << 0,
    binary = 1 << 1
};

/**
 * Wrapper around std::ofstream that provides cross-platform support for UTF-8 paths.
 */
struct cata_ofstream {
    public:
        cata_ofstream();
        cata_ofstream( const cata_ofstream & ) = delete;
        cata_ofstream( cata_ofstream &&x );
        ~cata_ofstream();
        auto operator=( const cata_ofstream & ) -> cata_ofstream & = delete;
        auto operator=( cata_ofstream && ) -> cata_ofstream &;

        inline auto mode( cata_ios_mode m ) -> cata_ofstream & {
            _mode = m;
            return *this;
        }

        auto open( const std::string &path ) -> cata_ofstream &;
        auto is_open() -> bool;
        auto fail() -> bool;
        auto bad() -> bool;
        void flush();
        void close();

        auto operator*() -> std::ostream &;
        auto operator->() -> std::ostream *;

    private:
        cata_ios_mode _mode = cata_ios_mode::none;
#if defined (_WIN32) && !defined (_MSC_VER)
        std::unique_ptr<std::ostream> _stream;
        std::unique_ptr<__gnu_cxx::stdio_filebuf<char, std::char_traits<char>>> _buffer;
        FILE *_file = nullptr;
#else
        std::unique_ptr<std::ofstream> _stream;
#endif
};

/**
 * Wrapper around std::ifstream that provides cross-platform support for UTF-8 paths.
 */
struct cata_ifstream {
    public:
        cata_ifstream();
        cata_ifstream( const cata_ifstream & ) = delete;
        cata_ifstream( cata_ifstream &&x );
        ~cata_ifstream();
        auto operator=( const cata_ifstream & ) -> cata_ifstream & = delete;
        auto operator=( cata_ifstream && ) -> cata_ifstream &;

        inline auto mode( cata_ios_mode m ) -> cata_ifstream & {
            _mode = m;
            return *this;
        }
        auto open( const std::string &path ) -> cata_ifstream &;
        auto is_open() -> bool;
        auto fail() -> bool;
        auto bad() -> bool;
        void close();

        auto operator*() -> std::istream &;
        auto operator->() -> std::istream *;

    private:
        cata_ios_mode _mode = cata_ios_mode::none;
#if defined (_WIN32) && !defined (_MSC_VER)
        std::unique_ptr<std::istream> _stream;
        std::unique_ptr<__gnu_cxx::stdio_filebuf<char, std::char_traits<char>>> _buffer;
        FILE *_file = nullptr;
#else
        std::unique_ptr<std::ifstream> _stream;
#endif
};

/**
 * Open a file for writing, calls the writer on that stream.
 *
 * If the writer throws, or if the file could not be opened or if any I/O error
 * happens, the function shows a popup containing the
 * \p fail_message, the error text and the path.
 *
 * @return Whether saving succeeded (no error was caught).
 * @throw The void function throws when writing failes or when the @p writer throws.
 * The other function catches all exceptions and returns false.
 */
///@{
auto write_to_file( const std::string &path, const std::function<void( std::ostream & )> &writer,
                    const char *fail_message ) -> bool;
void write_to_file( const std::string &path, const std::function<void( std::ostream & )> &writer );
///@}

class JsonDeserializer;

/**
 * Try to open and read from given file using the given callback.
 *
 * The file is opened for reading (binary mode), given to the callback (which does the actual
 * reading) and closed.
 * Any exceptions from the callbacks are caught and reported as `debugmsg`.
 * If the stream is in a fail state (other than EOF) after the callback returns, it is handled as
 * error as well.
 *
 * The callback can either be a generic `std::istream`, a @ref JsonIn stream (which has been
 * initialized from the `std::istream`) or a @ref JsonDeserializer object (in case of the later,
 * it's `JsonDeserializer::deserialize` method will be invoked).
 *
 * The functions with the "_optional" prefix do not show a debug message when the file does not
 * exist. They simply ignore the call and return `false` immediately (without calling the callback).
 * They can be used for loading legacy files.
 *
 * @return `true` is the file was read without any errors, `false` upon any error.
 */
/**@{*/
auto read_from_file( const std::string &path, const std::function<void( std::istream & )> &reader ) -> bool;
auto read_from_file_json( const std::string &path, const std::function<void( JsonIn & )> &reader ) -> bool;
auto read_from_file( const std::string &path, JsonDeserializer &reader ) -> bool;

auto read_from_file_optional( const std::string &path,
                              const std::function<void( std::istream & )> &reader ) -> bool;
auto read_from_file_optional_json( const std::string &path,
                                   const std::function<void( JsonIn & )> &reader ) -> bool;
auto read_from_file_optional( const std::string &path, JsonDeserializer &reader ) -> bool;
/**@}*/
/**
 * Wrapper around std::ofstream that handles error checking and throws on errors.
 *
 * Use like a normal ofstream: the stream is opened in the constructor and
 * closed via @ref close. Both functions check for success and throw std::exception
 * upon any error (e.g. when opening failed or when the stream is in an error state when
 * being closed).
 * Use @ref stream (or the implicit conversion) to access the output stream and to write
 * to it.
 *
 * @note: The stream is closed in the destructor, but no exception is throw from it. To
 * ensure all errors get reported correctly, you should always call `close` explicitly.
 *
 * @note: This uses exclusive I/O.
 */
class ofstream_wrapper
{
    private:
        cata_ofstream file_stream;
        std::string path;
        std::string temp_path;

        void open( cata_ios_mode mode );

    public:
        ofstream_wrapper( const std::string &path, cata_ios_mode mode );
        ~ofstream_wrapper();

        auto stream() -> std::ostream & {
            return *file_stream;
        }
        operator std::ostream &() {
            return *file_stream;
        }

        void close();
};

auto safe_getline( std::istream &ins, std::string &str ) -> std::istream &;

/**
 * @group JSON (de)serialization wrappers.
 *
 * The functions here provide a way to (de)serialize objects without actually
 * including "json.h". The `*_wrapper` function create the JSON stream instances
 * and therefor require "json.h", but the caller doesn't. Callers should just
 * forward the stream reference to the actual (de)serialization function.
 *
 * The inline function do this by calling `T::(de)serialize` (which is assumed
 * to exist with the proper signature).
 *
 * @throws std::exception Deserialization functions may throw upon malformed
 * JSON or unexpected/invalid content.
 */
/**@{*/
auto serialize_wrapper( const std::function<void( JsonOut & )> &callback ) -> std::string;
void deserialize_wrapper( const std::function<void( JsonIn & )> &callback,
                          const std::string &data );

template<typename T>
inline auto serialize( const T &obj ) -> std::string
{
    return serialize_wrapper( [&obj]( JsonOut & jsout ) {
        obj.serialize( jsout );
    } );
}

template<typename T>
inline void deserialize( T &obj, const std::string &data )
{
    deserialize_wrapper( [&obj]( JsonIn & jsin ) {
        obj.deserialize( jsin );
    }, data );
}
/**@}*/

#endif // CATA_SRC_FSTREAM_UTILS_H
