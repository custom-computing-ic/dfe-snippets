#!/usr/bin/env python

import os
import sys
import shutil
from os.path import basename, dirname

import argparse


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
        'standalone ' if options.language else '',
        options.language
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
    if options.language == 'python':
        files += ['DemoCpuCode.py']
    elif options.language == 'c99':
        files += ['DemoCpuCode.c']
    else:
        files += ['DemoCpuCode.cpp']

    shutil.copytree(demo_project_path, dest)

    # remove extra source files
    for f in os.listdir(srcRoot):
        if f not in files:
            os.remove(os.path.join(srcRoot, f))

    if options.language:
        # For standalone projects, copy makefiles
        for f in ['.Maia.hardware', '.Max3.hardware']:
            makefile = 'Makefile' + f
            shutil.copyfile(os.path.join(templates_path, makefile),
                            os.path.join(dest, makefile))

        if options.language == 'python':
            shutil.copyfile(os.path.join(templates_path, 'Makefile.py.common'),
                            os.path.join(dest, 'Makefile.common'))
        else:
            shutil.copyfile(os.path.join(templates_path, 'Makefile.common'),
                            os.path.join(dest, 'Makefile.common'))

    return files


def main():

    parser = argparse.ArgumentParser(
        description='Project creation utility.')
    parser.add_argument('-l', '--language',
                        default='cpp',
                        choices=['c99', 'c++', 'python'],
                        help = """Generate a standalone project for the given language. Use this
                                  when you want to create DFE projects for use
                                  outside of dfe-snippets.""")
    parser.add_argument('name', help='Name of the project')
    parser.add_argument('-c', '--concept', help='Project concept')
    args = parser.parse_args()

    if args.language:
        print "Creating standalone project."
    else:
        if not args.concept:
            print "You must specify a concept"
            return

        script = sys.argv[0]

        # TODO Add a a more reliable way to check if at root (perhaps
        # based on the readme location?)
        if not (script.startswith('scripts') or
                script.startswith('./scripts')):
            print 'Run this script from the maxdfe-snippets root directory.'
            return

    projectName = args.name
    projectConcept = args.concept

    # copy template to the target location
    if args.language:
        dest = projectName
        projectRoot = ".."
    else:
        dest = projectConcept + "/" + projectName
        projectRoot = "../../.."

    files = CopyTemplateFiles(dest, args)

    # replace macros and rename files based on project name
    macro_dict = {
        PROJECT_NAME_MACRO: projectName,
        PROJECT_ROOT_MACRO: projectRoot,
    }
    ProcessSourceFiles(files, dest, macro_dict)

    # printSummary
    PrintSummary(files, dest, args)

if __name__ == '__main__':
    main()
