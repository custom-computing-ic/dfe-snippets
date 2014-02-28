import sys
import shutil

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


if __name__ == '__main__':
    main()
