---
parent: Developer Documentation
nav_order: 6
---
# Contributing

## Introduction

### Purpose
This document describes how to contribute to the Dissolve project.

### Scope
This overview provides essential information for those wishing to contribute to Dissolve, covering processes and workflows for light-touch contributions from end-users through to more in-depth development.

## Basics

Dissolve is version controlled through Git, and is available from [https://www.github.com/trisyoungs/dissolve](). Dissolve adopts the Gitflow workflow for development, with `develop` being the main branch of the repository. `master` is reserved for released versions of the code, and associated hotfixes. All significant additions or fixes to the code should be made through a separate `feature` branch, rather than committing directly to `develop`. Minor changes (e.g. typographical errors) may be made directly to `develop`.

For a more in-depth explanation of gitflow please refer to the excellent guide at [Atlassian.com](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow) or the original [presentation of the workflow](https://nvie.com/posts/a-successful-git-branching-model/) by Vincent Driessen.

## Flagging Issues

If coding is not your intent or desire and the mere thought of it makes you feel ill, there are still valuable ways that you can contribute. Current issues with Dissolve are [listed at the main repository](https://github.com/trisyoungs/dissolve/issues) and contain feature requests, bug reports, suggestions, and discussions regarding all aspects of the code. If you think you have found a bug, or have a suggestion for improving the code, submit it as a new issue.

## External Contributors

Contributing to Dissolve as an interested third party is relatively straightforward. Follow the steps below.

### 1) Register on GitHub

Register an account on [GitHub](https://www.github.com) if you don't already have one.

### 2) Fork Dissolve

Go to the [Dissolve repository](httpsL//www.github.com/trisyoungs/Dissolve) and hit the **Fork** button at the top-right. This will give you your own copy of the Dissolve code as it currently stands. GitHub will take you to your own copy of Dissolve once the fork is completed, which you can should then clone / download on to your own machine.

### 3) Experiment / Fix / Break / Play

You can freely experiment with the source code without fear of breaking the actual version, as all the changes you make are isolated to your repository until such time as you choose to try and merge them back in to the main project. Follow the gitflow approach for moderate/major changes, or commit directly to `develop` for minor things.

### 4) Example (Small Fix)

Firstly, clone the repository if you haven't already:

```
bob@linux:~> git clone https://github.com/YOUR_GITHUB_USER_NAME/dissolve.git ./dissolve

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
bob@linux:~> git push

Counting objects: 5, done.
Delta compression using up to 12 threads.
Compressing objects: 100% (5/5), done.
Writing objects: 100% (5/5), 462 bytes | 462.00 KiB/s, done.
Total 5 (delta 4), reused 0 (delta 0)
remote: Resolving deltas: 100% (4/4), completed with 4 local objects.
To https://www.github.com/YOUR_GITHUB_USER_NAME/dissolve.git
   218d7884..758678a6  develop -> develop
```

At this stage, you can create a 'pull request' to ask for those changes to be copied from your repository and merged into the main Dissolve repository. The changes you've made will be reviewed by the lead author(s), and may be immediately accepted, commented on, or more changes / corrections may be requested.

## Collaborators

If you wish to become an official collaborator on Dissolve and want direct access to the main repository, e-mail the lead author [Tristan Youngs](mailto:tristan.youngs@stfc.ac.uk) to discuss.
