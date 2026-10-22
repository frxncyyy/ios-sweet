# Working on ios-sweet

- Read `NOTES.md` before starting; it contains the project goal, hardware facts,
  current blocker, local artifact locations and the next concrete action.
- The user switches agents frequently. Update the active handoff in `NOTES.md`
  after each milestone and before stopping. Record commits, CI run IDs, exact
  build/boot results and unfinished work. Distinguish observations from guesses.
- Commit completed changes. Use `gh` for GitHub Actions kernel builds and logs.
  Local single-file cross-compiles are useful before spending a CI run.
- On Nord 2, `fastboot boot` does not work. Device tests use an explicitly
  authorized flash of the exact prepared image to `boot` only. iOS now occupies
  `userdata`; never boot stock Android over it. Preserve bootloaders, recovery,
  modem, persist, GPT and verified partition backups. Use the tested RAM ADB
  rescue or a verified working iOS boot image for recovery. Confirm each new
  destructive test with the user after building and verifying its exact image;
  complete recovery already authorized with that test without asking again.
- Keep device dumps, serial numbers and downloaded Apple artifacts out of git.
