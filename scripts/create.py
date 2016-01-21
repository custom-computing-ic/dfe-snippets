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
    if os.path.isfile(os.path.join(dest, 'build', 'Makefile')):
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

    if options.makefile == 'cmake':
      # remove the build directory
      shutil.rmtree(os.path.join(dest, 'build'));
      return files

    if not options.concept:
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
    parser.add_argument(
        '-l', '--language',
        default='cpp',
        choices=['c99', 'c++', 'python'],
        help = 'Generate a project for the given language.')
    parser.add_argument('name', help='Name of the project')
    parser.add_argument(
        '-c', '--concept', help='Required to create a new snippet')
    parser.add_argument(
        '-m', '--makefile',
        choices=['gnumake', 'cmake'],
        default='gnumake',
        help='Makefile to use for the new snippet')
    args = parser.parse_args()

    if args.concept:
        print "Creating new snippet."
        script = sys.argv[0]
        # TODO Add a a more reliable way to check if at root (perhaps
        # based on the readme location?)
        if not (script.startswith('scripts') or
                script.startswith('./scripts')):
            print 'Run this script from the maxdfe-snippets root directory.'
            return
    else:
        print "Creating new standalone project."

    projectName = args.name
    projectConcept = args.concept

    # copy template to the target location
    dest = ''
    if args.concept:
        dest += 'test/' + projectConcept + "/" + projectName
        projectRoot = "../../.."
    else:
        dest += projectName
        projectRoot = ".."

    files = CopyTemplateFiles(dest, args)

    # replace macros and rename files based on project name
    macro_dict = {
        PROJECT_NAME_MACRO: projectName,
        PROJECT_ROOT_MACRO: projectRoot,
    }
    ProcessSourceFiles(files, dest, macro_dict)

    # printSummary
    PrintSummary(files, dest, args)
    if args.makefile == 'cmake':
      print 'NOTE, please add the following line to the top level CMakeLists.txt'
      print '   add_fpga_build({} {} {} {}Manager)'.format(
          projectConcept, projectName, projectName, projectName)

if __name__ == '__main__':
    main()
