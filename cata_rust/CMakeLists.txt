include(FetchContent)

FetchContent_Declare(
    Corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG v0.4
)
FetchContent_MakeAvailable(Corrosion)

set(cata_rust_target "cata_rust")

corrosion_import_crate(
    MANIFEST_PATH "${CMAKE_SOURCE_DIR}/cata_rust/Cargo.toml"
)

# We need the build dir because cxx puts our headers in there.
# Corrosion doesn't expose the build dir, so poke where we shouldn't.
# if(Rust_CARGO_TARGET)
#     set(rust_target_dir "${CMAKE_BINARY_DIR}/cargo/build/${_CORROSION_RUST_CARGO_TARGET}")
# else()
#     set(rust_target_dir "${CMAKE_BINARY_DIR}/cargo/build/${_CORROSION_RUST_CARGO_HOST_TARGET}")
#     corrosion_set_hostbuild(${cata_rust_target})
# endif()

if(CMAKE_CONFIGURATION_TYPES)
    set (rust_target_dir "${CMAKE_BINARY_DIR}/$<CONFIG>/cargo/build/${_CORROSION_RUST_CARGO_TARGET}")
else()
    set (rust_target_dir "${CMAKE_BINARY_DIR}/cargo/build/${_CORROSION_RUST_CARGO_TARGET}")
endif()

corrosion_set_env_vars(${cata_rust_target}
    "CATA_BUILD_DIR=${CMAKE_BINARY_DIR}"
    "CATA_RUST_TARGET_DIR=${rust_target_dir}"
)

target_include_directories(${cata_rust_target} INTERFACE
    "${rust_target_dir}/cxxbridge/${cata_rust_target}/src/"
)

# Tell fish what extra C++ files to compile.
define_property(
    TARGET PROPERTY fish_extra_cpp_files
    BRIEF_DOCS "Extra C++ files to compile for fish."
    FULL_DOCS "Extra C++ files to compile for fish."
)
