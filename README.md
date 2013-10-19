
A repository of code snippets for MaxCompiler (Maxeler DFEs).
==============================================================

It turns out that plenty of everyday problems for MaxCompiler require
coding similar problems again and again. This repository collects
examples of such elementary problems as self-contained MaxCompiler
projects to allow copy-pasting where necessary.



# Short summary of intentions:

 - purpose:  

     - re-using elementary code pieces to minimise reinvention of a wheel.

     - educational: how many possible implementations exists for the same or
          similar problems

 - git repository of complete self-contained MaxCompiler projects, independent
   from each other

 - self-explanatory directory structure + readme files + references to wiki

 - no common build infrastructure, it is a database of separate code snippets

 - each project should compile and work for at least one version of MaxCompiler

 - project should be minimalistic (educational/navigational purpose), no
   grouping of similar implementations into one project

 - it's good to keep technical entry barrier minimal to encourage wide
   contribution/usage

 - we encourage code contributions and welcome code review (external tools?)


# More details:

 - we still need some technical guideline to minimize maintenance and keep it
   tidy/usable (TODO: workflow explained)

 - branching model: master + feature branches. One project per feature branch.
   Merging new feature to master is only via pull requests (for code review).

 - we expect user to copy-paste and combine code from several projects into
   his/her own code

 - we believe user naturally produces projects (nearly) ready for submission
   to this repository as a part of coding his/her own solution (if no solution
   is already available here)

 - as long as project is added to repository, it doesn't require maintenance
   (porting to new MaxCompiler)

 - if someone updates existing project to new MaxCompiler, we encourage new
   updating commit. We prefer keeping single version in latest syntax, unless
   implementation details differ. If one needs keeping older version, it is kept
   as subdirectory in current project folder.

 - same time, it's good to keep older versions for a year or so back, to
   support older-but-usable soft- and hardware.

 - minimal technical set: git + wiki + script for automated testing.

 - wiki is necessary as a glue between projects + discussion board (?)

 - it's good to ensure everything is clonable and documented enough to survive
   over the years



# Discussion points:

 - full set of technical stuff: git repository + wiki + automatic comments
   gathering for wiki (like javadoc, doxygen...) + a script for testing all
   projects at once, code style checker, anything else?

 - making it public (github?), local web/git/wiki installation (gitlab)

 - conversion to KernelLib compatible code as separate activity
