import argparse
import shutil
from pathlib import Path

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="tools.install",
        description="Install or uninstall this template to ~/.xcpc.",
    )

    # 设置互斥组为必须提供其中一项
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument(
        "--install",
        action="store_true",
        help="Install this template to ~/.xcpc.",
    )
    mode.add_argument(
        "--uninstall",
        action="store_true",
        help="Uninstall this template from ~/.xcpc.",
    )

    parser.add_argument(
        "--path",
        type=Path,
        default=Path("~/.xcpc").expanduser(),
        help="Path to install/uninstall the template.",
    )

    # 自动解析 sys.argv[1:]
    args = parser.parse_args()

    if args.install:
        args.path.mkdir(parents=True, exist_ok=True)
        print("Installing template to", args.path)

        shutil.copytree(
            "src/alfred",
            args.path,
            dirs_exist_ok=True,
        )

        print("Template installed successfully.")
        print("Add this to your .vscode/c_cpp_properties.json:")
        print("""{
    "configurations": [
        {
            "includePath": [
                "~/.xcpc/", // or C:/Users/[username]/.xcpc/ on Windows
            ],
        }
    ],
}
""")

    elif args.uninstall:
        print("Uninstalling template from", args.path)
        shutil.rmtree(args.path, ignore_errors=True)
