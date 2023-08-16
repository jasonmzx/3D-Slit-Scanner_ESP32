import cv2
import numpy as np
import glob
from sklearn import linear_model


####################### Basic Helpers #######################
def print_title(str):
    print("#"*20+' '+str+' '+"#"*20)

def list_of_vec3_avg(vec_of_vec3): 
    length = len(vec_of_vec3)
    if length == 0:
        return [0, 0, 0]
        
    sum_0th = sum([sublist[0] for sublist in vec_of_vec3])
    sum_1st = sum([sublist[1] for sublist in vec_of_vec3])
    sum_2nd = sum([sublist[2] for sublist in vec_of_vec3])

    average_0th = sum_0th / length
    average_1st = sum_1st / length
    average_2nd = sum_2nd / length

    return [average_0th, average_1st, average_2nd]

####################### DEBUG CONTANTS #######################

IMSHOW_FOUND_CIRCLE_GRIDS = True
IMSHOW_BINARY_DIFFS = True

def find_circle_grid_in_imgs(img_folder):
    # grid size
    pattern_size = (4, 4)
    
    # prepare object points (3D points in real world space)
    objp = np.zeros((pattern_size[0]*pattern_size[1], 3), np.float32)
    objp[:,:2] = np.mgrid[0:pattern_size[0],0:pattern_size[1]].T.reshape(-1,2)

    # create arrays to store object points and image points
    objpoints = [] # 3d points in real world space
    imgpoints = [] # 2d points in image plane

    # get list of all .jpg images in the folder
    glob_default = glob.glob(f"{img_folder}/0*.jpg")
    glob_lazer = glob.glob(f"{img_folder}/1*.jpg")

    # separate the images with and without the laser

    for img_path in glob_default:
        img = cv2.imread(img_path)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        found, corners = cv2.findCirclesGrid(gray, pattern_size, cv2.CALIB_CB_SYMMETRIC_GRID)

        # if grid found, add object points and image points
        if found:
            objpoints.append(objp)
            imgpoints.append(corners)
        
        #* Debug stuff for showing cv mats:
            if IMSHOW_FOUND_CIRCLE_GRIDS:
                cv2.drawChessboardCorners(img, pattern_size, corners, found)
                cv2.imshow('Grid', img)
                cv2.waitKey(0)

    cv2.destroyAllWindows()

    # calibrate camera
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

    # print calibration results
    print("Camera matrix : \n")
    print(mtx)
    print("dist : \n")
    print(dist)
    print("rvecs : \n")
    print(rvecs)
    print("tvecs : \n")
    print(tvecs)

    avg_tvec = np.mean(tvecs, axis=0)

    print_title("Average of Translation Vector | Avg. T-VEC")
    print(avg_tvec)

    
    img_plane_equations = []

    # for each image with laser, detect laser line and fit plane
    for i, img_path in enumerate(glob_lazer):
        
        img = cv2.imread(glob_default[i])
        img_w_lzr = cv2.imread(img_path)
       
        #! DO BINARY DIFF OF img & img_w_lzr
        # subtract non-laser image from laser image
        diff = cv2.subtract(img_w_lzr, img)

        # convert the difference to grayscale
        gray_diff = cv2.cvtColor(diff, cv2.COLOR_BGR2GRAY)

       
        if IMSHOW_BINARY_DIFFS:
            cv2.imshow('Binary Difference', diff)
            cv2.waitKey(0)

        #! Extract 2D Lazer Line Points
        laser_line_points = []

        for y in range(gray_diff.shape[0]):
            # get the x-coordinate of the brightest pixel in the row
            max_x = np.argmax(gray_diff[y])
            laser_line_points.append([max_x, y])

        # convert list to numpy array
        laser_line_points = np.array(laser_line_points, dtype=np.float32)
        laser_line_points = np.expand_dims(laser_line_points, axis=1)

        # get the corresponding 3D points for the laser line
        laser_line_points_norm = cv2.undistortPoints(laser_line_points, mtx, dist)
       
        # Reshape laser_line_points_norm to (N, 3)
        laser_line_points_norm_reshaped = laser_line_points_norm.reshape(-1, 2)

        # Convert to homogeneous coordinates
        laser_line_points_homogeneous = cv2.convertPointsToHomogeneous(laser_line_points_norm_reshaped)

        # Transpose laser_line_points_homogeneous to have a shape of (3, N)
        laser_line_points_homogeneous = laser_line_points_homogeneous.reshape(-1, 3).T

        # Multiply with inverse of mtx
        laser_line_points_3d = np.matmul(np.linalg.inv(mtx), laser_line_points_homogeneous)

        # Transpose back to original shape
        laser_line_points_3d = laser_line_points_3d.T

        # Convert points back from homogeneous
        laser_line_points_3d = cv2.convertPointsFromHomogeneous(laser_line_points_3d)

        # fit a plane to the 3D points
        model_ransac = linear_model.RANSACRegressor(linear_model.LinearRegression())
        model_ransac.fit(laser_line_points_3d.reshape(-1,3)[:,:2], laser_line_points_3d.reshape(-1,3)[:,2])

                            # A * x + B * y + C
        abc_plane_eq = [model_ransac.estimator_.coef_[0], model_ransac.estimator_.coef_[1], model_ransac.estimator_.intercept_]
        
        #print(f"DEBUG: Plane equation for image {img_path}: {abc_plane_eq[0]}x + {abc_plane_eq[1]}y + {abc_plane_eq[2]}")
        img_plane_equations.append(abc_plane_eq)

    print_title("Average PLANE EQUATION Z = (A)x + (B)y + c ")
    print(list_of_vec3_avg(img_plane_equations))

# specify the folder containing images

# print("Please Enter PATH to Set of Dot Pattern Images:")
# img_folder_path = ""
# input(img_folder_path)
# find_circle_grid_in_imgs(img_folder_path)

img_folder = "C:/Users/jason/Documents/GitHub/3D-IoT-Object-Scanner/data_camera_calibration/ESP_CAM_Dot_Pattern_Diffs"
find_circle_grid_in_imgs(img_folder)