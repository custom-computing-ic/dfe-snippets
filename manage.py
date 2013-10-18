"""Simple script to manage the creation and verification of."""
import os
import re
from os.path import join

DESIGN_FILE_RE = re.compile(r'.*\.(maxj|java)')
CPU_FILE_RE = re.compile(r'.*\.(cpp|c)')
MAKEFILE_RE = re.compile(r'Makefile')

def LintJavaFile(path):
    print 'Liniting file ' + path

def LintMakefile(path):
    print 'Linting makefile ' + path

def LintCpuFile(path):
    print 'Linting CPU File ' + path

def LintAllFiles():
    for dirpath, dirnames, filenames in os.walk("."):
        for filename in filenames:
            path = os.path.join(dirpath, filename)
            if DESIGN_FILE_RE.match(path):
                LintJavaFile(path)
            elif CPU_FILE_RE.match(path):
                LintCpuFile(path)
            elif MAKEFILE_RE.match(path):
                LintMakefile(path)

def RunTests():
    print 'Running tests.'

def UpdateWiki():
    print 'Extract comments and update wiki'

def main():
    print DESIGN_FILE_RE.match('file.java')

    LintAllFiles()

    RunTests()

    UpdateWiki()

if __name__ == "__main__":
    main()

