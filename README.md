# Vhagar

Timetable:

|  Name | Phase 1 | Phase 2         | Phase 3           | Phase 4 | Phase 5 |
| --    | --      | --              | --                | --      | --      |
| Jonah | Main    | Parser          | Tests             | SCCP    | Strength Reduction |
| Tarik | Lexer   | Tests           | Lowering, LLVM    | LICM    | Tests   |
| Teodora | Tests   | Pretty Printing | Semantic Analysis | Tests   | Inliner |


## Lexical Analysis

The Lexer is implemented as a state machine.
When the function `nextToken()` is invoked the state machine starts in the first state
and moves character by character thru the state machine until a final state has been reached.
The final state corresponds to which kind of token has been read.
After the state-machine there is a big switch statement which allocates the correct token.

```C
struct Token {
  uint32_t kind    :  8;
  uint32_t offset  : 24;
  uint32_t content : 32;
};
```

Our tokens use up only 8 bytes because both the string content and the location have been outsourced
to their own data structures.
We deduplicate string data of e.g. identifiers, integer constants, string literals etc.
by inserting them into a hash table and storing only a key into this table.
The `Locator` class is responsible for recomputing the location of a token based on its offset into the string.

_This lexer was heavily inspired by [this](https://nothings.org/computer/lexing.html) blog post from Sean Barret._

## Abstract Syntax Tree
### Nodes

For efficiently storing the children, we are using a `std::variant`.
This allows us to only use heap-allocated memory when we have to store the children of a block. This will improve our memory consumptions greatly hopefully.

```cpp
using none_child_t = std::tuple<>;
using unary_children_t = node_t;
using binary_children_t = std::tuple<node_t, node_t>;
using ternary_children_t = std::tuple<node_t, node_t, node_t>;
using quaternary_children_t = std::tuple<node_t, node_t, node_t, node_t>;
using nary_children_t = struct AnatyChildren { ... }

using children_t = std::variant<
    none_child_t,
    unary_children_t,
    binary_children_t,
    quaternary_children_t,
    nary_children_t>;
```

Below you will find a list of all `NodeKinds` and what children type they are using:

| NodeKind | Children Type      | Assignment                       |
| -------- | ------------------ | -------------------------------- |
| ASSIGN   | binary_children_t  | identifier, expression           |
| BLOCK    | anary_children_t   | $st_1$, $st_2$, $\ldots$, $st_n$ |
| IF       | ternary_children_t | $cond$, $st_{\top}$, $st_{\bot}$ |
| WHILE    | binary_children_t  | $cond$, $st$                     |
| VAR      | unary_children_t   | identifier                       |
| INDIR    | unary_children_t   | $e$                              |
| CONST    | unary_children_t   | $e$                              |
| BINARY   | binary_children_t  | $e_1$, $e_2$                     |
| UNARY    | unary_children_t   | $e$                              |
| ADDR     | unary_children_t   | l-value                          |


## resources:

- https://alic.dev/blog/fast-lexing
- https://nothings.org/computer/lexing.html
- https://github.com/simdjson/simdjson
- https://youtu.be/ZI198eFghJk?si=o6IWVItLoH0NZ7eJ
- https://repository.tudelft.nl/file/File_17e6e3b0-07ff-4e54-8700-e914e678dbc2
- https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl03.html
- https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/basic-constructs/index.html
