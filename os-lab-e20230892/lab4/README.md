# OS Lab 4 — Linux I/O Redirection and Pipelines

- **Student Name:** Piseth Panhavorn
- **Student ID:** e20230892
- **Personal Values:** `status = ready`, `other = sent`

## 1. Pipeline Stages (Core 1)
- **Stage 1 (`grep '^ready,' data/events.csv | tee evidence/selected.txt`):** Receives `data/events.csv` and filters lines starting strictly with `ready,`, producing the 3 matching rows (`ready,8` three times) while excluding `ready-late,60`.
- **Stage 2 (`grep '^ready,' data/events.csv | cut -d, -f2 | tee evidence/fields.txt`):** Takes the selected lines from `grep` on `stdin` and splits on `,` to extract field 2, producing the 3 quantity values (`8`, `8`, `8`).
- **Stage 3 (`grep '^ready,' data/events.csv | wc -l > evidence/normal.txt`):** Counts the lines output by `grep` (`3`), saving the record count to `evidence/normal.txt`. `wc -l` counts matching lines (records), not the arithmetic sum of quantities.

## 2. Changed Inputs and Stream Tests (Core 2)
- **Prefix Matching (`^ready,`):** Anchoring at the start of the line (`^`) and ending the token with a comma (`,`) ensures `ready-late,60` and `ready-late,99` are excluded.
- **Records vs. Quantities (`data/changed.csv`):** Appending `ready,7` and `ready-late,99` increases the `wc -l` count from `3` to `4` (one new matching row), rather than adding `7` to the count.
- **Overwrite vs. Append (`>` vs. `>>`):** Writing `data/no-match.csv`'s count (`0`) with `>` creates/replaces `evidence/edge.txt`, whereas `>>` appends `data/changed.csv`'s count (`4`) on a second line without erasing `0`.
- **Stream Separation (`cat data/missing.csv > evidence/stdout.txt 2> evidence/stderr.txt`):** Because `data/missing.csv` does not exist, normal output (`stdout`, stream 1) is empty (`0` bytes in `stdout.txt`), while the error diagnostic goes to `stderr` (stream 2, saved in `stderr.txt`) and `$?` is nonzero (`1` in `status.txt`).

## 3. Plus, Challenge, and Debrief Observations
- **No-Match vs. Missing File:** `grep '^ready,' data/no-match.csv` produces no lines and exits with status `1` (no match found) with no `stderr` error, whereas `cat data/missing.csv` fails to open the file, writes a diagnostic to `stderr`, and exits nonzero.
- **Pipeline Exit Status:** In `cat data/missing.csv | wc -l`, `cat` writes an error to `stderr` and fails, while `wc -l` reads 0 bytes from `stdin`, prints `0`, and exits `0`. Because a pipeline reports the exit status of its final command, `$?` is `0` despite the input failure.
- **Challenge (`2>&1 | tee`):** `|` forwards only `stdout`. Redirecting `2>&1` before the pipe (`cat data/events.csv data/missing.csv 2>&1 | tee evidence/challenge.txt`) merges `stderr` into `stdout` so `tee` both displays and saves both streams.
