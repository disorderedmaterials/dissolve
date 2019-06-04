---
parent: Developer Documentation
nav_order: 6
mermaid: true
---
# Contributing

## Introduction

### Purpose
This document describes how to contribute to the Dissolve project.

### Scope
This overview provides essential information for those wishing to contribute to Dissolve, covering processes and workflows for light-touch contributions from end-users through to more in-depth development.

## Basics

Dissolve is version controlled through Git, and is available from [https://www.github.com/trisyoungs/dissolve](). Dissolve adopts the Gitflow workflow for development, with `developt` being the main branch of the repository. `master` is reserved for released versions of the code, and associated hotfixes. All significant additions or fixes to the code should be made through a separate `feature` branch, rather than committing directly to `develop`. Minor changes (e.g. typographical errors) may be made directly to `develop`.

For a more in-depth explanation of gitflow please refer to the excellent guide at [Atlassian.com](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow) or the original [presentation of the workflow](https://nvie.com/posts/a-successful-git-branching-model/) by Vincent Driessen.

## External Contributors

Contributing to Dissolve as an interested third party is relatively straightforward. Follow the steps below.

### 1) Register on GitHub

Register an account on [GitHub](https://www.github.com) if you don't already have one.

### 2) Fork Dissolve

Go to the [Dissolve repository](httpsL//www.github.com/trisyoungs/Dissolve) and hit the **Fork** button at the top-right. This will give you your own copy of the Dissolve code as it currently stands. GitHub will take you to your own copy of Dissolve once the fork is completed, which you can should then clone / download on to your own machine.

### 3) Experiment / Fix / Break / Play

You can freely experiment with the source code without fear of breaking the actual version, as all the changes you make are isolated to your repository until such time as you choose to try and merge them back in to the main project. Follow the gitflow approach for moderate/major changes, or commit directly to `develop` for minor things.

### 4) Example

Firstly, clone the repository if you haven't already:

```
bob@linux:~> git clone https://github.com/YOURGITHUBNAME/dissolve.git ./dissolve

Cloning into './dissolve'...
remote: Enumerating objects: 240, done.
remote: Counting objects: 100% (240/240), done.
remote: Compressing objects: 100% (131/131), done.
remote: Total 32706 (delta 139), reused 188 (delta 102), pack-reused 32466
Receiving objects: 100% (32706/32706), 146.78 MiB | 4.70 MiB/s, done.
Resolving deltas: 100% (27682/27682), done.
Checking out files: 100% (2309/2309), done.

bob@linux:~> cd dissolve
bob@linux:~> ls

AUTHORS     changeversion   commit        dissolve.kdev4  extra        NEWS    tests
autogen.sh  cmake           configure.ac  docs            INSTALL      README
ChangeLog   CMakeLists.txt  COPYING       examples        Makefile.am  src

bob@linux:~> git status

On branch develop
Your branch is up to date with 'origin/develop'.

nothing to commit, working tree clean
```

The output of `git status` tells you which branch you are currently on (`develop`) and the state of your branch relative to that on the server. Since you haven't yet made any changes, your working tree is 'clean'.

Next, we'll make a small change and fix a typo which I've just found. In `src/classes/configuration.h` there was a mis-spelling of 'specified' ('specifeid'):

```diff
bob@linux:~> vi src/classes/configuration.h   # Fix the typo

bob@linux:~> git status

On branch develop
Your branch is up to date with 'origin/develop'.

Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

	modified:   src/classes/configuration.h

no changes added to commit (use "git add" and/or "git commit -a")

bob@linux:~> git diff

diff --git a/src/classes/configuration.h b/src/classes/configuration.h
index 6c9ff564..2a17f1d3 100644
--- a/src/classes/configuration.h
+++ b/src/classes/configuration.h
@@ -113,7 +113,7 @@ class Configuration : public ListItem<Configuration>, public ObjectStore<Configu
        SpeciesInfo* usedSpeciesInfo(Species* sp);
        // Return list of SpeciesInfo for the Configuration
        List<SpeciesInfo>& usedSpecies();
-       // Return if the specifeid Species is present in the usedSpecies list
+       // Return if the specified Species is present in the usedSpecies list
        bool hasUsedSpecies(Species* sp);
        // Return total relative population of Species used by this Configuration
        double totalRelative() const;
```

`git status` will tell you which files have been changed since the last commit, and whether they are ready to commit. The `git diff` command gives you a complete list of all the things you have changed, relative to the last commit.

We'll now prepare this change ready for merging into your repository, as suggested by the output of `git status` above. This is known as 'staging' and is performed with `git add`:

```
bob@linux:~> git add src/classes/configuration.h
```

This will stage **all** changes in the specified files(s). If you want to cherry-pick certain changes within files for staging, use `git add --patch <files>`.

Now that our changes have been staged, we can commit them along with a suitable message giving a brief description of the changes. Commit messages should be written such that they make sense when prepended with "This commit will...", and be short and to the point:

```
bob@linux:~> git commit -m "Fix typo."

[develop 4bbffa94] Fix typo.
 1 file changed, 1 insertion(+), 1 deletion(-)

bob@linux:~> git status

On branch develop
Your branch is ahead of 'origin/develop' by 1 commit.
  (use "git push" to publish your local commits)

nothing to commit, working tree clean
```

`git status` now tells us that there are no more changes to any other files, but that we are one commit ahead of the repository on the server (the 'origin'). It is worth stating that all of these changes are still stored only on your local machine - to send them to the server you must 'push' them as follows:

```
git push


Code review by the principal author (trisyoungs) should always be requested when making a pull request.

## Collaborators
For instance, to clone the repository on Linux, make a small fix to a typo (which I've just found), and commit your changes (to `develop`):
