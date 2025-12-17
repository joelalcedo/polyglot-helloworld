#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static std::vector<std::string> split_tabs(const std::string& s) {
  std::vector<std::string> out;
  std::string cur;
  for (char c : s) {
    if (c == '\t') { out.push_back(cur); cur.clear(); }
    else { cur.push_back(c); }
  }
  out.push_back(cur);
  return out;
}

static std::string unescape(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '\\' && i + 1 < s.size()) {
      char n = s[i + 1];
      if (n == 'n') { out.push_back('\n'); ++i; continue; }
      if (n == 't') { out.push_back('\t'); ++i; continue; }
      if (n == '\\') { out.push_back('\\'); ++i; continue; }
    }
    out.push_back(s[i]);
  }
  return out;
}

static bool write_file_if_needed(const fs::path& p, const std::string& content, bool force) {
  if (fs::exists(p) && !force) return false;
  std::ofstream f(p, std::ios::binary);
  if (!f) throw std::runtime_error("Failed to write: " + p.string());
  f << content;
  return true;
}

int main(int argc, char** argv) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: scaffold <languages.tsv> [--force]\n";
      return 2;
    }

    const fs::path manifest = argv[1];
    const bool force = (argc >= 3 && std::string(argv[2]) == "--force");

    std::ifstream in(manifest);
    if (!in) {
      std::cerr << "Cannot open manifest: " << manifest << "\n";
      return 2;
    }

    std::string header;
    std::getline(in, header); // slug, file, base_image, build_cmd, run_cmd, hello

    const fs::path root = fs::current_path();
    const fs::path languages_dir = root / "languages";
    fs::create_directories(languages_dir);

    std::string line;
    while (std::getline(in, line)) {
      if (line.empty()) continue;
      auto cols = split_tabs(line);
      if (cols.size() < 6) {
        std::cerr << "Skipping malformed line (need 6 columns): " << line << "\n";
        continue;
      }

      const std::string slug = cols[0];
      const std::string file = cols[1];
      const std::string base_image = cols[2];
      const std::string build_cmd = cols[3];
      const std::string run_cmd = cols[4];
      const std::string hello = unescape(cols[5]);

      fs::path dir = languages_dir / slug;
      fs::create_directories(dir);

      // hello source
      write_file_if_needed(dir / file, hello + "\n", force);

      // Dockerfile
      std::ostringstream dockerfile;
      dockerfile
        << "FROM " << base_image << "\n"
        << "WORKDIR /app\n"
        << "COPY " << file << " .\n";
      if (!build_cmd.empty()) dockerfile << "RUN " << build_cmd << "\n";
      dockerfile << "CMD [\"sh\", \"-lc\", \"" << run_cmd << "\"]\n";

      write_file_if_needed(dir / "Dockerfile", dockerfile.str(), force);

      // run.sh
      std::ostringstream runsh;
      runsh
        << "#!/usr/bin/env bash\n"
        << "set -euo pipefail\n"
        << "IMG=\"hello-" << slug << "\"\n"
        << "docker build -t \"$IMG\" .\n"
        << "docker run --rm \"$IMG\"\n";

      write_file_if_needed(dir / "run.sh", runsh.str(), force);

      // make run.sh executable
      fs::permissions(dir / "run.sh",
                      fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                      fs::perm_options::add);

      std::cout << "Scaffolded: " << slug << "\n";
    }

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
}
