import cv2
import numpy as np
import math
import time
from threading import Thread
import matplotlib.pyplot as plt

class ImageHandler:
    def __init__(self, image):
        # self._image = cv2.imread(image)
        self._image = image
        
        self._height, self._width, _ = self._image.shape 
        # self._width, self._height = self._height, self._width
        
        self._image_copy = self._image.copy()
        self._image_denoise = self._denoise(self._image_copy)
        # print(cv2.useOptimized())
        
    def get_steering_angle(self):
        
        image_hsv = self._convert_to_HSV(self._image_denoise)
        filtered = self._filter_color(image_hsv)
        edges = self._detect_edges(filtered)
        roi = self._get_roi(edges)
        
        line_segments = self._detect_line(roi)
        lane_lines = self._avergae_slope_intercept(self._image_copy, line_segments)
        steering_angle = self._calculate_steering_angle(self._image_copy, lane_lines)
        # print(steering_angle)
        return steering_angle
                
    def _denoise(self, image):
        # image = cv2.fastNlMeansDenoisingColored(image, None, 10, 10, 7, 21)
        # image = cv2.GaussianBlur(image, (9, 9), 0)
        image = cv2.GaussianBlur(image, (5,5), 0)
        # image = cv2.bilateralFilter(image,9,75,75)
        return image
    
    def _convert_to_HSV(self, image):
        return cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    
    def _filter_color(self, image):
        # without casing and tutup
        # lower_color = np.array([36, 19, 63], np.uint8)
        # upper_color = np.array([80, 255, 255], np.uint8)
        
        lower_color = np.array([30, 40, 40], np.uint8)
        upper_color = np.array([80, 255, 255], np.uint8)
        
        # lower_color = np.array([25, 24, 75], np.uint8)
        # upper_color = np.array([86, 255, 255], np.uint8)
        
        mask = cv2.inRange(image, lower_color, upper_color)
        open_kern = np.ones((5, 5), dtype=np.uint8)
        image = cv2.morphologyEx(mask, cv2.MORPH_OPEN, open_kern, iterations=1)
        return image
    
    def _detect_edges(self, image_filtered):
        threshold = 50
        edges = cv2.Canny(image_filtered, threshold, threshold * 2)
        return edges

    def _get_roi(self, edges):
        # print(self._width,self._height)
        mask = np.zeros_like(edges)
        polygon = np.array([[
            (0, self._height),
            (0, self._height / 4),
            (self._width, self._height / 4),
            (self._width, self._height)
        ]], np.int32)
        
        cv2.fillPoly(mask, polygon, 255)
        roi = cv2.bitwise_and(edges, mask)
        return roi
        
    def _detect_line(self, roi):
        rho = 1
        theta = np.pi / 180
        # min_threshold = 11
        min_threshold = 20
        # line = cv2.HoughLinesP(roi, rho, theta, min_threshold, np.array([]), minLineLength=23, maxLineGap=50)
        line = cv2.HoughLinesP(roi, rho, theta, min_threshold, np.array([]), minLineLength=20, maxLineGap=20)
        
        # print(line)
        return line
        
    def _make_points(self, image, line):
        slope, intercept = line
        y1 = self._height
        y2 = int (y1/4)
        if slope == 0:
            slope == 0.1
        try:
            x1  = int ((y1-intercept) / slope)
            x2  = int ((y2-intercept) / slope)
        except OverflowError:
            x1 = 0
            x2 = 0
        return [[x1, y1, x2, y2]]
    
    def _avergae_slope_intercept(self, image, lines):
        lane_lines = []
        if lines is None:
            # print("No line segment detected")
            return lane_lines
        
        left_fit = []
        right_fit = []
        boundary = 1/3
        
        left_region_boundary = self._width * (1 - boundary)
        right_region_boundary = self._width * boundary
        
        for line in lines:
            for x1, y1, x2, y2 in line:
                if x1 == x2:
                    # print("skipping vertical lines (slope = infinity)")
                    continue
                slope = (y2 - y1) / (x2 - x1)
                intercept = y1 - (slope * x1)
                
                if (slope < 0):
                    if (x1 < left_region_boundary and x2 < left_region_boundary):
                        left_fit.append((slope, intercept))
                        
                else:
                    if (x1 > right_region_boundary and x2 > right_region_boundary):
                        right_fit.append((slope, intercept))

        left_fit_average = np.average(left_fit, axis=0)
        if len(left_fit) > 0:
            lane_lines.append(self._make_points(image, left_fit_average))
            
            
        right_fit_average = np.average(right_fit, axis=0)
        if len(right_fit) > 0:
            lane_lines.append(self._make_points(image, right_fit_average))
            
        return lane_lines
        
    def _calculate_steering_angle(self, image, lines):
        if len(lines) == 2:
            _, _, left_x2, _ = lines[0][0] # extract left x2 from lane_lines array
            _, _, right_x2, _ = lines[1][0] # extract right x2 from lane_lines array
            mid = int(self._width / 2)
            x_offset = (left_x2 + right_x2) / 2 - mid
            y_offset = int(self._height) 
            
        elif len(lines) == 1:
            x1, _, x2, _ = lines[0][0]
            x_offset = x2 - x1
            y_offset = int(self._height / 4)
        elif len(lines) == 0: # if no line is detected
            x_offset = 0
            y_offset = int(self._height)
            return -1    
            
        angle_to_mid_radian = math.atan(x_offset / y_offset)
        angle_to_mid_deg = int(angle_to_mid_radian * 180.0 / math.pi)  
        steering_angle = angle_to_mid_deg + 90 

        return steering_angle
    
    
# x = []
# y = []
# yfiltered = []

# before = 0

# filter_order = 5
# tmp = [0 for _ in range(filter_order)]
# def update_tmp(value):
#     for i in range(filter_order-2, -1, -1):
#         tmp[i + 1] = tmp[i]
#     tmp[0]= value
#     return tmp
    
# start = 1
# end = 1016
# for i in range (start, end):
#     x.append(i-start)

#     t1 = time.time()
#     image = cv2.imread(f"test5s/img_reg_{i}.jpg")
#     app = ImageHandler(image)

#     angle = app.get_steering_angle()
#     if (angle == -1):
#         angle = before
#     y.append(angle)
    
#     tmp = update_tmp(angle)
#     print(i, '->',  int(sum(tmp)/(len(tmp))))
#     yfiltered.append(int(sum(tmp)/(len(tmp))))
    
#     # print(f"image {i} steering angle : {angle}")
#     t0 = time.time()
#     # print(t0 - t1)
#     # print((t0 - t1) * 1000)
#     before = angle
# plt.plot(x, y, yfiltered)
# plt.show()
# # print(x)
# import numpy as np
# import pandas as pd
# y = np.array(y)
# yfiltered = np.array(yfiltered)


# df = pd.DataFrame({'y': y, 'yfilter': yfiltered})
# df.to_excel("test.xlsx", index=False)
# print(y)
# print()
# print(yfiltered)