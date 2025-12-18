#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
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

static std::string trim(std::string s) {
  auto notspace = [](unsigned char ch) { return !std::isspace(ch); };
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), notspace));
  s.erase(std::find_if(s.rbegin(), s.rend(), notspace).base(), s.end());
  return s;
}

static std::string lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return (char)std::tolower(c); });
  return s;
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

// Docker exec-form CMD is JSON. Escape so generated Dockerfile is always valid JSON.
static std::string json_escape(const std::string& s) {
  std::string out;
  out.reserve(s.size() + 8);
  for (unsigned char c : s) {
    switch (c) {
      case '\\': out += "\\\\"; break;
      case '"':  out += "\\\""; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        if (c < 0x20) {
          std::ostringstream oss;
          oss << "\\u" << std::hex << std::uppercase
              << std::setw(4) << std::setfill('0') << (int)c;
          out += oss.str();
        } else {
          out.push_back((char)c);
        }
    }
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

static bool looks_like_header(const std::vector<std::string>& cols) {
  if (cols.empty()) return false;
  return lower(trim(cols[0])) == "slug";
}

static bool ends_with(const std::string& s, const std::string& suf) {
  return s.size() >= suf.size() && s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

static std::string file_ext(const std::string& s) {
  auto pos = s.rfind('.');
  if (pos == std::string::npos) return "";
  return s.substr(pos); // includes '.'
}

static std::string strip_trailing_punct(std::string t) {
  while (!t.empty()) {
    char c = t.back();
    if (c == ';' || c == ',' || c == ')' || c == ']' || c == '\r' || c == '\n') t.pop_back();
    else break;
  }
  return t;
}

// basic tokenizer: whitespace split, respecting simple single/double quotes
static std::vector<std::string> shellish_split(const std::string& s) {
  std::vector<std::string> out;
  std::string cur;
  bool in_single = false, in_double = false;
  for (char c : s) {
    if (c == '\'' && !in_double) { in_single = !in_single; continue; }
    if (c == '"'  && !in_single) { in_double = !in_double; continue; }

    if (!in_single && !in_double && std::isspace((unsigned char)c)) {
      if (!cur.empty()) { out.push_back(cur); cur.clear(); }
      continue;
    }
    cur.push_back(c);
  }
  if (!cur.empty()) out.push_back(cur);
  return out;
}

static std::string normalize_filename(const std::string& tok) {
  // Strip leading ./ and any directories, keep just the leaf name.
  fs::path p(tok);
  auto leaf = p.filename().string();
  if (leaf.rfind("./", 0) == 0) leaf = leaf.substr(2);
  return leaf;
}

static std::string find_last_file_ref(const std::string& cmd, const std::string& ext) {
  if (cmd.empty() || ext.empty()) return "";
  std::string last;
  for (auto tok : shellish_split(cmd)) {
    tok = strip_trailing_punct(tok);
    tok = normalize_filename(tok);
    if (ends_with(tok, ext)) last = tok;
  }
  return last;
}

static void remove_quiet(const fs::path& p) {
  std::error_code ec;
  fs::remove(p, ec);
}

static void remove_case_insensitive_conflicts(const fs::path& dir, const std::string& target) {
  // On default macOS filesystems, changing only case can be tricky.
  // Remove any file whose lower(name) matches lower(target) but name != target.
  std::error_code ec;
  if (!fs::exists(dir, ec)) return;

  const std::string target_l = lower(target);
  for (const auto& entry : fs::directory_iterator(dir, ec)) {
    if (ec) break;
    if (!entry.is_regular_file(ec)) continue;
    const std::string name = entry.path().filename().string();
    if (lower(name) == target_l && name != target) {
      remove_quiet(entry.path());
    }
  }
  // Also remove exactly-target path if it exists (force regeneration cases).
  // (On case-insensitive FS this may remove the “other case” too, which is fine.)
  remove_quiet(dir / target);
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

    const fs::path root = fs::current_path();
    const fs::path languages_dir = root / "languages";
    fs::create_directories(languages_dir);

    std::unordered_map<std::string, size_t> h;
    bool has_header = false;

    std::string first;
    if (!std::getline(in, first)) return 0;

    auto first_cols = split_tabs(first);
    if (looks_like_header(first_cols)) {
      has_header = true;
      for (size_t i = 0; i < first_cols.size(); ++i) {
        auto key = lower(trim(first_cols[i]));
        if (!key.empty()) h[key] = i;
      }
    }

    const size_t kNoIndex = (size_t)-1;

    auto get = [&](const std::vector<std::string>& cols,
                   const std::string& name,
                   size_t fallback_index) -> std::string {
      if (has_header) {
        auto it = h.find(name);
        if (it != h.end() && it->second < cols.size()) return cols[it->second];
      }
      if (fallback_index != kNoIndex && fallback_index < cols.size()) return cols[fallback_index];
      return "";
    };

    auto process_line = [&](const std::string& raw_line) {
      std::string line = raw_line;
      if (trim(line).empty()) return;

      // allow comments in TSV
      if (!trim(line).empty() && trim(line)[0] == '#') return;

      auto cols = split_tabs(line);

      // Required-ish
      const std::string slug       = trim(get(cols, "slug",       0));
      const std::string file       = trim(get(cols, "file",       1));
      const std::string base_image = trim(get(cols, "base_image", 2));

      // Optional
      std::string install_cmd = trim(get(cols, "install_cmd", kNoIndex));
      std::string env_path    = trim(get(cols, "env_path",    kNoIndex));

      // Common
      std::string build_cmd   = trim(get(cols, "build_cmd",   3));
      std::string run_cmd     = trim(get(cols, "run_cmd",     4));
      std::string hello       = get(cols, "hello",           5);

      if (slug.empty() || file.empty() || base_image.empty() || run_cmd.empty()) {
        std::cerr << "Skipping malformed line: " << line << "\n";
        return;
      }

      // unescape fields
      install_cmd = unescape(install_cmd);
      env_path    = unescape(env_path);
      build_cmd   = unescape(build_cmd);
      run_cmd     = unescape(run_cmd);
      hello       = unescape(hello);

      // Julia PATH nudge
      if (env_path.empty() && base_image.rfind("julia:", 0) == 0) {
        env_path = "/usr/local/julia/bin";
      }

      // Determine filename to generate/copy.
      std::string effective_file = normalize_filename(file);
      const std::string ext = file_ext(effective_file);

      const std::string build_ref = normalize_filename(find_last_file_ref(build_cmd, ext));
      const std::string run_ref   = normalize_filename(find_last_file_ref(run_cmd, ext));

      if (!build_ref.empty()) effective_file = build_ref;
      else if (!run_ref.empty()) effective_file = run_ref;

      fs::path dir = languages_dir / slug;
      fs::create_directories(dir);

      // Make sure Docker build context includes the source file:
      // overwrite/replace any bad .dockerignore that might be excluding everything.
      // (Only overwrites when --force, otherwise creates if missing.)
      const std::string dockerignore =
        ".DS_Store\n"
        ".git\n"
        ".gitignore\n";
      write_file_if_needed(dir / ".dockerignore", dockerignore, force);

      // Critical macOS case-only rename handling:
      // remove conflicts, then write the file with the exact name Dockerfile expects.
      remove_case_insensitive_conflicts(dir, effective_file);
      write_file_if_needed(dir / effective_file, hello + "\n", true /* always write exact-name */);

      // Dockerfile
      std::ostringstream dockerfile;
      dockerfile
        << "# syntax=docker/dockerfile:1\n"
        << "FROM " << base_image << "\n"
        << "WORKDIR /app\n";
      if (!install_cmd.empty()) dockerfile << "RUN " << install_cmd << "\n";
      if (!env_path.empty()) dockerfile << "ENV PATH=\"" << env_path << ":$PATH\"\n";
      dockerfile << "COPY " << effective_file << " .\n";
      if (!build_cmd.empty()) dockerfile << "RUN " << build_cmd << "\n";
      dockerfile << "CMD [\"sh\", \"-c\", \"" << json_escape(run_cmd) << "\"]\n";

      write_file_if_needed(dir / "Dockerfile", dockerfile.str(), force);

      // run.sh
      std::ostringstream runsh;
      runsh
        << "#!/usr/bin/env bash\n"
        << "set -euo pipefail\n"
        << "IMG=\"hello-" << slug << "\"\n"
        << "PLATFORM=\"${POLYGLOT_PLATFORM:-}\"\n"
        << "if [ -n \"$PLATFORM\" ]; then\n"
        << "  docker build --platform \"$PLATFORM\" -t \"$IMG\" .\n"
        << "  docker run --rm --platform \"$PLATFORM\" \"$IMG\"\n"
        << "else\n"
        << "  docker build -t \"$IMG\" .\n"
        << "  docker run --rm \"$IMG\"\n"
        << "fi\n";

      write_file_if_needed(dir / "run.sh", runsh.str(), force);

      fs::permissions(dir / "run.sh",
                      fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                      fs::perm_options::add);

      std::cout << "Scaffolded: " << slug << "\n";
    };

    if (!has_header) process_line(first);

    std::string line;
    while (std::getline(in, line)) {
      process_line(line);
    }

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
}
