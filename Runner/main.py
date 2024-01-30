import time
import os
import subprocess
import tempfile, shutil

import cv2

IMGPATH = "..\\Render\\output.ppm"
EXEPATH = "..\\main.exe"
WINDOWNAME = "Output"
WINDOW_SIZE = (200, 100)

last_modified = 0

cv2.namedWindow(WINDOWNAME)
cv2.setWindowProperty(WINDOWNAME, cv2.WND_PROP_TOPMOST, 1)

while True:
    modified = os.stat(EXEPATH).st_mtime if os.path.exists(EXEPATH) else 0
    if modified != last_modified:
        last_modified = modified
        print("Modified, re-running exe...")

        temp_dir = tempfile.gettempdir()
        temp_exe = os.path.join(temp_dir, "main.exe")
        try:
            shutil.copy2(EXEPATH, temp_exe)
        except PermissionError:
            "File locked..."
            time.sleep(0.25)
            continue

        now = time.time()
        try:
            returncode = subprocess.Popen(
                temp_exe, cwd=os.getcwd() + "\\..").wait()
        except OSError:
            print("Exe not runnable")
            continue
        if returncode != 0:
            print("Exe failed")
            continue
        print(f"Runtime: {1000 * (time.time() - now):0.1f}ms")
        
        image = cv2.imread(IMGPATH)
        image = cv2.resize(image, WINDOW_SIZE, interpolation=cv2.INTER_LINEAR)
        cv2.imshow(WINDOWNAME, image)

    time.sleep(0.25)
    cv2.pollKey()
