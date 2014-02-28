import os
import sys
import shutil
from os.path import basename, dirname


PROJECT_NAME_MACRO = "%%%ProjectName%%%"

def ReplaceMacroWithProjectName(path, projectName):
    f = open(path)
    content = f.read()
    f.close()
    os.remove(path)

    content = content.replace(PROJECT_NAME_MACRO, projectName)
    new_file_name = basename(path).replace("Demo", projectName)
    dir_name = dirname(path)
    new_file_path = dir_name + "/" + new_file_name

    f = open(new_file_path, 'w')
    f.write(content)


def main():

    if len(sys.argv) != 3:
        print "Usage scripts/create.py <ProjectName> <ProjectConcept>"
        return

    script = sys.argv[0]
    if not script.startswith('scripts'):
        print 'Run this script from the maxdfe-snippets project root directory.'
        return

    projectName = sys.argv[1]
    projectConcept = sys.argv[2]

    # copy template to the target location
    dest = projectConcept + "/" + projectName
    shutil.copytree("scripts/template/DemoProject", dest)

    # replace macros with project name
    # just hardcode this for now, perhaps make it more flexible in the future
    for f in ['DemoKernel.maxj', 'DemoCpuCode.c', 'DemoManager.maxj']:
        ReplaceMacroWithProjectName(dest + "/src/" + f, projectName)

    ReplaceMacroWithProjectName(dest + "/build/Makefile", projectName)



if __name__ == '__main__':
    main()
