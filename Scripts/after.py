import os

def rename_file(old_name, new_name):
    try:
        os.rename(old_name, new_name)
        print(f"File renamed from {old_name} to {new_name}")
    except FileNotFoundError:
        print(f"File {old_name} not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

def main():
    old_name = "../Core/Src/main.c"
    new_name = "../Core/Src/main.cpp"
    rename_file(old_name, new_name)

if __name__ == "__main__":
    main()