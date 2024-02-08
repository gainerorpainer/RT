import os
import time
import ctypes
import tempfile, shutil
import uuid

import cv2

IMGPATH = "Render\\output.ppm"
DLLPATH = "rt.dll"
WINDOWNAME = "Output"
DOWNSIZE_FACTOR = 2

os.add_dll_directory("C:\\Windows\\system32")
os.add_dll_directory("C:\\ProgramData\\Symantec\\Symantec Endpoint Protection\\14.3.9210.6000.105\\Data/Sysfer\\x64")

last_modified = 0

cv2.namedWindow(WINDOWNAME)
cv2.setWindowProperty(WINDOWNAME, cv2.WND_PROP_TOPMOST, 1)

while True:
    modified = os.stat(DLLPATH).st_mtime if os.path.exists(DLLPATH) else 0
    if modified != last_modified:
        print("Modified, re-running...")
        
        try:
            # make a copy to give not lock access to dll
            temp_dll = os.path.join(tempfile.gettempdir(), f"{uuid.uuid4()}.dll")
            shutil.copy2(DLLPATH, temp_dll)

            dll = ctypes.WinDLL(temp_dll, winmode=0)
        except (PermissionError, OSError):
            print("File locked/invalid...")
            time.sleep(0.25)
            continue

        last_modified = modified

        now = time.time()
        dll.run()
        print(f"Runtime: {1000 * (time.time() - now):0.1f}ms")

        # most clean way to release dll
        del dll
        
        image = cv2.imread(IMGPATH)
        image = cv2.resize(image, (image.shape[1] // DOWNSIZE_FACTOR, image.shape[0] // DOWNSIZE_FACTOR), interpolation=cv2.INTER_LINEAR)
        cv2.imshow(WINDOWNAME, image)

    time.sleep(0.25)
    cv2.pollKey()
