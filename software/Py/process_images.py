import cv2
import numpy as np
import glob

def process_images(img_folder):
    # grid size
    pattern_size = (4, 4)
    
    # prepare object points (3D points in real world space)
    objp = np.zeros((pattern_size[0]*pattern_size[1], 3), np.float32)
    objp[:,:2] = np.mgrid[0:pattern_size[0],0:pattern_size[1]].T.reshape(-1,2)

    # create arrays to store object points and image points
    objpoints = [] # 3d points in real world space
    imgpoints = [] # 2d points in image plane

    # get list of all .jpg images in the folder
    images = glob.glob(f"{img_folder}/*.jpg")

    for img_path in images:
        img = cv2.imread(img_path)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        found, corners = cv2.findCirclesGrid(gray, pattern_size, cv2.CALIB_CB_SYMMETRIC_GRID)

        # if grid found, add object points and image points
        if found:
            objpoints.append(objp)
            imgpoints.append(corners)
            cv2.drawChessboardCorners(img, pattern_size, corners, found)
            cv2.imshow('Grid', img)
            cv2.waitKey(0)

    cv2.destroyAllWindows()

    # calibrate camera
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
    
    print("Camera matrix : \n")
    print(mtx)
    print("dist : \n")
    print(dist)
    print("rvecs : \n")
    print(rvecs)
    print("tvecs : \n")
    print(tvecs)

# specify the folder containing images
img_folder = "C:/Users/jason/Documents/GitHub/3D-IoT-Object-Scanner/camera-calibration-data/ESP_Dot_Pattern"
process_images(img_folder)
