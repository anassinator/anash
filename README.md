anash
=====

Welcome to `anash` -- the Anass Shell.

Compiling
---------
Just run:

```bash
make
```

Installing
----------
Just run:

```bash
sudo make install
```

Running
-------
Just run:

```bash
anash
```

if you have installed the executable, or

```bash
./anash
```

otherwise.

Features
--------
- Runs any external command
- Runs any command in the background with the `&` operator
- Redirects the `STDOUT` of any command to a file with the `>` operator
- Displays list of running background jobs with `jobs` command
- Brings a background job to the foreground with the `fg` command
- Maintains the past ten commands in history (not persistent)
- Lists last ten commands with the `history` command
- Runs a past command given its associated number (e.g. `12` runs the twelveth
  command ran since `anash` was started)

TODO
----
- [ ] Command piping with the `|` operator
- [ ] `STDERR` redirection with the `2>` and `&>` operators
- [ ] Append output redirection with the `>>` operator
- [ ] Allow logical command chaining with the `&&` and `||` operators
- [ ] Check for background tasks in a separate thread
- [ ] Persistent history in stored in file
- [ ] Run an `.anashrc` on startup
- [ ] Proper distinction of string literals denoted by `'` or `"`
- [ ] Allow prompt customizability with the `PS1` environment variable
- [ ] Allow local exports with the `export` and `local` command
- [ ] Allow basic `anash` scripting with functions, if/else's, and the like
- [ ] Syntax highlighting
- [ ] Built-in `sl` command
- [ ] World domination
- [ ] Backdoor
