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

// Unescape TSV fields (supports \n \t \r \\ \" \')
static std::string unescape(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '\\' && i + 1 < s.size()) {
      char n = s[i + 1];
      if (n == 'n')  { out.push_back('\n'); ++i; continue; }
      if (n == 't')  { out.push_back('\t'); ++i; continue; }
      if (n == 'r')  { out.push_back('\r'); ++i; continue; }
      if (n == '\\') { out.push_back('\\'); ++i; continue; }
      if (n == '"')  { out.push_back('"');  ++i; continue; }
      if (n == '\'') { out.push_back('\''); ++i; continue; }
    }
    out.push_back(s[i]);
  }
  return out;
}

static std::string strip_utf8_bom(std::string s) {
  // UTF-8 BOM: EF BB BF
  if (s.size() >= 3 &&
      (unsigned char)s[0] == 0xEF &&
      (unsigned char)s[1] == 0xBB &&
      (unsigned char)s[2] == 0xBF) {
    s.erase(0, 3);
  }
  return s;
}

static bool ends_with_nl(const std::string& s) {
  return !s.empty() && (s.back() == '\n');
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

static std::string read_file_or_empty(const fs::path& p) {
  std::ifstream f(p, std::ios::binary);
  if (!f) return "";
  std::ostringstream oss;
  oss << f.rdbuf();
  return oss.str();
}

// Writes if missing OR content differs (durable; avoids needing --force).
static bool write_file_if_changed(const fs::path& p, const std::string& content) {
  std::string existing = read_file_or_empty(p);
  if (!existing.empty() && existing == content) return false;

  std::ofstream f(p, std::ios::binary);
  if (!f) throw std::runtime_error("Failed to write: " + p.string());
  f << content;
  return true;
}

// Keep --force semantics for people who want to blast everything,
// but the default behavior now still updates when content differs.
static bool write_file(const fs::path& p, const std::string& content, bool force) {
  if (force) {
    std::ofstream f(p, std::ios::binary);
    if (!f) throw std::runtime_error("Failed to write: " + p.string());
    f << content;
    return true;
  }
  return write_file_if_changed(p, content);
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
  remove_quiet(dir / target);
}

struct LangSpec {
  std::string slug;
  std::string file;
  std::string base_image;
  std::string install_cmd;
  std::string env_path;
  std::string build_cmd;
  std::string run_cmd;
  std::string hello;
};

static bool icontains(const std::string& hay, const std::string& needle) {
  auto h = lower(hay);
  auto n = lower(needle);
  return h.find(n) != std::string::npos;
}

static void replace_all(std::string& s, const std::string& from, const std::string& to) {
  if (from.empty()) return;
  size_t pos = 0;
  while ((pos = s.find(from, pos)) != std::string::npos) {
    s.replace(pos, from.size(), to);
    pos += to.size();
  }
}

static void ensure_contains_pkg(std::string& install_cmd, const std::string& pkg_token) {
  // Very light heuristic: only add if not already present.
  if (icontains(install_cmd, pkg_token)) return;
  // Try to insert right after "--no-install-recommends" if present.
  auto key = std::string("--no-install-recommends");
  auto p = install_cmd.find(key);
  if (p != std::string::npos) {
    p += key.size();
    install_cmd.insert(p, " " + pkg_token);
  } else {
    // Otherwise just append token (works for simple one-line installs)
    install_cmd += " " + pkg_token;
  }
}

// Durable fixups so you don't hand-edit Dockerfiles.
static void apply_fixups(LangSpec& s) {
  // COBOL:
  // Your source is free-format (starts in column 1). GnuCOBOL defaults to fixed-format,
  // which causes "invalid indicator ... at column 7". Add `-free` to cobc.
  if (s.slug == "cobol") {
    if (icontains(s.build_cmd, "cobc") && !icontains(s.build_cmd, "-free")) {
      // Insert "-free" after "cobc" token.
      // Handles: "cobc -x ..." and "cobc ...".
      replace_all(s.build_cmd, "cobc -", "cobc -free -");
      if (s.build_cmd.rfind("cobc ", 0) == 0 && !icontains(s.build_cmd, "cobc -free")) {
        // If it was "cobc hello.cob ..." (no flags), just prefix.
        replace_all(s.build_cmd, "cobc ", "cobc -free ");
      }
      // If still not present (weird formatting), append as last resort.
      if (!icontains(s.build_cmd, "-free")) s.build_cmd = "cobc -free " + s.build_cmd.substr(5);
    }
  }

  // Emojicode:
  // Don’t splice into the user heredoc (it’s easy to break "\" continuations).
  // Instead, normalize to Ubuntu 20.04 and replace install_cmd with a robust heredoc
  // that installs LLVM/Clang 8 properly and builds emojicode.
  if (s.slug == "emojicode") {
    s.base_image = "ubuntu:20.04";
    s.env_path   = "/usr/local/bin";

    s.install_cmd =
      "<<'EOF'\n"
      "set -e\n"
      "export DEBIAN_FRONTEND=noninteractive\n"
      "apt-get update\n"
      "\n"
      "# Toolchain + deps\n"
      "apt-get install -y --no-install-recommends \\\n"
      "  ca-certificates \\\n"
      "  build-essential \\\n"
      "  cmake \\\n"
      "  git \\\n"
      "  libffi-dev \\\n"
      "  libedit-dev \\\n"
      "  zlib1g-dev \\\n"
      "  clang-8 \\\n"
      "  llvm-8 \\\n"
      "  llvm-8-dev \\\n"
      "  llvm-8-tools\n"
      "\n"
      "rm -rf /var/lib/apt/lists/*\n"
      "\n"
      "# Ensure v8 tools are the defaults (only if the paths exist)\n"
      "if [ -x /usr/bin/llvm-config-8 ]; then\n"
      "  update-alternatives --install /usr/bin/llvm-config llvm-config /usr/bin/llvm-config-8 100 || true\n"
      "fi\n"
      "if [ -x /usr/bin/clang-8 ]; then\n"
      "  update-alternatives --install /usr/bin/clang clang /usr/bin/clang-8 100 || true\n"
      "fi\n"
      "if [ -x /usr/bin/clang++-8 ]; then\n"
      "  update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-8 100 || true\n"
      "fi\n"
      "\n"
      "# Build emojicode\n"
      "git clone --depth=1 https://github.com/emojicode/emojicode.git /tmp/emojic\n"
      "mkdir -p /tmp/emojic/build\n"
      "cd /tmp/emojic/build\n"
      "\n"
      "LLVM_DIR=\"$(llvm-config --cmakedir 2>/dev/null || true)\"\n"
      "if [ -z \"$LLVM_DIR\" ]; then\n"
      "  LLVM_DIR=\"$(llvm-config --prefix)/lib/cmake/llvm\"\n"
      "fi\n"
      "\n"
      "cmake -DLLVM_DIR=\"$LLVM_DIR\" ..\n"
      "make -j\"$(nproc)\"\n"
      "make install\n"
      "rm -rf /tmp/emojic\n"
      "EOF";
  }

  // Julia PATH nudge
  if (s.env_path.empty() && s.base_image.rfind("julia:", 0) == 0) {
    s.env_path = "/usr/local/julia/bin";
  }
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
      if (!trim(line).empty() && trim(line)[0] == '#') return;

      auto cols = split_tabs(line);

      LangSpec spec;
      spec.slug       = trim(get(cols, "slug",       0));
      spec.file       = trim(get(cols, "file",       1));
      spec.base_image = trim(get(cols, "base_image", 2));

      spec.install_cmd = trim(get(cols, "install_cmd", kNoIndex));
      spec.env_path    = trim(get(cols, "env_path",    kNoIndex));

      spec.build_cmd   = trim(get(cols, "build_cmd",   3));
      spec.run_cmd     = trim(get(cols, "run_cmd",     4));
      spec.hello       = get(cols, "hello",           5);

      if (spec.slug.empty() || spec.file.empty() || spec.base_image.empty() || spec.run_cmd.empty()) {
        std::cerr << "Skipping malformed line: " << line << "\n";
        return;
      }

      // Unescape + strip BOMs
      spec.slug        = strip_utf8_bom(unescape(spec.slug));
      spec.file        = strip_utf8_bom(unescape(spec.file));
      spec.base_image  = strip_utf8_bom(unescape(spec.base_image));
      spec.install_cmd = strip_utf8_bom(unescape(spec.install_cmd));
      spec.env_path    = strip_utf8_bom(unescape(spec.env_path));
      spec.build_cmd   = strip_utf8_bom(unescape(spec.build_cmd));
      spec.run_cmd     = strip_utf8_bom(unescape(spec.run_cmd));
      spec.hello       = strip_utf8_bom(unescape(spec.hello));

      // Apply durable fixups
      apply_fixups(spec);

      // Determine filename to generate/copy.
      std::string effective_file = normalize_filename(spec.file);
      const std::string ext = file_ext(effective_file);

      const std::string build_ref = normalize_filename(find_last_file_ref(spec.build_cmd, ext));
      const std::string run_ref   = normalize_filename(find_last_file_ref(spec.run_cmd, ext));

      if (!build_ref.empty()) effective_file = build_ref;
      else if (!run_ref.empty()) effective_file = run_ref;

      fs::path dir = languages_dir / spec.slug;
      fs::create_directories(dir);

      // Ensure build context isn't accidentally excluding everything.
      const std::string dockerignore =
        ".DS_Store\n"
        ".git\n"
        ".gitignore\n";
      write_file(dir / ".dockerignore", dockerignore, force);

      // macOS case-only rename handling:
      remove_case_insensitive_conflicts(dir, effective_file);

      // Ensure hello ends with newline
      std::string hello_content = spec.hello;
      if (!ends_with_nl(hello_content)) hello_content.push_back('\n');
      write_file(dir / effective_file, hello_content, true /* always write exact-name */);

      // Dockerfile
      std::ostringstream dockerfile;
      dockerfile
        << "# syntax=docker/dockerfile:1\n"
        << "FROM " << spec.base_image << "\n"
        << "WORKDIR /app\n";

      if (!spec.install_cmd.empty()) {
        std::string trimmed_install = trim(spec.install_cmd);
        if (trimmed_install.rfind("<<", 0) == 0) {
          dockerfile << "RUN " << trimmed_install << "\n";
        } else {
          dockerfile << "RUN " << spec.install_cmd << "\n";
        }
      }

      if (!spec.env_path.empty())
        dockerfile << "ENV PATH=\"" << spec.env_path << ":$PATH\"\n";

      dockerfile << "COPY " << effective_file << " .\n";
      if (!spec.build_cmd.empty()) dockerfile << "RUN " << spec.build_cmd << "\n";
      dockerfile << "CMD [\"sh\", \"-c\", \"" << json_escape(spec.run_cmd) << "\"]\n";

      write_file(dir / "Dockerfile", dockerfile.str(), force);

      // run.sh
      std::ostringstream runsh;
      runsh
        << "#!/usr/bin/env bash\n"
        << "set -euo pipefail\n"
        << "IMG=\"hello-" << spec.slug << "\"\n"
        << "PLATFORM=\"${POLYGLOT_PLATFORM:-}\"\n"
        << "if [ -n \"$PLATFORM\" ]; then\n"
        << "  docker build --platform \"$PLATFORM\" -t \"$IMG\" .\n"
        << "  docker run --rm --platform \"$PLATFORM\" \"$IMG\"\n"
        << "else\n"
        << "  docker build -t \"$IMG\" .\n"
        << "  docker run --rm \"$IMG\"\n"
        << "fi\n";

      write_file(dir / "run.sh", runsh.str(), force);

      fs::permissions(dir / "run.sh",
                      fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                      fs::perm_options::add);

      std::cout << "Scaffolded: " << spec.slug << "\n";
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
