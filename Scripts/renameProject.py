import os
import shutil

LAST_PROJECT_NAME = "NUCLEO_F446RE_CubeIDE_template"  # プロジェクト名を変更する前の名前


def main():
    projectName = get_projectName()
    print("Project Name: " + projectName)
    rename_projectFile(projectName)  # .projectファイルのプロジェクト名を変更する
    rename_cprojectFile(projectName)  # .cprojectファイルのプロジェクト名を変更する
    rename_file(
        "../" + LAST_PROJECT_NAME + ".ioc", "../" + projectName + ".ioc"
    )  # *.iocファイルの名前を変更する
    delete_Files()  # .launchファイル，Debugフォルダ，Releaseフォルダを削除する(実は.gitignoreで外しているので不要かも)


def get_projectName():
    os.chdir("../")  # Scriptsフォルダからプロジェクトフォルダに移動
    path = os.getcwd()
    os.chdir("Scripts")  # プロジェクトフォルダからScriptsフォルダに移動
    projectName = os.path.basename(path)
    return projectName


def rename_projectFile(projectName):
    projectFile = open("../.project", "r")
    projectFileData = projectFile.read()
    projectFile.close()

    projectFileData = projectFileData.replace(LAST_PROJECT_NAME, projectName)

    projectFile = open("../.project", "w")
    projectFile.write(projectFileData)
    projectFile.close()
    print("Project Name changed in .project file")


def rename_cprojectFile(projectName):
    projectFile = open("../.cproject", "r")
    projectFileData = projectFile.read()
    projectFile.close()

    projectFileData = projectFileData.replace(LAST_PROJECT_NAME, projectName)

    projectFile = open("../.cproject", "w")
    projectFile.write(projectFileData)
    projectFile.close()
    print("Project Name changed in .cproject file")


def rename_file(old_name, new_name):
    try:
        os.rename(old_name, new_name)
        print(f"File renamed from {old_name} to {new_name}")
    except FileNotFoundError:
        print(f"File {old_name} not found.")
    except Exception as e:
        print(f"An error occurred: {e}")


def delete_Files():
    os.chdir("../")  # Scriptsフォルダからプロジェクトフォルダに移動
    Files = os.listdir()
    for file in Files:
        if file == "Debug":
            print("Delete Debug Folder : " + file)
            shutil.rmtree(file)
        elif file == "Release":
            print("Delete Release Folder : " + file)
            shutil.rmtree(file)
        elif file.endswith(".launch"):
            print("Delete Launch File : " + file)
            os.remove(file)
    os.chdir("Scripts")  # プロジェクトフォルダからScriptsフォルダに移動


if __name__ == "__main__":
    main()
