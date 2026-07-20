# Core Testament Framework

> **Note:** This is an evaluation project. No public release has been published yet.

Core Testament is a macro-free C++23 test framework with compile-time-validated
test definitions and automatic static registration before `main()`.

## Documentation

- [Documentation overview and API guide](docs/README.md)
- [Getting-started tutorial](docs/tutorial/README.md)

The tutorial builds a small test executable from scratch. The documentation
overview covers fixtures, options, registration, multiple translation units,
reporters and package consumption in more detail.

## Quickstart

The quickstart builds the Podman image and runs the complete build and test suite:

```bash
./quickstart.sh
```

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for code style, tests and Conventional
Commit conventions.

## License

This project is licensed under the [MIT License](LICENSE).
