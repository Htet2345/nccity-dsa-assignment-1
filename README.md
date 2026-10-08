# Assignment 1: User Manager

`main.c` is a menu-driven standard C console program using functions, arrays,
and structures. Each `User` contains an ID, name, age, and address. A file-scope
array stores up to 100 users; `userCount` tracks active records and `nextId`
tracks the next automatically generated ID, starting at 1.

Create appends a complete record. Read prints ID/name/age/address in that order.
`findUserIndex` searches by ID and returns an array index or -1. Update replaces
the fields while keeping the ID. Delete shifts subsequent whole structures left.
IDs are never renumbered or reused during a run. Both menu option 5 and 0 exit.

## Pointer constraint

There are no explicit pointer variables, pointer arithmetic, custom pointer
parameters, or dynamic allocation. Helpers use integer parameters and share
file-scope input storage. Input uses `getchar`, so even address-of arguments are
unnecessary here. Ordinary address-of arguments required by standard input
functions would be allowed under the assignment's interpretation.

This does **not** mean C has no pointer-related semantics: arrays and string
literals undergo standard conversions when passed to library functions such as
`strcpy` and `printf`. `strcpy` is safe here because input is checked against the
destination's capacity before copying.

## Reasonable defensive choices

These choices extend the demonstration; they are not additional teacher rules:

- Names allow 99 bytes and addresses 199 bytes, plus null termination. Internal
  spaces are preserved; leading/trailing whitespace is trimmed; blank fields
  are rejected. Limits are bytes, not Unicode character counts.
- Overlong lines are fully consumed and rejected, avoiding buffer overflow and
  leftover input at later prompts. Enter the replacement line after the error.
- Numeric input must be a whole number within `-INT_MAX` to `INT_MAX`; optional
  signs are accepted. Malformed or overflowing numbers are retried. Age is 1–150.
- Unknown update/delete IDs retry inside the operation. Empty operations return
  to the menu. Invalid numeric menu choices print `Invalid option!`.
- At 100 active users, creation returns to the menu without asking for fields.
  An integer-limit guard prevents the next ID from overflowing.
- EOF exits cleanly. A final line without a newline is accepted. Incomplete
  create/update operations do not commit changes.

## Build and run

This Windows project has a portable GCC 16.2.0 toolchain and GDB debugger in
`.tools/w64devkit`, downloaded from the official
[w64devkit 2.10.0 release](https://github.com/skeeto/w64devkit/releases/tag/v2.10.0)
and verified against its published SHA-256 checksum. No extra libraries are needed.

### VS Code on this computer

Open this entire project folder in VS Code. Microsoft's C/C++ extension is
already installed. Press **F5** to build and run; enter menu choices in the
**Terminal** panel. **Ctrl+F5** runs without debugging, and **Ctrl+Shift+B** builds.
You can also choose **Terminal > Run Task > Run Assignment 1**.

The `.vscode` files configure the compiler, debugger, and build task. New VS Code
terminals in this folder have GCC on PATH. If a terminal was already open, close
it and open a new one. The portable tools are local to this project; they are
not included in the two-file assignment submission.

Windows PowerShell (works even outside VS Code):

```powershell
Set-Location 'D:\ncc data structure assignment-1'
& '.\.tools\w64devkit\bin\gcc.exe' -std=c11 -Wall -Wextra -Wpedantic main.c -o users.exe
if ($LASTEXITCODE -eq 0) { .\users.exe }
```

On another Windows computer with GCC on PATH, use `gcc` instead of the local
compiler path above. On Linux/macOS, with GCC installed, run from the project
folder (on macOS `gcc` may invoke Apple Clang):

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic main.c -o users
./users
```

## Exact demonstration input

Save the following as a temporary `demo.txt`, one entry per line:

```text
50
2
1
John
18
New York
1
Bob
22
Landon
3
50
2
BobUpdate
28
Tokyo
1
Tim
25
Oregon
4
2
2
0
```

After a successful build, run it in PowerShell with
`Get-Content .\demo.txt | .\users.exe`, or on Linux/macOS with
`./users < demo.txt`.

Expected behavior: 50 prints `Invalid option!`; the initial list prints
`Reading All Users` and `No users found!`. Creation assigns IDs 1, 2, and 3.
Update ID 50 prints `Your provided id not found! Try again!`, then accepts ID 2
and replaces Bob's fields. Deleting ID 2 leaves this final list:

```text
Reading All Users
ID: 1
Name: John
Age: 18
Address: New York

ID: 3
Name: Tim
Age: 25
Address: Oregon
```

Entering 0 prints `Goodbye!` and exits.

## Verification status

Compiled successfully on Windows with GCC 16.2.0 using
`-std=c11 -Wall -Wextra -Wpedantic -Werror -g`, with no warnings. All **29 automated
checks passed**, covering the exact demonstration, updated field values,
multiword fields, malformed/overflowing numbers, age boundaries, blank fields,
text length limits, long-line recovery, deletion positions, ID reuse prevention,
empty operations, capacity exhaustion, EOF, and both exit choices.

The local helper can be rerun with `python .verification/test_program.py` after
building `users.exe`. The demonstration output is saved locally in
`.verification/demo-output.txt`. These are verification aids, not submission
files. Linux/macOS execution has not been tested.
GDB was also verified by launching the executable and stopping at `main`.
The VS Code configuration files were checked for valid JSON; the F5 workflow
has not been exercised through the editor UI.

## Short manual test checklist

- Run the exact demonstration and compare the final two records above. Try 5
  separately to verify the displayed Exit option also works.
- At the menu, enter `abc`, `1x`, and a very large integer, then a valid option.
  Try age `abc`, 0, 151, then 18; verify recovery and rejection messages.
- Create `Mary Jane` at `12 Main Street, New York`. Try blank/whitespace-only
  fields, 100-byte names and 200-byte addresses (reject), then valid replacements.
  Check 99-byte names and 199-byte addresses (accept), and internal double spaces.
- Create five users; delete first ID 1, middle ID 3, and last ID 5. List remaining
  IDs 2 and 4. Create again and check ID 6. Try unknown delete ID 50, then ID 2.
- In a fresh run, use read/update/delete while empty; each returns to the menu.
- Send EOF at the menu and during name/age/address or ID input. On Windows use
  Ctrl+Z then Enter; on Linux/macOS use Ctrl+D at an empty prompt. Check clean
  exit. Re-run with a redirected final line lacking a newline as well.
- Fill all 100 slots, reject one more create, delete ID 50, create again, and
  verify ID 101 with 100 active records.

For a repeatable capacity test, with Python available, generate temporary input:

```sh
python -c "from pathlib import Path; Path('capacity.txt').write_text(''.join('1\nUser '+str(i)+'\n20\nTest Address\n' for i in range(1,101))+'1\n4\n50\n1\nAfter Delete\n21\nNew Address\n2\n0\n', encoding='ascii')"
```

Then run `Get-Content .\capacity.txt | .\users.exe` in PowerShell or
`./users < capacity.txt` on Linux/macOS. Expect the capacity message, deletion of
ID 50, creation of ID 101, and a list containing 100 records without ID 50.

## Limitations and submission

All data exists only during the current run; restarting resets records and IDs.
There is no persistence, database, authentication, networking, or GUI. Storage
holds at most 100 active users. IDs stop before `INT_MAX`. The program does not
perform Unicode validation, and terminal encoding affects displayed text.

To submit, sign in to GitHub and create or choose your repository. Use **Add file
> Upload files**, select only `main.c` and `README.md`, and commit the upload.
Submit the repository link as instructed by your teacher. Temporary test input
and compiled executables are not submission files. No repository was created or
pushed automatically.
