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

Contributions to Dissolve are welcome, and can come in many forms - the ability to write code is not a prerequisite! For instance, you could:

- Fix typos, grammar, or inconsistencies in the documentation, or add to sections that are missing content or could be improved.
- Test it by attempting to reproduce results obtained previously with EPSR.
- Break it somehow, and [submit an issue](https://github.com/projectdissolve/dissolve/issues) describing the problem and how you created it.
- Document an example use-case (most likely a system you're interested in) and submit it for inclusion.
- Fix something in the code, or add a missing feature (see the current list of [issues](https://github.com/projectdissolve/dissolve/issues)).
- Open a new [issue](https://github.com/projectdissolve/dissolve/issues) describing something you'd like to see added, and why.
- Create a new [module](Overviews/Modules) with new functionality.

Most of these require at least an account on [GitHub](https://github.com), so if you want to contribute the first step is to register and go to the [Dissolve repository](https://github.com/projectdissolve/dissolve).

## Contributing to Documentation

Making changes to the documentation is one of the easiest routes to contributing. While a GitHub account is still required, the documentation can be edited directly in the Dissolve repository on the GitHub website. The repository will automatically be [forked](https://help.github.com/en/articles/fork-a-repo) and a [pull request](https://help.github.com/en/articles/about-pull-requests) made on your behalf once you've finished editing the file, which will then go to the developers for review before the changes are incorporated into the main repository.

More details can be found in this [GitHub guide](https://help.github.com/en/articles/editing-files-in-another-users-repository).

## Contributing Code (as a member of ProjectDissolve)

The main Dissolve development team adopts the Gitflow workflow, with `develop` being the main branch of the repository. `master` is reserved for released versions of the code, and associated hotfixes. All significant additions or fixes to the code should be made through a separate `feature` branch, rather than committing directly to `develop`. Minor changes (e.g. typographical errors) may be made directly to `develop`.

For a more in-depth explanation of gitflow please refer to the excellent guide at [Atlassian.com](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow) or the original [presentation of the workflow](https://nvie.com/posts/a-successful-git-branching-model/) by Vincent Driessen.

## Contributing Code (as a third party)

Contributing code to Dissolve as an interested third party is relatively straightforward:

### 1) Register on GitHub

Register an account on [GitHub](https://github.com) if you don't already have one.

### 2) Fork Dissolve

Go to the [Dissolve repository](https://github.com/projectdissolve/dissolve) and hit the **Fork** button at the top-right. This will give you your own copy of the Dissolve code as it currently stands. GitHub will take you to your own copy of Dissolve once the fork is completed, which you can should then clone / download on to your own machine.

### 3) Experiment / Fix / Break / Play

You can freely experiment with the source code without fear of breaking the actual version, as all the changes you make are isolated to your repository until such time as you choose to try and merge them back in to the main project.

### 4) Submit a Pull Request

If you have added / fixed something useful, and think it's ready for inclusion back in to the main branch, then it's time to submit a pull request. Make sure you have pushed all the changes you want to your repository, then go to your repository on GitHub.com and press the "New pull request" button. Write a short comment summarising the changes / fixes you've made, and then press "Create pull request".

Several things will then happen. Most importantly, the developers will be notified that you have submitted a request to merge your changes into the main repository. At the same time, a series of automated checks will be run on the merged code state to try to ensure that nothing obvious is broken by your changes. The team will then review the code changes, and comment / accept / request further changes accordingly.

### 5) Staying Up-to-date

Your fork is essentially independent of the main Dissolve repository, and can wander off in any direction it wants to. However, in most cases you will want to keep it up-to-date with the main repository (i.e. be able to pull changes made to the main repository in to your own fork). This can be easily achieved by adding an additionaln remote repository to your fork:

```
bob@linux:~> git remote add upstream https://github.com/projectdissolve/dissolve.git

bob@linux:~> git remote -v

origin    https://github.com/YOUR_NAME/dissolve.git (fetch)
origin    https://github.com/YOUR_NAME/dissolve.git (push)
upstream  https://github.com/projectdissolve/dissolve.git (fetch)
upstream  https://github.com/projectdissolve/dissolve.git (push)
```

To receive any changes from the main Dissolve repository you can simply run `git pull` with an explicit remote name (in this case, `upstream`):

```
bob@linux:~> git pull upstream
```

Note that running `git push` and `git pull` without an explicit remote will still default to pushing / pulling changes to / from your fork.

### 6) Example (Small Fix)

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
To https://github.com/YOUR_GITHUB_USER_NAME/dissolve.git
   218d7884..758678a6  develop -> develop
```

At this stage, you can create a 'pull request' to ask for those changes to be copied from your repository and merged into the main Dissolve repository. The changes you've made will be reviewed by the development team, and may be immediately accepted, commented on, or more changes / corrections may be requested.

## Collaborators

If you wish to become an official collaborator on Dissolve and want to join the [ProjectDissolve organisation](https://github.com/projectdissolve), e-mail the lead author [Tristan Youngs](mailto:tristan.youngs@stfc.ac.uk) to discuss.
