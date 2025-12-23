# Polyglot Hello World 🌍👋

This repo is my attempt to do something mildly unhinged but deeply satisfying: **run `Hello World` in as many programming languages as possible, using a single, repeatable framework**.

I fell down this rabbit hole after stumbling across the *Hello World* collection around the internet (see: http://helloworldcollection.de). One thing led to another, and I decided I didn’t just want to *read* these snippets — I wanted to **execute every single one**.

So here we are.

![updated_video_small](https://github.com/user-attachments/assets/af5cd7ad-8017-4e1d-98ef-540b387dcaf4)

---

## What this is (high level)

This project is a **language-agnostic execution framework**:

* One language definition file
* One code generator
* One command
* “Hello World” being invoked back at you

All isolated. All reproducible. All Dockerized.

---

## What platforms does this work on?

If you can run **Docker**, you’re good.

* ✅ macOS (Intel + Apple Silicon)
* ✅ Linux
* ✅ Windows (via Docker Desktop / WSL2)

The host OS doesn’t matter. Everything runs inside containers. Your system stays clean. Your sanity mostly intact.

---

## How it works (the moving pieces)

Here’s the architecture, from boring to fun:

```
┌─────────────────┐
│  languages.tsv  │  ← single source of truth containing language commands, Docker install and other relevant information
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  scaffold.cpp   │  ← generates Dockerfiles, scripts, folders
└────────┬────────┘
         │
         ▼
┌────────────────────────────────────┐
│ language-specific Docker containers│
└────────┬───────────────────────────┘
         │
         ▼
┌─────────────────┐
│   run_all.sh    │  ← one command to execute all languages
└─────────────────┘
```

### 1. `languages.tsv`

A TSV file that does all the heavy lifting.

Each row defines:

* The language
* The base Docker image
* How to install what’s needed
* How to compile / interpret
* Language specific script to run `Hello World`

Add a row → get a new language.

### 2. `scaffold.cpp`

This is the factory.

It:

* Reads the TSV
* Creates a directory per language
* Writes Dockerfiles
* Writes run scripts
* Writes language-specific "hello world" script
* Keeps everything consistent

### 3. `run_all.sh`

The fun part.

This script loops through every generated directory and executes the program inside its container.

Result:

```
Hello World (C)
Hello World (Rust)
Hello World (COBOL)
Hello World (…why does this exist)
```

Over. And over. And over.

---

## Why Docker?

Because the alternative is suffering.

Docker gives us:

* Isolation
* Reproducibility
* Zero dependency hell
* The ability to run *Shakespeare* without ruining your weekend

Which brings me to…

---

## 🏆 The Shakespeare Bounty

Yes. **That Shakespeare.**

The Shakespeare Programming Language technically supports `Hello World`.

Practically? It has defeated me.

💰 **Bounty:** Eternal gratitude + public credit + bragging rights

If you can:

* Produce a **working** Shakespeare `Hello World`
* Integrate it cleanly into this framework

You win.

PRs welcome. Mockery optional.

---

## Contributing

Contributions are very welcome.

Ways to help:

* Add new languages
* Improve existing definitions
* Clean up output formatting
* Fix the things I’m pretending are “features”
* Open an issue.

---

## Roadmap / To‑Dos

### ✅ Completed

* [x] Core architecture
* [x] TSV-driven language definitions
* [x] Docker-based execution per language
* [x] Single-command execution (`run_all.sh`)
* [x] 50 languages supported
* [x] Improve terminal output formatting (grouping, colors, etc.)

### 🚧 In Progress / Planned

* [ ] Expand to **100+ languages**
* [ ] Expand to **200+ languages**
* [ ] Expand to **300+ languages** (because why not)
* [ ] Generalize the framework so it can deploy *any* mix of programs, not just Hello World
* [ ] Better error reporting when a language inevitably breaks
* [ ] Optional parallel execution (for maximum chaos)

---

## Final thoughts

This project is equal parts:

* Curiosity
* Engineering
* Self-inflicted pain

If you’ve ever wondered *“does this language actually work?”* — now you can find out with a single command.

If you add something weird, wonderful, or cursed: thank you.

And if you solve Shakespeare… we need to talk.

— Joel
