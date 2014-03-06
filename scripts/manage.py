"""Simple script to manage the creation and verification of projects."""
# TODO Update to work with projects that have maultiple maxcompiler implementations
# At the moment, the assumed dir structure is Concept/Implementation/*.*

import os
import re
import subprocess
from optparse import OptionParser


from os.path import join


# Custom exception definitions
class LinterException(Exception):
    pass


# Some regular expresssion definitions useful for this module
DESIGN_FILE_RE = re.compile(r'.*\.(maxj|java)')
CPU_FILE_RE = re.compile(r'.*\.(cpp|c)')
MAKEFILE_RE = re.compile(r'Makefile')

START_COMMENT_RE = re.compile(r'(\s)*/\*\*\*') # start comments with /***
END_COMMENT_RE = re.compile(r'(\s)*\*/') # end comments with */

GIT_BRANCH = re.compile(r'\* .*')
PROJECT_RE = re.compile(r'../(?P<concept>.*)/(?P<project>.*)')

# The project  directory (assumes this script lives in ROOT_DIR/scripts/)
ROOT_DIR = "../"
REPO_NAME = "dfe-snippets"
WIKI_PATH = '../wiki'
WIKI_URL = 'git@bitbucket.org:customcomputing/' + REPO_NAME + '.git/wiki'

# Projects that should be excluded from linting / testing / post-processing
NON_PROJECT_DIRS = ['scripts', '.git', 'WISHLIST', 'Readme.md', 'wiki']
NON_IMPL_FILES = ['README']


STARTING_COMMENT_MAP = {}

class WikiPage(object):
    """Represents the wiki page corresponding to a project."""

    def __init__(self, contents, project=None):
        self.contents = contents
        self.project = project

    def WriteToFile(self, wiki_path, name=None):
        if not name and not self.project:
            print "Can't generate project names"
            return
        filename = self.project.concept + '-'  if self.project else ''
        filename += name if name else self.project.name
        filename += '.md'
        f = open(os.path.join(wiki_path, filename), 'w')
        f.write(self.contents)
        f.close()


class Comment(object):
    """A comment extracted from a source file."""

    def __init__(self, comment, snippet):
        self.comment = comment
        self.snippet = snippet

    def __str__(self):
        return 'Comment(c={}, s={})'.format(self.comment, self.snippet)


class Project(object):
    """Represents a project in the snippets repository."""

    def __init__(self, concept, name, path, new=False):
        super(Project, self).__init__()
        print 'Creating project ' + str(concept) + ' ' + str(name)
        self.concept = concept
        self.name = name
        self.path = path
        self.new = new
        self.starting_comments = {}
        self.in_line_comments = {}

    def GetFullPathRelativeToTopDir(self):
        return os.path.join(self.concept, self.name)

    def InterestingFile(path):
        return DESIGN_FILE_RE.match(path) or CPU_FILE_RE.match(path) or MAKEFILE_RE.match(path)

    def GetSourceFiles(self):
        return [os.path.join(self.GetSourceDir(), p) for p in os.listdir(self.GetSourceDir())]

    def GetComments(self, file_name):
        return ExtractCommentsFromFile(file_name, self)

    def GetSourceDir(self):
        return os.path.join(self.path, 'src')

    def IsValid(self):
        return os.path.exists(self.GetSourceDir())

    def __str__(self):
        return 'Project(concept={}, name={}, path={}, files={})'.format(
            self.concept, self.name, self.path, [str(f) for f in self.GetSourceFiles()])

    def __hash__(self):
        return hash(self.concept)

    def __eq__(self, other):
        return self.concept == other.concept and self.name == other.name


def ExtractCommentsFromFile(path, project):
    class State(object):
        NONE = 0
        PARSING_COMMENT = 1
        PARSING_SNIPPET = 2

    try:
        file = open(path)
    except:
        print "Could not open file " + path
        return ([], [])

    first = True
    first_comment = True
    parsed_code = False
    comment = ""
    snippet = ""

    s = State.NONE

    starting_comment = None
    in_line_comments = []

    for line in file.readlines():
        if s == State.PARSING_SNIPPET:
            if line.strip():
                snippet += line
                continue

            filename = os.path.basename(path)
            c = Comment(comment, snippet)
            if first_comment and not parsed_code:
                starting_comment = c
                first_comment = False
            else:
                in_line_comments.append(c)
            comment = ''
            snippet = ''
            s = State.NONE
        elif s == State.PARSING_COMMENT:
            if re.match(END_COMMENT_RE, line):
                s = State.PARSING_SNIPPET
                continue
            comment += line.lstrip()
            if not line.lstrip():
                comment += '\n'
        else:
            if re.match(START_COMMENT_RE, line):
                s = State.PARSING_COMMENT
            elif line.strip():
                parsed_code = True

    print file
    print "Starting comment."
    print starting_comment
    print in_line_comments
    return starting_comment, in_line_comments


def ExtractCheckStatusBlock(path):
    return None

def LintJavaFile(path, project):
    ExtractComments(path, project)
    return start_comment

def LintCpuFile(path, project):
    ExtracOmments(path, projects)
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
    print 'concept filename: ' + filename
    match = PROJECT_RE.match(filename)
    if match:
        return match.group('concept')
    return None


def LoadGitProjectData(git_process):

    modified_projs = set()
    new_projs = set()

    while True:
        line = git_process.stdout.readline()
        if not line:
            break

        # git status returns ' M <file>', diff returns 'M <file>'
        line = line.strip()
        change_type = line[0]

        line = line[2:].strip()
        project_name = GetProjectName(line)
        project_path = os.path.dirname(line)
        concept = GetProjectConcept(line)
        print 'concept {}' % (concept, )
        if project_name:
            if change_type == 'M':
                modified_projs.add(Project(concept, project_name, project_path))
            elif change_type == 'A':
                new_projs.add(Project(concept, project_name, project_path, True))

    return modified_projs, new_projs


def GetLocallyModifiedFiles():
    git_proc = subprocess.Popen(['git', 'status', '-s'], stdout=subprocess.PIPE)
    a, b =  LoadGitProjectData(git_proc)
    return a, b


def GetModifiedFilesInBranch(localGitBranch):
    """This assumes that the local branch is tracking the same named remote branch."""
    remote_branch = 'remotes/origin/' + localGitBranch
    git_proc = subprocess.Popen(['git', 'diff', '--name-status',
                                 remote_branch, localGitBranch, '--'],
                                stdout=subprocess.PIPE)
    return LoadGitProjectData(git_proc)



def GetAllModifiedOrNewFiles(localGitBranch):
    # get changes committed locally that differ from remote branch
    modified_projs, new_projs = GetLocallyModifiedFiles()

    # get un-commited local changes
    local_modified_projs, local_new_projs = GetModifiedFilesInBranch(localGitBranch)


    return list(modified_projs | local_modified_projs), list(new_projs | local_new_projs)


def GetNewOrRecentlyModifiedProjects():
    current_git_branch = GetCurrentGitBranch()
    modified_projects = GetAllModifiedOrNewFiles(current_git_branch)
    return modified_projects


def RunTests(projects):
    pass

def ReadWholeFile(file):
    if os.path.isfile(file):
        f = open(file)
        return f.read()
    return ""

def GenerateWikiPage(project):
    contents = ""
    comment = (None, None)
    for f in project.GetSourceFiles():
        if project.IsValid():
            comments = project.GetComments(f)
        print "comments"
        print comments
        contents += os.path.basename(f) + '\n====\n'
        if not comments[0]:
            contents += '\n```\n'
            contents += ReadWholeFile(f)
            contents += '\n```\n'
            contents += '\n\n'
            continue
        contents += comments[0].comment.strip() + '\n'
        if comments[1]:
            contents += "##Snippets\n"
        for c in comments[1]:
            contents += c.comment.strip()
            if c.snippet:
                contents += '\n```\n'
                contents += c.snippet
                contents += '\n```\n'
        contents += '\n'

    return WikiPage(contents, project)


def GenerateWikiPages(projects):
    wiki_pages = []
    for project in projects:
        wiki_pages.append(GenerateWikiPage(project))
    return wiki_pages

def GetProjectPaths():
    projects = []
    concepts = os.listdir(ROOT_DIR)

    for c in concepts:
        c_path = os.path.join(ROOT_DIR, c)
        if not os.path.isdir(c_path) or c in NON_PROJECT_DIRS:
            continue

        impls = os.listdir(c_path)
        for impl in impls:
            if impl not in NON_IMPL_FILES:
                projects.append(os.path.join(c_path, impl))

    return projects


def GenerateContentsPage():
    projects = GetProjectPaths()
    print projects

    contents = 'Projects\n===\n'
    contents += 'This is a list of all projects.'
    contents += ' NOTE! This page and all project pages are generated!'
    contents += ' Do not edit manually!\n\n'

    for (i, p) in enumerate(projects):
        print p
        concept = GetProjectConcept(p)
        impl = GetProjectName(p)
        print concept
        print impl
        wiki_page = concept + '-' + impl
        contents += '{}. [{}]({})\n'.format(
            str(i), wiki_page, wiki_page)

    return WikiPage(contents)


def GetAllProjects():
    """Returns a list of all projects contained in the repository."""

    project_paths = GetProjectPaths()
    projects = []

    for p in project_paths:
        concept = GetProjectConcept(p)
        impl = GetProjectName(p)
        print 'concept {}'.format(concept)
        projects.append(Project(concept, impl, p))

    return projects


def main():
    # TODO: in the long term this should be an interactive shell based
    # program or at the very least take some command line args to
    # support selective:
    #   1. linting
    #   2. testing
    #   4. new project creation


    parser = OptionParser()
    parser.add_option('-a', '--all', default=False, action='store_true',
                      help='re-generate wiki for all projects')
    (options, args) = parser.parse_args()


    projects_to_analyze = []

    if options.all:
        print '1. Re-generating wiki for all projects, this might take a while'
        projects_to_analyze = GetAllProjects()
        print '\tFound {} projects to analyze'.format(len(projects_to_analyze))
    else:
        modified_projs, new_projects = GetNewOrRecentlyModifiedProjects()
        changed_projects = new_projects + modified_projs
        print '1. Found {} new or recently modified projects:'.format(
            len(changed_projects))
        for proj in changed_projects:
            print '\t' + str(proj) + '\n'
        projects_to_analyze = changed_projects


    for p in projects_to_analyze:
        if not p.IsValid():
            print 'Warning! project: ' + p.name + ' is not a valid project!'
            projects_to_analyze.remove(p)

    print '2. Linting all changed projects'
#    lint_stats = LintProjects(changed_projects)
#    print '\tLinted {} Design file(s), {} CPU file(s) and {} Makefiles\n'.format(
#        lint_stats[0], lint_stats[1], lint_stats[2])


    print '3. Testing all changed projects\n'
# TODO RunTests(changed_projects)

    print '4. Generating wiki page for changed projects ({})'.format(
        [str(s) for s in projects_to_analyze])


    if not os.path.isdir(WIKI_PATH):
        print 'Error: Could not find wiki directory {}.'.format(WIKI_PATH)
        print '\tPlease do a git checkout of the wiki from {}'.format(WIKI_URL)
        return 1

    for page in GenerateWikiPages(projects_to_analyze):
        page.WriteToFile(WIKI_PATH)
    GenerateContentsPage().WriteToFile(WIKI_PATH, name='Projects')

if __name__ == "__main__":
    main()
