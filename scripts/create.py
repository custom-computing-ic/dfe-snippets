#!/usr/bin/env python

import os
import sys
import shutil
from os.path import basename, dirname
from optparse import OptionParser


PROJECT_NAME_MACRO = "%%%ProjectName%%%"
PROJECT_ROOT_MACRO = "%%%ProjectRoot%%%"


def ReplaceMacros(path, project_macros):
    f = open(path)
    content = f.read()
    f.close()
    os.remove(path)

    for k, v in project_macros.iteritems():
        content = content.replace(k, v)
    new_file_name = basename(path).replace("Demo",
                                           project_macros.get(PROJECT_NAME_MACRO))
    dir_name = dirname(path)
    new_file_path = dir_name + "/" + new_file_name

    f = open(new_file_path, 'w')
    f.write(content)


def PrintSummary(files, dest, options):
    print 'Created {}{} project.'.format(
        'standalone ' if options.standalone else '',
        'C99' if options.clang else 'C++'
    )
    print '   Path:  {}'.format(dest)
    print '   Files: '
    for root, dirs, files in os.walk(dest):
        for f in files:
            print '     ' + os.path.join(root, f)


def ProcessSourceFiles(files, dest, macro_dict):
    srcRoot = os.path.join(dest, 'src')
    for f in files:
        ReplaceMacros(srcRoot + "/" + f, macro_dict)
    ReplaceMacros(dest + "/build/Makefile", macro_dict)


def CopyTemplateFiles(dest, options):
    templates_path = os.path.join(os.path.dirname(__file__), 'template')
    srcRoot = os.path.join(dest, 'src')
    demo_project_path = os.path.join(templates_path, 'DemoProject')

    files = [
        'DemoKernel.maxj',
        'DemoManager.maxj'
    ]
    files.append('DemoCpuCode.c' if options.clang else 'DemoCpuCode.cpp')
    shutil.copytree(demo_project_path, dest)

    # remove extra source files
    for f in os.listdir(srcRoot):
        if f not in files:
            os.remove(os.path.join(srcRoot, f))

    if options.standalone:
        # For standalone projects, copy makefiles
        for f in ['.common', '.Maia.hardware', '.Max3.hardware']:
            makefile = 'Makefile' + f
            shutil.copyfile(os.path.join(templates_path, makefile),
                            os.path.join(dest, makefile))

    return files


def main():

    parser = OptionParser()
    parser.add_option('-s', '--standalone', default=False, action='store_true',
                      help="""Generate a completely standalone project. Use this
                              when you want to create DFE projects for use
                              outside of the dfe-snippets project.""")
    parser.add_option('-c', '--clang', default=False, action='store_true',
                      help="""Generate C99 for CPU code.""")

    (options, args) = parser.parse_args()

    if options.standalone:
        print "Creating standalone project."
        if len(sys.argv) < 3:
            print "Usage create.py <ProjectName> -s"
            return
    else:
        if len(sys.argv) < 3:
            print "Usage scripts/create.py <ProjectName> <ProjectConcept>"
            return

        script = sys.argv[0]

        # TODO Add a a more reliable way to check if at root (perhaps
        # based on the readme location?)
        if not (script.startswith('scripts') or
                script.startswith('./scripts')):
            print 'Run this script from the maxdfe-snippets root directory.'
            return

    projectName = sys.argv[1]
    projectConcept = None if options.standalone else sys.argv[2]

    # copy template to the target location
    if options.standalone:
        dest = projectName
        projectRoot = ".."
    else:
        dest = projectConcept + "/" + projectName
        projectRoot = "../../.."

    files = CopyTemplateFiles(dest, options)

    # replace macros and rename files based on project name
    macro_dict = {
        PROJECT_NAME_MACRO: projectName,
        PROJECT_ROOT_MACRO: projectRoot,
    }
    ProcessSourceFiles(files, dest, macro_dict)

    # printSummary
    PrintSummary(files, dest, options)

if __name__ == '__main__':
    main()
