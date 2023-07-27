# Tokenization in C

### Tokenization refers to a process by which a piece of sensitive data, such as a credit card number, is replaced by a surrogate value known as a token. The sensitive data still generally needs to be stored securely at one centralized location for subsequent reference and requires strong protections around it. The security of a tokenization approach depends on the security of the sensitive values and the algorithm and process used to create the surrogate value and map it back to the original value.

THIS REPOSITORY CONTAINS IMPLEMENTATION OF TOKENIZATION IN C

The [Makefile](Makefile) contains the following targets:

- `make all` - compile everything
- `make tokenize` - compile the tokenizer demo
- `make tokenize-tests` - compile the tokenizer demo
- `make shell` - compile the shell
- `make shell-tests` - run a few tests against the shell
- `make test` - compile and run all the tests
- `make clean` - perform a minimal clean-up of the source tree


The [examples](examples/) directory contains an example tokenizer. It might help.
