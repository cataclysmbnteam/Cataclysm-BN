fn main() -> miette::Result<()> {
    let rust_dir = std::env::var("CARGO_MANIFEST_DIR").expect("Env var CARGO_MANIFEST_DIR missing");
    let target_dir =
        std::env::var("CATA_RUST_TARGET_DIR").unwrap_or(format!("{}/{}", rust_dir, "target/"));

    let cata_src_dir = format!("{}/{}", rust_dir, "../src/");

    // Where cxx emits its header.
    let cxx_include_dir = format!("{}/{}", target_dir, "cxxbridge/rust/");

    // If CATA_BUILD_DIR is given by CMake, then use it; otherwise assume it's at ../build.
    let cata_build_dir =
        std::env::var("CATA_BUILD_DIR").unwrap_or(format!("{}/{}", rust_dir, "../build/"));

    // Emit cxx junk.
    // This allows "Rust to be used from C++"
    let source_files = vec![
        "src/url_utility.rs"
    ];
    cxx_build::bridges(&source_files)
        .flag_if_supported("-std=c++17")
        .include(&cata_src_dir)
        .include(&cata_build_dir) // For config.h
        .include(&cxx_include_dir) // For cxx.h
        .compile("cata_rust");

    for file in source_files {
        println!("cargo:rerun-if-changed={file}");
    }

    Ok(())
}
