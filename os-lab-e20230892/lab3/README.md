# Lab 3 — Wildcards and Links: individual report

- **Student Name:** Piseth Panhavorn
- **Student ID:** e20230892
- **Ubuntu Username & Environment:** `gic-piseth-panhavorn` on `i4gic-os` (Ubuntu Server 24.04, kernel Linux 6.8.0-45-generic x86_64)
- **Artifact Paths:**
  - [`evidence/links.txt`](evidence/links.txt) — Inode/target numbers, before-rename state, failure after rename, and repaired link verification
  - [`evidence/replacement.txt`](evidence/replacement.txt) — Recreated `source.txt` with new inode and content isolation observations

---

## Original Prediction

**Prediction written before execution (captured on course worksheet/paper):**
> *When `source.txt` is renamed to `renamed.txt`:*
> 1. **Hard link (`hard.txt`):** Will still successfully read the original file contents (`version 1`).
>    - *Reason:* A hard link points directly to the underlying inode data structure on the filesystem. Renaming the source file modifies the directory entry (unlinking `source.txt` and linking `renamed.txt`), but does not delete the inode or alter its data blocks. Since `hard.txt` directly references that same inode, its reference remains completely valid.
> 2. **Symbolic link (`soft.txt`):** Will fail with `No such file or directory` (dangling link).
>    - *Reason:* A symbolic link does not point to an inode directly; it only stores a string containing a target file path (`source.txt`). When accessed, the OS resolves that path relative to the directory containing the symlink. Because the directory entry `source.txt` no longer exists after the rename, path resolution fails.

---

## Test Evidence

Artifact records:
- [`evidence/links.txt`](evidence/links.txt)
- [`evidence/replacement.txt`](evidence/replacement.txt)

### Summary Evidence Table

| Test | Claim | Result that would contradict it | Observed result | What remains unproven |
|---|---|---|---|---|
| **Two link types before rename** | `source.txt` and `hard.txt` share the same inode number (link count = 2), while `soft.txt` has a distinct inode and stores `source.txt` as target text. | `hard.txt` having a different inode number than `source.txt`, or `soft.txt` having the same inode as `source.txt`. | Inode `18743759` shared by `source.txt` and `hard.txt` (link count 2). `soft.txt` has distinct inode `18743767` with target `source.txt`. Both read `version 1`. | Does not prove behavior across different filesystem boundaries (hard links cannot cross filesystem mounts). |
| **Dangling link and recreated filename** | Renaming breaks `soft.txt` (dangling pointer) but leaves `hard.txt` functional. Recreating `source.txt` creates a new inode (`18743769`), keeping `hard.txt` bound to `18743759`. | `soft.txt` continuing to work without repair after target rename, or `hard.txt` changing content to `version 2` when new `source.txt` is created. | `cat soft.txt` failed with `cat: soft.txt: No such file or directory`. `hard.txt` still read `version 1`. After creating new `source.txt` (`version 2`), `source.txt` had inode `18743769` while `hard.txt` stayed at `18743759` reading `version 1`. | Does not prove behavior if disk runs out of inodes or blocks during file allocation. |

---

## Explanation and Correction

### 1. Inode Identity & Target Resolution
From our server run on `i4gic-os`:
- **Initial State:**
  - `source.txt`: Inode `18743759`, link count `2`, content `version 1`
  - `hard.txt`: Inode `18743759`, link count `2`, content `version 1`
  - `soft.txt`: Inode `18743767`, link count `1`, target `source.txt`
- **After Rename (`mv -- source.txt renamed.txt`):**
  - `renamed.txt`: Inode `18743759`, link count `2`
  - `hard.txt`: Inode `18743759`, link count `2` — still reads `version 1` directly from inode `18743759`
  - `soft.txt`: Inode `18743767` — points to nonexistent name `source.txt` -> broken / dangling link (`cat: soft.txt: No such file or directory`)
- **After Repair (`rm soft.txt; ln -s renamed.txt soft.txt`):**
  - `soft.txt`: Inode `18743767` recreated, target `renamed.txt` -> resolves to inode `18743759` (`version 1`)
- **After Recreated Source (`printf 'version 2\n' > source.txt`):**
  - `source.txt`: Inode `18743769` (new inode allocated by kernel filesystem driver), content `version 2`
  - `hard.txt`: Inode `18743759`, content `version 1`

### 2. Core OS Mechanism Answers
- **(a) Which file change broke the symbolic reference?**
  Renaming `source.txt` to `renamed.txt` broke `soft.txt`. A symbolic link stores only a path string (`source.txt`). When the pathname in the directory was changed to `renamed.txt`, the kernel could no longer find any directory entry named `source.txt` in `links/`, returning `ENOENT` (`No such file or directory`).
- **(b) Why did the hard link survive?**
  A hard link is an independent directory entry pointing directly to the file's underlying inode number (`18743759`). Renaming `source.txt` merely updated the directory table entry for the source name to `renamed.txt`; the inode itself, its data blocks, and its link count were preserved. Because `hard.txt` points directly to inode `18743759`, it accessed the exact same data uninterrupted.
- **(c) What happens if the new filename contains different bytes?**
  When a new `source.txt` containing `version 2` was created, the shell and filesystem allocated a completely new inode (`18743769`) with fresh data blocks. It did not alter inode `18743759`. Consequently, `source.txt` reads `version 2`, while `hard.txt` continues to read `version 1`. Filenames in directories are merely mappings to inode numbers; reusing a filename does not reclaim or overwrite an existing separate inode.

### 3. Prediction Evaluation
- **Status:** **Confirmed**.
- **Evidence Citation:** Confirmed by `evidence/links.txt` (lines 4-11) and `evidence/replacement.txt`. The hard link read `version 1` throughout all renames, while `soft.txt` broke upon renaming `source.txt` and resumed functioning only after updating the stored target path to `renamed.txt`.

---

## Individual Changed-case Checkpoint

> **Scenario:** A symbolic link inside `lab3/links/` stores `../source.txt`. Only `lab3/links/source.txt` exists. Resolve the stored path and explain why the link fails; give a suitable target for the existing file.

1. **Diagnosis & Result:**
   - The symbolic link is broken (dangling link). Attempting to open, read, or dereference it (e.g. `cat`) fails with `No such file or directory` (error `ENOENT`).
2. **Mechanism:**
   - Relative symbolic link targets are resolved **relative to the directory containing the link file itself** (`lab3/links/`), NOT relative to the process's current working directory.
   - When the kernel resolves `../source.txt` from inside `lab3/links/`, it moves up one level (`..`) to `lab3/` and searches for `source.txt` (i.e. `lab3/source.txt`).
   - Because `source.txt` only exists inside `lab3/links/source.txt` and does NOT exist in `lab3/source.txt`, the kernel cannot locate the file and path resolution fails.
3. **Suitable Target:**
   - The target path should be `source.txt` (or `./source.txt`).
4. **Observation to Check:**
   - Run `readlink <link_name>` to verify that the stored target is `../source.txt`.
   - Run `ls -l ../source.txt` from inside `lab3/links/` to confirm that `lab3/source.txt` does not exist.
   - Run `cat <link_name>` to confirm that the operating system returns `cat: <link_name>: No such file or directory`.

---

## Wildcards Explanation (Task 1 Guided Example)

- **Pattern Expansion vs Literal String:**
  - When running `printf '<%s>\n' practice/*.txt`, the shell (Bash) scans the `practice/` directory and expands `practice/*.txt` into matching filenames (`practice/report1.txt`, `practice/report2.txt`) **before** invoking `printf`.
  - In contrast, when single quotes are used (`'practice/*.txt'`), the shell disables expansion and passes the literal characters `practice/*.txt` as the argument to the program.
  - The `?` wildcard matches exactly one single character (e.g. `report?.txt` matches `report1.txt` and `report2.txt`, but not `report.log`).
- **Relative Target in Guided Links:**
  - In `ln -s report1.txt practice/soft-demo.txt`, the relative target stored inside the symlink is `report1.txt`.
  - Because `soft-demo.txt` resides in `practice/`, the OS resolves `report1.txt` as `practice/report1.txt`. If we had stored `practice/report1.txt`, looking it up from within `practice/` would have resolved to `practice/practice/report1.txt`, which would fail.

---

## AI Note (optional)

- **Helpful Suggestion:** Used AI assistance to clarify and verify how relative symbolic links resolve paths (confirming that the kernel resolves them relative to the directory containing the link, rather than the caller's `$PWD`).
- **Verification Test:** Tested creating a symlink `ln -s ../source.txt bad_link.txt` inside `lab3/links/` and verified with `readlink bad_link.txt` and `cat bad_link.txt`, confirming that the kernel attempts resolution at `lab3/source.txt` and fails with `No such file or directory`.

---

## Cleanup

- No background daemons, cron jobs, network sockets, or filesystem mounts were left running.
- All temporary practice links created for diagnosis were removed.
- Evidence files were verified and preserved in `evidence/links.txt` and `evidence/replacement.txt`.
