---
title: Contributing
description: Contributing to the project
---

## Introduction

### Purpose
This document describes how to contribute to the Dissolve project.

### Scope
This overview provides essential information for those wishing to contribute to Dissolve, covering processes and workflows for light-touch contributions from end-users through to more in-depth development.

## Basics

Contributions to Dissolve are welcome, and can come in many forms - the ability to write code is not a prerequisite! For instance, you could:

- Fix typos, grammar, or inconsistencies in the documentation, or add to sections that are missing content or could be improved.
- Test it by attempting to reproduce results obtained previously with EPSR.
- Break it somehow, and [submit a bug report](https://www.github.com/disorderedmaterials/dissolve/issues) describing the problem and how you created it.
- Document an example use-case (most likely a system you're interested in) and submit it for inclusion.
- Fix something in the code, or add a missing feature (see the current list of [issues](https://www.github.com/disorderedmaterials/dissolve/issues)).
- Open a new [feature request issue](https://www.github.com/disorderedmaterials/dissolve/issues) describing something you'd like to see added, and why.
- Create a new [module]({{< ref "modules" >}}) to extend Dissolve's functionality.

Most of these require at least an account on [GitHub](https://www.github.com), so if you want to contribute the first step is to register and go to the [Dissolve repository](https://www.github.com/disorderedmaterials/dissolve).

## Contributing to Documentation

Making changes to the documentation is one of the easiest routes to contributing. While a GitHub account is still required, the documentation can be edited directly in the Dissolve repository on the GitHub website. The repository will automatically be [forked](https://help.github.com/en/articles/fork-a-repo) and a [pull request](https://help.github.com/en/articles/about-pull-requests) made on your behalf once you've finished editing the file, which will then go to the developers for review before the changes are incorporated into the main repository.

More details can be found in this [GitHub guide](https://help.github.com/en/articles/editing-files-in-another-users-repository).

## Contributing to the Codebase

Contributing code to Dissolve as an interested third party is relatively straightforward. Dissolve is version controlled with [git](https://git-scm.com/) and hosted on GitHub at [https://www.github.com/disorderedmaterials/dissolve](). Dissolve adopts a simple branching model for development, with `develop` being the main branch of the repository, and versioned releases branching off from that. Any additions or fixes to the code should be made through a suitable pull request from your own forked repository.

### 1) Register on GitHub

Register an account on [GitHub](https://www.github.com) if you don't already have one.

### 2) Fork Dissolve

Go to the [Dissolve repository](https://www.github.com/disorderedmaterials/dissolve) and hit the **Fork** button at the top-right. This will give you your own copy of the Dissolve code as it currently stands. GitHub will take you to your own copy of Dissolve once the fork is completed, which you can should then clone / download on to your own machine.

### 3) Experiment / Fix / Break / Play

You can freely experiment with the source code without fear of breaking the actual version, as all the changes you make are isolated to your repository until such time as you choose to submit a pull request and

### 4) Contribution Primer for a Small Fix

Firstly, clone your repository to your local machine if you haven't already:

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
bob@linux:~> git submodule update --init --recursive
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

At this stage, you can create a pull request to ask for those changes to be copied from your repository and merged into the main Dissolve repository. The changes you've made will be reviewed by the lead author(s), and may be immediately accepted, commented on, or more changes / corrections may be requested.

## Pull Request Conventional Commits

The title of each pull request needs to begin with a tag describing the nature of the commit.  These tags, as part of the [Conventional Commits](https://www.conventionalcommits.org/) framework, enable easier change management.  A GitHub action warns when the title of the pull request does not meet the requirements.

The tags that we recognise are:

feat
: New features that are use visible
fix
: Corrections for errors in the code
docs
: Changes and corrections to the documentation
perf
: Improvements in the performance
refactor
: Changes to the code that are not intended to change any behaviour
style
test
: Updates to the testing framework
chore
: Commits that should have no visible impact to the user
revert
: Changes that remove other changes
build
: Modifications of the build system used to compile and package Dissolve


## Collaborators

If you wish to become an official collaborator on Dissolve and want direct access to the main repository, e-mail the lead author [Tristan Youngs](mailto:tristan.youngs@stfc.ac.uk) to discuss.
