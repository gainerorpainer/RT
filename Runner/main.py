import time
import os
import subprocess

import cv2

imgpath = "..\\Render\\output.ppm"
exepath = "..\\main.exe"

last_modified = 0

while True:
    modified = os.stat(exepath).st_mtime
    if modified != last_modified:
        last_modified = modified
        print("Modified, re-running exe...")

        now = time.time()
        try:
            returncode = subprocess.Popen(exepath, cwd=os.getcwd() + "\\..").wait()
        except PermissionError:
            "File locked..."
            time.sleep(0.25)
            continue
        if returncode != 0:
            print("Exe failed")
            continue

        print(f"Runtime: {1000 * (time.time() - now):0.1f}ms")
        image = cv2.imread(imgpath)
        cv2.imshow("Output", image)

    time.sleep(0.25)
    cv2.pollKey()