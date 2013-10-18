"""Simple script to manage the creation and verification of projects."""
import os
import re
import subprocess


from os.path import join


# Custom exception definitions
class LinterException(Exception):
    pass


# Some regular expresssion definitions useful for this module
DESIGN_FILE_RE = re.compile(r'.*\.(maxj|java)')
CPU_FILE_RE = re.compile(r'.*\.(cpp|c)')
MAKEFILE_RE = re.compile(r'Makefile')
BLOCK_COMMENT_LINE_RE = re.compile(r'(\s)*(/\*|\*/|\*|/\*\*)')
GIT_BRANCH = re.compile(r'\* .*')
PROJECT_RE = re.compile(r'(?P<concept>.*)/(?P<project>.*)/.*')

# The project  directory (assumes this script lives in ROOT_DIR/scripts/)
ROOT_DIR = "../"


# Projects that should be excluded from linting / testing / post-processing
NON_PROJECT_DIRS = ['scripts', '.git', 'WISHLIST', 'Readme.md']


STARTING_COMMENT_MAP = {}


class Project(object):

    def __init__(self, concept, name, new=False):
        super(Project, self).__init__()
        self.concept = concept
        self.name = name
        self.new = new
        self.comments = {}

    def GetFullPathRelativeToTopDir(self):
        return os.path.join(self.concept, self.name)

    def __str__(self):
        return 'Project(concept={}, name={})'.format(self.concept, self.name)

    def __hash__(self):
        return hash(self.concept)

    def __eq__(self, other):
        return self.concept == other.concept and self.name == other.name

def BlockComment(line):
    return BLOCK_COMMENT_LINE_RE.match(line)


def ExtractStartingBlockComment(path, project):
    file = open(path)
    first = True
    block_comment = ''
    for line in file.readlines():       
        if not BlockComment(line) and first:
            print line
            raise LinterException(
                'Expected file {} to begin with block comment!'.format(path))
        if not BlockComment(line):
            break
        block_comment += re.sub(BLOCK_COMMENT_LINE_RE, '', line).strip() + ' '
        first = False

    if block_comment:
        filename = os.path.basename(path)
        project.comments[filename] = block_comment.strip()
    return block_comment


def ExtractCheckStatusBlock(path):
    return None


def LintJavaFile(path, project):
    start_comment = ExtractStartingBlockComment(path, project)
    return start_comment


def LintCpuFile(path, project):
    start_comment = ExtractStartingBlockComment(path, project)
    # TODO: checksReturnStatus = ExtractCheckStatusBlock(path)
    # return start_comment and checksReturnStatus
    return start_comment


def LintProjects(projects):
    # represents the number of linted files (Design, CPU, Makefile)
    stats = [0, 0, 0]
    for proj in projects:
        top_rel_path = proj.GetFullPathRelativeToTopDir()
        proj_path = os.path.join(ROOT_DIR, top_rel_path)
        print '\tLinting ' + top_rel_path
        for dirpath, dirnames, filenames in os.walk(proj_path):
            print '\t\tFiles ' + str(filenames)
            for filename in filenames:
                path = os.path.join(dirpath, filename)
                lint = True
                if DESIGN_FILE_RE.match(path):
                    lint = LintJavaFile(path, proj)
                    stats[0] += 1
                elif CPU_FILE_RE.match(path):
                    lint = LintCpuFile(path, proj)
                    stats[1] += 1
                if not lint:
                    print 'Linting file ' + path + ' failed'

    return stats


def GetProjectDirs():   
    return [d for d in os.listdir(ROOT_DIR) if d not in NON_PROJECT_DIRS]


def GetCurrentGitBranch():
    git_proc = subprocess.Popen(['git', 'branch'], stdout=subprocess.PIPE)

    current_git_branch = None
    while True:
        line = git_proc.stdout.readline()        
        if not line:
            break
        if GIT_BRANCH.match(line):
            current_git_branch = line[2:].strip()
            break
        
    return current_git_branch


def GetProjectName(filename):
    match = PROJECT_RE.match(filename)
    if match:
        return match.group('project')
    return None


def GetProjectConcept(filename):
    match = PROJECT_RE.match(filename)
    if match:
        return match.group('concept')
    return None
    

def GetModifiedFilesInBranch(localGitBranch):
    """This assumes that the local branch is tracking the same named remote branch."""
    remote_branch = 'remotes/origin/' + localGitBranch
    git_proc = subprocess.Popen(['git', 'diff', '--name-status', 
                                remote_branch, localGitBranch, '--'], 
                               stdout=subprocess.PIPE)
    modified_projs = set()
    new_projs = set()
    
    while True:
        line = git_proc.stdout.readline()
        if not line:
            break
        change_type = line[0]
        line = line[2:].strip()
        project_name = GetProjectName(line)
        concept = GetProjectConcept(line)
        if project_name:
            if change_type == 'M':
                modified_projs.add(Project(concept, project_name))
            elif change_type == 'A':
                new_projs.add(Project(concept, project_name, True))

    return list(modified_projs), list(new_projs)

        
def GetNewOrRecentlyModifiedProjects():
    current_git_branch = GetCurrentGitBranch()
    modified_projects = GetModifiedFilesInBranch(current_git_branch)
    return modified_projects


def RunTests(projects):
    pass


def ExtractCommentsFromNewFiles(projects):
    return [proj.comments for proj in projects if proj.new]

def main():
    # TODO: in the long term this should be an interactive shell based
    # program or at the very least take some command line args to
    # support selective: 
    #   1. linting
    #   2. testing
    #   3. comment extraction and updates
    #   4. new project creation

    modified_projs, new_projects = GetNewOrRecentlyModifiedProjects()
    changed_projects = new_projects + modified_projs
    print '1. Found {} new or recently modified projects: {}\n'.format(
        len(changed_projects), [str(s) for s in changed_projects])


    print '2. Linting all changed projects' 
    lint_stats = LintProjects(changed_projects)
    print '\tLinted {} Design file(s), {} CPU file(s) and {} Makefiles\n'.format(
        lint_stats[0], lint_stats[1], lint_stats[2])


    print '3. Testing all changed projects\n'
    # TODO RunTests(changed_projects)

    print '4. Generating wiki comments for new projects only ({})'.format(new_projects)
    comments = ExtractCommentsFromNewFiles(new_projects)
    print comments

if __name__ == "__main__":
    main()
