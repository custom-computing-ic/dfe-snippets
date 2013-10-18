"""Simple script to manage the creation and verification of projects."""
import os
import re
from os.path import join

# Custom exception definitions
class LinterException(Exception):
    pass


# Some regular expresssion definitions useful for this module
DESIGN_FILE_RE = re.compile(r'.*\.(maxj|java)')
CPU_FILE_RE = re.compile(r'.*\.(cpp|c)')
MAKEFILE_RE = re.compile(r'Makefile')
BLOCK_COMMENT_LINE_RE = re.compile(r'(\s)*(/\*|\*|\*/|/\*\*)')


# The project  directory (assumes this script lives in ROOT_DIR/scripts/)
ROOT_DIR = "../"


# Projects that should be excluded from linting / testing / post-processing
NON_PROJECT_DIRS = ['scripts', '.git', 'WISHLIST', 'Readme.md']


def BlockComment(line):
    return BLOCK_COMMENT_LINE_RE.match(line)

    
def ExtractStartingBlockComment(path):
    print 'Extracting block comment'
    file = open(path)
    first = True
    blockComment = ''
    for line in file.readlines():       
        if not BlockComment(line) and first:
            print line
            raise LinterException(
                'Expected file {} to begin with block comment!'.format(path))
        if not BlockComment(line):
            break
        blockComment += line
        first = False
    print 'Start comment is ' + blockComment
    return blockComment


def ExtractCheckStatusBlock(path):
    return None


def LintJavaFile(path):
    print 'Linting file ' + path
    startComment = ExtractStartingBlockComment(path)
    return startComment


def LintCpuFile(path):
    print 'Linting file ' + path
    startComment = ExtractStartingBlockComment(path)
    # TODO: checksReturnStatus = ExtractCheckStatusBlock(path)
    # return startComment and checksReturnStatus
    return startComment


def LintAllFiles():
    for dirpath, dirnames, filenames in os.walk(ROOT_DIR):
        for filename in filenames:
            path = os.path.join(dirpath, filename)
            lint = True
            if DESIGN_FILE_RE.match(path):
                lint = LintJavaFile(path)
            elif CPU_FILE_RE.match(path):
                lint = LintCpuFile(path)
            if not lint:
                print 'Linting file ' + path + ' failed'
                # TODO add descriptive error messages


def GetProjectDirs():   
    return [d for d in os.listdir(ROOT_DIR) if d not in NON_PROJECT_DIRS]


def GetNewOrRecentlyModifiedProjects():
    # TODO: use information from git to find these
    return []

def RunTests():
    print 'Running tests.'


def UpdateWiki():
    print 'Extract comments and update wiki'


def main():
    # TODO: in the long term this should be an interactive shell based
    # program or at the very least take some command line args to
    # support selective: 
    #   1. linting
    #   2. testing
    #   3. comment extraction and updates
    #   4. new project creation

    print '1.Found the following projects {}\n'.format(GetProjectDirs())

    print '2.Found new or recently modified projects {}\n'.format(
        GetNewOrRecentlyModifiedProjects())

    print 'Linting new projects' 
    # TODO: only lint new or recently modified files
    LintAllFiles()

    print 'Testing new files'
    # TODO: only test new or recently modified files
    # TODO: RunTests()

    print 'Generating wiki pages for new projects'
    # TODO: UpdateWiki()

if __name__ == "__main__":
    main()

