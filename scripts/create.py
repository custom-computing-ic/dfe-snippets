#!/usr/bin/env python

import os
import sys
import shutil
from os.path import basename, dirname
from optparse import OptionParser


PROJECT_NAME_MACRO = "%%%ProjectName%%%"
PROJECT_ROOT_MACRO = "%%%ProjectRoot%%%"

def ReplaceMacros(path, projectName, project_macros):
    f = open(path)
    content = f.read()
    f.close()
    os.remove(path)

    for k, v in project_macros.iteritems():
        content = content.replace(k, v)
    new_file_name = basename(path).replace("Demo", projectName)
    dir_name = dirname(path)
    new_file_path = dir_name + "/" + new_file_name

    f = open(new_file_path, 'w')
    f.write(content)


def main():

    parser = OptionParser()
    parser.add_option('-s', '--standalone', default=False, action='store_true',
                      help="""Generate a completely standalone project. Use this
                              when you want to create DFE projects for use
                              outside of the dfe-snippets project.""")
    (options, args) = parser.parse_args()

    if options.standalone:
        print "Creating standalone project."
        if len(sys.argv) != 3:
            print "Usage create.py <ProjectName> -s"
            return
    else:
        if len(sys.argv) != 3:
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
    projectConcept = sys.argv[2] if len(sys.argv) > 2 else None

    # copy template to the target location
    if options.standalone:
        dest = projectName
        projectRoot = ".."
    else:
        projectRoot = "../../.."
        dest = projectConcept + "/" + projectName

    d = os.path.dirname(__file__)
    templates_path=os.path.join(d, 'template')
    demo_project_path=os.path.join(templates_path, 'DemoProject')
    shutil.copytree(demo_project_path, dest)

    macro_dict = {
        PROJECT_NAME_MACRO: projectName,
        PROJECT_ROOT_MACRO: projectRoot,
        }


    # replace macros with project name
    # just hardcode this for now, perhaps make it more flexible in the future
    for f in ['DemoKernel.maxj', 'DemoCpuCode.c', 'DemoManager.maxj']:
        ReplaceMacros(dest + "/src/" + f, projectName, macro_dict)
    ReplaceMacros(dest + "/build/Makefile", projectName, macro_dict)


    if options.standalone:
        # For standalone projects, copy makefiles
        for f in ['.common', '.Maia.hardware', '.Max3.hardware']:
            makefile = 'Makefile' + f
            shutil.copyfile(os.path.join(templates_path, makefile),
                            os.path.join(dest, makefile))

if __name__ == '__main__':
    main()
