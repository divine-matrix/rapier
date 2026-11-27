from datetime import datetime
import os
import zipfile


class FilterData:

    def __init__(self):
        # Extension settings
        self.whitelist_extensions = []
        self.blacklist_extensions = []
        self.extension_filter_mode = (
            "whitelist"  # "whitelist", "blacklist", "both", or "none"
        )

        # Directory settings
        self.whitelist_dirs = []
        self.blacklist_dirs = []
        self.dir_filter_mode = (
            "blacklist"  # "whitelist", "blacklist", "both", or "none"
        )

        # Paths and Backup
        self.target_dir = "."
        self.backup_zip_path = f"backup_{datetime.now().strftime('%Y%m%d_%H%M%S')}.zip"


def normalize_extensions(ext_list):
    return [
        ext.lower() if ext.startswith(".") else f".{ext.lower()}" for ext in ext_list
    ]


def is_dir_allowed(dir_name, filter_data):
    mode = filter_data.dir_filter_mode
    if mode == "whitelist":
        return dir_name in filter_data.whitelist_dirs
    elif mode == "blacklist":
        return dir_name not in filter_data.blacklist_dirs
    elif mode == "both":
        return (
            dir_name in filter_data.whitelist_dirs
            and dir_name not in filter_data.blacklist_dirs
        )
    return True


def is_ext_allowed(ext, whitelist, blacklist, mode):
    if mode == "whitelist":
        return ext in whitelist
    elif mode == "blacklist":
        return ext not in blacklist
    elif mode == "both":
        return ext in whitelist and ext not in blacklist
    return True


def find_files(filter_data):
    files = []
    whitelist_ext = normalize_extensions(filter_data.whitelist_extensions)
    blacklist_ext = normalize_extensions(filter_data.blacklist_extensions)

    for dirpath, dirnames, filenames in os.walk(filter_data.target_dir):
        # Modify dirnames in-place so os.walk skips excluded subdirectories
        dirnames[:] = [d for d in dirnames if is_dir_allowed(d, filter_data)]

        for file in filenames:
            _, ext = os.path.splitext(file)
            if is_ext_allowed(
                ext.lower(),
                whitelist_ext,
                blacklist_ext,
                filter_data.extension_filter_mode,
            ):
                files.append(os.path.join(dirpath, file))

    return files


def create_backup(file_paths, target_dir, zip_path):
    if not file_paths:
        print("No files matched. Skipping backup.")
        return False

    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zipf:
        for file_path in file_paths:
            # Store files using their relative path inside the zip archive
            arcname = os.path.relpath(file_path, start=target_dir)
            zipf.write(file_path, arcname=arcname)

    print(f"Backup created: {zip_path} ({len(file_paths)} files)")
    return True


def search_and_replace(file_paths, search_keyword, replace_keyword):
    for file_path in file_paths:
        try:
            with open(file_path, "r", encoding="utf-8") as f:
                content = f.read()

            if search_keyword in content:
                updated_content = content.replace(search_keyword, replace_keyword)
                with open(file_path, "w", encoding="utf-8") as f:
                    f.write(updated_content)
                print(f"Updated: {file_path}")

        except UnicodeDecodeError:
            print(f"Skipped (not a UTF-8 text file): {file_path}")
        except Exception as e:
            print(f"Error processing {file_path}: {e}")


# --- Usage Example ---
dat = FilterData()
dat.target_dir = "."

# Configure extension filtering
dat.whitelist_extensions = ["cpp", "h"]
dat.extension_filter_mode = "whitelist"

# Configure directory filtering (e.g., skip build and git folders)
dat.blacklist_dirs = [".git", "build", "node_modules"]
dat.dir_filter_mode = "blacklist"

# 1. Find matching files
matched_files = find_files(dat)

# 2. Back up matched files before modifying anything
if create_backup(matched_files, dat.target_dir, dat.backup_zip_path):
    # 3. Perform search and replace
    search_and_replace(matched_files, "sabre_", "")
