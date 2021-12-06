# xv6 homework repo

## Schedule 

The following table is written in the order of: Lab no., topic, due date 1(KST), due date 2(ET) 
(Since Most of Michigan is in the Eastern Time Zone, I basically followed ET.)


| Lab No. | Topic | Due (KST) | Due (ET) |
| --------|--------|--------|--------|
|: 1 :|:  UNIX Utilities   :|  2021. 12. 06. 00:00   | 2021. 12. 05. 22:00  |
| 2 |  System Calls  |  2021. 12. 13. 00:00   | 2021. 12. 12. 22:00  |
| 3 |  Page Tables  |  2021. 12. 20. 00:00   | 2021. 12. 19. 22:00  |
| 4 |  Traps  |  2021. 12. 27. 00:00   | 2021. 12. 26. 22:00  |
| 5 |  Copy-On-Write fork  |  2022. 01. 03. 00:00   | 2022. 01. 02. 22:00  |
| 6 |  Multithreading  |  2022. 01. 17. 00:00   | 2022. 01. 16. 22:00  |
| 7 |  Network Driver  |     |   |
| 8 |  Parallelism/Locking  |     |   |
| 9 |  File System  |     |   |
|: 10 :|  Mmap  |     |   |

## Instructions

### Initial Setup

You only do this one time.

```bash
# Clone this repository to your Linux machine.
git clone git@github.com:go-go-carrot/OS_xv6.git

# Walk into the repository. You'll be on the master branch.
cd OS_xv6
```

### Doing Labs

For instance, let's say that you'll work on lab 'util'. The branch corresponding to lab 'util' is named `util` (see the MIT 6.S081 website for the name of the branch).

```bash
# You should be in the root directory of the repo.
# Checkout the util branch.
git checkout util

# Work on the project.
nvim user/sleep.c
nvim Makefile
make qemu

# Submit the project to this repo for review.
# WARNING: `git add .` basically adds every file in your directory.
#          This will probably add files that the grader is not interested
#          in (e.g., object files). You should run `make clean` to remove
#          all compilation artifacts and check `git status` to identify
#          only the source code files that you modified, and only `git add`
#          those. For example:
git add user/sleep.c user/find.c Makefile

# Commit.
git commit -m "Lab util done"

# Create a pull request (PR) to the util-orig branch.
# Running `git push` will print out a URL where you can create a PR. Click the link.
# The initial setting of the PR is:
# - base: master
# - compare: util
# Change it to this:
# - base: util-orig
# - compare: util
# Explain your changes in the body of the PR, and hit 'create pull request'.
# Creating a PR will automatically notify me by email. You'll also receive emails
# when I add review comments on the PR.
git push
```

### Advice

- `make clean` removes object files, binaries, etc that were newly created by `make qemu`. It won't reset or delete the source code files.
- Make sure your code compiles before you commit.
- Utilize `git commit` while you're working on your project. It serves as a snapshot of your code, and you can revert back to a commit when you mess up and make things worse.
- Utilize `git diff`. It basically shows you all the changes made between two commits. It will answer questions such as "Why doesn't xv6 boot anymore? What did I change?".
