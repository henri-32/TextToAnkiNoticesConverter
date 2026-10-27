# Description

A minimalistic parser that converts `.md` or `.txt` files into Anki notes.

It is not intended to manage the Anki database. Its sole purpose is to eliminate the repetitive keystrokes required to create individual notes in Anki, allowing you to maintain your Neovim workflow and work entirely from the keyboard.

# Setup

Build the program using:

```bash
make
```

AnkiConnect must be installed, and Anki must be running in the background.

# Usage

There are only two command-line options:

```bash
anki_converter --file <filename> --deck <deckname>
```

Both options also have short forms (`-f` and `-d`).

The program checks whether the specified deck exists in the local Anki database. If it does not, the deck is created automatically.

## Syntax

A line starting with `Q:` marks the beginning of a question. The `?` character marks its end.

Similarly, `A:` marks the beginning of an answer, and `!` marks its end.

Both questions and answers can span multiple lines.

Example:

```text
Q: Is this a valid question?
A: Yes, it is!

Q: What do multiline questions look like
if you have a lot to ask?

A: Just like that!
```

The `!` as well as "Q:" and "A:" are stripped from the answer, as it is only used as a delimiter. Any text following the terminating `?` or `!` is also discarded.

# Limitations

As mentioned above, the intended use case is to write a collection of `Q:` and `A:` pairs in a text file and convert them into Anki notes.

The program is deliberately minimalistic and is not intended to serve as an Anki plugin or a database management tool.

Currently, the AnkiConnect API rejects requests containing notes that already exist in the database.
