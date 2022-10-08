#pragma once
#ifndef CATA_SRC_FILESYSTEM_H
#define CATA_SRC_FILESYSTEM_H

#include <string>
#include <vector>

/**
 * Create directory if it does not exist.
 * @return true if directory exists or was successfully created.
 */
auto assure_dir_exist( const std::string &path ) -> bool;
/**
 * Check if directory exists.
 * @return false if directory does not exist or if unable to check.
 */
auto dir_exist( const std::string &path ) -> bool;
/**
 * Check if file exists.
 * @return false if file does not exist or if unable to check.
 */
auto file_exist( const std::string &path ) -> bool;
/**
 * Remove a file. Does not remove directories.
 * @return true on success.
 */
auto remove_file( const std::string &path ) -> bool;
/**
 * Remove an empty directory.
 * @return true on success, false on failure (e.g. directory is not empty).
 */
auto remove_directory( const std::string &path ) -> bool;
/**
 * Rename a file, overwriting the target. Does not overwrite directories.
 * @return true on success, false on failure.
 */
auto rename_file( const std::string &old_path, const std::string &new_path ) -> bool;
/**
 * Check if can write to the given directory (write permission, disk space).
 * @return false if cannot write or if unable to check.
 */
auto can_write_to_dir( const std::string &dir_path ) -> bool;
/**
 * Copy file, overwriting the target. Does not overwrite directories.
 * @return true on success, false on failure.
 */
auto copy_file( const std::string &source_path, const std::string &dest_path ) -> bool;
/** Get process id string. Used for temporary file paths. */
auto get_pid_string() -> std::string;

/**
 * Read entire file to string.
 * @return empty string on failure.
 */
auto read_entire_file( const std::string &path ) -> std::string;

namespace cata_files
{
auto eol() -> const char *;
} // namespace cata_files

//--------------------------------------------------------------------------------------------------
/**
 * Returns a vector of files or directories matching pattern at @p root_path.
 *
 * Searches through the directory tree breadth-first. Directories are searched in lexical
 * order. Matching files within in each directory are also ordered lexically.
 *
 * @param pattern The sub-string to match.
 * @param root_path The path relative to the current working directory to search; empty means ".".
 * @param recursive_search Whether to recursively search sub directories.
 * @param match_extension If true, match pattern at the end of file names. Otherwise, match anywhere
 *                        in the file name.
 */
auto get_files_from_path( const std::string &pattern,
        const std::string &root_path = "", bool recursive_search = false,
        bool match_extension = false ) -> std::vector<std::string>;

//--------------------------------------------------------------------------------------------------
/**
 * Returns a vector of directories which contain files matching any of @p patterns.
 *
 * @param patterns A vector or patterns to match.
 * @see get_files_from_path
 */
auto get_directories_with( const std::vector<std::string> &patterns,
        const std::string &root_path = "", bool recursive_search = false ) -> std::vector<std::string>;

auto get_directories_with( const std::string &pattern,
        const std::string &root_path = "", bool recursive_search = false ) -> std::vector<std::string>;

/**
 *  Replace invalid characters in a string with a default character; can be used to ensure that a file name is compliant with most file systems.
 *  @param file_name Name of the file to check.
 *  @return A string with all invalid characters replaced with the replacement character, if any change was made.
 *  @note  The default replacement character is space (0x20) and the invalid characters are "\\/:?\"<>|".
 */
auto ensure_valid_file_name( const std::string &file_name ) -> std::string;

#endif // CATA_SRC_FILESYSTEM_H
