# Markdown to HTML Compiler

A small compiler written in C++17 that turns a Markdown file into an HTML page.
It works like a real compiler: it reads the input, builds a tree (AST), checks it,
and then generates HTML from that tree.

## Project structure

```
src/      source code of the compiler
demo/     sample Markdown files
mdc       the compiled program
```

## Build

A ready-made `mdc` (built on macOS, Apple Silicon) is included, so you can run it directly.
If it does not run on your computer, build it yourself:

```sh
g++ -std=c++17 -o mdc src/*.cpp
```

## Run

```sh
./mdc input.md [-o out.html] [--tokens] [--ast]
```

| Option | What it does |
|---|---|
| (none) | Writes `input.html` next to `input.md` |
| `-o out.html` | Writes the HTML to `out.html` |
| `--tokens` | Prints the tokens of every block |
| `--ast` | Prints the syntax tree |

Example:

```sh
./mdc demo/chat_example.md
```

Input:

```markdown
# Intro
This is **bold** and *italic* with `code`.
- item one
- item [link](#intro)
```

Output (inside `<body>`):

```html
<h1 id="intro">Intro</h1>
<p>This is <strong>bold</strong> and <em>italic</em> with <code>code</code>.</p>
<ul>
<li>item one</li>
<li>item <a href="#intro">link</a></li>
</ul>
```

## Supported Markdown

- Headings `#` to `######`
- Paragraphs
- Bullet lists (`-`, `*`) and numbered lists (`1.`)
- Code blocks with ```` ``` ````
- **Bold**, *italic* and `inline code`
- Links `[text](url)` and images `![alt](src)`

## How it works

| Step | File | Job |
|---|---|---|
| 1. Block parser | `src/block_parser.cpp` | Reads the file line by line and finds headings, paragraphs, lists and code blocks |
| 2. Inline lexer | `src/inline_lexer.cpp` | Splits text into tokens like `**`, `*`, `` ` ``, `[`, `]` |
| 3. Inline parser | `src/inline_parser.cpp` | Recursive descent parser that builds bold, italic, code, link and image nodes |
| 4. AST | `src/ast.cpp` | The tree that all steps share |
| 5. Semantic check | `src/semantic.cpp` | Gives headings ids and warns about broken `#links` and duplicate headings |
| 6. HTML generator | `src/html_gen.cpp` | Walks the tree and writes the HTML |
| Driver | `src/main.cpp` | Reads options and runs the steps in order |

Warnings are printed as `file:line: warning: message` and do not stop the compiler.

## Limitations

- No nested lists, tables, blockquotes or raw HTML
- No `_underscore_` emphasis
- Lines of a paragraph are joined with a space
- Heading ids keep only `a-z`, `0-9` and `-`

## Demo

```sh
./mdc demo/chat_example.md --tokens
./mdc demo/chat_example.md --ast
./mdc demo/readme_sample.md
./mdc demo/warnings.md
```

Open `demo/readme_sample.html` in a browser to see the result.
