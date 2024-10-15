import os
import time
import sys
import logging
from datetime import datetime

def restart_script():
    logging.info("Restarting the script...")
    print("Restarting the script...")
    time.sleep(3)
    os.execv(sys.executable, [sys.executable] + sys.argv)

try:
    import git
    import schedule
except:
    import Installers.Install_Py_Libs
    restart_script()


# Define the path to your local repository (current directory)
REPO_PATH = "."
BRANCH = "main"
LOG_FILE = "./GitMonitorLog.txt"
MAX_LOG_SIZE = 100 * 1024  # 100KB in bytes

# Set up logging
logging.basicConfig(
    filename=LOG_FILE,
    filemode='a',  # Append to the file
    format='%(asctime)s - %(levelname)s - %(message)s',
    level=logging.INFO
)

def log_startup():
    logging.info("Git Monitor Script started.")
    print("Git Monitor Script started.")

def log_shutdown():
    logging.info("Git Monitor Script stopped.")
    print("Git Monitor Script stopped.")

def log_error(error_message):
    logging.error(f"Error occurred: {error_message}")
    print(f"Error occurred: {error_message}")

def log_change_detected():
    logging.info("Change detected in the remote repository. Repository updated.")
    print("Change detected in the remote repository. Repository updated.")

def trim_log_file():
    """Ensure the log file doesn't exceed MAX_LOG_SIZE by trimming older content."""
    if os.path.isfile(LOG_FILE) and os.path.getsize(LOG_FILE) > MAX_LOG_SIZE:
        with open(LOG_FILE, 'r+') as f:
            lines = f.readlines()
            # Keep reducing lines until size is under the threshold
            while os.path.getsize(LOG_FILE) > MAX_LOG_SIZE and lines:
                lines.pop(0)  # Remove the oldest line
                f.seek(0)
                f.writelines(lines)
                f.truncate()
        logging.info(f"Log file trimmed to stay below {MAX_LOG_SIZE / 1024}KB.")
        print(f"Log file trimmed to stay below {MAX_LOG_SIZE / 1024}KB.")
    
def check_for_updates():
    try:
        # Access the local repository
        repo = git.Repo(REPO_PATH)
        # Check the status of the remote branch
        current_branch = repo.active_branch

        if current_branch.name != BRANCH:
            logging.info(f"Currently on branch {current_branch.name}, switching to {BRANCH}.")
            print(f"Currently on branch {current_branch.name}, switching to {BRANCH}.")
            repo.git.checkout(BRANCH)

        # Fetch the latest updates from the remote
        origin = repo.remotes.origin
        origin.fetch()

        # Get the local and remote commit hashes
        local_commit = repo.head.commit.hexsha
        remote_commit = origin.refs[BRANCH].commit.hexsha

        # If local and remote commits differ, pull the changes
        if local_commit != remote_commit:
            log_change_detected()
            origin.pull(BRANCH)
            restart_script()
        else:
            logging.info("No new changes found.")
            print("No new changes found.")
        
    except Exception as e:
        log_error(str(e))

# Schedule the task to run every couple of minutes (e.g., 5 minutes)
schedule.every(1).minutes.do(check_for_updates)

if __name__ == "__main__":
    try:
        # Log the startup
        log_startup()

        # Initial check at startup
        check_for_updates()

        # Keep the script running and checking for updates periodically
        while True:
            schedule.run_pending()
            time.sleep(1)

            # Trim the log file if it gets too big
            trim_log_file()

    except KeyboardInterrupt:
        # Log the shutdown if the script is interrupted
        log_shutdown()
