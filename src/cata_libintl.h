#pragma once
#ifndef CATA_SRC_CATA_LIBINTL_H
#define CATA_SRC_CATA_LIBINTL_H

#include <memory>
#include <string>
#include <vector>

/**
 * Runtime localization system for Cataclysm: Bright Nights.
 *
 * Implements translated string lookup within gettext MO files
 * with full support for integer plural forms.
 *
 * Key differences from GNU libintl:
 * 1. Does not support translation domains
 * 2. Does not support encoding conversion, operates on UTF-8 only
 * 3. Does not depend on locale / environment variables
 * 4. Supports loading MO files from arbitrary paths
 * 5. Supports loading multiple MO files into a single "domain"
 *
 * For MO file structure, see GNU gettext manual:
 * https://www.gnu.org/software/gettext/manual/
 */

namespace cata_libintl
{
using u8 = uint8_t;
using u32 = uint32_t;

/**
 * Plural forms AST node type.
 * Also used for token identification during parsing.
 */
enum class PlfOp : int {
    Mod,       // a % b
    Eq,        // a == b
    NotEq,     // a != b
    GreaterEq, // a >= b
    Greater,   // a > b
    LessEq,    // a <= b
    Less,      // a < b
    And,       // a && b
    Or,        // a || b
    TerCond,   // ?

    Literal,   // numeric literal
    Variable,  // the variable (n)

    BrOpen,    // (
    BrClose,   // )
    TerDelim,  // :

    NumOps,
};

struct PlfNode;
using PlfNodePtr = std::unique_ptr<PlfNode>;

/** Plural forms AST node */
struct PlfNode {
    size_t literal_val;
    PlfNodePtr a;
    PlfNodePtr b;
    PlfNodePtr c;
    PlfOp op = PlfOp::NumOps;

    auto eval( size_t n ) const -> size_t;
    auto debug_dump() const -> std::string;
};

/**
 * Parse plural rules expression and build AST.
 * @throws std::runtime_error on failure.
 */
auto parse_plural_rules( const std::string &s ) -> PlfNodePtr;

/**
 * Translation catalogue. Corresponds to single MO file.
 */
class trans_catalogue
{
    private:
        // Represents 1 entry in MO string table
        struct string_descr {
            u32 length;
            u32 offset;
        };
        struct catalogue_plurals_info {
            size_t num = 0;
            PlfNodePtr expr = nullptr;
        };
        using meta_headers = std::vector<std::string>;

        // =========== MEMBERS ===========

        bool is_little_endian = true; // File endianness
        std::string buffer; // Data buffer
        catalogue_plurals_info plurals; // Plural rules
        u32 number_of_strings = 0; // Number of strings (id-translation pairs)
        u32 offs_orig_table = 0; // Offset of table with original strings
        u32 offs_trans_table = 0; // Offset of table with translated strings

        // =========== METHODS ===========

        explicit trans_catalogue( std::string buffer );

        inline void set_buffer( std::string buffer ) {
            this->buffer = std::move( buffer );
        }
        inline auto buf_size() const -> u32 {
            return static_cast<u32>( buffer.size() );
        }

        auto get_u8( u32 offs ) const -> u8;
        inline auto get_u8_unsafe( u32 offs ) const -> u8 {
            return static_cast<u8>( buffer[offs] );
        }

        auto get_u32( u32 offs ) const -> u32;
        inline auto get_u32_unsafe( u32 offs ) const -> u32 {
            if( is_little_endian ) {
                return get_u8_unsafe( offs ) |
                       get_u8_unsafe( offs + 1 ) << 8 |
                       get_u8_unsafe( offs + 2 ) << 16 |
                       get_u8_unsafe( offs + 3 ) << 24;
            } else {
                return get_u8_unsafe( offs + 3 ) |
                       get_u8_unsafe( offs + 2 ) << 8 |
                       get_u8_unsafe( offs + 1 ) << 16 |
                       get_u8_unsafe( offs ) << 24;
            }
        }

        auto get_string_descr( u32 offs ) const -> string_descr;
        auto get_string_descr_unsafe( u32 offs ) const -> string_descr;

        inline auto offs_to_cstr( u32 offs ) const -> const char * {
            return &buffer[offs];
        }

        void process_file_header();
        void check_string_terminators();
        void check_string_plurals();
        auto get_metadata() const -> std::string;
        static void check_encoding( const meta_headers &headers );
        static auto parse_plf_header( const meta_headers &headers ) -> catalogue_plurals_info;

    public:
        /**
         * Load translation catalogue from given MO file.
         * @throws std::runtime_error on failure.
         */
        static auto load_from_file( const std::string &file_path ) -> trans_catalogue;
        /**
         * Load translation catalogue from given MO file in memory.
         * @throws std::runtime_error on failure.
         */
        static auto load_from_memory( std::string mo_file ) -> trans_catalogue;

        /** Number of entries in the catalogue. */
        inline auto get_num_strings() const -> u32 {
            return number_of_strings;
        }
        /** Get singular translated string of given entry. */
        auto get_nth_translation( u32 n ) const -> const char *;
        /** Get correct plural translated string of given entry for given number. */
        auto get_nth_pl_translation( u32 n, size_t num ) const -> const char *;
        /** Get original msgid (with msgctxt) of given entry. */
        auto get_nth_orig_string( u32 n ) const -> const char *;
        /** Check whether translated string contains plural forms. */
        auto check_nth_translation_has_plf( u32 n ) const -> bool;
};

/**
 * Translation library.
 * Represents collection of catalogues merged into a single pool ready for use.
 */
class trans_library
{
    private:
        // Describes which catalogue the string comes from
        struct library_string_descr {
            u32 catalogue;
            u32 entry;
        };

        // Full index of loaded strings
        std::vector<library_string_descr> strings;

        // Full index of loaded catalogues
        std::vector<trans_catalogue> catalogues;

        void build_string_table();
        auto find_entry( const char *id ) const -> std::vector<library_string_descr>::const_iterator;
        auto lookup_string( const char *id ) const -> const char *;
        auto lookup_pl_string( const char *id, size_t n ) const -> const char *;

    public:
        /**
         * Create new library from catalogues.
         *
         * If 2 or more catalogues have entries with same id (msgid + optional msgctxt),
         * only the entry from the first such catalogue is used.
         */
        static auto create( std::vector<trans_catalogue> catalogues ) -> trans_library;

        /**
         * @name Translation lookup
         * @param msgid singular original string
         * @param msgid_pl plural original string
         * @param msgctxt translation context
         * @param n number to choose plural form for
         * @returns If translation is found, returns translated string or correct plural form.
         *          Otherwise, returns original string or original plural string
         *          depending on n (n==1 ? msgid : msgid_pl).
         *
         * @{
         */
        auto get( const char *msgid ) const -> const char *;
        auto get_pl( const char *msgid, const char *msgid_pl, size_t n ) const -> const char *;
        auto get_ctx( const char *msgctxt, const char *msgid ) const -> const char *;
        auto get_ctx_pl( const char *msgctxt, const char *msgid, const char *msgid_pl,
                                size_t n ) const -> const char *;
        /** @} */
};
} // namespace cata_libintl

#endif // CATA_SRC_CATA_LIBINTL_H
