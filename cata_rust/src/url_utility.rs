use url::Url;

#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn github_issue_url(report: String) -> String;
        fn open_url(url: String);
        fn encode_url(url: String) -> String;
    }
}

const ORG: &str = "cataclysmbnteam";
const REPO: &str = "Cataclysm-BN";

fn github_issue_url(report: String) -> String {
    Url::parse_with_params(
        format!("https://github.com/{ORG}/{REPO}/issues/new").as_str(),
        &[
            ("labels", "bug"),
            ("template", "bug_report.yml"),
            ("versions-and-configuration", report.as_str()),
        ],
    )
    .unwrap()
    .into()
}

fn open_url(url: String) {
    webbrowser::open(&url).unwrap();
}

fn encode_url(data: String) -> String {
    Url::parse(data.as_str()).unwrap().into()
}
