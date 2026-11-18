# SPDX-License-Identifier: Apache-2.0 OR MIT
#
# Rendered by `make demo` (vhs, https://github.com/charmbracelet/vhs) from
# the repository root, after it has built examples/random_draws.cpp into
# build-demo/. On screen, the example run twice: the library version, 16
# random bytes from the operating system's CSPRNG as hex, and five die
# rolls from random_int(6). The values differ between the runs because
# they are fresh draws. Nothing leaves the machine.
Output .github/demo.gif

Set Shell "bash"
Set FontSize 14
Set Width 1200
Set Height 420
Set Padding 20
Set Theme "Catppuccin Mocha"
Set WaitTimeout 60s
Set WaitPattern /^\$ ?$/

Hide
Type `export PATH="$PWD/build-demo:$PATH" PS1='$ ' && clear`
Enter
Sleep 500ms
Show

Type "jspassgen_random_draws"
Sleep 500ms
Enter
Wait
Sleep 3s

Type "jspassgen_random_draws"
Sleep 500ms
Enter
Wait
Sleep 6s
