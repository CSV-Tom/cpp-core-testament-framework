# Contributing to Core Testament Framework

Thanks for your interest in contributing! This document explains the conventions used in this codebase and how to submit changes.

## Code style

- No macros in test definitions — this is a core design goal of the   framework, not just a style preference.
- Prefer modern C++23 idioms over legacy patterns.
- Format your code consistently with the rest of the codebase before committing.

## Tests

New functionality should come with tests. Make sure the existing test suite still passes before submitting a change.

## Commit messages

This project follows [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/#specification).
Each commit message should be structured as:

```text
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

**Common types used in this repository:**

- `feat` — a new feature
- `fix` — a bug fix
- `refactor` — a code change that neither fixes a bug nor adds a feature
- `docs` — documentation-only changes (README, CHANGELOG, this file, ...)
- `test` — adding or correcting tests
- `build` / `chore` — build system, CI, container/toolchain, or tooling changes
- `style` — formatting-only changes

**Notes:**

- Use `!` after the type/scope (e.g. `feat!:`) or a `BREAKING CHANGE:` footer for breaking changes.
- Keep the description short and imperative ("add", not "added" or "adds").
- Scope your commit to a single logical change; avoid bundling unrelated changes into one commit.

## Submitting changes

1. Fork the repository and create a feature branch.
2. Make your changes and verify the test suite still passes.
3. Update `CHANGELOG.md` with a short entry describing your change.
4. Open a pull request describing the change and its motivation.

## License

By contributing, you agree that your contributions will be licensed under
the project's [MIT License](LICENSE).
